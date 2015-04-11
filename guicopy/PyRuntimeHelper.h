#pragma once 

struct PyRuntimeHelper
{
	PyRuntimeHelper()
	{
		Py_Initialize();
        main_module = boost::python::import("__main__");
        if (main_module.ptr()==NULL)
        {
            OutputDebugString(_T("Failed to import __main__"));
            return;
        }
		main_namespace = main_module.attr("__dict__");
	}

	~PyRuntimeHelper()
	{
		Py_Finalize();

	}
    boost::python::object main_module;
	boost::python::object main_namespace;
private:
	PyRuntimeHelper(const PyRuntimeHelper &);
	PyRuntimeHelper & operator=(const PyRuntimeHelper &);

};