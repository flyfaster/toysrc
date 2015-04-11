#include "guicopy_include.h"
#include "mydlg.h"
#include "mainapp.h"
#include "PythonPage.h"
#include "PyRuntimeHelper.h"
#include "dbg_buf.h"
#include "py_boost_function.hpp"

BEGIN_EVENT_TABLE(PythonPage, wxPanel)
EVT_BUTTON(wxID_ANY, PythonPage::OnButton)
END_EVENT_TABLE()

template<>
struct Loki::ImplOf<PythonPage>
{
	wxTextCtrl* m_python_code;
	wxTextCtrl* m_python_output;
	void RedirectPythonStdIo( boost::python::object main_namespace );
	void OnOK();
	void OnCancel();

};

void module_greeter_f(std::string const& origin)
{
	std::cout << "Hello world, by " << origin << std::endl;
	wxLogVerbose( to_tchar_string(__FUNCTION__).c_str());
}



BOOST_PYTHON_MODULE( foo ) 
{
// http://www.boost.org/doc/libs/1_44_0/libs/python/doc/tutorial/doc/html/index.html
	using namespace boost::python;
    def("module_greeter", module_greeter_f);
	//boost::function< void( std::string const& ) > module_greeter( 
	//	module_greeter_f
	//	) ;

	//def_function< void(std::string const&) >(
	//	"greeter_function_t", 
	//	"A greeting function" 
	//	);
		//scope().attr("module_greeter") = module_greeter;
}

PythonPage::PythonPage(wxWindow* parent):wxPanel(parent, wxID_ANY)
{
	d->m_python_code = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition,
		wxDefaultSize, wxTE_MULTILINE);
	d->m_python_output = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition,
		wxDefaultSize, wxTE_MULTILINE);

	wxFlexGridSizer* sizer = new wxFlexGridSizer(2, 1, 0, 0);
	sizer->AddGrowableCol(0);
	sizer->AddGrowableRow(0);
	sizer->AddGrowableCol(1);
	sizer->Add(d->m_python_code,  1, wxTOP|wxRIGHT|wxLEFT|wxEXPAND, 0);
	sizer->Add(d->m_python_output,  1, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 0);

	this->SetSizer(sizer);
	sizer->SetSizeHints(this);
	sizer->Fit(this);
	d->OnCancel();
}

PythonPage::~PythonPage(void)
{

}

class PythonStdIoRedirect {
public:
	typedef boost::circular_buffer<tchar_string> ContainerType;
	void Write( tchar_string const& str ) {
		if (m_outputs.capacity()<100)
			m_outputs.resize(100);
		m_outputs.push_back(str);
	}
	static tchar_string GetOutput()
	{
		tchar_string ret;
		tstringstream ss;
		for(ContainerType::const_iterator it=m_outputs.begin();
			it!=m_outputs.end();
			it++)
		{
			ss << *it;
		}
		m_outputs.clear();
		ret =  ss.str();
		return ret;
	}
private:
	static ContainerType m_outputs;
};
PythonStdIoRedirect::ContainerType PythonStdIoRedirect::m_outputs;

void Loki::ImplOf<PythonPage>::OnOK()
{
	//PyImport_AppendInittab("foo", initfoo);

	PyRuntimeHelper python_runtime;
	using namespace boost::python;
	object main_namespace = python_runtime.main_namespace;
	try {
		//PyRun_SimpleString("import cStringIO");
		//PyRun_SimpleString("import sys");
		//PyRun_SimpleString("sys.stderr = cStringIO.StringIO()");

		main_namespace["PathUtilApp"] = class_<PathUtilApp, boost::noncopyable>("PathUtilApp", no_init)
			.def("Close", &PathUtilApp::Close)
			.def("Dump", &PathUtilApp::Dump)
			.staticmethod("Dump") 
			.def("GetDirSize", &PathUtilApp::GetDirSize)
			.staticmethod("GetDirSize")
			.add_property("ModulePath", &PathUtilApp::GetModulePath)
			.def_readonly("ModuleDirectory", &PathUtilApp::ModuleDirectory)
			.def_readonly("ModuleName", &PathUtilApp::ModuleName)
			.def("GetMemoryString", &PathUtilApp::GetMemoryString)
			.def("UpdateMemoryString", &PathUtilApp::UpdateMemoryString)
			;
		main_namespace["PythonPage"] = class_<PythonPage, boost::noncopyable>("PythonPage", no_init)
			.def("WriteString", &PythonPage::WriteString)
			.staticmethod("WriteString") 
			.def("write", &PythonPage::write)
			;
		main_namespace["PythonStdIoRedirect"] = class_<PythonStdIoRedirect>("PythonStdIoRedirect", init<>())
			.def("write", &PythonStdIoRedirect::Write);
		PythonStdIoRedirect python_stdio_redirector;
		using namespace boost::python::api;
		object_attribute pystdout = boost::python::import("sys").attr("stdout");
		object_attribute pystderr = boost::python::import("sys").attr("stderr");

		boost::python::import("sys").attr("stderr") = python_stdio_redirector;
		boost::python::import("sys").attr("stdout") = python_stdio_redirector;

		boost::python::handle<>  rxa(
			PyRun_String("class OutputCatcher:\n"
			"\tdef __init__(self):\n"
			"\t\tself.data=''\n"
			"\tdef write(self, stuff):\n"
			"\t\tself.data = self.data + stuff\n\n"
			"import sys\n"
			"_catcher = OutputCatcher()\n"
			//"sys.stdout = _catcher\n"
			//"sys.stderr = _catcher\n"
			,Py_file_input, main_namespace.ptr(), main_namespace.ptr()));

		main_namespace["MainApp"] = ptr(&wxGetApp());
		main_namespace["PythonPage"] = ptr(SingleThreadsCol::Instance().m_python_page);
		handle<> ignored(( PyRun_String( 
			const_cast<char*>((const char*)(wxConvCurrent->cWX2MB(/*d->*/m_python_code->GetValue()))),
			Py_file_input,
			main_namespace.ptr(),
			main_namespace.ptr() ) ));
		boost::python::import("sys").attr("stderr") = pystderr;
		boost::python::import("sys").attr("stdout") = pystdout;
		RedirectPythonStdIo(main_namespace);
	} 
	catch( error_already_set ) 
	{
		PyErr_Print();
		// get the string data out of this object.
		RedirectPythonStdIo(main_namespace);
	}

}

void Loki::ImplOf<PythonPage>::OnCancel()
{
	/*d->*/m_python_code->SetValue(_T("print 'Embedded python script running in this software.'\n")
		_T("PathUtilApp.Dump('test method call')\nprint 'size is ', str(PathUtilApp.GetDirSize('c:\\\\temp'))\n")
		//_T("PythonPage.write('test method call of PythonPage.write')\n")
		//_T("MainApp.Close()\n")
		_T("print MainApp.ModuleName\n")
		_T("print MainApp.GetMemoryString(MainApp.ModuleName)\n")
		);
    m_python_code->SetValue(_T("print 'Embedded python script running in this software.'\n"));
	/*d->*/m_python_output->SetValue(
		_T("Exposed classes:\n")
		_T("class PathUtilApp\n")
		_T("\t[method] Dump\n")
		_T("\t[method] Close\n")
		_T("\t[method] GetMemoryString\n")
		_T("\t[method] UpdateMemoryString\n")
		_T("\t[property] ModuleName\n")
		_T("\t[property] ModulePath\n")
		_T("\t[property] ModuleDirectory\n")
		_T("class PythonPage\n")
		_T("\t[method] WriteString\n")
		_T("\t[method] write\n")
		_T("Exposed objects:\n")
		_T("\tPathUtilApp MainApp\n")
		_T("\tPythonPage PythonPage\n")
		);

}

PyObject *pystdoutf = NULL;

void PythonPage::WriteString( const char* str )
{
	if (pystdoutf==NULL)
		pystdoutf = PySys_GetObject("stdout");
	if (pystdoutf)
		PyFile_WriteString(str, pystdoutf);
	wxLogMessage(to_tchar_string(str).c_str());
}

void PythonPage::write( tchar_string const& str )
{
	d->m_python_output->SetValue( str);
}

void PythonPage::OnButton( wxCommandEvent& event )
{
	wxButton* active_button = dynamic_cast<wxButton*>(event.GetEventObject());
    try
    {
        if (active_button)
        {
            if (active_button->GetLabelText()==wxT("OK"))
            {
                d->OnOK();
            }
            if (active_button->GetLabelText()==wxT("Cancel"))
            {
                d->OnCancel();
            }
        }
    }
    catch (const std::runtime_error& e)
    {
        OutputDebugStringA(e.what());
    }
    catch (const std::exception& e)
    {
        OutputDebugStringA(e.what());
    }

}
void Loki::ImplOf<PythonPage>::RedirectPythonStdIo( boost::python::object main_namespace )
{
    tchar_string extractedError=wxT("cannot print exception");
	boost::python::object catcher(main_namespace["_catcher"]);
	if (catcher.ptr())
	{
	boost::python::object cstr(catcher.attr("data"));
    extractedError = to_tchar_string(boost::python::extract<std::string>(cstr));
    //extractedError = PyObject_REPR(cstr.ptr());
	//if (char *sstr = PyString_AsString(cstr.ptr()))
	//	extractedError= to_tchar_string(sstr);
	}
	extractedError += PythonStdIoRedirect::GetOutput();
    m_python_output->SetValue( to_tchar_string(extractedError));
}