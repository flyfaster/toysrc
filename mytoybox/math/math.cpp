#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

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

int main()
{
    for (int i = 0; i < 9; ++i)
        cout << binet_formula(i) << " ";
    cout << "\n";

    for (int i = 0; i < 20; ++i)
    {
        if (binet_formula(i) != fibonacci(i))
            cout << i << "th value mismatch " << binet_formula(i) << " " << fibonacci(i)
                 << "\n";
    }
    return 0;
}
