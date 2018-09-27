#include <cmath>
#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE rabin_karp
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

/*
Rabin-Karp(T, pattern, radix, prime)
	n = len(T)
	m = len(pattern)
	h = std::pow(radix, m-1) % prime
	p = t0 = 0
	for i = 1 to len(pattern)
		p = (radix * p + pattern[i]) % prime
		t0 = (radix * t0 + T[i]) % prime

	for s = 0 to n - m
		if p == t0
			if patter[1..m] == T[s+1..s+m]
				print "match at offset s"
		if s < n - m
			t[s+1] = (d*(t[s] - T[s+1]*h) + T[s + m + 1] ) % prime

*/

size_t rabin_karp(const char* txt, const char* pattern)
{
    size_t radix = 256;
    size_t prime = 100000007; // 65537
    size_t n = strlen(txt);
    size_t m = strlen(pattern);
    size_t pattern_hash = 0;
    size_t txt_hash = 0;

    if (n < m)
        return -1;

    size_t h = 1;
    for (int i = 0; i < m - 1; ++i)
        h = (h * radix) % prime;
    // std::pow overflow so it can't be used
    //h = (size_t) std::pow(radix, m - 1)%prime;

    for (int i = 0; i < m; ++i)
    {
        pattern_hash = ((radix * pattern_hash + pattern[i]) % prime);
        txt_hash = ((radix * txt_hash + txt[i]) % prime);
    }

    for (int s = 0; s <= n - m; ++s)
    {
        if (pattern_hash == txt_hash)
        {
            if (std::strncmp(pattern, txt + s, m) == 0)
                return s;
        }

        if (s < n - m)
        {
        	while(txt_hash < txt[s]*h)
        		txt_hash += prime;

        	txt_hash = (radix * (txt_hash - txt[s]*h) + txt[s+m]) % prime;
        }
    }
    return -1;
}

BOOST_AUTO_TEST_CASE(check_rabin_karp)
{
    std::string text = "abcd";
    std::string pattern = "abcde";
    BOOST_TEST_CONTEXT("search " << text << " for " << pattern)
    BOOST_CHECK_EQUAL(-1, rabin_karp(text.c_str(), pattern.c_str()));

    pattern = "b";
    BOOST_TEST_CONTEXT("search " << text << " for " << pattern)
    BOOST_CHECK_EQUAL(1, rabin_karp(text.c_str(), pattern.c_str()));

    pattern = "a";
    BOOST_TEST_CONTEXT("search " << text << " for " << pattern)
    BOOST_CHECK_EQUAL(0, rabin_karp(text.c_str(), pattern.c_str()));

    pattern = "d";
    BOOST_TEST_CONTEXT("search " << text << " for " << pattern)
    BOOST_CHECK_EQUAL(3, rabin_karp(text.c_str(), pattern.c_str()));

    pattern = text;
    BOOST_TEST_CONTEXT("search " << text << " for " << pattern)
    BOOST_CHECK_EQUAL(0, rabin_karp(text.c_str(), pattern.c_str()));

    text = "Supercalifragilisticexpialidocious";
    pattern = "fragilisticexpiali";
    BOOST_TEST_CONTEXT("search " << text << " for " << pattern)
    BOOST_CHECK_EQUAL(text.find(pattern), rabin_karp(text.c_str(), pattern.c_str()));

    pattern = "ticexpialidocious";
    BOOST_TEST_CONTEXT("search " << text << " for " << pattern)
    BOOST_CHECK_EQUAL(text.find(pattern), rabin_karp(text.c_str(), pattern.c_str()));

}
