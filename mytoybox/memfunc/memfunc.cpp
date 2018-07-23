#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include "FastDelegate.h"

struct mystruct
{
    int data=0;
    mystruct(int val) : data(val)
    {
    }
    mystruct(std::string val) : data(val.size())
    {
    }
    bool operator==(mystruct const& rhs) const
    {
        std::cout << "member function " << __func__ << "\n";
        return data == rhs.data;
    }
};

bool operator==(mystruct const& lhs, mystruct const& rhs)
{
    std::cout << "external function " << __func__ << "\n";
    return lhs.operator==(rhs);
}

struct base
{
    virtual void set_current_position(int pos)
    {
        ++base_set_current_position;
        std::cout << "base::" << __func__ << " pos " << pos << std::endl;
    }
    virtual bool has_instance_at_current_position()
    {
        ++base_has_instance_at_current_position;
        std::cout << "base::" << __func__ << std::endl;
        return true;
    }
    virtual ~base()
    {
        if (base_set_current_position + base_has_instance_at_current_position)
            std::cout << "base_set_current_position = " << base_set_current_position
                      << ", base_has_instance_at_current_position = "
                      << base_has_instance_at_current_position << std::endl;
    }
    size_t base_set_current_position = 0;
    size_t base_has_instance_at_current_position = 0;
};

struct A : public base
{
    void test()
    {
        std::cout << "A::" << __func__ << " data " << m_data << std::endl;
    }

    virtual void set_current_position(int pos)
    {
        ++A_set_current_position;
        std::cout << "A::" << __func__ << " pos " << pos << std::endl;
    }
    virtual bool has_instance_at_current_position()
    {
        ++A_has_instance_at_current_position;
        std::cout << "A::" << __func__ << std::endl;
        return true;
    }
    ~A()
    {
        if (A_set_current_position + A_has_instance_at_current_position)
            std::cout << "A_set_current_position = " << A_set_current_position
                      << ", A_has_instance_at_current_position = "
                      << A_has_instance_at_current_position << std::endl;
    }
private:
    size_t A_set_current_position = 0;
    size_t A_has_instance_at_current_position = 0;
    int m_data = __LINE__;
};

struct B : public base
{
    virtual void set_current_position(int pos)
    {
        ++B_set_current_position;
        m_bdata = pos;
    }
    virtual bool has_instance_at_current_position()
    {
        ++B_has_instance_at_current_position;
        return m_bdata % __LINE__;
    }

    void test()
    {
        std::cout << "B " << __func__ << " data " << m_bdata << std::endl;
    }
    ~B()
    {
        if (B_set_current_position + B_has_instance_at_current_position)
            std::cout << "B_set_current_position = " << B_set_current_position
                      << ", B_has_instance_at_current_position = "
                      << B_has_instance_at_current_position << std::endl;
    }
private:
    int m_bdata = __LINE__;
    size_t B_set_current_position = 0;
    size_t B_has_instance_at_current_position = 0;
};

struct C : public B
{
    virtual void set_current_position(int pos)
    {
        ++C_set_current_position;
        m_cdata += pos;
    }
    virtual bool has_instance_at_current_position()
    {
        ++C_has_instance_at_current_position;
        return m_cdata % __LINE__;
    }
    ~C()
    {
        if (C_set_current_position + C_has_instance_at_current_position)
            std::cout << "C_set_current_position = " << C_set_current_position
                      << ", C_has_instance_at_current_position = "
                      << C_has_instance_at_current_position << std::endl;
    }

    void test()
    {
        std::cout << "C::" << __func__ << " data " << m_cdata << std::endl;
    }
private:
    int m_cdata = __LINE__;
    size_t C_set_current_position = 0;
    size_t C_has_instance_at_current_position = 0;
};

// static void (A::*gptr)() = &A::test; // Explicitly initialized.
inline void call_memptr(A* ptr, void (A::*gptr)())
{
    (ptr->*gptr)();
}

inline bool call_memptr(A* ptr, bool (A::*bool_f_void)())
{
    return (ptr->*bool_f_void)();
}

inline bool call_bool_f_void(base* ptr)
{
    return ptr->has_instance_at_current_position();
}

typedef void (A::*FxPtrA)(void);
typedef void (B::*FxPtrB)(void);
typedef bool (A::*FxPtrA_bool_f_void)();
typedef bool (B::*FxPtrB_bool_f_void)();
typedef bool (C::*FxPtrC_bool_f_void)();

void test_has_instance_at_current_position()
{
    size_t call_cnt = 1024 * 1024 * 64;
    std::cout << __func__ << " start calling " << call_cnt << " times\n";
    A a_inst;
    B b_inst;
    C c_inst;
    union
    {
        FxPtrA_bool_f_void pfa;
        FxPtrB_bool_f_void pfb;
        FxPtrC_bool_f_void pfc;
        std::ptrdiff_t rf;
    };
    pfa = &A::has_instance_at_current_position;
    call_memptr(&a_inst, &A::test);
    pfb = &B::has_instance_at_current_position;
    call_memptr((A*) &b_inst, pfa);
    pfc = &C::has_instance_at_current_position;

    auto t1 = std::chrono::high_resolution_clock::now();
    auto t2 = std::chrono::high_resolution_clock::now();
    auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < call_cnt; ++i)
        c_inst.has_instance_at_current_position();
    t2 = std::chrono::high_resolution_clock::now();
    int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << "direct function call used " << int_ms.count() << " milliseconds\n";

    t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < call_cnt; ++i)
        (c_inst.*pfc)();
    t2 = std::chrono::high_resolution_clock::now();
    int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << "member function pointer call used " << int_ms.count()
              << " milliseconds\n";

    t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < call_cnt; ++i)
    	call_bool_f_void(&c_inst);
    t2 = std::chrono::high_resolution_clock::now();
    int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << "virtual function call used " << int_ms.count() << " milliseconds\n";

    t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < call_cnt; ++i)
        call_memptr((A*) &c_inst, pfa);
    t2 = std::chrono::high_resolution_clock::now();
    int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << "call_memptr used " << int_ms.count() << " milliseconds\n";

    using BoolMyDelegate = fastdelegate::FastDelegate0<bool> ;
    BoolMyDelegate newdeleg = fastdelegate::MakeDelegate(&b_inst, &B::has_instance_at_current_position);

    t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < call_cnt; ++i)
    	newdeleg();
    t2 = std::chrono::high_resolution_clock::now();
    int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << "fastdelegate used " << int_ms.count() << " milliseconds\n";
}

int main()
{
    A a_inst;
    B b_inst;
    C c_inst;
    union
    {
        FxPtrA pfa;
        FxPtrB pfb;
        std::ptrdiff_t rf;
    };

    call_memptr(&a_inst, &A::test);
    call_memptr((A*) &b_inst, (void (A::*)()) & B::test);
    pfa = &A::test;
    call_memptr(&a_inst, pfa);
    pfb = &B::test;
    call_memptr((A*) &b_inst, pfa);
    std::cout << std::boolalpha << (4 == std::string("1234")) << std::endl;

    test_has_instance_at_current_position();
}

// https://www.codeproject.com/Articles/7150/Member-Function-Pointers-and-the-Fastest-Possible
