#include <map>
#include <deque>

#ifdef __x86_64__ // http://sourceforge.net/p/predef/wiki/Architectures/
typedef uint64_t ptrtype;
#else
typedef uint32_t ptrtype;  
#endif

struct funcperf {
  funcperf() __attribute__ ((no_instrument_function));
  ~funcperf() __attribute__ ((no_instrument_function));
  ptrtype function;
  ptrtype caller;
  uint64_t duration;
};

struct sfuncperf {
  __attribute__ ((no_instrument_function)) sfuncperf() ;
  __attribute__ ((no_instrument_function)) ~sfuncperf() ;
  void add(uint64_t sometime) __attribute__ ((no_instrument_function));
  void add(const sfuncperf& other) __attribute__ ((no_instrument_function));
  ptrtype function;
  ptrtype caller;
  uint64_t duration;
  uint64_t cnt;
  uint64_t minval;
  uint64_t maxval;
};

typedef std::deque<funcperf> deque_funcperf;
typedef std::map<ptrtype, sfuncperf> map_func_perf;

void mergeperf(map_func_perf& dst, const funcperf& fp) __attribute__ ((no_instrument_function));
void mergeperf(std::map<ptrtype, sfuncperf>& dst,  std::map<ptrtype, sfuncperf>& src) __attribute__ ((no_instrument_function));


#ifdef __cplusplus
extern "C" {
#endif

  
/* Static functions. */
int cygprofile_enable (int) __attribute__ ((no_instrument_function));


/* Note that these are linked internally by the compiler. 
 * Don't call them directly! */
void __cyg_profile_func_enter (void *this_fn, void *call_site) __attribute__ ((no_instrument_function));
void __cyg_profile_func_exit (void *this_fn, void *call_site) __attribute__ ((no_instrument_function));
void pre_func(void) __attribute__ ((no_instrument_function));
void pos_func(void) __attribute__ ((no_instrument_function));
//static __inline__ uint64_t rdtsc(void)  __attribute__ ((no_instrument_function));
uint64_t rdtsc(void)  __attribute__ ((no_instrument_function));
//uint64_t rdtsc(void);

void resolve(ptrtype address, char* buf_func, size_t sz) __attribute__ ((no_instrument_function));

#ifdef __cplusplus
};
#endif

//extern uint64_t bentime_nsec(void) __attribute__ ((no_instrument_function));
uint64_t bentime_nsec(void) __attribute__ ((no_instrument_function));
int delay_timer_nsec(uint64_t time_nsec) __attribute__ ((no_instrument_function));
