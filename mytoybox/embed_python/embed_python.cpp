#include <iostream>
#include <iomanip>
#include <Python.h>
#if PY_MAJOR_VERSION >= 3
#define IS_PY3K
#endif
class CPyInstance
{
public:
	CPyInstance()
	{
		Py_Initialize();
	}

	~CPyInstance()
	{
		Py_Finalize();
	}
};


class CPyObject
{
private:
	PyObject *p{nullptr};
public:
	CPyObject(PyObject* _p) : p(_p)
	{}


	~CPyObject()
	{
		Release();
	}

	PyObject* getObject()
	{
		return p;
	}

	PyObject* setObject(PyObject* _p)
	{
		return (p=_p);
	}

	PyObject* AddRef()
	{
		if(p)
		{
			Py_INCREF(p);
		}
		return p;
	}

	void Release()
	{
		if(p)
		{
			Py_DECREF(p);
		}

		p= NULL;
	}

	PyObject* operator ->()
	{
		return p;
	}

	bool is()
	{
		return p ? true : false;
	}

	operator PyObject*()
	{
		return p;
	}

	PyObject* operator = (PyObject* pp)
	{
		p = pp;
		return p;
	}

	operator bool()
	{
		return p ? true : false;
	}
};

void call_python_func()
{
	PyImport_ImportModule("platform");
	PyObject * main_module(PyImport_AddModule("__main__"));
	const char* python_func_body=R"(
def mul(a, b):
	print("from python " + platform.python_version())
	return a * b
)";
	PyRun_SimpleString(python_func_body);

	PyObject * func(PyObject_GetAttrString(main_module, "mul"));
	PyObject *args(PyTuple_Pack(2, PyFloat_FromDouble(3.0), PyFloat_FromDouble(4.0)));
	PyObject *result = PyObject_CallObject(func, args);

	std::cout << __func__ << " python function return " << PyFloat_AsDouble(result) << "\n";
}

int main()
{
	// brew upgrade python
    CPyInstance pyInstance;

	PyRun_SimpleString("print('Hello World from Embedded Python {}'.format(__import__('platform').python_version()))");

	call_python_func();

	return 0;
}
