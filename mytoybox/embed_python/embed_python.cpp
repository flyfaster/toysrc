#include <iostream>
#include <iomanip>
#include <thread>
#include <Python.h>
#if PY_MAJOR_VERSION >= 3
#define IS_PY3K
#endif

extern int Py_NoSiteFlag;

class CPyInstance
{
public:
	CPyInstance()
	{
//		Py_SetPythonHome(L"/usr/local/Cellar/python/3.7.0/Frameworks/Python.framework/Versions/3.7");
//		Py_SetProgramName(L"/usr/local/bin/python3");
		std::cout << Py_GetVersion() << "\n";
//	    Py_NoSiteFlag = 1;
//	    Py_InitializeEx(0);
		Py_Initialize();
	    if (!Py_IsInitialized())
	    	std::cerr << "Py_InitializeEx failed\n";

	    if (!PyEval_ThreadsInitialized())
	    	PyEval_InitThreads();

	    std::wcout << L"Py_GetPath:" << Py_GetPath() << std::endl;
	    std::wcout << L"Py_GetPrefix:" << Py_GetPrefix() << std::endl;
	    std::wcout << L"Py_GetExecPrefix:" << Py_GetExecPrefix() << std::endl;
	    std::wcout << L"Py_GetProgramFullPath:" << Py_GetProgramFullPath() << std::endl;

	    std::wcout << L"Py_GetProgramName:" << Py_GetProgramName() << std::endl;

	    if (Py_GetPythonHome())
	    	std::wcout << L"Py_GetPythonHome:" << Py_GetPythonHome() << std::endl;

//	    state = PyThreadState_Get();
            /*
             * Release the global interpreter lock (if it has been created and thread
             * support is enabled) and reset the thread state to NULL, returning the
             * previous thread state (which is not NULL). If the lock has been created,
             * the current thread must have acquired it.
             */
		state = PyEval_SaveThread();
		interpreterState = state->interp;
		std::cout << __func__ << " done\n";
	}

	~CPyInstance()
	{
		PyEval_RestoreThread(state);
		Py_Finalize();
	}

	void SaveThread()
	{
		++lock_cnt;
		PyEval_SaveThread();
	}

	void RestoreThread()
	{
		if (lock_cnt)
		{
			--lock_cnt;
			PyEval_RestoreThread(state);
		}
	}
	PyThreadState* state{nullptr};
	PyInterpreterState* interpreterState{nullptr};
	size_t lock_cnt = 0;
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
	PyObject * main_module=(PyImport_AddModule("__main__"));
	const char* python_func_body=R"(
def mul(a, b):
	print("from python " + __import__('platform').python_version())
	print(__import__('sys').path)
	print(os.getenv('PYTHONHOME', 'missing PYTHONHOME'))
	print(os.getenv('PYTHONPATH', 'missing PYTHONPATH'))
	return a * b
)";
	PyRun_SimpleString("import os\n");
	PyRun_SimpleString(python_func_body);

	PyObject * func=(PyObject_GetAttrString(main_module, "mul"));
	PyObject *args=(PyTuple_Pack(2, PyFloat_FromDouble(3.0), PyFloat_FromDouble(4.0)));
	PyObject *result = PyObject_CallObject(func, args);

	std::cout << __func__ << " python function return " << PyFloat_AsDouble(result) << "\n";
}

void thread_proc(PyInterpreterState* interpreterState)
{
	// https://stackoverflow.com/questions/15470367/pyeval-initthreads-in-python-3-how-when-to-call-it-the-saga-continues-ad-naus/42667657#42667657
//	https://gist.github.com/sterin/61561c3139dd49da1f43
	//	PyThreadState* state = PyThreadState_Get();
//	PyInterpreterState* interpreterState = state->interp;
//	PyEval_AcquireLock(); // acquire lock on the GIL
	std::cout << "thread_proc start\n";

	PyThreadState* pThreadState = Py_NewInterpreter();
	if (!pThreadState)
	{
		std::cerr << "Py_NewInterpreter return null\n";
		return;
	}
	std::cout << "Py_NewInterpreter return\n";

	auto thrd_state = PyThreadState_New(interpreterState);
	std::cout << "PyThreadState_New return\n";

	PyEval_RestoreThread(thrd_state);
	std::cout << "PyEval_RestoreThread return\n";

	// Perform some Python actions here
	PyRun_SimpleString("import sys\n");
	PyRun_SimpleString("import platform\n");
//	PyImport_ImportModule("platform");
	PyObject * main_module(PyImport_AddModule("__main__"));
	const char* python_func_body=R"(
def mul(a, b):
	print("from python " + __import__('platform').python_version())
	print(__import__('sys').path)
	return a * b
)";
	PyRun_SimpleString(python_func_body);

	PyObject * func(PyObject_GetAttrString(main_module, "mul"));
	PyObject *args(PyTuple_Pack(2, PyFloat_FromDouble(3.0), PyFloat_FromDouble(4.0)));
	PyObject *result = PyObject_CallObject(func, args);

	std::cout << __func__ << " python function return " << PyFloat_AsDouble(result) << "\n";

	// Release Python GIL
	PyEval_SaveThread();
	PyThreadState_Delete(thrd_state);
}

void thread_proc1(CPyInstance& pyInstance)
{
	// https://stackoverflow.com/questions/15470367/pyeval-initthreads-in-python-3-how-when-to-call-it-the-saga-continues-ad-naus/42667657#42667657
//	https://gist.github.com/sterin/61561c3139dd49da1f43
	//	PyThreadState* state = PyThreadState_Get();
//	PyInterpreterState* interpreterState = state->interp;
//	PyEval_AcquireLock(); // acquire lock on the GIL
	std::cout << "thread_proc start\n";
	PyEval_RestoreThread(pyInstance.state);

	PyThreadState* pThreadState = Py_NewInterpreter();
	if (!pThreadState)
	{
		std::cerr << "Py_NewInterpreter return null\n";
		return;
	}
	std::cout << "Py_NewInterpreter return\n";

	auto thrd_state = PyThreadState_New(pyInstance.interpreterState);
	std::cout << "PyThreadState_New return\n";

	PyEval_SaveThread();
	std::cout << "PyEval_SaveThread return\n";

	PyEval_RestoreThread(thrd_state);
	std::cout << "PyEval_RestoreThread return\n";

	// Perform some Python actions here
	PyRun_SimpleString("import sys\n");
	PyRun_SimpleString("import platform\n");
//	PyImport_ImportModule("platform");
	PyObject * main_module(PyImport_AddModule("__main__"));
	const char* python_func_body=R"(
def mul(a, b):
	print("from python " + __import__('platform').python_version())
	print(__import__('sys').path)
	return a * b
)";
	PyRun_SimpleString(python_func_body);

	PyObject * func(PyObject_GetAttrString(main_module, "mul"));
	PyObject *args(PyTuple_Pack(2, PyFloat_FromDouble(3.0), PyFloat_FromDouble(4.0)));
	PyObject *result = PyObject_CallObject(func, args);

	std::cout << __func__ << " python function return " << PyFloat_AsDouble(result) << "\n";

	// Release Python GIL
//	PyEval_SaveThread();
	PyThreadState_Swap(pyInstance.state); // (pyInstance.state); //pyInstance.RestoreThread();
	PyThreadState_Clear(thrd_state);
	PyThreadState_Delete(thrd_state);
	pyInstance.SaveThread();
}

int main(int argc, char* argv[])
{
	std::cout << "killall -9 " << argv[0] << "\n"
			<< "kill -9 " << getpid() << "\n";
	// brew uninstall --ignore-dependencies python@2
	// brew upgrade python
//	setenv("PYTHONHOME", "/usr/local/Cellar/python/3.7.0/Frameworks/Python.framework/Versions/3.7", 1);
//	setenv("PYTHONPATH", "/usr/local/lib/python3.7/site-packages", 1);
	wchar_t progname[1024];
	mbstowcs(progname, argv[0], strlen(argv[0]) + 1);
	Py_SetProgramName(progname);
    CPyInstance pyInstance;

//	PyRun_SimpleString("print('Hello World from Embedded Python {}'.format(__import__('platform').python_version()))");
//	call_python_func();
//    thread_proc();
//	std::thread thrd(thread_proc, pyInstance.interpreterState);
	std::thread thrd(thread_proc1, std::ref(pyInstance));
	thrd.join();
	return 0;
}
