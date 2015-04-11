#include "stdafx.h"
#include "greenroaddlg.h"
#include <jni.h>
#pragma comment(lib,"jvm.lib")
#include <comdef.h>
#pragma comment(lib,"comsupp")
#include <shlwapi.h>
#include <Dbghelp.h>
#pragma comment(lib,"dbghelp")
#include <atlbase.h>
#include <strsafe.h>

#define PATH_SEPARATOR ';' /* define it to be ':' on Solaris */
LPCSTR java_cls_name = "UnZip2";
struct ReportTime
{
	DWORD m_StartTime;
	std::basic_string<TCHAR> m_name;
	ReportTime(LPCTSTR name)
	{
		m_name = name;
		m_StartTime = GetTickCount();
	}
	~ReportTime()
	{
		CString msg;
		msg.Format(_T("%s use %d seconds"),m_name.c_str(),(GetTickCount()-m_StartTime)/1000);
		AfxMessageBox(msg);
	}
};
jstring NewJString(JNIEnv* env,LPCTSTR str) 
{ 
	if(!env || !str) 
		return 0; 
	int slen = _tcslen(str); 
#ifdef _UNICODE
	jstring js = env->NewString(str,slen); 
#else
	jchar* buffer = new jchar[slen]; 
	int len = MultiByteToWideChar(CP_ACP,0,str,strlen(str),buffer,slen); 
	if(len>0 && len < slen) 
		buffer[len]=0; 
	jstring js = env->NewString(buffer,len); 
	delete [] buffer; 
#endif
	return js; 
} 
/** 
第一个参数是虚拟机的环境指针 
第二个参数为待转换的Java字符串定义 
第三个参数是本地存储转换后字符串的内存块 
第三个参数是内存块的大小 
*/ 
int JStringToChar(JNIEnv *env, jstring str, LPTSTR desc, int desc_len) 
{ 
	int len = 0; 
	if(desc==NULL||str==NULL) 
		return -1; 
	//在VC中wchar_t是用来存储宽字节字符(UNICODE)的数据类型 
	wchar_t *w_buffer = new wchar_t[1024]; 
	ZeroMemory(w_buffer,1024*sizeof(wchar_t)); 
	//使用GetStringChars而不是GetStringUTFChars 
	StringCchCopyW(w_buffer,sizeof(w_buffer)/sizeof(w_buffer[0]),env->GetStringChars(str,0)); 
	//wcscpy(w_buffer,env->GetStringChars(str,0)); 
	env->ReleaseStringChars(str,w_buffer); 
	ZeroMemory(desc,desc_len); 
	//调用字符编码转换函数(Win32 API)将UNICODE转为ASCII编码格式字符串 
	//关于函数WideCharToMultiByte的使用请参考MSDN 
#ifdef _UNICODE
	StringCchCopy(desc,desc_len,w_buffer); 
#else
	len = WideCharToMultiByte(CP_ACP,0,w_buffer,1024,desc,desc_len,NULL,NULL); 
#endif
	//len = wcslen(w_buffer); 
	if(len>0 && len<desc_len) 
		desc[len]=0; 
	delete[] w_buffer; 
	return _tcslen(desc); 
} 
JNIEnv *static_env = NULL;
JavaVM *static_jvm = NULL;
std::basic_string<TCHAR> get_app_dir()
{
	TCHAR buf[PATH_LEN];
	buf[0] = _T('\0');
	int buf_length = sizeof(buf)/sizeof(buf[0]);
	DWORD dwret = GetModuleFileName(NULL,buf, buf_length);
	if(dwret == buf_length)
	{
		buf[0] = 0;
	}
	std::basic_string<TCHAR> directory;
	std::basic_string<TCHAR> application_path = buf;
	std::basic_string<TCHAR>::size_type find_position = application_path.rfind(_T('\\'));
	if(find_position != std::basic_string<TCHAR>::npos)
	{
		directory = application_path.substr(0,find_position+1);
	}
	return directory;
}
// DoesFileExist : Determines whether a specified file exists
// Parameters:
//     pszFile - File name to check.
// Returns TRUE if the specified file is found, or FALSE otherwise.
/////////////////////////////////////////////////////////////////////
BOOL DoesFileExist(LPCTSTR pszFile)
{
	if (pszFile == NULL)
	{
		return FALSE;
	}

	if (lstrlen(pszFile) > MAX_PATH)
	{
		return FALSE;
	}

	// We don't want any error message box to pop up when we try to test
	// if the required file is available to open for read.

	UINT uErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

	HANDLE hFile = CreateFile(pszFile, GENERIC_READ, FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	SetErrorMode(uErrorMode);  // restore error mode

	if (INVALID_HANDLE_VALUE  == hFile) 
	{
		return FALSE;    
	}

	CloseHandle(hFile);
	return TRUE;
} 
struct jvm_killer
{
	bool m_jvm_exist;
	HMODULE m_mod_jvm;
	jvm_killer()
	{
		m_jvm_exist = false;
		m_mod_jvm = NULL;
		CRegKey reg;
		LPCTSTR java_root = _T("SOFTWARE\\JavaSoft\\Java Runtime Environment");
		if(ERROR_SUCCESS==reg.Open(HKEY_LOCAL_MACHINE,java_root,KEY_READ))
		{
			TCHAR buf[PATH_LEN];
			memset(buf,0,sizeof(buf));
			ULONG buf_len = sizeof(buf)/sizeof(buf[0]);
			reg.QueryStringValue(_T("CurrentVersion"),buf,&buf_len);
			TCHAR rt_name[PATH_LEN];
			memset(rt_name,0,sizeof(rt_name));
			StringCchPrintf(rt_name,sizeof(rt_name)/sizeof(rt_name[0]),_T("%s\\%s"),java_root,buf);
			CRegKey rt;
			if(ERROR_SUCCESS==rt.Open(HKEY_LOCAL_MACHINE,rt_name,KEY_READ))
			{
				memset(buf,0,sizeof(buf));
				buf_len = sizeof(buf)/sizeof(buf[0]);
				rt.QueryStringValue(_T("RuntimeLib"),buf,&buf_len);
				if(_tcslen(buf)>0)
				{
					m_mod_jvm = LoadLibrary(buf);
					if(m_mod_jvm)
					{
						m_jvm_exist = true;
					}
				}
			}
		}
		if(!m_jvm_exist)
		{
			AfxMessageBox(_T("must install java runtime.\n download from http://java.sun.com/downloads/index.html"));
		}
		else
		{
			std::basic_string<TCHAR> zip_cls_name = get_app_dir();
			zip_cls_name += (LPCTSTR)_bstr_t(java_cls_name);
			zip_cls_name += _T(".class");
			if(!DoesFileExist(zip_cls_name.c_str()))
			{
				extract_cls(zip_cls_name);
				if(!DoesFileExist(zip_cls_name.c_str()))
				{
					m_jvm_exist = false;
					CString msg;
					msg.Format(_T("missing file: %s"),zip_cls_name.c_str());
					AfxMessageBox(msg);
				}
			}
		}
	}
	~jvm_killer()
	{
		if(m_jvm_exist)
		{
			if(static_jvm)
			{
				static_jvm->DestroyJavaVM();
			}
		}
		if(m_mod_jvm)
			FreeLibrary(m_mod_jvm);
	}
	void extract_cls(std::basic_string<TCHAR> filename)
	{
		HRSRC hResLoad = NULL;     // handle to loaded resource 
		HMODULE hExe = NULL;        // handle to existing .EXE file 
		HRSRC hRes = NULL;         // handle/ptr. to res. info. in hExe 
		LPVOID lpResLock = NULL;    // pointer to resource data 
		hRes = FindResource(hExe, MAKEINTRESOURCE(IDR_JAVA_FILE), _T("BIN")); 
		if (hRes == NULL) 
		{ 
			//Verbose(_T("Could not locate resource.")); 
			return;
		} 
		hResLoad = (HRSRC)LoadResource(hExe, hRes); 
		if (hResLoad == NULL) 
		{ 
			return;
			//Verbose(_T("Could not load resource.")); 
		} 
		lpResLock = LockResource(hResLoad); 
		if (lpResLock == NULL) 
		{ 
			//Verbose(_T("Could not lock resource.")); 
			return;
		} 
		int fileSize = SizeofResource(hExe, hRes);
		CFile cls_file;
		if(cls_file.Open(filename.c_str(),CFile::modeCreate|CFile::modeWrite))
		{
			cls_file.Write(lpResLock,fileSize);
			cls_file.Close();
		}
	}
};


jvm_killer jvm_manager;
jint create_jvm(JNIEnv **penv,JavaVM **pjvm)
{
	jint res;
	if( static_env && static_jvm)
	{
		*pjvm = static_jvm;
		*penv = static_env;
		return 0;
	}
	TCHAR buf[PATH_LEN];
	buf[0] = _T('\0');
	int buf_length = sizeof(buf)/sizeof(buf[0]);
	std::basic_string<TCHAR> directory = get_app_dir();
	StringCchPrintf(buf,sizeof(buf)/sizeof(buf[0]),_T("-Djava.class.path=%s"),directory.c_str());
	if(jvm_manager.m_jvm_exist)
	{
#ifdef JNI_VERSION_1_2
	JavaVMInitArgs vm_args;
	JavaVMOption options[1];
	options[0].optionString = (LPSTR)(LPCSTR)_bstr_t(buf);
		//"-Djava.class.path=C:\\test\\java\\compress" /*USER_CLASSPATH*/;
	vm_args.version = 0x00010002;
	vm_args.options = options;
	vm_args.nOptions = 1;
	vm_args.ignoreUnrecognized = JNI_TRUE;
	/* Create the Java VM */
	res = JNI_CreateJavaVM(pjvm, (void**)penv, &vm_args);
#else
	JDK1_1InitArgs vm_args;
	char classpath[1024];
	vm_args.version = 0x00010001;
	JNI_GetDefaultJavaVMInitArgs(&vm_args);
	/* Append USER_CLASSPATH to the default system class path */
	sprintf(classpath, "%s%c%s",
		vm_args.classpath, PATH_SEPARATOR,(LPSTR)(LPCSTR)_bstr_t(buf) /*USER_CLASSPATH*/);
	vm_args.classpath = classpath;
	/* Create the Java VM */
	res = JNI_CreateJavaVM(pjvm, penv, &vm_args);
#endif /* JNI_VERSION_1_2 */
		static_env = *penv;
		static_jvm = *pjvm;
		return res;
	}
	else
		return -1;
}
std::basic_string<TCHAR> CGreenroadDlg::zip()
{
	//return zip file name
	//return "" if zip failed
	//http://java.sun.com/docs/books/jni/html/invoke.html#28719
	int item_count = m_lv_files.GetItemCount();
	if(item_count<1)
		return _T("");
	bool zip_ok = false;
	JNIEnv *env = NULL;
	JavaVM *jvm = NULL;
	jint res;
	jclass cls;
	jstring jstr;
	res = create_jvm(&env,&jvm);
	if (res < 0 ||NULL == jvm || NULL== env) 
	{
		CString msg;
		msg.Format(_T("can't create jvm, error code:%d"),res);
		AfxMessageBox(msg);
		return _T("");
	}

	cls = env->FindClass( java_cls_name);//
	if (cls == NULL) 
	{
		goto destroy;
	}
	jmethodID constructor_mid = env->GetMethodID(cls,"<init>","()V"); 
	jmethodID addFile_mid = env->GetMethodID(cls, "addFile","(Ljava/lang/String;)V");
	jmethodID compress_mid = env->GetMethodID(cls, "compress","(Ljava/lang/String;)Z");
	if(NULL==constructor_mid || NULL == addFile_mid || NULL == compress_mid)
	{
		AfxMessageBox(_T("failed to get method"));
		goto destroy;
	}
	jobject zip_obj = env->NewObject(cls,constructor_mid,0); 
	TCHAR src_path[PATH_LEN];
	for(int i=0;i<item_count;i++)
	{
		memset(src_path,0,sizeof(src_path));
		m_lv_files.GetItemText(i,0,src_path,sizeof(src_path)/sizeof(src_path[0]));
		jstr = env->NewStringUTF( (LPCSTR)_bstr_t(src_path));
		if (jstr == NULL) 
		{
			goto destroy;
		}
		env->CallVoidMethod(zip_obj,addFile_mid,jstr);
	}
	memset(src_path,0,sizeof(src_path));
	GetDlgItemText(IDE_RESULT_FILE,src_path,sizeof(src_path)/sizeof(src_path[0]));
	StringCchPrintf(src_path,sizeof(src_path)/sizeof(src_path[0]),_T("%stmp%d.zip"),get_work_dir().c_str(),GetTickCount());
	DeleteFile(src_path);
	jstr = NewJString( env,src_path );
	jboolean ret = env->CallBooleanMethod(zip_obj,compress_mid,jstr);
	if(0 == ret)
	{
		AfxMessageBox(_T("failed to compress files"));
		memset(src_path,0,sizeof(src_path));
	}
	else
	{
		zip_ok = true;
	}
destroy:
	if (env->ExceptionOccurred()) 
	{
		env->ExceptionDescribe();
	}
	//jvm->DestroyJavaVM();
	if(zip_ok)
		return src_path;
	return _T("");

}
void CGreenroadDlg::OnDo() 
{
	// TODO: Add your control notification handler code here
	if(!read_key())
		return;
	TCHAR src_path[PATH_LEN];
	TCHAR dest_path[PATH_LEN];
	src_path[0] = 0;
	dest_path[0] = 0;
	GetDlgItemText(IDE_RESULT_FILE,dest_path,sizeof(dest_path)/sizeof(TCHAR));
	GetDlgItemText(IDE_SOURCE_FILE,src_path,sizeof(src_path)/sizeof(TCHAR));
	if(_tcslen(dest_path)<1)
	{
		AfxMessageBox(_T("must set destination file name"));
		return;
	}
	if(m_lv_files.GetItemCount()<1)
	{
		AfxMessageBox(_T("must drag file/directory into the grid for processing"));
		return;
	}
	{
		ReportTime watch(__TFUNCTION__);
		std::basic_string<TCHAR> return_file = zip();
		if(return_file.length()>1)
		{
			common_step(m_key_buf,m_key_len,true,return_file.c_str(),dest_path);
			DeleteFile(return_file.c_str());
		}
	}
	
	//common_step(m_key_buf,m_key_len,true);
}

std::basic_string<TCHAR> CGreenroadDlg::get_work_dir()
{
	TCHAR path_buf[PATH_LEN];	
	memset(path_buf,0,sizeof(path_buf));
	//GetDlgItemText(IDE_RESULT_FILE,path_buf,sizeof(path_buf)/sizeof(path_buf[0]));
	GetTempPath(sizeof(path_buf)/sizeof(path_buf[0]),path_buf);
	int path_len = _tcslen(path_buf);
	if(PathIsDirectory(path_buf))
	{
		//add trailing slash if necessary
		if(path_buf[path_len-1] != _T('\\') )
			path_buf[path_len] = _T('\\'); //dont have to worry about null, for there should be a lot via memcpy
	}
	else
	{
		path_len--;
		while(path_len>0)
		{
			if(path_buf[path_len] == _T('\\') )
			{
				path_buf[path_len+1] = _T('\0');
				break;
			}
		}
	}
	return path_buf;
}

bool CGreenroadDlg::unzip(std::basic_string<TCHAR> zip_name,std::basic_string<TCHAR> dest_dir)
{
	//unzip to 
	bool zip_ok = false;
	JNIEnv *env = NULL;
	JavaVM *jvm = NULL;
	jint res;
	jclass cls;
	res = create_jvm(&env,&jvm);
	if (res < 0 ||NULL == jvm || NULL== env) 
	{
		CString msg;
		msg.Format(_T("can't create jvm, error code:%d"),res);
		AfxMessageBox(msg);
		return false;
	}
	cls = env->FindClass( java_cls_name);//
	if (cls == NULL) 
	{
		goto destroy;
	}
	jmethodID unzip_mid = env->GetStaticMethodID(cls, "unzipFile","(Ljava/lang/String;Ljava/lang/String;)V");
	if(NULL==unzip_mid )
	{
		AfxMessageBox(_T("failed to get method"));
		goto destroy;
	}
	jstring src_file = NewJString(env,zip_name.c_str());
	jstring dest_file_name = NewJString(env,dest_dir.c_str());
	env->CallStaticObjectMethod(cls,unzip_mid,src_file,dest_file_name);
destroy:
	if (env->ExceptionOccurred()) 
	{
		env->ExceptionDescribe();
	}
	//jvm->DestroyJavaVM();
	return true;
}

void CGreenroadDlg::OnUndo() 
{
	// TODO: Add your control notification handler code here
	if(!read_key())
		return;
	TCHAR src_path[PATH_LEN];
	TCHAR dest_path[PATH_LEN];
	src_path[0] = 0;
	memset(dest_path,0,sizeof(dest_path));
	GetDlgItemText(IDE_RESULT_FILE,dest_path,sizeof(dest_path)/sizeof(TCHAR));
	GetDlgItemText(IDE_SOURCE_FILE,src_path,sizeof(src_path)/sizeof(TCHAR));
	if(_tcslen(src_path)<1)
	{
		AfxMessageBox(_T("must choose source file"));
		return;
	}
	if(_tcslen(dest_path)<1)
	{
		AfxMessageBox(_T("must choost destionation directory\n you can type it into the input box,")
			_T("\n or click button to select")
			_T("\n or drag a directory into the input box"));
		return;
	}
	if(dest_path[_tcslen(dest_path)-1]!=_T('\\'))
	{
		dest_path[_tcslen(dest_path)] = _T('\\');
	}
	MakeSureDirectoryPathExists( (LPCSTR)_bstr_t(dest_path));
	if(!PathIsDirectory(dest_path))
	{
		AfxMessageBox(_T("destination must be a directory name"));
		return;
	}
	std::basic_string<TCHAR> plain_file = get_temp_filename();
	{
		ReportTime watch(__TFUNCTION__);
		common_step(m_key_buf,m_key_len,false,src_path,plain_file.c_str());
		unzip(plain_file,dest_path);
		DeleteFile(plain_file.c_str());
	}

}
std::basic_string<TCHAR> CGreenroadDlg::get_temp_filename()
{
	TCHAR src_path[PATH_LEN];
	memset(src_path,0,sizeof(src_path));
	GetDlgItemText(IDE_RESULT_FILE,src_path,sizeof(src_path)/sizeof(src_path[0]));
	StringCchPrintf(src_path,sizeof(src_path)/sizeof(src_path[0]),_T("%stmp%d.tmp"),get_work_dir().c_str(),GetTickCount());
	return src_path;
}
