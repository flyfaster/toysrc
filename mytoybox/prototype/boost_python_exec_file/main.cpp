//#include <QtGui/QApplication>
//#include "mainwindow.h"
#include <boost/python.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include <limits.h>
#include <unistd.h>

// compiled as: g++ python.cpp -o p -v -lpython2.5 -lboost_python -I /usr/include/python2.5

using namespace boost::python;
boost::python::object python_import(dict& globals, const std::string& module_name,
                              const std::string& module_path)
{
    // If path is /Users/whatever/blah/foo.py
    dict locals;
    locals["modulename"] = module_name; // foo -> module name
    locals["path"]   = module_path; // /Users/whatever/blah/foo.py
    exec("import imp\n"
         "newmodule = imp.load_module(modulename,open(path),path,('py','U',imp.PY_SOURCE))\n",
         globals,locals);
    return locals["newmodule"];
}

int python_eval()
{
    object main_module = import("__main__");
    object main_namespace = main_module.attr("__dict__");
    dict globals;
    globals["a"] = 4;
    object result = eval(str(std::string("a+3")), main_namespace, globals);
    std::cout << extract<int>(result) << std::endl;
   return 0;
}
void python_exec_file(const char* python_file_path)
{
    std::cout << __FUNCTION__ << " exec_file " << python_file_path << std::endl;
    object module = import("__main__");
    object name_space = module.attr("__dict__");
    exec_file(python_file_path, name_space, name_space);
    object MyFunc = name_space["MyFunc"];
    object result = MyFunc("some_args");
    // result is a dictionary
    std::string val = extract<std::string>(result);
    std::cout << __FUNCTION__ << " call MyFunc return " << val<<std::endl;
}

std::string getexepath()
  {
  char result[ PATH_MAX ];
  size_t count = readlink( "/proc/self/exe", result, PATH_MAX );
  return std::string( result, (count > 0) ? count : 0 );
  }

int main(int argc, char **argv)
{
    std::cout << "usage: " << argv[0] << " <python file path>\n";
    std::cout << __FUNCTION__ << "(" << argc
            << ", " << argv[0] << ")" << std::endl;
    std::cout << getexepath() << std::endl;

    Py_Initialize();    
    try
    {
        if (argc>1)
            python_exec_file(argv[1]);
    }
    catch (error_already_set)
    {
        PyErr_Print();
    }
    Py_Finalize();
    return 0;
}

//int main1(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
//    return a.exec();
//}
