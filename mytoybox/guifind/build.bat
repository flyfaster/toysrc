rem call C:\Progra~2\MI1E4E~1\vcbuildtools.bat amd64
call "C:\Program Files (x86)\Microsoft Visual C++ Build Tools\vcbuildtools.bat" amd64

cd /d %~dp0

cl /MD /Zi /W4 /EHsc /DWIN32 /D_CRT_SECURE_NO_DEPRECATE=1 /D_CRT_NON_CONFORMING_SWPRINTFS=1 /D_SCL_SECURE_NO_WARNINGS=1 /DUNICODE=1 /DWXUSINGDLL=1 /I C:\workspace\wxWidgets-3.1.0\include /I C:\workspace\wxWidgets-3.1.0\lib\vc140_x64_dll\mswu /I C:\workspace\boost_1_62_0 CustomEvent.cpp  FindThread.cpp  GuiFindApp.cpp  GuiFindDlg.cpp  VerboseDialog.cpp /link /LIBPATH:C:\workspace\wxWidgets-3.1.0\lib\vc140_x64_dll /LIBPATH:C:\workspace\boost_1_62_0\stage\lib /VERBOSE:LIB /out:guifind.exe wxmsw31u_adv.lib wxmsw31u_aui.lib wxmsw31u_stc.lib wxmsw31u_xrc.lib wxbase31u.lib wxmsw31u_core.lib