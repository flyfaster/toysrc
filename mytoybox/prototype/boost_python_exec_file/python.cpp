#include <boost/python.hpp>
#include <iostream>

// compiled as: g++ python.cpp -o p -lpython2.5 -lboost_python -I /usr/include/python2.5
// /usr/include/python3.1
// /usr/lib/libpython3.1.so
// /usr/include/python3.1/Python.h
// /usr/lib/python3.1/config/libpython3.1.a
// /usr/local/lib
// g++ python.cpp -o p -lpython3.1 -lboost_python -I /usr/include/python3.1 -L /usr/local/lib
using namespace boost::python;

int main(int argc, char *argv[])
{
   Py_Initialize();

   try {
      object main_module = import("__main__");
      object main_namespace = main_module.attr("__dict__");

      dict globals;
      globals["a"] = 4;
      object result = eval(str(std::string("a+3")), main_namespace, globals);
      std::cout << extract<int>(result) << std::endl;

   } catch (error_already_set const &) {
      PyErr_Print();
   }
   return 0;
}
