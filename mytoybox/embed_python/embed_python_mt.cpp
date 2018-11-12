#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>
#include <Python.h>

PyThreadState* py_main_thrd_state;

void do_stuff_in_thread(PyInterpreterState* interp)
{
    // acquire the GIL
	PyEval_RestoreThread(py_main_thrd_state); // PyEval_AcquireLock();

    // create a new thread state for the the sub interpreter interp
    PyThreadState* ts = PyThreadState_New(interp);

    // make ts the current thread state
    PyThreadState* old = PyThreadState_Swap(ts);

    // at this point:
    // 1. You have the GIL
    // 2. You have the right thread state - a new thread state (this thread was not created by python) in the context of interp

    // PYTHON WORK HERE
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

    // release ts
    PyThreadState_Swap(old);
    std::cout << "After PyThreadState_Swap();\n";

    // clear and delete ts
    PyThreadState_Clear(ts);
    std::cout << "After PyThreadState_Clear();\n";

    PyThreadState_Delete(ts);
    std::cout << "After PyThreadState_Delete();\n";

    // release the GIL
    PyEval_SaveThread(); // PyEval_ReleaseLock();
    std::cout << "After PyEval_ReleaseLock();\n";
}

int main(int argc, char* argv[])
{
	std::cout << "killall -9 " << argv[0] << "\n"
			<< "kill -9 " << getpid() << "\n";

	Py_Initialize();
	py_main_thrd_state = PyThreadState_Get();

	std::vector<PyThreadState*> py_thrd_states;

	unsigned int thread_count = 1;
	for(unsigned int t=0; t<thread_count; ++t)
		py_thrd_states.emplace_back(Py_NewInterpreter());
//	PyThreadState* ts1 = Py_NewInterpreter();
//	PyThreadState* ts2 = Py_NewInterpreter();


	PyEval_ReleaseLock();

	std::vector<std::thread> thrds;
	for(unsigned int t=0; t<thread_count; ++t)
		thrds.emplace_back(do_stuff_in_thread, py_thrd_states[t]->interp);
//	std::thread thrd(do_stuff_in_thread, ts1->interp);
//	thrd.join();
	std::for_each(thrds.begin(), thrds.end(), std::mem_fn(&std::thread::join));

	PyEval_RestoreThread(py_main_thrd_state);
	std::cout << "After PyEval_RestoreThread();\n";

	Py_Finalize();
	return 0;
}
