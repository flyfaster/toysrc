#include <wx/wx.h>
#include <sstream>
#include <wx/clipbrd.h>
#include <wx/gbsizer.h>
#include <iostream>
#include <iomanip>
#include <thread>
#include <fstream>
#include <codecvt>
#include <Python.h>

#define MEXPAND(m) m
#define MAKE_WIDE_IMPL(x) L ## x
#define MAKE_WIDE(x) MAKE_WIDE_IMPL(x)

class CPyInstance
{
public:
    CPyInstance()
    {
        std::cout << Py_GetVersion() << "\n";
        Py_Initialize();
        if (!Py_IsInitialized())
            std::cerr << "Py_InitializeEx failed\n";

        if (!PyEval_ThreadsInitialized())
            PyEval_InitThreads();

        std::wstringstream strm;
        strm << L"Py_GetPath:" << Py_GetPath() << std::endl;
        strm << L"Py_GetPrefix:" << Py_GetPrefix() << std::endl;
        strm << L"Py_GetExecPrefix:" << Py_GetExecPrefix() << std::endl;
        strm << L"Py_GetProgramFullPath:" << Py_GetProgramFullPath() << std::endl;
        strm << L"Py_GetProgramName:" << Py_GetProgramName() << std::endl;

        if (Py_GetPythonHome())
            strm << L"Py_GetPythonHome:" << Py_GetPythonHome() << std::endl;

        /*
         * Release the global interpreter lock (if it has been created and thread
         * support is enabled) and reset the thread state to NULL, returning the
         * previous thread state (which is not NULL). If the lock has been created,
         * the current thread must have acquired it.
         */
        SaveThread();
        interpreterState = state->interp;
        RestoreThread();
        static std::wstring ws_func(
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(
                __func__));
        strm << ws_func << L" done\n";
        std::wcout << strm.str();
    }

    ~CPyInstance()
    {
        //		PyEval_RestoreThread(state);
        Py_Finalize();
        //		Py_Exit(0);
    }

    void SaveThread()
    {
        ++lock_cnt;
        state = PyEval_SaveThread();
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

// https://github.com/mloskot/workshop/blob/master/python/emb/emb.cpp
namespace emb {

using stdout_write_type = std::function<void(std::string)> ;

struct Stdout
{
    PyObject_HEAD stdout_write_type write;
};

PyObject* Stdout_write(PyObject* self, PyObject* args)
{
    std::size_t written(0);
    auto selfimpl = reinterpret_cast<Stdout*>(self);
    if (selfimpl->write)
    {
        char* data = nullptr;
        if (!PyArg_ParseTuple(args, "s", &data))
            return nullptr;

        std::string str(data);
        selfimpl->write(str);
        written = str.size();
    }
    return PyLong_FromSize_t(written);
}

PyObject* Stdout_flush(PyObject* self, PyObject* args)
{
    // no-op
    return Py_BuildValue("");
}

PyMethodDef Stdout_methods[] = {
    {"write", Stdout_write, METH_VARARGS, "sys.stdout.write"},
    {"flush", Stdout_flush, METH_VARARGS, "sys.stdout.write"},
    {0, 0, 0, 0} // sentinel
};

PyTypeObject StdoutType = {
    PyVarObject_HEAD_INIT(0, 0) "emb.StdoutType", /* tp_name */
    sizeof(Stdout),                               /* tp_basicsize */
    0,                                            /* tp_itemsize */
    0,                                            /* tp_dealloc */
    0,                                            /* tp_print */
    0,                                            /* tp_getattr */
    0,                                            /* tp_setattr */
    0,                                            /* tp_reserved */
    0,                                            /* tp_repr */
    0,                                            /* tp_as_number */
    0,                                            /* tp_as_sequence */
    0,                                            /* tp_as_mapping */
    0,                                            /* tp_hash  */
    0,                                            /* tp_call */
    0,                                            /* tp_str */
    0,                                            /* tp_getattro */
    0,                                            /* tp_setattro */
    0,                                            /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                           /* tp_flags */
    "emb.Stdout objects",                         /* tp_doc */
    0,                                            /* tp_traverse */
    0,                                            /* tp_clear */
    0,                                            /* tp_richcompare */
    0,                                            /* tp_weaklistoffset */
    0,                                            /* tp_iter */
    0,                                            /* tp_iternext */
    Stdout_methods,                               /* tp_methods */
    0,                                            /* tp_members */
    0,                                            /* tp_getset */
    0,                                            /* tp_base */
    0,                                            /* tp_dict */
    0,                                            /* tp_descr_get */
    0,                                            /* tp_descr_set */
    0,                                            /* tp_dictoffset */
    0,                                            /* tp_init */
    0,                                            /* tp_alloc */
    0,                                            /* tp_new */
};

PyModuleDef embmodule = {
    PyModuleDef_HEAD_INIT, "emb", 0, -1, 0,
};

// Internal state
PyObject* g_stdout;
PyObject* g_stdout_saved;

PyMODINIT_FUNC PyInit_emb(void)
{
    g_stdout = nullptr;
    g_stdout_saved = nullptr;

    StdoutType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&StdoutType) < 0)
        return 0;

    PyObject* m = PyModule_Create(&embmodule);
    if (m)
    {
        Py_INCREF(&StdoutType);
        PyModule_AddObject(m, "Stdout", reinterpret_cast<PyObject*>(&StdoutType));
    }
    return m;
}

void set_stdout(stdout_write_type write)
{
    if (!g_stdout)
    {
        g_stdout_saved = PySys_GetObject("stdout"); // borrowed
        g_stdout = StdoutType.tp_new(&StdoutType, 0, 0);
    }

    auto impl = reinterpret_cast<Stdout*>(g_stdout);
    impl->write = write;
    PySys_SetObject("stdout", g_stdout);
}

void reset_stdout()
{
    if (g_stdout_saved)
        PySys_SetObject("stdout", g_stdout_saved);

    Py_XDECREF(g_stdout);
    g_stdout = 0;
}

} // namespace emb

class MiniWxApp : public wxApp
{
public:
    bool OnInit() override;
};

IMPLEMENT_APP (MiniWxApp)

class MyFrame: public wxFrame {
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	void OnQuit(wxCommandEvent& event);
	void OnRun(wxCommandEvent&);
	void OnCopy(wxCommandEvent&);
	void OnClear(wxCommandEvent&);
private:
	wxTextCtrl *m_script;
	wxTextCtrl *m_result;
	wxButton *m_run;
	wxButton *m_copy;
	wxButton *m_reset;
	void RunFile();
	void RunPythonRedirectOutput();
};

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size) :
    wxFrame(nullptr, -1, title, pos, size)
{
    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(wxID_EXIT, wxT("E&xit"));
    menuBar->Append(menuFile, wxT("&File"));
    SetMenuBar(menuBar);

    m_run = new wxButton(this, wxID_ANY, wxT("Run"), wxDefaultPosition, wxDefaultSize, 0);
    m_copy =
        new wxButton(this, wxID_ANY, wxT("Copy"), wxDefaultPosition, wxDefaultSize, 0);
    m_reset =
        new wxButton(this, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0);
    m_run->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MyFrame::OnRun),
                   NULL, this);
    m_copy->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MyFrame::OnCopy),
                    NULL, this);
    m_reset->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                     wxCommandEventHandler(MyFrame::OnClear), NULL, this);

    m_script = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                              wxDefaultSize, wxTE_MULTILINE | wxHSCROLL);
    m_result = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                              wxDefaultSize, wxTE_MULTILINE | wxHSCROLL);
    m_result->SetWindowStyle((m_result->GetWindowStyle() & ~wxTE_DONTWRAP) |
                             wxTE_BESTWRAP);

    auto m_fgsizer = new wxGridBagSizer;
    int row = 0;
    m_fgsizer->Add(m_run, wxGBPosition(row, 0));
    m_fgsizer->Add(m_script, wxGBPosition(row, 1), wxGBSpan(2, 1), wxGROW);
    row++;
    m_fgsizer->AddGrowableRow(row);
    row++;
    m_fgsizer->Add(m_copy, wxGBPosition(row, 0));
    m_fgsizer->Add(m_result, wxGBPosition(row, 1), wxGBSpan(3, 1), wxGROW);
    row++;
    m_fgsizer->Add(m_reset, wxGBPosition(row, 0));
    m_fgsizer->AddGrowableCol(1);
    m_fgsizer->AddGrowableRow(row);
    this->SetSizer(m_fgsizer);

    Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED,
            (wxObjectEventFunction) &MyFrame::OnQuit);

    wxString default_script = R"(import os
import platform
print('test ' + os.name + " " + platform.python_version())
)";
	m_script->SetValue("import os\nprint(' test ' + os.name)");
	m_script->SetValue(default_script);
#ifdef __WXMAC__
	m_script->OSXDisableAllSmartSubstitutions();
	//	https://github.com/wxWidgets/wxWidgets/blob/master/src/osx/textctrl_osx.cpp
	//	defaults write -g NSAutomaticQuoteSubstitutionEnabled 0
	//	UIKeyboardType	//	m_script->EnableAutomaticQuoteSubstitution(false);
#endif

#ifdef _MSC_VER
    SetIcon(wxICON(IDR_MYICON));
#endif
}

bool MiniWxApp::OnInit()
{
    wxString title;
    title.Printf(wxT("Python pid:%lu "), wxGetProcessId());
    MyFrame* frame = new MyFrame(title, wxDefaultPosition, wxDefaultSize);
    frame->Show(true);
    SetTopWindow(frame);
    return true;
}

void MyFrame::OnQuit(wxCommandEvent& event)
{
    Close(true);
}

void MyFrame::RunFile()
{
    CPyInstance pyInstance;
    wxString filename = "wxembed_python_tmp.py";
    std::ofstream tmpfile(filename.mb_str());
    tmpfile << m_script->GetValue().ToStdString();
    tmpfile.close();
    FILE* fp = _Py_fopen(filename.mb_str(), "r");
    PyRun_SimpleFile(fp, filename.mb_str());
}

void MyFrame::OnRun(wxCommandEvent& event)
{
	//alternative : https://stackoverflow.com/questions/4307187/how-to-catch-python-stdout-in-c-code/4307737#4307737
	return RunPythonRedirectOutput();

    std::string stdOutErr =
R"(
import sys
class CatchOutErr:
    def __init__(self):
        self.value = ''
    def write(self, txt):
        self.value += txt
catchOutErr = CatchOutErr()
sys.stdout = catchOutErr
sys.stderr = catchOutErr
)"; //this is python code to redirect stdouts/stderr

    CPyInstance pyInstance;
    PyObject *main_module = PyImport_AddModule("__main__"); //create main module
    PyRun_SimpleString(stdOutErr.c_str()); //invoke code to redirect

    PyRun_SimpleString(m_script->GetValue().ToStdString().c_str());

    PyObject *catcher = PyObject_GetAttrString(main_module, "catchOutErr"); //get our catchOutErr created above
    PyErr_Print(); //make python print any errors
    if (!catcher)
    {
    	m_result->SetValue("catchOutErr return null");
    	return;
    }

    PyObject* output = PyObject_GetAttrString(
        catcher, "value"); // get the stdout and stderr from our catchOutErr object
    if (output)
    {
    	std::cout << Py_TYPE(output) << std::endl;
    	auto evalVal = output;
        if (PyUnicode_Check(evalVal))
        {
            PyObject* temp_bytes =
                PyUnicode_AsEncodedString(evalVal, "UTF-8", "strict"); // Owned reference
            if (temp_bytes)
            {
                auto my_result = PyBytes_AS_STRING(temp_bytes); // Borrowed pointer
                m_result->SetValue(my_result);
                Py_DECREF(temp_bytes);
            }
            else
            {
            	m_result->SetValue("PyUnicode_AsEncodedString error");
            }
        }
        else
        {
        	m_result->SetValue("PyUnicode_Check error");
        }
    }
    else
        m_result->SetValue("PyObject_GetAttrString return null");
    return;

    auto evalDict = PyModule_GetDict(main_module);
    if (!main_module)
    {
    	m_result->SetValue("PyModule_GetDict is null");
    	return;
    }

    auto evalVal = PyDict_GetItemString(evalDict, "result");

    if (!evalVal)
    {
    	m_result->SetValue("PyDict_GetItemString is null");
        PyErr_Print();
        return;
    }
    else
    {
    	std::cout << Py_TYPE(evalVal);
        if (PyUnicode_Check(evalVal))
        {
            PyObject* temp_bytes =
                PyUnicode_AsEncodedString(evalVal, "UTF-8", "strict"); // Owned reference
            if (temp_bytes)
            {
                auto my_result = PyBytes_AS_STRING(temp_bytes); // Borrowed pointer
                m_result->SetValue(my_result);
                Py_DECREF(temp_bytes);
            }
            else
            {
            	m_result->SetValue("PyUnicode_AsEncodedString error");
            }
        }
        else
        {
        	m_result->SetValue("PyUnicode_Check error");
        }
    }
}

void MyFrame::OnClear(wxCommandEvent& event)
{
	m_result->Clear();
}

void MyFrame::OnCopy(wxCommandEvent& event)
{
	if (wxTheClipboard->Open())
	{
		wxTheClipboard->SetData(new wxTextDataObject(m_result->GetValue()));
		wxTheClipboard->Close();
	}
}

void MyFrame::RunPythonRedirectOutput()
{
    PyImport_AppendInittab("emb", emb::PyInit_emb);
    CPyInstance pyInstance;
    PyImport_ImportModule("emb");
    std::string buffer;
    {
        // switch sys.stdout to custom handler
        emb::stdout_write_type write = [&buffer] (std::string s) { buffer += s; };
        emb::set_stdout(write);
        PyRun_SimpleString(m_script->GetValue().ToStdString().c_str());
        emb::reset_stdout();
    }
    m_result->SetValue(buffer);
}
