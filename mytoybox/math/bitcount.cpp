#include <bitset>
#include <iostream>
#include <limits>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE bitcount
#include <boost/test/unit_test.hpp>
#include <boost/test/results_reporter.hpp>

using namespace std;

int count_bits(int data)
{
    bitset<numeric_limits<decltype(data)>::digits> d_bits(data);
    return d_bits.count();
}

int countbits1(int n)
{
    int res = 0;
    while (n)
    {
        ++res;
        n = n & (n - 1); // change the right most bit to 0
    }
    return res;
}

int countbits(int n)
{
    int res = 0;
    for (; n; ++res, n = n & (n - 1))
        ;
    return res;
}

void process_subset(int x)
{
    cout << __func__ << "(" << x << ")\n";
    int b = 0;
    do
    {
        cout << b << " ";
    } while ((b = (b - x) & x));
    cout << "\n";
}

BOOST_AUTO_TEST_CASE(check_num_bits)
{
    int mismatch_cnt = 0;
    for (int i = 0; i < 64; ++i)
    {
        auto a = count_bits(i);
        auto b = countbits1(i);
        auto c = countbits(i);
        if (a != b || a != c)
        {
            cout << i << " mismatch by counting bits in different ways:" << a << ", " << b
                 << ", " << c << "\n";
            ++mismatch_cnt;
        }

        {
            int x = i;
            int a = x & ~(x - 1); // only right most bit left, other bits are 0
            int b = x & (-x);
            if (a != b || count_bits(a) > 1)
            {
                cout << i << " lowest bit " << a << "!=" << b << "\n";
                ++mismatch_cnt;
            }
        }
    }
    BOOST_CHECK_EQUAL(0, mismatch_cnt);

    for (int x = 0; x < 16; ++x)
        process_subset(x);
}

int get_msb(int n)
{
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n += 1;
    return n >> 1;
}

BOOST_AUTO_TEST_CASE(check_msb)
{
    int src = 5;
    BOOST_TEST_CONTEXT("Get MSB of " << src)
    BOOST_CHECK_EQUAL(4, get_msb(src));

    src = 1;
    BOOST_TEST_CONTEXT("Get MSB of " << src)
    BOOST_CHECK_EQUAL(1, get_msb(src));

    for (src = 1; src < 65536; ++src)
    {
        int exponent = (int) std::log2(src);
        BOOST_TEST_CONTEXT("Get MSB of " << src)
        BOOST_CHECK_EQUAL(1 << exponent, get_msb(src));
    }
}

int clear_lsb(int n)
{
    return n & (n - 1);
}

int get_lsb(int n)
{
    return n & ~(n - 1);
}

int get_lsb2(int n)
{
    return n & -n;
}

BOOST_AUTO_TEST_CASE(check_lsb)
{
    int src = 5;
    int target = src & (src - 1);
    BOOST_TEST_CONTEXT("src & (src-1) clear LSB from " << src)
    BOOST_CHECK_EQUAL(4, clear_lsb(src));

    target = get_lsb(src);
    BOOST_TEST_CONTEXT("src & ~(src-1) get LSB from " << src)
    BOOST_CHECK_EQUAL(1, target);

    target = src & -src;
    BOOST_TEST_CONTEXT("src & -src get LSB from " << src)
    BOOST_CHECK_EQUAL(1, target);

    src = 6;
    target = get_lsb2(src);
    BOOST_TEST_CONTEXT("src & -src get LSB from " << src)
    BOOST_CHECK_EQUAL(2, target);

    src = 7;
    target = src & -src;
    BOOST_TEST_CONTEXT("src & -src get right most 1 from " << src)
    BOOST_CHECK_EQUAL(1, target);

    src = 0;
    target = src & -src;
    BOOST_TEST_CONTEXT("src & -src get right most 1 from " << src)
    BOOST_CHECK_EQUAL(0, target);

    src = 0;
    target = src & (src - 1);
    BOOST_TEST_CONTEXT("src & (src-1) clear right most 1 from " << src)
    BOOST_CHECK_EQUAL(0, target);
}

BOOST_AUTO_TEST_CASE(check_shift)
{
    int src = -1;
    int target = src >> 1;	// sign bit is preserved
    BOOST_TEST_CONTEXT("verify arithmetic shift of C++ " << src)
    BOOST_CHECK_EQUAL(src, target);
    unsigned int usrc = 0xFFFFFFFF;
    BOOST_CHECK_EQUAL(usrc/2, usrc>>1);
}
