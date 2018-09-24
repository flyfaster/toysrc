#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
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
}
