#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <inttypes.h>
#include <deque>
#include <algorithm>
#include <iostream>
#include <cxxabi.h>
#include <execinfo.h>
#include <signal.h>

#include <bfd.h> // yum install binutils-devel
#include "profiler.h"

__attribute__ ((__destructor__)) void pos_func(void)
		__attribute__ ((no_instrument_function));

sfuncperf::sfuncperf() {
	cnt = 0;
	duration = 0;
	function = 0;
	minval = 0;
	maxval = 0;
}

sfuncperf::~sfuncperf() {
}

void sfuncperf::add(const sfuncperf& other) {
	if (!cnt) {
		function = other.function;
		cnt = other.cnt;
		duration = other.duration;
		minval = other.minval;
		maxval = other.maxval;
	} else {
		cnt += other.cnt;
		duration += other.duration;
		function = other.function;
		minval = std::min(minval, other.minval);
		maxval = std::max(maxval, other.maxval);
	}
}

void sfuncperf::add(uint64_t sometime) {
	cnt++;
	duration += sometime;
	if (cnt == 1) {
		minval = maxval = sometime;
	} else {
		minval = std::min(minval, sometime);
		maxval = std::max(maxval, sometime);
	}
}

funcperf::funcperf() {
	duration = 0;
	function = 0;
}

funcperf::~funcperf() {
}

/*
 global call count
 per thread call count
 max call per thread 
 max call per process
 how to exclude cost of __cyg_profile_func_enter and __cyg_profile_func_exit
 record parent function?
 each time allocate 16K funcperf buffers

 */
/* Private variables.  */
static __thread int level = 0;
static __thread int cyg_profile_enabled = 1;

const int MAX_DEPTH = 1024;
const uint32_t MAX_PRECISE_PERF = 1024 * 1024 * 4;
#define MAX_LEVEL 100
__thread deque_funcperf *perthreadfuncperf;
__thread map_func_perf *perthreadsfp;
volatile int datacount = 0;
__thread uint64_t enter_time[MAX_DEPTH];
funcperf* global_fps;

//lock_free::stack<deque_funcperf*> global_funcperf_deques; // c++/4.8.2/atomic:225: undefined reference to `__atomic_exchange_16'
//lock_free::stack<map_func_perf*> global_funcperf_maps;
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
deque_funcperf* global_funcperf_deques[1024];
map_func_perf* global_funcperf_maps[1024];
volatile int global_funcperf_deques_cnt = 0;
volatile int global_funcperf_maps_cnt = 0;

static struct sigaction act;
void sighandler(int signum, siginfo_t *info, void *ptr)
		__attribute__ ((no_instrument_function));

void sighandler(int signum, siginfo_t *info, void *ptr) {
	printf("Received signal %d, Signal originates from process %lu\n", signum,
			(unsigned long) info->si_pid);
	pos_func();
}

__attribute__ ((__constructor__))
void pre_func(void) {
	printf("%s\n", __FUNCTION__);
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = sighandler;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &act, NULL);
	global_fps = new funcperf[MAX_PRECISE_PERF];
}

const size_t CTRACE_BUF_LEN = 256;

static bfd* abfd = 0;
static asymbol **syms = 0;
static asection *text = 0;

void resolve(ptrtype address, char* buf_func, size_t sz) {
	// http://en.wikibooks.org/wiki/Linux_Applications_Debugging_Techniques/Print_Version
	buf_func[0] = 0;

	if (!abfd) {
		static char ename[1024];
		int l = readlink("/proc/self/exe", ename, sizeof(ename));
		if (l == -1) {
			perror("failed to find executable\n");
			return;
		}
		ename[l] = 0;

		bfd_init();

		abfd = bfd_openr(ename, 0);
		if (!abfd) {
			perror("bfd_openr failed: ");
			return;
		}

		/* oddly, this is required for it to work... */
		bfd_check_format(abfd, bfd_object);

		unsigned storage_needed = bfd_get_symtab_upper_bound(abfd);
		syms = (asymbol **) malloc(storage_needed);
		unsigned cSymbols = bfd_canonicalize_symtab(abfd, syms);

		text = bfd_get_section_by_name(abfd, ".text");
	}

	ptrtype offset = ((ptrtype) address) - text->vma;
	if (offset > 0) {
		const char *file;
		const char *func;
		unsigned line;
		char demanglenamebuf[128 + 1];
		int status = 0;
		size_t bufsize = 128;
		if (bfd_find_nearest_line(abfd, text, syms, offset, &file, &func, &line)
				&& file) {  // works better than backtrace_symbols for me
			abi::__cxa_demangle(func, demanglenamebuf, &bufsize, &status);
			demanglenamebuf[bufsize] = 0;
			//sprintf(buf_func, "%s(%u) %s", file, line, (status==0)?demanglenamebuf:func);
			strcpy(buf_func, (status == 0) ? demanglenamebuf : func);
		}
	}
	if (buf_func[0] == 0)
		sprintf(buf_func, "unknown - %p", (void*)address);
}

void mergeperf(std::map<ptrtype, sfuncperf>& dst,
		std::map<ptrtype, sfuncperf>& src) {
	int oldval = cygprofile_enable(0);
	std::cout << __FUNCTION__ << " dst size " << dst.size() << ", source size "
			<< src.size() << std::endl;
	for (const auto& mm : src) {
		if (dst.find(mm.first) == dst.end())
			dst[mm.first] = mm.second;
		else {
			dst[mm.first].add(mm.second);
		}
	}
	cygprofile_enable(oldval);
}

void mergeperf(map_func_perf& dst, deque_funcperf& src) {
	for (auto fp : src) {
		mergeperf(dst, fp); 
	}
}

void mergeperf(map_func_perf& dst, const funcperf& fp) {
	sfuncperf tmp2;
	tmp2.function = fp.function;
	auto maploc = dst.find(fp.function);
	if (maploc == dst.end()) {
		tmp2.add(fp.duration);
		dst[fp.function] = tmp2;
	} else {
		maploc->second.add(fp.duration);
	}
}

volatile int aaa = 0;
__attribute__ ((__destructor__))
void pos_func(void) {
	int tmpcnt = __sync_fetch_and_add(&aaa, 1);
	if (tmpcnt > 1) {
		__sync_fetch_and_sub(&aaa, 1);
		return;
	}
	cygprofile_enable(0);
	char buf_func[CTRACE_BUF_LEN + 1] = { 0 };
	printf("%s print function performance data\n", __FUNCTION__);
	if (!perthreadfuncperf) {
		__sync_fetch_and_sub(&aaa, 1);
		std::cout << __func__ << " perthreadfuncperf disabled\n";
	}

	map_func_perf mysum;

	for (int i = 0; i < ARRAY_SIZE(global_funcperf_deques); i++) {
		if (global_funcperf_deques[i])
			mergeperf(mysum, *global_funcperf_deques[i]);
	}

	for (int i = 0; i < ARRAY_SIZE(global_funcperf_maps); i++) {
		if (global_funcperf_maps[i])
			mergeperf(mysum, *global_funcperf_maps[i]);
	}

	for (int i = 0; i < MAX_PRECISE_PERF; i++) {
		mergeperf(mysum, global_fps[i]);
	}

	for (auto& mm : mysum) {
		sfuncperf sfp = mm.second;
		resolve(sfp.function, buf_func, CTRACE_BUF_LEN);
		std::cout << buf_func << "\t" << sfp.function << " called " << sfp.cnt
				<< " times, " << "execution min time " << sfp.minval
				<< ", max time " << sfp.maxval << ", average time "
				<< sfp.duration / sfp.cnt << std::endl;
	}
	__sync_fetch_and_sub(&aaa, 1);
	printf("%s done\n", __FUNCTION__);
}

int cygprofile_isenabled(void) {
	return cyg_profile_enabled;
}

int cygprofile_enable(int newvalue) {
	int oldval = cyg_profile_enabled;
	cyg_profile_enabled = newvalue;
	return oldval;
}

void __cyg_profile_func_enter(void *this_fn, void *call_site) {
	if (!cyg_profile_enabled)
		return;

	level++;
	if (level < MAX_DEPTH) {
		enter_time[level] = bentime_nsec();
	}

//  printf("%04d %p enter\n", level, this_fn);

}

void record_func_perf(void *this_fn, uint64_t duration) {
	if (!perthreadsfp) {
		int tmppos = __sync_fetch_and_add(&global_funcperf_maps_cnt, 1) - 1;
		if (tmppos < ARRAY_SIZE(global_funcperf_maps)) {
			perthreadsfp = new std::map<ptrtype, sfuncperf>;
			global_funcperf_maps[tmppos] = perthreadsfp;
		}
	}
	if (perthreadsfp) {
		sfuncperf sfp;
		sfp.function = (ptrtype) this_fn;
		if (perthreadsfp->find(sfp.function) == perthreadsfp->end()) {
			sfp.add(duration);
			(*perthreadsfp)[sfp.function] = sfp;
		} else {
			(*perthreadsfp)[sfp.function].add(duration);
		}
	}
}

void __cyg_profile_func_exit(void *this_fn, void *call_site) {
	if (!cyg_profile_enabled)
		return;

	int oldval = cygprofile_enable(0);
	if (level < MAX_DEPTH) {

		uint64_t duration = bentime_nsec() - enter_time[level];

		int tmpcnt = __sync_fetch_and_add(&datacount, 1);
		if (global_fps && tmpcnt < MAX_PRECISE_PERF) {
			global_fps[tmpcnt - 1].function = (ptrtype) this_fn;
			global_fps[tmpcnt - 1].caller = (ptrtype) __builtin_return_address(
					0);
			global_fps[tmpcnt - 1].duration = duration;
		} else {
			record_func_perf(this_fn, duration); // add to map, a lot of overhead
		}

//     if (0) {
//       if (!perthreadfuncperf)
//       {
// 	int tmppos = __sync_fetch_and_add(&global_funcperf_deques_cnt, 1)-1;
// 	if (tmppos < ARRAY_SIZE(global_funcperf_deques))
// 	{
// 	  perthreadfuncperf = new std::deque<funcperf>;
// 	  global_funcperf_deques[tmppos] = perthreadfuncperf;
// 	}
//       }
//       
//       if (perthreadfuncperf && tmpcnt <MAX_PRECISE_PERF)
//       {
// 	funcperf fp;
// 	fp.function = (ptrtype) this_fn;
// 	fp.duration = duration;
// 	perthreadfuncperf->push_back(fp);
//       }
//       else
//       {
// 	record_func_perf(this_fn, duration);
//       }
//     }

	}
	cygprofile_enable(oldval);
//  printf("%04d %p exit\n", level, this_fn);
	level--;
}

