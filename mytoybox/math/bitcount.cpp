#include <bitset>
#include <iostream>
#include <limits>
using namespace std;

int get_bits(int data) {
	bitset<numeric_limits<decltype(data)>::digits> d_bits(data);
	return d_bits.count();
}

int countbits1(int n) {
	int res = 0;
	while (n) {
		++res;
		n = n & (n - 1); //change the right most bit to 0
	}
	return res;
}

int countbits(int n) {
	int res = 0;
	for (; n; ++res, n = n & (n - 1))
		;
	return res;
}

int main() {
	int mismatch_cnt = 0;
	for (int i = 0; i < 64; ++i) {
		auto a = get_bits(i);
		auto b = countbits1(i);
		auto c = countbits(i);
		if (a != b || a != c) {
			cout << i << " mismatch by counting bits in different ways:" << a
					<< ", " << b << ", " << c << "\n";
			++mismatch_cnt;
		}
	}
	if (!mismatch_cnt)
		cout << "Got the same result by counting bits in different ways.\n";
	return 0;
}