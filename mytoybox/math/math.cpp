#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <vector>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE math
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

using namespace std;

int binet_formula(int n)
{
    return (pow(1 + sqrt(5), n) - pow(1 - sqrt(5), n)) / (pow(2, n) * sqrt(5));
}

int fibonacci(int n)
{
    if (n < 2)
        return n;
    int a = 0;
    int b = 1;
    int c = 0;
    for (int i = 2; i <= n; ++i)
    {
        c = a + b;
        a = b;
        b = c;
    }
    return c;
}

int max_sub_sum(const vector<int>& array)
{
    int best = 0, sum = 0;
    for (int data: array)
    {
        sum = max(data, sum + data);
        best = max(best, sum);
    }
    return best;
}

int gcd(int greater, int smaller)
{
    while (smaller != 0)
    {
        greater %= smaller;
        std::swap(greater, smaller);
    }
    return std::abs(greater);
}

std::vector<int> get_prime(int n)
{
    vector<int> isprime(max(2, n + 1), 1);         // init all prime, at least 2 [0,1].
    fill(isprime.begin(), isprime.begin() + 2, 0); // 0 and 1 are not prime
    for (int i = 2; i * i <= n; ++i)
    {
        if (isprime[i])
            for (int j = i + i; j <= n; j += i)
                isprime[j] = 0; // not prime
    }
    vector<int> prime_list;
    for (int i = 2; i <= n; ++i)
        if (isprime[i])
            prime_list.push_back(i);
    return prime_list;
}

BOOST_AUTO_TEST_CASE(check_get_prime)
{
    cout << "BOOST_AUTO_TEST_CASE(get the list of primes up to n.)" << endl;
    int n = 1000;

    auto prime_list = get_prime(1000);
    BOOST_CHECK_EQUAL(prime_list.size(), 168);
    BOOST_CHECK_EQUAL(prime_list[0], 2);
    BOOST_CHECK_EQUAL(prime_list[1], 3);
    BOOST_CHECK_EQUAL(*prime_list.rbegin(), 997);
}

BOOST_AUTO_TEST_CASE(check_binet_formula)
{
    cout << "BOOST_AUTO_TEST_CASE(binet_formula)" << endl;
    int n = 20;
    cout << "binet_formula\n";
    for (int i = 0; i < n; ++i)
        cout << binet_formula(i) << " ";
    cout << "\n";

    for (int i = 0; i < n * 2; ++i)
    {
        BOOST_TEST_CONTEXT("fibonacci(" << i << ")")
        BOOST_CHECK_EQUAL(binet_formula(i), fibonacci(i));
    }
}

BOOST_AUTO_TEST_CASE(check_round)
{
    cout << "BOOST_AUTO_TEST_CASE(lrint vs lround)" << endl;
    int n = 2000;
    for (int i = 1; i < n; ++i)
    {
    	auto order = std::log2(i);
        BOOST_TEST_CONTEXT("lrint and lround(" << i << ")")
        BOOST_CHECK_EQUAL(std::lrint(order), std::lround(order));
    }

    for (int i = 1; i < n; ++i)
    {
    	auto order = std::log2(i);
    	auto c = (order - std::floor(order))>=0.5? std::ceil(order):std::floor(order);
        BOOST_TEST_CONTEXT("lrint and ceil, floor(" << i << ")")
        BOOST_CHECK(std::lrint(order)==c);
    }

    float fd = 100000000.0;
    while (fd * 10.0 > fd)
    {
        fd *= 10.0;
        auto next = std::nextafter(fd, fd * 1.1);
        cout << "nextafter " << std::setprecision(20) << fd << " is " << next << endl;
        if (next - fd > 2)
        	break;
    }
    //    nextafter 1000000000 is 1000000000.0000001192
    //    nextafter 10000000000 is 10000000000.000001907
    //    nextafter 99999997952 is 99999997952.000015259
    //    nextafter 999999995904 is 999999995904.00012207
    //    nextafter 9999999827968 is 9999999827968.0019531
    //    nextafter 100000000376832 is 100000000376832.01562
    //    nextafter 999999986991104 is 999999986991104.125
    //    nextafter 10000000272564224 is 10000000272564226
    //    nextafter 99999998430674944 is 99999998430674960

    fd = 10000000272564224.f;
    for (int i = 0;; i++)
    {
        if (fd < fd + i)
        {
            cout << std::setprecision(30) << fd << " + " << i << "=" << (fd + i) << endl;
            auto epsilon = fd * std::numeric_limits<decltype(fd)>::epsilon();
            cout << std::setprecision(30) << fd
                 << " * FLT_EPSILON = " << (fd * std::numeric_limits<decltype(fd)>::epsilon())
                 << endl;
            cout << "i/epsilon = " << i/epsilon <<endl;
            break;
        }
    }
//    10000000272564224 + 536870945=10000001346306048
//    10000000272564224 * FLT_EPSILON = 1192092928
//    i/epsilon = 0.450360000133514404296875
    auto fd1 = std::numeric_limits<float>::max() - 1;
    auto fd2 = std::numeric_limits<float>::max() - 2;
    // 3.40282346638528859811704183485e+38 == 3.40282346638528859811704183485e+38
    if (fd1 == fd2)
    	cout << fd1 << " == " << fd2 << endl;
    else
    	cout << fd1 << " != " << fd2 << endl;
}

vector<vector<int>> GeneratePascalTriangle(int num_rows)
{
    vector<vector<int>> pascal_triangle;
    for (int i = 0; i < num_rows; ++i)
    {
        vector<int> curr_row;
        for (int j = 0; j <= i; ++j)
        {
            // Sets this entry to the sum of the two above adjacent entries if they
            // exist.
            curr_row.emplace_back(0 < j && j < i ? pascal_triangle.back()[j - 1] +
                                                       pascal_triangle.back()[j] :
                                                   1);
        }
        pascal_triangle.emplace_back(std::move(curr_row));
    }
    return pascal_triangle;
}

bool is_palindrome(const vector<int>& nums)
{
    size_t lpos = 0;
    size_t rpos = nums.size() - 1;
    while (lpos < rpos)
        if (nums[lpos--] != nums[rpos++])
            return false;
    return true;
}

BOOST_AUTO_TEST_CASE(check_pascal_triangle)
{
    cout << "BOOST_AUTO_TEST_CASE(pascal_triangle)" << endl;
    auto pascal_triangle = GeneratePascalTriangle(4);
    for (const auto& vec: pascal_triangle)
    {
    	copy(vec.begin(), vec.end(), ostream_iterator<int>(std::cout, " "));
    	cout << "\n";
    	BOOST_CHECK(is_palindrome(vec));
    }
}
