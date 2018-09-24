#include <initializer_list>
#include <iostream>

using namespace std;
// C++Now 2018: Jason Turner “Initializer Lists Are Broken, Let's Fix Them”

auto f(int i, int j, int k)
{
    return initializer_list<int>{i, j, k};
}

int main(int argc, const char* [])
{
    for (int i : f(argc + 1, argc + 2, argc + 3))
        cout << i << " ";
    cout << "\n";
    return 0;
}
