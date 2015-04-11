// OzSvcClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OzSvcClient.h"
#include "OzSvcClientDlg.h"
#include <comdef.h>
using namespace std;

#include "StdRegProv.h"

#include "WMIHelper.h"
#include "Win32_Service.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

OzIPCDlg::OzIPCDlg(CWnd* pParent )
	: CDialog(OzIPCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void OzIPCDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SHARE_TYPE, m_share_type);
}

BEGIN_MESSAGE_MAP(OzIPCDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDB_START_CLIENT, &OzIPCDlg::OnBnClickedStartClient)
    ON_BN_CLICKED(IDB_START_SERVER, &OzIPCDlg::OnBnClickedStartServer)
    ON_WM_MOUSEMOVE()
    ON_BN_CLICKED(IDB_MEMORY_TEST, &OzIPCDlg::OnBnClickedMemoryTest)
    ON_WM_COPYDATA()
    ON_CBN_SELENDOK(IDC_SHARE_TYPE, &OzIPCDlg::OnCbnSelendokShareType)
    ON_BN_CLICKED(IDB_UPDATE_CLIENT, &OzIPCDlg::OnBnClickedUpdateClient)
	//ON_CONTROL_RANGE(EN_CHANGE, IDE_MESSAGE_COUNT,IDE_MESSAGE_SIZE,  &OzIPCDlg::OnEnChange)
END_MESSAGE_MAP()


// OzIPCDlg message handlers
LPCTSTR IPC_REMOTE_MEMORY_TYPE = _T("RemoteMemory");
LPCTSTR IPC_WINDOWS_SHARED_MEMORY_TYPE = _T("WindowsSharedMemory");
LPCTSTR IPC_POSIX_SHARED_MEMORY_TYPE = _T("PosixSharedMemory");
LPCTSTR IPC_COPYDATA_TYPE = _T("WM_COPYDATA");
LPCTSTR IPC_SOCKET_TYPE = _T("MFC.CSocket");
LPCTSTR IPC_BOOST_SOCKET = _T("Boost.ASIO");
LPCTSTR IPC_NAMED_PIPE = _T("Named Pipe");
LPCTSTR IPC_MAILSLOT = _T("Mailslot");


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void OzIPCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR OzIPCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void OzIPCDlg::OnBnClickedOk()
{
    CString server_address;
    CString port;
    CString filename;
    GetDlgItemText(IDE_SERVER_ADDR, server_address);
    GetDlgItemText(IDE_PORT, port);
    GetDlgItemText(IDE_FILENAME, filename);

    using boost::asio::ip::tcp;
    boost::asio::io_service io_service;
    // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(io_service);
    tcp::resolver::query query( (LPCSTR)server_address, (LPCSTR)port);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;
    tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
        socket.close();
        socket.connect(*endpoint_iterator++, error);
    }
    if (error)
        throw boost::system::system_error(error);
    boost::system::error_code errcode;
    {
        int command_id_buffer[1];
        command_id_buffer[0] = 10001;
        boost::asio::write(socket, boost::asio::buffer(command_id_buffer));
    }

    std::size_t length_buffer[1];
    length_buffer[0]= filename.GetLength()+1;
    socket.write_some(boost::asio::buffer(length_buffer), errcode);
    socket.write_some(boost::asio::buffer((LPCSTR)filename, length_buffer[0]), errcode);
    std::stringstream ss;
    ss << "OzSvcClientTmp" << GetTickCount() << ".txt";
    std::ofstream output_file(ss.str().c_str(), std::ios_base::out|std::ios_base::trunc|std::ios_base::binary);
    socket.read_some(boost::asio::buffer(length_buffer), errcode);
    ss.str("");
    ss << "file size is " << length_buffer[0];
    OutputDebugString(ss.str().c_str());
    boost::array<char, 1500> buf; 
    std::size_t total_get = 0;
    while (total_get<length_buffer[0])
    {
        std::size_t want_size = min(buf.size(), length_buffer[0]-total_get);
        std::size_t get_size = socket.read_some( boost::asio::buffer(buf.data(), want_size), errcode);
        if (errcode)
            break;
        ss.str("");
        ss << "want " << want_size << ", get " << get_size << " bytes, total get" << total_get;
        OutputDebugString(ss.str().c_str());
        if (get_size>0)
        {
            total_get += get_size;
            output_file.write(buf.data(), get_size);
        }
        else
            break;
    }
    output_file.flush();
    CString msg;
    msg.Format("File size:%d, total get:%d, received size:%d", length_buffer[0], total_get, output_file.tellp());
    output_file.close();
    AfxMessageBox(msg);
}

void DebugTellp( std::ostream& os, LPCSTR title)
{
    std::stringstream ss;
    ss << title << " tellp() return " << os.tellp();
    OutputDebugStringA(ss.str().c_str());
}

void OzIPCDlg::OnBnClickedTest()
{
    // Test std::ofstream::tellp()
    std::ofstream output_file;
    output_file.open("testfile.txt", std::ios_base::out|std::ios_base::trunc|std::ios_base::binary);
    DebugTellp(output_file, "Just open file,");
    char buf[1024];
    memset(buf, 'a', sizeof(buf));
    output_file.write(buf, sizeof(buf));
    DebugTellp(output_file, "after write some bytes,");
    //Sleep(10);
    output_file.write(buf, sizeof(buf));
    //Sleep(10);
    output_file.write(buf, 656);
    std::stringstream ss;
    ss << output_file.tellp();
    AfxMessageBox(ss.str().c_str());
    output_file.close();
}

void OzIPCDlg::OnBnClickedTest2()
{
	// TODO: Add your control notification handler code here
    CString server_address;
    CString port;
    CString filename;
    GetDlgItemText(IDE_SERVER_ADDR, server_address);
    GetDlgItemText(IDE_PORT, port);
    GetDlgItemText(IDE_FILENAME, filename);

    using boost::asio::ip::tcp;
    boost::asio::io_service io_service;
    // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(io_service);
    tcp::resolver::query query( (LPCSTR)server_address, (LPCSTR)port);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;
    tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
        socket.close();
        socket.connect(*endpoint_iterator++, error);
    }
    if (error)
        throw boost::system::system_error(error);
    boost::system::error_code errcode;
    {
        int command_id_buffer[1];
        command_id_buffer[0] = 10001;
        boost::asio::write(socket, boost::asio::buffer(command_id_buffer));
    }

    std::size_t length_buffer[1];
    length_buffer[0]= filename.GetLength()+1;
    boost::asio::write(socket, boost::asio::buffer(length_buffer));
    boost::asio::write(socket, boost::asio::buffer((LPCSTR)filename, length_buffer[0]));
    std::stringstream ss;
    ss << "OzSvcClientTmp" << GetTickCount() << ".bin";
    std::ofstream output_file(ss.str().c_str(), std::ios_base::out|std::ios_base::trunc|std::ios_base::binary);
    boost::asio::read(socket, boost::asio::buffer(length_buffer));
    ss.str("");
    ss << "file size is " << length_buffer[0];
    OutputDebugString(ss.str().c_str());
    boost::array<char, 1500> buf; 
    std::size_t total_get = 0;
    while (total_get<length_buffer[0])
    {
        int want_size = min(buf.size(), length_buffer[0]-total_get);
        int get_size = boost::asio::read(socket, boost::asio::buffer(buf.data(), want_size));
        if (errcode)
            break;
        ss.str("");
        ss << "want " << want_size << ", get " << get_size << " bytes, total get" << total_get
            << ", tellp() returns " << output_file.tellp();
        OutputDebugString(ss.str().c_str());
        if (get_size>0)
        {
            total_get += get_size;
            output_file.write(buf.data(), get_size);
        }
        else
            break;
    }
    CString msg;
    msg.Format("File size:%d, total get:%d, received size:%d", length_buffer[0], total_get, output_file.tellp());
    output_file.close();
    AfxMessageBox(msg);
}

struct FileInfo
{
    std::size_t file_size;
    time_t last_modify;
    DWORD crc32;
};


void OzIPCDlg::OnBnClickedGetFileInfo()
{
    CString server_address;
    CString port;
    CString filename;
    GetDlgItemText(IDE_SERVER_ADDR, server_address);
    GetDlgItemText(IDE_PORT, port);
    GetDlgItemText(IDE_FILENAME, filename);

    using boost::asio::ip::tcp;
    boost::asio::io_service io_service;
    // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(io_service);
    tcp::resolver::query query( (LPCSTR)server_address, (LPCSTR)port);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;
    tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
        socket.close();
        socket.connect(*endpoint_iterator++, error);
    }
    if (error)
        throw boost::system::system_error(error);
    std::size_t send_bytes = 0;
    std::stringstream ss;
    boost::system::error_code errcode;
    {
        int command_id_buffer[1];
        command_id_buffer[0] = 10002;
        send_bytes = boost::asio::write(socket, boost::asio::buffer(command_id_buffer));
        ss << "send " << send_bytes << std::endl;
    }

    int length_buffer[1];
    length_buffer[0]= filename.GetLength()+1;
    send_bytes = boost::asio::write(socket, boost::asio::buffer(length_buffer));
    ss << "send " << send_bytes << std::endl;

    send_bytes = boost::asio::write(socket, boost::asio::buffer((LPCSTR)filename, length_buffer[0]));
    ss << "send " << send_bytes << std::endl;
    OutputDebugStringA(ss.str().c_str());

    FileInfo file_info;
    boost::asio::read(socket, boost::asio::buffer(&file_info, sizeof(file_info)));
    CString msg;
    msg.Format("File size:%d, CRC32:%08x", file_info.file_size, file_info.crc32);
    AfxMessageBox(msg);
}

void Dump(std::stringstream& os)
{
	std::cout << os.str() << std::endl;
    OutputDebugStringA(os.str().c_str());
    os.str("");
}

 void OzIPCDlg::OnBnClickedStartService()
{
    CoInitializeEx(0, COINIT_MULTITHREADED); 
    //control_service(L"Admin-PC", L"Admin-PC", L"Admin", L"verigy", L"OzSvc", L"StartService");
	control_service(L"CNU6180RRB", L"ent", L"onega.zhang", L"Winter124", 
		L"RAService", L"UserControlService");
    CoUninitialize();
}

void OzIPCDlg::OnBnClickedStopService()
{
    // TODO: Add your control notification handler code here
    CoInitializeEx(0, COINIT_MULTITHREADED); 
    control_service(L"Admin-PC", L"Admin-PC", L"Admin", L"verigy", L"OzSvc", L"StopService");
    CoUninitialize();
}


void OzIPCDlg::OnBnClickedUpload()
{
    // TODO: Add your control notification handler code here
    CString server_address;
    CString port;
    CString filename;
    GetDlgItemText(IDE_SERVER_ADDR, server_address);
    GetDlgItemText(IDE_PORT, port);
    GetDlgItemText(IDE_FILENAME, filename);

    using boost::asio::ip::tcp;
    boost::asio::io_service io_service;
    // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(io_service);
    tcp::resolver::query query( (LPCSTR)server_address, (LPCSTR)port);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;
    tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
        socket.close();
        socket.connect(*endpoint_iterator++, error);
    }
    if (error)
        throw boost::system::system_error(error);
    std::size_t send_bytes = 0;
    std::stringstream ss;
    boost::system::error_code errcode;
    {   // send command ID
        int command_id_buffer[1];
        command_id_buffer[0] = 10003;
        send_bytes = boost::asio::write(socket, boost::asio::buffer(command_id_buffer));
        ss << "send " << send_bytes << std::endl;
    }

    // send file name length
    int length_buffer[1];
    length_buffer[0]= filename.GetLength()+1;
    send_bytes = boost::asio::write(socket, boost::asio::buffer(length_buffer));
    ss << "send " << send_bytes << std::endl;

    // send file size 
    __int64 file_size = 0;
    std::ifstream src_file;
    src_file.open(filename, std::ios_base::binary);
    src_file.seekg(0, std::ios_base::end);
    file_size = src_file.tellg();
    src_file.seekg(0, std::ios_base::beg);
    send_bytes = boost::asio::write(socket, boost::asio::buffer(&file_size, sizeof(file_size)));
    ss << "send " << send_bytes << std::endl;

    // send file name
    send_bytes = boost::asio::write(socket, boost::asio::buffer((LPCSTR)filename, length_buffer[0]));
    ss << "send " << send_bytes << std::endl;
    OutputDebugStringA(ss.str().c_str());

    // send file content
    boost::array<char, 1024*256> buf;
    while(!src_file.eof())
    {
        src_file.read(buf.data(), buf.size());
        if (src_file.gcount()>0)
            send_bytes = boost::asio::write(socket, boost::asio::buffer(buf.data(), src_file.gcount()));
    }
    src_file.close();
    const std::string FILEUPLOAD_RESPONSE = "FileUploadDone\0";

    boost::asio::read(socket, boost::asio::buffer(buf.data(), FILEUPLOAD_RESPONSE.size()+1));
    CString msg;
    msg.Format("response:%s", buf.data());
    AfxMessageBox(msg);
}

struct SharedData
{
    DWORD pid;
    std::size_t client_read_count;
    std::size_t message_size;
    std::size_t queue_size;
    std::size_t message_count;
    HWND server_wnd;
    HWND client_wnd;
    HWND server_ipc_type_wnd;
    HWND client_ipc_type_wnd;
    HWND active_type_wnd;
    HWND start_server_button;
    HWND start_client_button;
    char share_type[64];
};

LPCSTR PROCESS_SHARED_MEMORY_NAME = "server_shared_memory";
typedef boost::interprocess::windows_shared_memory windows_shm;
typedef boost::shared_ptr<windows_shm> ptr_shared_memory;
typedef std::list<ptr_shared_memory> shm_list;
ptr_shared_memory g_shm_server;
ptr_shared_memory g_shm_client;
shm_list active_shm_list;
std::size_t remove_shm_count = 0;
std::size_t MAX_MSG_COUNT = 1000;
std::size_t MSG_SIZE_MAX = 1024*1024*2;
std::size_t QUEUE_SIZE = 8;
const std::size_t QUEUE_MSG_SIZE_MAX = 100;
#include "SafeShardQueue.h"

const std::size_t SHM_SERVER_SIZE = sizeof(SharedData);
CString share_type;
SharedData *pshare = NULL;
#include "PerformanceProfile.h"

BOOL OzIPCDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
	AfxOleInit();
    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    // TODO: Add extra initialization here
    SetDlgItemText(IDE_SERVER_ADDR, "10.18.57.83");
    SetDlgItemText(IDE_PORT, "2245");
    SetDlgItemText(IDE_FILENAME, "D:\\a.txt");

    m_share_type.AddString(IPC_WINDOWS_SHARED_MEMORY_TYPE);
    m_share_type.AddString(IPC_REMOTE_MEMORY_TYPE);
    m_share_type.AddString(IPC_POSIX_SHARED_MEMORY_TYPE);
    m_share_type.AddString(IPC_COPYDATA_TYPE);
    m_share_type.AddString(IPC_SOCKET_TYPE);
    m_share_type.AddString(IPC_BOOST_SOCKET);
    m_share_type.AddString(IPC_NAMED_PIPE);
    m_share_type.AddString(IPC_MAILSLOT);
    m_share_type.SelectString(-1, IPC_MAILSLOT);
    m_old_selection = m_share_type.GetCurSel();
    SetDlgItemText(IDE_MESSAGE_COUNT, _T("1000"));
    SetDlgItemText(IDE_QUEUE_SIZE, _T("8"));
    SetDlgItemText(IDE_MESSAGE_SIZE, _T("2097152"));
    CString title;
    try
    {
        g_shm_client.reset(new windows_shm(boost::interprocess::open_only, 
            PROCESS_SHARED_MEMORY_NAME,
            boost::interprocess::read_write));
        boost::interprocess::mapped_region region(*g_shm_client, boost::interprocess::read_write, 
            0, SHM_SERVER_SIZE);
        pshare = (SharedData*)region.get_address();
        share_type = pshare->share_type;
        pshare->client_wnd = GetSafeHwnd();
        pshare->client_ipc_type_wnd = GetDlgItem(IDC_SHARE_TYPE)->GetSafeHwnd();
        pshare->start_client_button = GetDlgItem(IDB_START_CLIENT)->GetSafeHwnd();
        GetDlgItem(IDB_START_SERVER)->EnableWindow(FALSE);
        GetDlgItem(IDB_UPDATE_CLIENT)->EnableWindow(FALSE);
        //GetDlgItem(IDC_SHARE_TYPE)->EnableWindow(FALSE);
        title.Format("Benchmark test client PID: %d", GetCurrentProcessId());
    }
    catch (const std::exception& e)
    {
    	e;
    }
    if (g_shm_client.get()==NULL)
    {
        // store server PID, client read message count on shared memory
        g_shm_server.reset(new windows_shm(boost::interprocess::open_or_create, 
            PROCESS_SHARED_MEMORY_NAME,
            boost::interprocess::read_write, SHM_SERVER_SIZE));
        boost::interprocess::mapped_region region(*g_shm_server, boost::interprocess::read_write, 
            0, SHM_SERVER_SIZE);
        memset(region.get_address(), 0, region.get_size());
        pshare = (SharedData*)region.get_address();
        pshare->pid = GetCurrentProcessId();
        pshare->server_wnd = GetSafeHwnd();
        pshare->server_ipc_type_wnd = GetDlgItem(IDC_SHARE_TYPE)->GetSafeHwnd();
        pshare->active_type_wnd = NULL;

        GetDlgItem(IDB_START_CLIENT)->EnableWindow(FALSE);
        title.Format("Benchmark test server PID: %d", GetCurrentProcessId());
    }

    SetWindowText(title);

	//	CStdRegProv remote_reg;
	//remote_reg.Connect("\\\\bajie.ent.rt.verigy.net\\root\\default",
	//	"", "ent\\onega.zhang", "Winter125");
	//remote_reg.SetDWORDValue((unsigned)HKEY_LOCAL_MACHINE, "SOFTWARE\\MPICH2", "testSetDWORDValue", 123456);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void OzIPCDlg::OnBnClickedStartClient()
{
    GetDlgItem(IDB_START_CLIENT)->EnableWindow(FALSE);
    Sleep(100);
    InitBenchmarkTest();

    boost::interprocess::mapped_region region(*g_shm_client, boost::interprocess::read_write, 
        0, SHM_SERVER_SIZE);
    pshare = (SharedData*)region.get_address();
    share_type = pshare->share_type;

    if (share_type == IPC_COPYDATA_TYPE)
    {
        StartCopyDataSender();
    }
    else if (share_type == IPC_SOCKET_TYPE)
    {
        StartSockSender();
    }
    else if (share_type == IPC_BOOST_SOCKET)
    {
        StartBoostAsioSender();
    }
	else if (share_type==IPC_NAMED_PIPE)
	{
		StartNamedPipeClient();
	}
	else if (share_type == IPC_MAILSLOT)
	{
		StartMailslotClient();
	}
    else
    {
        IPCTestClient();
    }
    GetDlgItem(IDB_START_CLIENT)->EnableWindow(TRUE);

}


void OzIPCDlg::OnBnClickedStartServer()
{
	OnBnClickedUpdateClient();
    GetDlgItem(IDB_START_SERVER)->EnableWindow(FALSE);
    InitBenchmarkTest();
    boost::interprocess::mapped_region region(*g_shm_server, boost::interprocess::read_write, 
        0, SHM_SERVER_SIZE);
    pshare = (SharedData*)region.get_address();
    pshare->pid = GetCurrentProcessId();
    pshare->server_wnd = GetSafeHwnd();
    pshare->client_read_count = 0;
    pshare->message_count = MAX_MSG_COUNT;
    pshare->message_size = MSG_SIZE_MAX;
    pshare->queue_size = QUEUE_SIZE;
    strcpy(pshare->share_type, (LPCSTR)share_type);

    ::PostMessage(pshare->client_wnd, 
        WM_COMMAND, 
        MAKELONG(::GetDlgCtrlID(pshare->start_client_button),BN_CLICKED),
        (LPARAM)pshare->start_client_button);
    if (share_type == IPC_WINDOWS_SHARED_MEMORY_TYPE)
    {
        BenchmarkWindowsSharedMemoryServer();
    }
    else if (share_type == IPC_REMOTE_MEMORY_TYPE)
    {
        BenchmarkRemoteMemoryServer();
    }

    if (share_type == IPC_POSIX_SHARED_MEMORY_TYPE) 
    {
        BenchmarkPosixSharedMemoryServer();
    }

    if (share_type == IPC_SOCKET_TYPE)
    {
        StartSockListener();
    }
    if (share_type == IPC_BOOST_SOCKET)
        StartBoostAsioListener();
	if (share_type == IPC_NAMED_PIPE)
		StartNamedPipeServer();
	if (share_type == IPC_MAILSLOT)
		StartMailslotServer();
    GetDlgItem(IDB_START_SERVER)->EnableWindow(TRUE);
}

//boost::circular_buffer<std::string> dump_container(4);
void OzIPCDlg::Dump( const std::string s )
{
    OutputDebugStringA(s.c_str());
	//dump_container.push_back(s);
	//std::stringstream ss;
	//for (boost::circular_buffer<std::string>::const_iterator it=dump_container.begin();
	//	it!=dump_container.end();
	//	it++)
	//{
	//	ss << *it << "\r\n";
	//}
	//CDialog* pdlg = (CDialog*)AfxGetMainWnd();
	//pdlg->SetDlgItemText(IDE_DUMP, ss.str().c_str());

}

void OzIPCDlg::Dump( const std::wstring s )
{
	Dump((LPCSTR)_bstr_t(s.c_str()));
}

void OzIPCDlg::BenchmarkWindowsSharedMemoryServer()
{
    try
    {
        scoped_SafeSharedQueue test_queue(SafeShardQueue::create()) ;
        char msg_buf[QUEUE_MSG_SIZE_MAX];
        std::size_t server_send_count = 0;

        while(server_send_count<MAX_MSG_COUNT)
        {
            SafeShardQueue::scoped_mutex locker(*(test_queue->mu));

            while (remove_shm_count<pshare->client_read_count)
            {
                active_shm_list.pop_front();
                remove_shm_count++;
                std::stringstream ss;
                ss << "Client read " << (pshare->client_read_count) << " messages, "
                    << "currently removing " << remove_shm_count;
                //Dump(ss.str());

            }
            if (test_queue->msgs->get_num_msg()<test_queue->msgs->get_max_msg())
            {
                wsprintf(msg_buf, "wshared_mem_%d", server_send_count);
                ptr_shared_memory tmp(new windows_shm(boost::interprocess::create_only,
                    msg_buf, boost::interprocess::read_write, MSG_SIZE_MAX));
                boost::interprocess::mapped_region region(*tmp, boost::interprocess::read_write, 0, MSG_SIZE_MAX, 0);
                memset(region.get_address(), server_send_count, MSG_SIZE_MAX);
                active_shm_list.push_back(tmp);

                test_queue->msgs->send(msg_buf, sizeof(msg_buf), 0);
                server_send_count++;
                // 
                if (test_queue->msgs->get_num_msg()==1)
                {
                    test_queue->data_avail_condition->notify_all();
                }
                if (server_send_count>=MAX_MSG_COUNT)
                {
                    test_queue->data_avail_condition->notify_all();
                    break;
                }
            }
            else
            {
                test_queue->queue_empty_condition->wait(locker);
            }
        }
        // wait for client read all messages
        while(true)
        {
            SafeShardQueue::scoped_mutex locker(*(test_queue->mu));
            while (remove_shm_count<pshare->client_read_count)
            {
                active_shm_list.pop_front();
                remove_shm_count++;
                std::stringstream ss;
                ss << "Client read " << (pshare->client_read_count) << " messages, "
                    << "currently removing " << remove_shm_count;
                //Dump(ss.str());

            }
            if (remove_shm_count>=MAX_MSG_COUNT)
                break;
            else
                test_queue->queue_empty_condition->wait(locker);
        }
        //Dump("server completed");
    }
    catch (const boost::interprocess::interprocess_exception& e)
    {
        Dump(e.what());
    }
}

typedef boost::shared_array<char> ptr_shared_array;
std::list<ptr_shared_array> active_remote_list;

void OzIPCDlg::BenchmarkRemoteMemoryServer()
{
    //PerformanceProfile profile(__FUNCTION__);
    try
    {
        scoped_SafeSharedQueue test_queue(SafeShardQueue::create()) ;
        char msg_buf[QUEUE_MSG_SIZE_MAX];
        std::size_t server_send_count = 0;

        while(server_send_count<MAX_MSG_COUNT)
        {
            // fill message queue until it is full
            while ( (test_queue->msgs->get_num_msg()<test_queue->msgs->get_max_msg())
                &&  (server_send_count<MAX_MSG_COUNT)
                )
            {
                ptr_shared_array tmp(new char[MSG_SIZE_MAX]);
                wsprintf(msg_buf, "r%d", tmp.get()); // remote memory
                memset(tmp.get(), server_send_count, MSG_SIZE_MAX);
                active_remote_list.push_back(tmp);

                test_queue->msgs->send(msg_buf, sizeof(msg_buf), 0);
                server_send_count++;
            }
            SafeShardQueue::scoped_mutex locker(*(test_queue->mu));
            // release memory that client has processed.
            while (remove_shm_count<pshare->client_read_count)
            {
                active_remote_list.pop_front();
                remove_shm_count++;
                std::stringstream ss;
                ss << "Client read " << (pshare->client_read_count) << " messages, "
                    << "currently removing " << remove_shm_count;
                //Dump(ss.str());
            }
            if (test_queue->msgs->get_num_msg()==test_queue->msgs->get_max_msg())
            {                
                test_queue->data_avail_condition->notify_all();
                test_queue->queue_empty_condition->wait(locker);
            }
        }
        test_queue->data_avail_condition->notify_all();

        // wait for client read all messages
        while(remove_shm_count<MAX_MSG_COUNT)
        {
            SafeShardQueue::scoped_mutex locker(*(test_queue->mu));
            while (remove_shm_count<pshare->client_read_count)
            {
                active_remote_list.pop_front();
                remove_shm_count++;
                std::stringstream ss;
                ss << "Client read " << (pshare->client_read_count) << " messages, "
                    << "currently removing " << remove_shm_count;
                //Dump(ss.str());

            }
            if (remove_shm_count>=MAX_MSG_COUNT)
                break;
            else
            {
                test_queue->queue_empty_condition->wait(locker);
            }
        }
        Dump("server completed");
    }
    catch (const boost::interprocess::interprocess_exception& e)
    {
        Dump(e.what());
    }
}

void OzIPCDlg::IPCTestClient()
{
    PerformanceProfile profile( __FUNCTION__);
    try
    {
        HANDLE parent_handle = OpenProcess(PROCESS_VM_READ, FALSE, pshare->pid);
        if (parent_handle==NULL)
        {
            std::stringstream ss;
            ss << __FUNCTION__ << " failed to open pid " << pshare->pid
                << ", error code " << GetLastError();
            Dump(ss.str());
            return;
        }
        scoped_SafeSharedQueue test_queue(SafeShardQueue::open()) ;
        boost::shared_array<BYTE> data_buf(new BYTE[MSG_SIZE_MAX]);
        char msg_buf[QUEUE_MSG_SIZE_MAX];
        while(pshare->client_read_count< MAX_MSG_COUNT)
        {
            std::size_t received_size = 0;
            unsigned int priority = 0;
			
            SafeShardQueue::scoped_mutex locker(*(test_queue->mu));
            while ( test_queue->msgs->get_num_msg() > 0)
            {
                test_queue->msgs->receive(msg_buf, sizeof(msg_buf), received_size, priority);
                BYTE* remote_data_ptr = NULL;
                switch(msg_buf[0])
                {
                case 'w': // shared memory
                    {
                        windows_shm shm(boost::interprocess::open_only,
                            msg_buf, boost::interprocess::read_only);
                        boost::interprocess::mapped_region region(shm, boost::interprocess::read_only);
                        memcpy(data_buf.get(), region.get_address(), MSG_SIZE_MAX);
                        remote_data_ptr = (BYTE*)data_buf.get();
                        if (remote_data_ptr)
                        {
                            std::stringstream ss;
                            ss << __FUNCTION__ << " get message " << msg_buf << ", shared memory size: " 
                                << ", first byte is " << (int)data_buf[0];
                            //Dump(ss.str());
                        }
                    }
                    break;
                case 'p':
                    {
                        typedef boost::interprocess::shared_memory_object posix_shm;
                        posix_shm shm(boost::interprocess::open_only, msg_buf, boost::interprocess::read_only);
                        boost::interprocess::mapped_region region(shm, boost::interprocess::read_only);
                        memcpy(data_buf.get(), region.get_address(), MSG_SIZE_MAX);
                        remote_data_ptr = (BYTE*)data_buf.get();
                        if (remote_data_ptr)
                        {
                            std::stringstream ss;
                            ss << __FUNCTION__ << " get message " << msg_buf << ", shared memory size: " 
                                << ", first byte is " << (int)data_buf[0];
                        }
                    }
                case 'r': // native pointer, need to use ReadProcessMemory
                    {
                        remote_data_ptr = (BYTE*)(atoi(msg_buf+1));
                        SIZE_T bytes_read = 0;
                        ReadProcessMemory(parent_handle, remote_data_ptr, data_buf.get(), MSG_SIZE_MAX, &bytes_read);
                        if (bytes_read>0)
                        {
                            std::stringstream ss;
                            ss << __FUNCTION__ << " get message " << msg_buf << ", shared memory size: " 
                                << ", first byte is " << (int)data_buf[0];
                            //Dump(ss.str());
                        }
                    }
                    break;

                default:
                    break;
                }
				pshare->client_read_count++;
                //if (msg_count_in_queue == QUEUE_SIZE)
                //    test_queue->queue_empty_condition->notify_all();
            }
            if (pshare->client_read_count>= (int)MAX_MSG_COUNT)
            {
                test_queue->queue_empty_condition->notify_all();
                break;
            }
            else
            {
                test_queue->queue_empty_condition->notify_all();
                test_queue->data_avail_condition->wait(locker);
            }
        }
		test_queue->queue_empty_condition->notify_all();
    }
    catch (const boost::interprocess::interprocess_exception& e)
    {
        Dump(e.what());
    }
}

void OzIPCDlg::InitBenchmarkTest()
{
    m_share_type.GetLBText(m_share_type.GetCurSel(), share_type);

    CString msg_count_string;
    GetDlgItemText(IDE_MESSAGE_COUNT, msg_count_string);
    if (msg_count_string.GetLength())
        MAX_MSG_COUNT = _ttoi(msg_count_string);
    CString queue_size_string;
    GetDlgItemText(IDE_QUEUE_SIZE, queue_size_string);
    if (queue_size_string.GetLength())
        QUEUE_SIZE = _ttoi(queue_size_string);
    CString msg_size_string;
    GetDlgItemText(IDE_MESSAGE_SIZE, msg_size_string);
    if (msg_size_string.GetLength())
        MSG_SIZE_MAX = _ttoi(msg_size_string);
    remove_shm_count = 0;
}
void OzIPCDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    // Enable dragging window by left mouse drag.
    if (nFlags == MK_LBUTTON)
    {
        ::ReleaseCapture();
        ::SendMessage(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
    }
    CDialog::OnMouseMove(nFlags, point);
}

typedef boost::shared_ptr<boost::interprocess::shared_memory_object> ptr_posix_shm;
typedef std::list<ptr_posix_shm> posix_list;
posix_list active_posix_list;

void OzIPCDlg::BenchmarkPosixSharedMemoryServer()
{
    try
    {
        scoped_SafeSharedQueue test_queue(SafeShardQueue::create()) ;
        char msg_buf[QUEUE_MSG_SIZE_MAX];
        std::size_t server_send_count = 0;

        while(server_send_count<MAX_MSG_COUNT)
        {
            while (test_queue->msgs->get_num_msg()<test_queue->msgs->get_max_msg())
            {
                wsprintf(msg_buf, "pshared_mem_%d", server_send_count);
                using boost::interprocess::shared_memory_object;
                shared_memory_object::remove(msg_buf);
                ptr_posix_shm tmp(new shared_memory_object(boost::interprocess::create_only,
                    msg_buf, boost::interprocess::read_write));
                tmp->truncate(MSG_SIZE_MAX);
                boost::interprocess::mapped_region region(*tmp, boost::interprocess::read_write);
                memset(region.get_address(), server_send_count, MSG_SIZE_MAX);
                active_posix_list.push_back(tmp);
                test_queue->msgs->send(msg_buf, sizeof(msg_buf), 0);
                server_send_count++;
                if (server_send_count>=MAX_MSG_COUNT)
                {
                    break;
                }
            }
			SafeShardQueue::scoped_mutex locker(*(test_queue->mu));
			while (remove_shm_count<pshare->client_read_count)
			{
				active_posix_list.pop_front();
				remove_shm_count++;
				std::stringstream ss;
				ss << "Client read " << (pshare->client_read_count) << " messages, "
					<< "currently removing " << remove_shm_count;
				//Dump(ss.str());
			}
            if ((test_queue->msgs->get_num_msg()==test_queue->msgs->get_max_msg())
				&& (server_send_count<MAX_MSG_COUNT))
            {
				test_queue->data_avail_condition->notify_all();
                test_queue->queue_empty_condition->wait(locker);
            }
        }
		test_queue->data_avail_condition->notify_all();
        // wait for client read all messages
        while(remove_shm_count<MAX_MSG_COUNT)
        {
            SafeShardQueue::scoped_mutex locker(*(test_queue->mu));
            while (remove_shm_count<pshare->client_read_count)
            {
                active_posix_list.pop_front();
                remove_shm_count++;
                std::stringstream ss;
                ss << "Client read " << (pshare->client_read_count) << " messages, "
                    << "currently removing " << remove_shm_count;
                //Dump(ss.str());
            }
            if (remove_shm_count>=MAX_MSG_COUNT)
                break;
            else
			{
                test_queue->queue_empty_condition->wait(locker);
			}
        }
        Dump("server completed");
    }
    catch (const boost::interprocess::interprocess_exception& e)
    {
        Dump(e.what());
    }
}

void OzIPCDlg::OnBnClickedMemoryTest()
{
    // test the speed of memory allocation & release.
    {
        PerformanceProfile profile( "Init memory, scoped_array");
        for (std::size_t i=0; i<MAX_MSG_COUNT; i++)
        {
            boost::scoped_array<char> buf(new char[MSG_SIZE_MAX]);
            memset(buf.get(), i, MSG_SIZE_MAX);
            boost::scoped_array<char> buf2(new char[MSG_SIZE_MAX]);
            memcpy(buf2.get(), buf.get(), MSG_SIZE_MAX);
        }
    }
    {
        PerformanceProfile profile( "Init memory, new/delete");
        for (std::size_t i=0; i<MAX_MSG_COUNT; i++)
        {
            char* buf=(new char[MSG_SIZE_MAX]);
            memset(buf, i, MSG_SIZE_MAX);
            char* buf2=(new char[MSG_SIZE_MAX]);
            memcpy(buf2, buf, MSG_SIZE_MAX);
            delete[] buf;
            delete[] buf2;
        }
    }
    {
        PerformanceProfile profile( "Init memory, malloc/free");
        for (std::size_t i=0; i<MAX_MSG_COUNT; i++)
        {
            char* buf= (char*)malloc(MSG_SIZE_MAX);
            memset(buf, i, MSG_SIZE_MAX);
            char* buf2=(char*)malloc(MSG_SIZE_MAX);
            memcpy(buf2, buf, MSG_SIZE_MAX);
            free( buf);
            free( buf2);
        }
    }
    {
        PerformanceProfile profile( "Init memory, HeapAlloc");
        HANDLE hh = GetProcessHeap();
        for (std::size_t i=0; i<MAX_MSG_COUNT; i++)
        {
            char* buf= (char*)HeapAlloc(hh, HEAP_NO_SERIALIZE, MSG_SIZE_MAX);
            char* buf2= (char*)HeapAlloc(hh, HEAP_NO_SERIALIZE, MSG_SIZE_MAX);
            if (buf && buf2)
            {
                memset(buf, i, MSG_SIZE_MAX);
                memcpy(buf2, buf, MSG_SIZE_MAX);

            }
            HeapFree(hh, HEAP_NO_SERIALIZE, buf);
            HeapFree(hh, HEAP_NO_SERIALIZE, buf2);
        }
    }
    {
        PerformanceProfile profile( "Init memory, VirtualAlloc");
        for (std::size_t i=0; i<MAX_MSG_COUNT; i++)
        {
            char* buf= (char*)VirtualAlloc(NULL, 
                MSG_SIZE_MAX,
                MEM_COMMIT,
                PAGE_READWRITE);
            char* buf2= (char*)VirtualAlloc(NULL, 
                MSG_SIZE_MAX,
                MEM_COMMIT,
                PAGE_READWRITE);
            if (buf && buf2)
            {
                memset(buf, i, MSG_SIZE_MAX);
                memcpy(buf2, buf, MSG_SIZE_MAX);

            }
            VirtualFree(buf, 0, MEM_RELEASE);
            VirtualFree(buf2, 0, MEM_RELEASE);
        }
    }

}




BOOL OzIPCDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
    // Receive WM_COPYDATA from server.
    if (pCopyDataStruct->dwData == 0)
    {
        boost::scoped_array<char > g_copydata_buf(new char[MSG_SIZE_MAX]);
        memcpy(g_copydata_buf.get(), pCopyDataStruct->lpData, pCopyDataStruct->cbData);
    }
    else
    {
        // update UI
        boost::interprocess::mapped_region region(*g_shm_client, boost::interprocess::read_write, 
            0, SHM_SERVER_SIZE);
        pshare = (SharedData*)region.get_address();
        share_type = pshare->share_type;
        m_share_type.SelectString(-1, share_type);
        SetDlgItemInt(IDE_MESSAGE_COUNT, pshare->message_count);
        SetDlgItemInt(IDE_MESSAGE_SIZE, pshare->message_size);
        SetDlgItemInt(IDE_QUEUE_SIZE, pshare->queue_size);
    }

    return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}


void OzIPCDlg::StartCopyDataSender()
{
    PerformanceProfile profile( __FUNCTION__);
    try
    {
        boost::shared_array<BYTE> data_buf(new BYTE[MSG_SIZE_MAX]);
        for (std::size_t i=0; i<MAX_MSG_COUNT; i++)
        {
            memset(data_buf.get(), i, MSG_SIZE_MAX);
            COPYDATASTRUCT cds;
            cds.cbData = MSG_SIZE_MAX;
            cds.lpData = data_buf.get();
            cds.dwData = 0;
            ::SendMessage(pshare->server_wnd, WM_COPYDATA, 0, (LPARAM)&cds);
        }    
    }
    catch (const boost::interprocess::interprocess_exception& e)
    {
        Dump(e.what());
    }
}

const unsigned short BENCHMARK_SOCK_PORT = 4567;
const unsigned short MFC_SOCK_PORT = 4568;
void OzIPCDlg::StartSockListener()
{
    CSocket sockSrvr;
    sockSrvr.Create(MFC_SOCK_PORT); // Creates our server socket
    sockSrvr.Listen(); // Start listening for the client at PORT
    CSocket sockRecv;
    sockSrvr.Accept(sockRecv); // Use another CSocket to accept the connection
    PerformanceProfile profile( __FUNCTION__);
    BOOL f = TRUE;
    sockSrvr.SetSockOpt(TCP_NODELAY, &f, sizeof(BOOL),IPPROTO_TCP);
    boost::shared_array<BYTE> data_buf(new BYTE[MSG_SIZE_MAX]);
    __int64 total_recv_size = 0;
    while (total_recv_size<MSG_SIZE_MAX*MAX_MSG_COUNT)
    {
        int recv_size = sockRecv.Receive(data_buf.get(), MSG_SIZE_MAX, 0);
        if (recv_size <=0)
            break;
        else
            total_recv_size += recv_size;
    }
    sockRecv.Close();
}

void OzIPCDlg::StartSockSender()
{

    CSocket sockClient;
    sockClient.Create();
    PerformanceProfile profile( __FUNCTION__);
    boost::shared_array<BYTE> data_buf(new BYTE[MSG_SIZE_MAX]);
    sockClient.Connect("localhost", MFC_SOCK_PORT);
    BOOL f = TRUE;
    sockClient.SetSockOpt(TCP_NODELAY, &f, sizeof(BOOL),IPPROTO_TCP);
    for (std::size_t i=0; i<MAX_MSG_COUNT; i++)
    {
        memset(data_buf.get(), i, MSG_SIZE_MAX);
        std::size_t total_send = 0;
        while(total_send<MSG_SIZE_MAX)
        {
            int current_send = sockClient.Send(data_buf.get()+total_send, 
                MSG_SIZE_MAX-total_send, 0);
            if (current_send>0)
                total_send+= current_send;
            else
                break;
        }
    }    
    sockClient.Close();
}
using boost::asio::ip::tcp;

typedef boost::shared_ptr<tcp::socket> socket_ptr;
boost::asio::io_service io_service;

void session(socket_ptr sock)
{
    __int64 total_recv_size = 0;
    PerformanceProfile profile( __FUNCTION__);
    boost::shared_array<BYTE> data_buf(new BYTE[MSG_SIZE_MAX]);
    try
    {
        while (total_recv_size < MSG_SIZE_MAX*MAX_MSG_COUNT)
        {
            boost::system::error_code error;
            int recv_size = sock->read_some(boost::asio::buffer(data_buf.get(), MSG_SIZE_MAX), error);
            if (recv_size <=0 || error == boost::asio::error::eof)
                break;
            else
                total_recv_size += recv_size;
            if (total_recv_size == MSG_SIZE_MAX*MAX_MSG_COUNT)
                break;    
            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.
        }
    }
    catch (std::exception& e)
    {
        OzIPCDlg::Dump(e.what());
    }
}

void OzIPCDlg::StartBoostAsioListener()
{
    //using namespace boost::asio;
    tcp::acceptor a(io_service, tcp::endpoint(tcp::v4(), BENCHMARK_SOCK_PORT));
    socket_ptr sock(new tcp::socket(io_service));
    a.accept(*sock);
    //boost::thread t(boost::bind(session, sock));
    session(sock);
}

void OzIPCDlg::StartBoostAsioSender()
{
    PerformanceProfile profile( __FUNCTION__);
    boost::shared_array<BYTE> data_buf(new BYTE[MSG_SIZE_MAX]);

    try
    {
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), "127.0.0.1", "");
        tcp::resolver::iterator iterator = resolver.resolve(query);
        tcp::endpoint end_point = *iterator;
        end_point.port(BENCHMARK_SOCK_PORT);
        tcp::socket s(io_service);
        s.connect(end_point);
        for (std::size_t i=0; i<MAX_MSG_COUNT; i++)
        {
            memset(data_buf.get(), i, MSG_SIZE_MAX);
            boost::asio::write(s, boost::asio::buffer(data_buf.get(), MSG_SIZE_MAX));
        }    
    }
    catch (std::exception& e)
    {
        OzIPCDlg::Dump(e.what());
    }

}


void OzIPCDlg::OnCbnSelendokShareType()
{
    // TODO: Add your control notification handler code here
    if (m_old_selection == m_share_type.GetCurSel())
        return;
    m_old_selection = m_share_type.GetCurSel();
    HWND sendto_wnd = NULL;
    if (g_shm_server.get())
    {
        // this is server, synchronize IPC type to client
        boost::interprocess::mapped_region region(*g_shm_server, boost::interprocess::read_write, 
            0, SHM_SERVER_SIZE);
        pshare = (SharedData*)region.get_address();
        sendto_wnd = pshare->client_ipc_type_wnd;
    }
    if (g_shm_client.get())
    {
        boost::interprocess::mapped_region region(*g_shm_client, boost::interprocess::read_write, 
            0, SHM_SERVER_SIZE);
        pshare = (SharedData*)region.get_address();
        sendto_wnd = pshare->server_ipc_type_wnd;

    }

    if (sendto_wnd)
    {
        ::SendMessage(sendto_wnd, CB_SETCURSEL, m_share_type.GetCurSel(), 0);
        ::UpdateWindow(sendto_wnd);
    }
    else
    {
        Dump("Window is null");
    }
}

void OzIPCDlg::OnBnClickedUpdateClient()
{
    InitBenchmarkTest();
    boost::interprocess::mapped_region region(*g_shm_server, boost::interprocess::read_write, 
        0, SHM_SERVER_SIZE);
    pshare = (SharedData*)region.get_address();
    pshare->pid = GetCurrentProcessId();
    pshare->server_wnd = GetSafeHwnd();
    pshare->client_read_count = 0;
    pshare->message_count = MAX_MSG_COUNT;
    pshare->message_size = MSG_SIZE_MAX;
    pshare->queue_size = QUEUE_SIZE;
    strcpy(pshare->share_type, (LPCSTR)share_type);
    // notify client to update UI
    COPYDATASTRUCT cds;
    cds.cbData = 0;
    cds.lpData = NULL;
    cds.dwData = 1;
    ::SendMessage(pshare->client_wnd, WM_COPYDATA, 0, (LPARAM)&cds);
}

LPTSTR lpszPipename = "\\\\.\\pipe\\SamplePipe";
void OzIPCDlg::StartNamedPipeServer()
{
#define PIPE_TIMEOUT 5000
    BOOL fConnected;
    DWORD cbBytesRead;
    BOOL fSuccess;
    HANDLE hPipe;
    hPipe = CreateNamedPipe ( lpszPipename,
        PIPE_ACCESS_DUPLEX, // read/write access
        PIPE_TYPE_MESSAGE | // message type pipe
        PIPE_READMODE_MESSAGE | // message-read mode
        PIPE_WAIT, // blocking mode
        PIPE_UNLIMITED_INSTANCES, // max. instances
        MSG_SIZE_MAX, // output buffer size
        MSG_SIZE_MAX, // input buffer size
        PIPE_TIMEOUT, // client time-out
        NULL); // no security attribute

    if (hPipe == INVALID_HANDLE_VALUE)
    {
        Dump("Failed to start named pipe server");
        return;
    }

    // Trying to connect namedpipe in sample for CreateNamedPipe
    // Wait for the client to connect; if it succeeds,
    // the function returns a nonzero value. If the function returns
    // zero, GetLastError returns ERROR_PIPE_CONNECTED.

    fConnected = ConnectNamedPipe(hPipe, NULL) ?
                    TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (fConnected)
    {
		boost::shared_array<BYTE> data_buf(new BYTE[MSG_SIZE_MAX]);
		__int64 total_recv_size = 0;
		while (total_recv_size<MSG_SIZE_MAX*MAX_MSG_COUNT)
		{
			fSuccess = ReadFile (hPipe, // handle to pipe
				data_buf.get(), // buffer to receive data
				MSG_SIZE_MAX, // size of buffer
				&cbBytesRead, // number of bytes read
				NULL); // not overlapped I/O
			if (cbBytesRead>0)			
				total_recv_size += cbBytesRead;
			if (! fSuccess || cbBytesRead == 0)
				break;
		}
		FlushFileBuffers(hPipe);			
        DisconnectNamedPipe(hPipe);
		if (total_recv_size < MSG_SIZE_MAX * MAX_MSG_COUNT)
		{
			std::stringstream ss;
			ss << __FUNCTION__ << " received " << total_recv_size;
			Dump(ss.str());
		}
    }
    else
	{
        Dump("No client connected to pipe");
	}
    CloseHandle(hPipe); 
}

void OzIPCDlg::StartNamedPipeClient()
{
    HANDLE hFile;
    BOOL flg;
    DWORD dwWrite;
    hFile = CreateFile(lpszPipename, GENERIC_WRITE,
        0, NULL, OPEN_EXISTING,
        0, NULL);

    if(hFile == INVALID_HANDLE_VALUE)
    {
        Dump("CreateFile failed for Named Pipe client\n" );
    }
    else
    {
		PerformanceProfile profile( __FUNCTION__);
		boost::shared_array<BYTE> data_buf(new BYTE[MSG_SIZE_MAX]);
		for (std::size_t i=0; i<MAX_MSG_COUNT; i++)
		{
			memset(data_buf.get(), i, MSG_SIZE_MAX);
			std::size_t total_send = 0;
			while(total_send<MSG_SIZE_MAX)
			{
				flg = WriteFile(hFile, data_buf.get()+total_send, MSG_SIZE_MAX-total_send, &dwWrite, NULL);
				if (FALSE == flg)
				{
					Dump("WriteFile failed for Named Pipe client\n");
					break;
				}
				else
				{
					if (dwWrite>0)
					{
						total_send += dwWrite;
					}
					else
						break;
				}
			}
		}    

        CloseHandle(hFile);
    }
}

LPTSTR lpszSlotName = TEXT("\\\\.\\mailslot\\sample_mailslot");
void OzIPCDlg::StartMailslotServer()
{
	HANDLE hSlot;
	hSlot = CreateMailslot(lpszSlotName, 
		MSG_SIZE_MAX,                             // no maximum message size 
		MAILSLOT_WAIT_FOREVER,         // no time-out for operations 
		(LPSECURITY_ATTRIBUTES) NULL); // default security

	if (hSlot == INVALID_HANDLE_VALUE) 
	{ 
		CString msg;
		msg.Format(_T("CreateMailslot failed with %d\n"), GetLastError());
		Dump((LPCTSTR)msg);
		return; 
	} 
	HANDLE hEvent;
	OVERLAPPED ov;

	hEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("ExampleSlot"));
	if( NULL == hEvent )
	{
		Dump("Failed to create event");
		return;
	}
	ov.Offset = 0;
	ov.OffsetHigh = 0;
	ov.hEvent = hEvent;

	boost::shared_array<BYTE> data_buf(new BYTE[MSG_SIZE_MAX]);
	__int64 total_recv_size = 0;
	DWORD cbMessage = 0;
	DWORD cMessage = 0;
	DWORD cbRead = 0;
	while (total_recv_size<MSG_SIZE_MAX*MAX_MSG_COUNT)
	{
		do 
		{
			BOOL fResult = GetMailslotInfo( hSlot, // mailslot handle 
				(LPDWORD) NULL,               // no maximum message size 
				&cbMessage,                   // size of next message 
				&cMessage,                    // number of messages 
				(LPDWORD) NULL);              // no read time-out 
			if (cMessage <= 0)
			{
				WaitForSingleObject(ov.hEvent, 10);
				break;
			}
			fResult = ReadFile(hSlot, 
				data_buf.get(), 
				cbMessage, 
				&cbRead, 
				NULL); 
			if (cbRead>0)
				total_recv_size += cbRead;
			if (   (cbMessage != MSG_SIZE_MAX)
				|| (cbRead != MSG_SIZE_MAX) )
			{
				std::stringstream ss;
				ss << __FUNCTION__ << " cbMessage=" << cbMessage
					<< ", cbRead=" << cbRead;
				Dump(ss.str());
				break;
			}
			if (total_recv_size>=MSG_SIZE_MAX*MAX_MSG_COUNT)
				break;
		} while (cMessage>0);		
	}
	CloseHandle(hEvent);
	CloseHandle(hSlot);
}

void OzIPCDlg::StartMailslotClient()
{
	HANDLE hFile; 
	boost::shared_array<BYTE> data_buf(new BYTE[MSG_SIZE_MAX]);
	PerformanceProfile profile( __FUNCTION__);

	hFile = CreateFile(lpszSlotName, 
		GENERIC_WRITE, 
		FILE_SHARE_READ,
		(LPSECURITY_ATTRIBUTES) NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		(HANDLE) NULL); 

	if (hFile == INVALID_HANDLE_VALUE) 
	{ 
		Dump("CreateFile failed with %d.\n"); 
		return; 
	} 
	HANDLE hEvent;
	OVERLAPPED ov;

	hEvent = OpenEvent(SYNCHRONIZE, FALSE, TEXT("ExampleSlot"));
	if( NULL == hEvent )
	{
		Dump("Failed to open event");
		return;
	}
	ov.Offset = 0;
	ov.OffsetHigh = 0;
	ov.hEvent = hEvent;

	for (std::size_t i=0; i<MAX_MSG_COUNT; i++)
	{
		memset(data_buf.get(), i, MSG_SIZE_MAX);
		std::size_t total_send = 0;
		while(total_send<MSG_SIZE_MAX)
		{
			BOOL fResult; 
			DWORD current_send; 

			fResult = WriteFile(hFile, 
				data_buf.get()+total_send, 
				MSG_SIZE_MAX-total_send,  
				&current_send, 
				(LPOVERLAPPED) NULL); 
			SetEvent(hEvent);
			if (!fResult) 
			{ 
				std::stringstream ss;
				ss << __FUNCTION__ << " WriteFile failed with " << GetLastError();
				Dump(ss.str()); 
				return; 
			} 
			if (current_send>0)
				total_send+= current_send;
			else
				break;
		}
	}
	CloseHandle(hEvent);
	CloseHandle(hFile); 
}
//void OzIPCDlg::OnEnChange(UINT nID )
//{
//	// synchronize change from server to client.
//	if (::GetActiveWindow()==GetSafeHwnd())
//	{
//		if (g_shm_server.get())
//			OnBnClickedUpdateClient();
//	}
//}
