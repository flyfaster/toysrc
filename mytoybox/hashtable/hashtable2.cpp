#include <array>
#include <iostream>
#include <algorithm>
#include <memory>
#include <functional>
#include <iterator>
#include <iomanip>
#include <vector>
#include <utility>
#include <limits>
#include <cmath>

using namespace std;

template<typename T>
class hash_table {
	template<typename T2>
	struct slot{
		T2 m_data;
		bool is_valid;
		size_t next;
	};
public:
	hash_table(size_t sz, std::function<size_t(const T& data)> hasher, std::function<bool(T const& , T const& )> eq) :
		m_sz(sz), m_hash{hasher}, m_eq{eq} {
		m_stg = std::make_unique<slot<T>[]>(sz);
	}

	void put(const T& data) {
		auto pos = m_hash(data) % m_sz;

		if (m_stg[pos].is_valid==false) { // the chain is empty
			++m_cnt;
			m_stg[pos].is_valid = true;
			m_stg[pos].m_data = data;
			m_stg[pos].next = m_sz;
			return;
		}

		// slot is in use
		if (m_eq(m_stg[pos].m_data, data))
		{
			// same key, replace it in case its value is different
			m_stg[pos].m_data = data;
			return;
		}

		size_t empty_pos = m_sz;
		auto prev = pos;
		// check next until found same key, or next is null
		while(m_stg[pos].next < m_sz)
		{
			prev = pos;
			pos = m_stg[pos].next;
			if (m_eq(m_stg[pos].m_data, data))
			{
				// same key, replace it in case its value is different
				m_stg[pos].m_data = data;
				return;
			}
		}

		if (m_cnt>=m_sz)
			return;

		// next is m_sz, need to assign new slot for this data
		if (empty_pos>=m_sz) // no empty slot in existing chain
			for (empty_pos=0; empty_pos<m_sz; ++empty_pos)
			{
				if (!m_stg[empty_pos].is_valid) {
					++m_cnt;
					m_stg[empty_pos].is_valid = true;
					m_stg[empty_pos].m_data = data;
					m_stg[empty_pos].next = m_sz;
					m_stg[pos].next = empty_pos;
					return;
				}
			}
	}

	bool get(T& data) {
		size_t hc = m_hash(data)% m_sz;

		while(hc < m_sz && m_stg[hc].is_valid) {
			if (m_eq(data, m_stg[hc].m_data))
			{
				data = m_stg[hc].m_data;
				return true;
			}
			hc = m_stg[hc].next;
		}
		return false;
	}

	bool del(T const& data) {
		size_t hc = m_hash(data) % m_sz;

		if (m_stg[hc].is_valid && m_stg[hc].next == m_sz)
		{ // the chain contains single element
			m_stg[hc].is_valid = false;
			return true;
		}
		auto parent = hc;
		auto pos = hc;
		while(m_stg[pos].is_valid) {
			if (m_eq(data, m_stg[pos].m_data))
			{
				auto next_elem = m_stg[pos].next;
				if (next_elem >= m_sz) {
					// it is last element in the chain
					// mark this slot as deleted
					m_stg[pos].is_valid = false;
					// mark its parent point to end
					m_stg[parent].next = m_sz;
					return true;
				}
				// it is middle element in the chain, copy next element to pos
				m_stg[pos] = m_stg[next_elem];
				m_stg[next_elem].is_valid = false;
				m_stg[next_elem].next = m_sz;
				return true;
			}
			parent = pos;
			pos = m_stg[pos].next;
		}
		return false;
	}

	void resize(size_t nsz) {

	}
	size_t size() { return m_cnt;}
private:
	size_t m_sz;
	size_t m_cnt{0};
	std::unique_ptr<slot<T>[]> m_stg;
	std::function<size_t(const T& )> m_hash;
	std::function<bool(T const& , T const& )> m_eq;
};

using pii = pair<int, int>;

struct universal_hashing
{
    universal_hashing(size_t buckets)
    {
        while (buckets)
        {
            ++M;
            buckets >>= 1;
        }
        b = b % (size_t) std::pow(2, w - M);
    }
    size_t operator()(const pii& data) const
    {
        // https://en.wikipedia.org/wiki/Universal_hashing
        return (a * data.first + b) >> (w - M);
    }

private:
    static constexpr size_t w = std::numeric_limits<int>::digits;
    size_t M;
    size_t a = 104723; // where a is a random odd positive integer with a<2^{w}
    size_t b = 104231; // b is a random non-negative integer with b<2^{w-M}
};

int main() {

	std::function<size_t(const pii& data)> hasher = [](const pii& data) ->size_t {
		return 104723 * (size_t)data.first + (size_t)104231;
	};

	std::function<bool(pii const& , pii const& )> eq = [](pii const& lhs, pii const& rhs) {
		return lhs.first == rhs.first;
	};

	hash_table<pii> ht( (size_t)5, universal_hashing(5), eq);

	vector<int> putlist{1, 4, 2, 5, 9};
	vector<int> getlist{3, 5, 6};
	vector<int> delist{1, 2, 4, 5, 9};
	for (auto data: putlist)
	{
		ht.put({data, data*data});
	}

	for (auto data: getlist)
	{
		pii dd{data, 0};
		bool res = ht.get(dd);
		cout << "get " << data << " return " << dd.second << "\n";
	}

	for (auto data: delist)
	{
		pii dd{data, 0};
		cout << "del " << data << " return " << ht.del(dd)
				<< ", get " << data << " return " << ht.get(dd)<< "\n";
	}
	return 0;
}
