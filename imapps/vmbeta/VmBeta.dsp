# Microsoft Developer Studio Project File - Name="VmBeta" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=VmBeta - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VmBeta.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VmBeta.mak" CFG="VmBeta - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VmBeta - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "VmBeta - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VmBeta - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "VmBeta - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "VmBeta - Win32 Release"
# Name "VmBeta - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "VmSource"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\iacciden.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ibitmapc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\icanvas.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ifstream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\igdibmp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\iplatfrm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ivmsbeta.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ivmsca.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\VmBeta.cpp
# End Source File
# Begin Source File

SOURCE=.\VmBetaV.cpp
# End Source File
# Begin Source File

SOURCE=.\VmWin.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "VmHeader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\iacciden.h
# End Source File
# Begin Source File

SOURCE=..\..\ibitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\ibitmapc.h
# End Source File
# Begin Source File

SOURCE=..\..\icanvas.h
# End Source File
# Begin Source File

SOURCE=..\..\ifstream.h
# End Source File
# Begin Source File

SOURCE=..\..\igdibmp.h
# End Source File
# Begin Source File

SOURCE=..\..\iini.h
# End Source File
# Begin Source File

SOURCE=..\..\ikeybord.h
# End Source File
# Begin Source File

SOURCE=..\..\iplatfrm.h
# End Source File
# Begin Source File

SOURCE=..\..\istream.h
# End Source File
# Begin Source File

SOURCE=..\..\istring.h
# End Source File
# Begin Source File

SOURCE=..\..\itemplat.h
# End Source File
# Begin Source File

SOURCE=..\..\ivms.h
# End Source File
# Begin Source File

SOURCE=..\..\ivmsbeta.h
# End Source File
# Begin Source File

SOURCE=..\..\ivmsca.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Terminal.h
# End Source File
# Begin Source File

SOURCE=.\VmBetaV.h
# End Source File
# Begin Source File

SOURCE=.\VmWin.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\IconMain.ico
# End Source File
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# End Group
# End Target
# End Project
