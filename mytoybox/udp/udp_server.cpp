//#define __CYGWIN__
#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <thread>
#include <mutex>
#include <memory>
#include <limits>

using boost::asio::ip::udp;

std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}

class udp_server
{
public:
    udp_server(boost::asio::io_context& io_context, unsigned short port_num) :
        socket_(io_context, udp::endpoint(udp::v4(), port_num))
    {
        start_receive();
    }

private:
    void start_receive()
    {
        socket_.async_receive_from(
            boost::asio::buffer(recv_buffer_), remote_endpoint_,
            boost::bind(&udp_server::handle_receive, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }

    void handle_receive(const boost::system::error_code& error,
                        std::size_t /*bytes_transferred*/)
    {
        if (!error)
        {
            boost::shared_ptr<std::string> message(
                new std::string(make_daytime_string()));

            socket_.async_send_to(
                boost::asio::buffer(*message), remote_endpoint_,
                boost::bind(&udp_server::handle_send, this, message,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));

            start_receive();
        }
    }

    void handle_send(boost::shared_ptr<std::string> /*message*/,
                     const boost::system::error_code& /*error*/,
                     std::size_t /*bytes_transferred*/)
    {
    }

    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    boost::array<char, 1> recv_buffer_;
};

int udp_server_main(boost::asio::io_context& io_context, unsigned short port_num)
{
    try
    {
    	io_context.reset();
        udp_server server(io_context, port_num);
        io_context.run();
        std::cout << __func__ << " done with port " << port_num << "\n";
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

class MyApp : public wxApp
{
public:
    bool OnInit() override;
    void Stop()
    {
    	if (m_thread)
    	{
    		io_context.stop();
    		m_thread->join();
    		m_thread.reset();
    	}
    }
    boost::asio::io_context io_context;
    std::unique_ptr<std::thread> m_thread;
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    void OnHello(wxCommandEvent& event);
    void OnStart(wxCommandEvent& event);
    void OnDaytime(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    unsigned short GetPort()
    {
    	unsigned short port_num = 13;
    	long val = 65536;
    	m_port->GetValue().ToLong(&val);
    	port_num = static_cast<unsigned short>(val);
    	return port_num;
    }

    enum
    {
        ID_Hello = 1
    };

    wxTextCtrl* m_port;
    wxButton* m_start;
    wxButton* m_exit;
    wxButton* m_get_daytime;
    wxTextCtrl* m_daytime;
    wxDECLARE_EVENT_TABLE();
};

// clang-format off
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(ID_Hello, MyFrame::OnHello)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
	EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);
// clang-format on

bool MyApp::OnInit()
{
    auto title = wxString::Format("Hello %s (pid %lu)", wxVERSION_STRING,
                                  wxGetProcessId());
    MyFrame* frame = new MyFrame(title, wxPoint(50, 50), wxSize(450, 340));
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size) :
    wxFrame(NULL, wxID_ANY, title, pos, size)
{
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);
    CreateStatusBar();
    SetStatusText(wxVERSION_STRING);

	m_port = new wxTextCtrl(this, wxID_ANY,
			"32000", wxDefaultPosition, wxDefaultSize);
	m_daytime = new wxTextCtrl(this, wxID_ANY,
			wxEmptyString, wxDefaultPosition, wxSize(240, -1));

    m_start = new wxButton(this, wxID_ANY, _T("Start"));
    m_start->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(MyFrame::OnStart), nullptr, this);

    m_exit = new wxButton(this, wxID_ANY, _T("Exit"));
	m_exit->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(MyFrame::OnExit), nullptr, this);

    m_get_daytime = new wxButton(this, wxID_ANY, _T("Daytime"));
    m_get_daytime->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(MyFrame::OnDaytime), nullptr, this);

	wxGridBagSizer *m_fgsizer = new wxGridBagSizer;
	int row = 0;
	m_fgsizer->Add(new wxStaticText(this, wxID_ANY, "port:"), wxGBPosition(row,0));
	m_fgsizer->Add(m_port, wxGBPosition(row,1), wxGBSpan(1,1));
	++row;
	m_fgsizer->Add(m_start, wxGBPosition(row,0), wxGBSpan(1,1), wxGROW);
	m_fgsizer->Add(m_exit, wxGBPosition(row,1), wxGBSpan(1,1));
	++row;
	m_fgsizer->Add(m_get_daytime, wxGBPosition(row,0), wxGBSpan(1,1), wxGROW);
	m_fgsizer->Add(m_daytime, wxGBPosition(row,1), wxGBSpan(1,1), wxGROW);

	this->SetSizer(m_fgsizer);
}

void MyFrame::OnStart(wxCommandEvent& event)
{
    wxGetApp().Stop();

    wxGetApp().m_thread.reset(
        new std::thread(udp_server_main, std::ref(wxGetApp().io_context), GetPort()));
}

void MyFrame::OnDaytime(wxCommandEvent& event)
{
    try
    {
        boost::asio::io_context io_context;
        udp::resolver resolver(io_context);

        udp::endpoint receiver_endpoint(udp::v4(), GetPort());

        udp::socket socket(io_context);
        socket.open(udp::v4());

        boost::array<char, 1> send_buf = {{0}};
        socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

        boost::array<char, 128> recv_buf;
        memset(recv_buf.data(), 0, recv_buf.size());
        udp::endpoint sender_endpoint;
        size_t len = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);

        m_daytime->SetValue(recv_buf.data());

//        std::cout.write(recv_buf.data(), len);

    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void MyFrame::OnExit(wxCommandEvent& event)
{
	wxGetApp().Stop();

    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets+boost.asio udp sample", "About udp_server",
                 wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello from " wxVERSION_STRING);
}
