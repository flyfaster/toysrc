#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <boost/program_options.hpp>
using namespace std;

struct app_setup_t
{
    void parse_cmd_line(int argc, char* argv[])
    {
        // clang-format off
	    desc.add_options()
	    	("help,h", "Find kth element (0 based) via medium of medium algorithm")
			("start_num", boost::program_options::value<int>(&start_num)->default_value(start_num),"specify value of start_num")
	    	("max_num", boost::program_options::value<int>(&max_num)->default_value(max_num),"specify value of max_num")
			("kth", boost::program_options::value<int>(&kth)->default_value(kth),"specify kth value to search")
			;
        // clang-format on
        boost::program_options::store(
            boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);
    }

    boost::program_options::variables_map vm;
    boost::program_options::options_description desc{"Allowed options"};
    int start_num = 1;
    int max_num = 64;
    int kth = rand() % max_num;
    int error_cnt = 0;
};

void print_array(const char* msg, int* a, int size)
{
    if (size < 0)
    {
        cout << msg << " {";
        copy(a, a + size, ostream_iterator<int>(cout, ", "));
        cout << "}\n";
    }
}

int find_k(int* a, int size, int k)
{

    if (size <= 5)
    {
        sort(a, a + size);
        return a[k];
    }

    int groups = size / 5;
    do
    {
        for (int i = 0; i < groups; ++i)
        {
            int* beg = a + i * 5;
            sort(beg, beg + 5);
            swap(a[i], beg[2]);
        }
        groups /= 5;
    } while (groups > 5);

    sort(a, a + groups);
    int median_val = a[groups / 2];
    swap(a[groups / 2], a[size - 1]); // trick from https://stackoverflow.com/questions/37769227/partitioning-a-vector-using-stdpartition

    auto iter =
        stable_partition(a, a + size - 1, [median_val](int n) { return n < median_val; });
    swap(*iter, a[size - 1]); // separate left and right sequence by medium value
    int mom_idx = iter - a;

    if (k == mom_idx)
        return a[k];
    if (k < mom_idx)
        return find_k(a, mom_idx, k);
    else
    {
        mom_idx++;
        return find_k(a + mom_idx, size - mom_idx, k - mom_idx);
    }
}

void verify(vector<int>& nums, int k, int expected_val)
{
    vector<int> dup_nums = nums;
    int res = find_k(&nums[0], nums.size(), k);
    if (res != expected_val)
    {
        cout << "dup_nums:{";
        copy(dup_nums.begin(), dup_nums.end(), ostream_iterator<int>(cout, ", "));
        cout << "};\n";
        cout << k << "th value " << res << " differ from expected value " << expected_val
             << "\n";
    }
}

int main(int argc, char* argv[])
{
    app_setup_t app_setup;
    app_setup.parse_cmd_line(argc, argv);
    if (app_setup.vm.count("help"))
    {
        std::cout << app_setup.desc << std::endl;
        return 0;
    }

    for (int i = app_setup.start_num; i < app_setup.max_num; ++i)
    {
        vector<int> nums(i);
        std::iota(nums.begin(), nums.end(), 1);
        std::random_shuffle(nums.begin(), nums.end());
        int k = nums.size() / 2;
        k = app_setup.kth;
        k = min(k, (int) nums.size() - 1);
        verify(nums, k, k + 1);
    }
    cout << "There are " << app_setup.error_cnt << " errors\n";

    return 0;
}

/*
randomized-select(A, p, r, nth)
	if p==r
		return A[p]
	pivot_pos = randomized_partition(A, p, r)
	k = pivot_pos - p + 1
	if nth == k
		return A[pivot_pos]
	if nth < k
		return randomized-select(A, p, k, nth)
	return randomized-select(A, pivot_pos+1, r, nth-k)

*/
