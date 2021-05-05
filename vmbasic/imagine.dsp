# Microsoft Developer Studio Project File - Name="imagine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=imagine - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "imagine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "imagine.mak" CFG="imagine - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "imagine - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "imagine - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "imagine - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "objs\release"
# PROP Intermediate_Dir "objs\release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /NODEFAULTLIB

!ELSEIF  "$(CFG)" == "imagine - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "objs\debug"
# PROP Intermediate_Dir "objs\debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "imagine - Win32 Release"
# Name "imagine - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\i_math.c
# End Source File
# Begin Source File

SOURCE=.\iacciden.cpp
# End Source File
# Begin Source File

SOURCE=.\ibitmapc.cpp
# End Source File
# Begin Source File

SOURCE=.\icanvas.cpp
# End Source File
# Begin Source File

SOURCE=.\idxdraw.cpp
# End Source File
# Begin Source File

SOURCE=.\ifstream.cpp
# End Source File
# Begin Source File

SOURCE=.\igdibmp.cpp
# End Source File
# Begin Source File

SOURCE=.\iini.cpp
# End Source File
# Begin Source File

SOURCE=.\iplatfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ivmsbeta.cpp
# End Source File
# Begin Source File

SOURCE=.\ivmsca.cpp
# End Source File
# Begin Source File

SOURCE=.\ivmscexp.cpp
# End Source File
# Begin Source File

SOURCE=.\ivmscs.cpp
# End Source File
# Begin Source File

SOURCE=.\ivmscv.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\EasyGl.h
# End Source File
# Begin Source File

SOURCE=.\i_math.h
# End Source File
# Begin Source File

SOURCE=.\iacciden.h
# End Source File
# Begin Source File

SOURCE=.\ibitmap.h
# End Source File
# Begin Source File

SOURCE=.\ibitmapc.h
# End Source File
# Begin Source File

SOURCE=.\icanvas.h
# End Source File
# Begin Source File

SOURCE=.\idxdraw.h
# End Source File
# Begin Source File

SOURCE=.\ifstream.h
# End Source File
# Begin Source File

SOURCE=.\igdibmp.h
# End Source File
# Begin Source File

SOURCE=.\iini.h
# End Source File
# Begin Source File

SOURCE=.\ikeybord.h
# End Source File
# Begin Source File

SOURCE=.\iplatfrm.h
# End Source File
# Begin Source File

SOURCE=.\iscreen.h
# End Source File
# Begin Source File

SOURCE=.\istream.h
# End Source File
# Begin Source File

SOURCE=.\istring.h
# End Source File
# Begin Source File

SOURCE=.\itemplat.h
# End Source File
# Begin Source File

SOURCE=.\ivector.h
# End Source File
# Begin Source File

SOURCE=.\ivms.h
# End Source File
# Begin Source File

SOURCE=.\ivmsbeta.h
# End Source File
# Begin Source File

SOURCE=.\ivmsca.h
# End Source File
# Begin Source File

SOURCE=.\ivmscc.h
# End Source File
# End Group
# End Target
# End Project
