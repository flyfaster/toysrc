#include <initializer_list>
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;
// C++Now 2018: Jason Turner “Initializer Lists Are Broken, Let's Fix Them”

auto f(int i, int j, int k)
{
    return initializer_list<int>{i, j, k};
}

class rule_of_five
{
public:
	char* cstring; // raw pointer used as a handle to a dynamically-allocated memory block
    rule_of_five(const char* s, std::size_t n) // to avoid counting twice
        :
        cstring(new char[n]) // allocate
    {
    	cout << "user ctor(" << s << ")\n";
        std::memcpy(cstring, s, n); // populate
    }

    rule_of_five(const char* s = "") : rule_of_five(s, std::strlen(s) + 1)
    {
    	cout << "ctor(" << cstring << ")\n";
    }
    ~rule_of_five()
    {
    	cout << "dtor(" << cstring << ")\n";
        delete[] cstring; // deallocate
    }
    rule_of_five(const rule_of_five& other) // copy constructor
        :
        rule_of_five(other.cstring)
    {
    	cout << "copy ctor\n";
    }
    rule_of_five(rule_of_five&& other) noexcept // move constructor
        :
        cstring(std::exchange(other.cstring, nullptr))
    {
    	cout << "move ctor\n";
    }
    rule_of_five& operator=(const rule_of_five& other) // copy assignment
    {
    	cout << "copy assignment\n";

        return *this = rule_of_five(other);
    }
    rule_of_five& operator=(rule_of_five&& other) noexcept // move assignment
    {
    	cout << "move assignment\n";
        std::swap(cstring, other.cstring);
        return *this;
    }
    // alternatively, replace both assignment operators with
    //  rule_of_five& operator=(rule_of_five other) noexcept
    //  {
    //      std::swap(cstring, other.cstring);
    //      return *this;
    //  }
};

std::ostream& operator<<(std::ostream& ostrm, const rule_of_five& lhs)
{
	ostrm << lhs.cstring;
	return ostrm;
}

// https://isocpp.org/wiki/faq/const-correctness#const-overloading
struct overloading
{
	overloading():cvr("cvr"),ref("ref")
	{}
    const rule_of_five& get() const
    {
    	cout << "return cvr\n";
        return cvr;
    }

    rule_of_five& get()
    {
    	cout << "return ref\n";
        return ref;
    }

    rule_of_five cvr;
    rule_of_five ref;
};

constexpr char* cstr1 = "constexpr char*";
constexpr const char* cstr2 = "constexpr const char*";
int main(int argc, const char* [])
{
	cout << "constexpr char* and constexpr const char* std::is_same " << std::boolalpha << std::is_same<decltype(cstr1),decltype(cstr2)>::value << "\n";

    for (int i : f(argc + 1, argc + 2, argc + 3))
        cout << i << " ";
    cout << "\n";

    overloading ar;

    auto invoke_from_ref = [](overloading& lhs) {
    	cout << std::left << std::setw(80) << std::setfill('-')<< "invoke_from_ref" << "\n";
        auto auto_from_ref = lhs.get();
        cout << "auto_from_ref got " << auto_from_ref << "\n";
        const auto& cvr_auto_from_ref = lhs.get();
        cout << "cvr_auto_from_ref got " << cvr_auto_from_ref << "\n";
    };
    invoke_from_ref(ar);
    cout << std::left << std::setw(80) << std::setfill('-')<< "invoke_from_ref done" << "\n";
//    invoke_from_ref-----------------------------------------------------------------
//    return ref
//    user ctor(ref)
//    ctor(ref)
//    copy ctor
//    auto_from_ref got ref
//    return ref
//    cvr_auto_from_ref got ref
//    dtor(ref)
//    invoke_from_ref done------------------------------------------------------------
    auto invoke_from_const_ref = [](const overloading& lhs) {
    	cout << std::left << std::setw(80) << std::setfill('-')<< "invoke_from_const_ref" << "\n";
        auto auto_from_const_obj = lhs.get();
        cout << "auto_from_const_obj got " << auto_from_const_obj << "\n";
        const auto& cvr_auto_from_const_obj = lhs.get();
        cout << "cvr_auto_from_const_obj got " << cvr_auto_from_const_obj << "\n";
    };
    invoke_from_const_ref(ar);
    cout << std::left << std::setw(80) << std::setfill('-')<< "invoke_from_const_ref" << "\n";
//    invoke_from_const_ref-----------------------------------------------------------
//    return cvr
//    user ctor(cvr)
//    ctor(cvr)
//    copy ctor
//    auto_from_const_obj got cvr
//    return cvr
//    cvr_auto_from_const_obj got cvr
//    dtor(cvr)
//    invoke_from_const_ref-----------------------------------------------------------
    return 0;
}
