#include "stdafx.h"
#include "lut_creator.h"
#include <strsafe.h>
//#include "md5.h"
//#include "self_verify.h"

UINT8 exe_digest[]=
{
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,
	0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,
	0x1d,0x7a,0x9d,0x6b,0xf5,0x90,0x2a,0x9c,
	0x91,0xec,0xef,0x56,0xa9,0xfa,0x97,0xe5
};
std::basic_string<TCHAR> to_string(const BYTE* pdigest,int size)
{
	std::basic_stringstream<TCHAR> oss;
	for(int i=0;i<size;i++)
	{
		oss<<std::uppercase <<std::setw(2)<<std::setfill(_T('0'))<<std::hex<< (int)pdigest[i];
	}
	return oss.str();
}



//void output_self_digest()
//{
//	//print digest of executable, depends on exe_digest
//	//modify the exe to correct its digest.
//#ifdef _DEBUG
//	return;
//#endif
//	rename_file();
//	bool bret = false;
//	HANDLE hFile = NULL; 
//	TCHAR buf[MAX_PATH+1024];
//	TCHAR out_file_name[MAX_PATH + 1024];
//	buf[0] = 0;
//	out_file_name[0] = 0;
//	bool rename_file = false;
//	try
//	{
//		DWORD dwret = GetModuleFileName(NULL,buf,sizeof(buf)/sizeof(buf[0]) );
//		if(dwret<=0||dwret>=sizeof(buf)/sizeof(buf[0]))
//		{
//			bret = false;
//		}
//		else
//		{
//			StringCchCopy(out_file_name,sizeof(out_file_name)/sizeof(out_file_name[0]),buf);
//			int dot_pos = _tcslen(out_file_name)-1;
//			while(dot_pos>0)
//			{
//				if(out_file_name[dot_pos] == _T('.'))
//				{
//					out_file_name[dot_pos] = 0;
//					StringCchCat(out_file_name,sizeof(out_file_name)/sizeof(out_file_name[0]),_T("r.exe"));
//					break;
//				}
//				dot_pos--;
//			}
//			hFile = CreateFile(buf,          
//				GENERIC_READ,              // open for reading 
//				FILE_SHARE_READ,           // share for reading 
//				NULL,                      // no security 
//				OPEN_EXISTING,             // existing file only 
//				FILE_ATTRIBUTE_NORMAL,     // normal file 
//				NULL);                     // no attr.  
//			if (hFile == INVALID_HANDLE_VALUE) 
//			{ 
//				bret = false;
//				return;
//			}
//			char filebuf[1024+sizeof(exe_digest)];
//			CryptoPP::SHA256 md;
//			CryptoPP::SecByteBlock digest(md.DigestSize());
//			bool bfound = false;
//			int digest_offset = 0;
//			while(true)
//			{
//				DWORD nread = 0;
//				ReadFile(hFile,filebuf,sizeof(filebuf),&nread,NULL);
//				if(nread>0)
//				{
//					if(nread>=sizeof(exe_digest))
//					{
//						if(!bfound)
//						{
//							for(size_t i=0;i<=nread-sizeof(exe_digest);i++)
//							{
//								if(memcmp(filebuf+i,exe_digest,sizeof(exe_digest))==0)
//								{
//									digest_offset = SetFilePointer(hFile,0,NULL,FILE_CURRENT)- nread +i;
//									bfound = true;
//									md.Update( (const byte*)filebuf, i);
//									md.Update( (const byte*)filebuf+i+sizeof(exe_digest),nread-i-sizeof(exe_digest));
//									break;
//								}
//							}
//							if(!bfound)
//							{
//								if(nread>sizeof(exe_digest))
//								{
//									md.Update( (const byte*)filebuf, nread-sizeof(exe_digest));
//									SetFilePointer(hFile,0-sizeof(exe_digest),NULL,FILE_CURRENT);
//								}
//								else
//								{
//									md.Update( (const byte*)filebuf, nread);
//									break;
//								}
//							}
//
//						}
//						else
//						{
//							md.Update( (const byte*)filebuf, nread);
//						}
//					}
//					else
//					{
//						md.Update( (const byte*)filebuf, nread);
//					}
//				}
//				else
//					break;
//			}
//			md.Final(digest);
//			CryptoPP::MD5 key_md5;
//			key_md5.Update(exe_digest,md.DigestSize());
//			CryptoPP::SecByteBlock key_digest(key_md5.DigestSize());
//			key_md5.Final(key_digest);
//			if( memcmp( (const byte*)key_digest, exe_digest+md.DigestSize(),key_md5.DigestSize())==0)
//			{
//				//std::ostringstream oss;
//				//oss<<buf << std::endl;
//				//oss<< "digiest is:"<< to_string( (const BYTE*)digest,md.DigestSize())<<std::endl;
//				//oss<<"exe_digest:"<<to_string(exe_digest,md.DigestSize())<<std::endl;
//				//oss<<"digest offset:"<<digest_offset<<"(0x"<<std::hex<<std::uppercase<<digest_offset<<")";
//				//OutputDebugString(oss.str().c_str());
//				//modify digest of the exe file
//				HANDLE hOutFile = CreateFile(out_file_name,          
//					GENERIC_WRITE,              // open for reading 
//					FILE_SHARE_READ,           // share for reading 
//					NULL,                      // no security 
//					CREATE_ALWAYS,             // existing file only 
//					FILE_ATTRIBUTE_NORMAL,     // normal file 
//					NULL);                     // no attr.  
//				if (hOutFile == INVALID_HANDLE_VALUE) 
//				{ 
//					bret = false;
//					return;
//				}
//				SetFilePointer(hFile,0,NULL,FILE_BEGIN);
//				int totalread = 0;
//				int totalwrite = 0;
//				DWORD nread = 0;
//				do{
//					ReadFile(hFile,filebuf,sizeof(filebuf),&nread,NULL);
//					if(nread>0)
//					{
//						totalread += nread;
//						DWORD byteswriten = 0;
//						if(totalread<=digest_offset||totalwrite>=digest_offset+md.DigestSize())
//						{
//							WriteFile(hOutFile,filebuf,nread,&byteswriten, NULL);
//							totalwrite += byteswriten;
//						}
//						else
//						{
//							WriteFile(hOutFile,filebuf,digest_offset-totalwrite,&byteswriten, NULL);
//							WriteFile(hOutFile,digest,md.DigestSize(),&byteswriten, NULL);
//							totalwrite = digest_offset + md.DigestSize();
//							totalread = SetFilePointer(hFile,totalwrite,NULL,FILE_BEGIN);
//						}
//					}
//				}
//				while(nread>0);
//				if(hOutFile)
//				{
//					CloseHandle(hOutFile);
//					rename_file = true;
//				}
//			}
//		}
//	}
//	catch(...)
//	{
//		bret = false;
//	}
//	if(hFile)
//	{
//		CloseHandle(hFile);
//	}
//	if(rename_file)
//	{
//		//create a second process to rename the file
//		STARTUPINFO si;
//		PROCESS_INFORMATION pi;
//
//		ZeroMemory( &si, sizeof(si) );
//		si.cb = sizeof(si);
//		ZeroMemory( &pi, sizeof(pi) );
//		TCHAR cmdbuf[4096];
//		LONG hcurrent_proc = (LONG)(GetCurrentProcessId());
//		StringCchPrintf(cmdbuf,sizeof(cmdbuf)/sizeof(cmdbuf[0]),"\"%s\" -PID%d-name%s",
//			out_file_name,(LONG)hcurrent_proc,buf);
//		// Start the child process. 
//		if( !CreateProcess( out_file_name, // No module name (use command line). 
//			cmdbuf, // Command line. 
//			NULL,             // Process handle not inheritable. 
//			NULL,             // Thread handle not inheritable. 
//			FALSE,            // Set handle inheritance to FALSE. 
//			0,                // No creation flags. 
//			NULL,             // Use parent's environment block. 
//			NULL,             // Use parent's starting directory. 
//			&si,              // Pointer to STARTUPINFO structure.
//			&pi )             // Pointer to PROCESS_INFORMATION structure.
//			) 
//		{
//			//ErrorExit( "CreateProcess failed." );
//		}
//		// Close process and thread handles. 
//		CloseHandle( pi.hProcess );
//		CloseHandle( pi.hThread );
//		ExitProcess(0);
//	}
//}
//bool verify_exe()
//{
//#ifdef _DEBUG
//	return true;
//#endif
//	bool bret = false;
//	try
//	{
//		TCHAR buf[MAX_PATH+1024];
//		DWORD dwret = GetModuleFileName(NULL,buf,sizeof(buf)/sizeof(buf[0]) );
//		if(dwret<=0||dwret>=sizeof(buf)/sizeof(buf[0]))
//		{
//			return false;
//		}
//		else
//		{
//			HANDLE hFile; 
//			hFile = CreateFile(buf,           
//				GENERIC_READ,              // open for reading 
//				FILE_SHARE_READ,           // share for reading 
//				NULL,                      // no security 
//				OPEN_EXISTING,             // existing file only 
//				FILE_ATTRIBUTE_NORMAL,     // normal file 
//				NULL);                     // no attr.  
//			if (hFile == INVALID_HANDLE_VALUE) 
//			{ 
//				return false;
//			}
//			char filebuf[1024+sizeof(exe_digest)];
//			CryptoPP::SHA256 md;
//			CryptoPP::SecByteBlock digest(md.DigestSize());
//			bool bfound = false;
//			int total_read = 0;
//			int digest_offset = 0;
//			while(true)
//			{
//				DWORD nread = 0;
//				ReadFile(hFile,filebuf,sizeof(filebuf),&nread,NULL);
//				if(nread>0)
//				{
//					if(nread>=sizeof(exe_digest))
//					{
//						if(!bfound)
//						{
//							for(size_t i=0;i<=nread-sizeof(exe_digest);i++)
//							{
//								if(memcmp(filebuf+i,exe_digest,sizeof(exe_digest))==0)
//								{
//									digest_offset = SetFilePointer(hFile,0,NULL,FILE_CURRENT)- nread +i;
//									bfound = true;
//									md.Update( (const byte*)filebuf, i);
//									md.Update( (const byte*)filebuf+i+sizeof(exe_digest),nread-i-sizeof(exe_digest));
//									break;
//								}
//							}
//							if(!bfound)
//							{
//								if(nread>sizeof(exe_digest))
//								{
//									md.Update( (const byte*)filebuf, nread-sizeof(exe_digest));
//									SetFilePointer(hFile,0-sizeof(exe_digest),NULL,FILE_CURRENT);
//								}
//								else
//								{
//									md.Update( (const byte*)filebuf, nread);
//									break;
//								}
//							}
//
//						}
//						else
//						{
//							md.Update( (const byte*)filebuf, nread);
//						}
//					}
//					else
//					{
//						md.Update( (const byte*)filebuf, nread);
//					}
//				}
//				else
//					break;
//			}
//			md.Final(digest);
//			if(memcmp(exe_digest,(const BYTE*)digest,md.DigestSize())==0)
//			{
//				bret = true;
//			}
//		}
//	}
//	catch(...)
//	{
//		bret = false;
//	}
//	if(!bret)
//	{
//		ExitProcess(__LINE__);
//	}
//	return bret;
//}

