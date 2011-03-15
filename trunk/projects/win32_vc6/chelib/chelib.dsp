# Microsoft Developer Studio Project File - Name="chelib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=chelib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "chelib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "chelib.mak" CFG="chelib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "chelib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "chelib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "chelib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../build/win32_vc6/Release/chelib.lib"

!ELSEIF  "$(CFG)" == "chelib - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../build/win32_vc6/Debug/chelib.lib"

!ENDIF 

# Begin Target

# Name "chelib - Win32 Release"
# Name "chelib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "pdf"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\pdf\che_pdf_filter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\pdf\che_pdf_objects.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\pdf\che_pdf_parser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\pdf\che_pdf_xref.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\src\che_base.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\che_bitmap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\che_datastructure.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\che_dynbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\che_string.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "pdf header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\include\pdf\che_pdf_define.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\pdf\che_pdf_filter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\pdf\che_pdf_objects.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\pdf\che_pdf_parser.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\pdf\che_pdf_xref.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\include\che_base.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\che_bitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\che_datastructure.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\che_define.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\che_dynBuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\che_string.h
# End Source File
# End Group
# End Target
# End Project
