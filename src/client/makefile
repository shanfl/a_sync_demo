##################################################
#
# Generic Makefile for wxWidgets Program.
#
# Author: zdx (zdxster AT gmail DOT com)
# Date: 2006/05/08

### Variables: ###
CC = cl.exe
RC = rc.exe
LINK = link.exe
CMNCCFLAGS = /D "_UNICODE" /D "UNICODE" /Od /D "WIN32" /D "WXUSINGDLL" /EHsc /GS /W3 /nologo /c /Wp64 /TP 
CMNLDFLAGS = /NOLOGO /SUBSYSTEM:WINDOWS
CMNLDLIBS = kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib
PROGRAM = demo
OBJS = ..\output
EXECFILE = $(OBJS)\$(PROGRAM).exe
WXPATH = $(DEVROOT)\deps\wxWidgets
ACEPATH = $(DEVROOT)\deps\ACE_wrappers

!IF "$(BUILD)" == ""
BUILD = Debug
!ENDIF

!IF "$(BUILD)" == "Release"
CCFLAGS = $(CMNCCFLAGS) /MD /FD /Zi /D "NDEBUG" 
EXTRAFLAGS = /I $(WXPATH) /I $(WXPATH)\release\mswu /I $(ACEPATH) /I. 
RCFLAGS = /d WIN32 /d _WINDOWS /d NOPCH 
LDFLAGS = $(CMNLDFLAGS) /INCREMENTAL:NO 
LDPATH = /LIBPATH:$(WXPATH)\release /LIBPATH:$(ACEPATH)\release
LDLIBS = $(CMNLDLIBS) wxadv26u.lib wxbase26u.lib wxcore26u.lib wxhtml26u.lib ACE.lib 

!ELSEIF "$(BUILD)" == "Debug"
CCFLAGS = $(CMNCCFLAGS) /Gm /RTC1 /MDd /ZI /D "_DEBUG" /D __WXMSW__ /D__WXDEBUG__
EXTRAFLAGS = /I $(WXPATH) /I $(WXPATH)\debug\mswud /I $(ACEPATH) /I. 
RCFLAGS = /d WIN32 /d _DEBUG /d __WXMSW__ /d __WXDEBUG__ /d _WINDOWS /d NOPCH 
LDFLAGS = $(CMNLDFLAGS) /incremental:yes /debug 
LDPATH = /LIBPATH:$(WXPATH)\debug /LIBPATH:$(ACEPATH)\debug
LDLIBS = $(CMNLDLIBS) wxadv26ud.lib wxbase26ud.lib wxcore26ud.lib wxhtml26ud.lib wxnet26ud.lib ACEd.lib 
!ENDIF

!IF "$(BUILD)" == "Release" || "$(BUILD)" == "Debug"
ALL_OBJECTS = \
		$(OBJS)\test.obj \
		$(OBJS)\engine.obj \
		$(OBJS)\mytask.obj \
		$(OBJS)\Req_TimeOut_Handler.obj \
		$(OBJS)\connector.obj \
		$(OBJS)\client.obj \
		$(OBJS)\test.res

### Conditionally setting: ###
all: $(OBJS)
$(OBJS):
	-if not exist $(OBJS) mkdir $(OBJS)

### Targets: ###
all: $(EXECFILE) data

clean: 
	-if exist $(OBJS)\*.obj del $(OBJS)\*.obj
	-if exist $(OBJS)\*.res del $(OBJS)\*.res
	-if exist $(OBJS)\*.pch del $(OBJS)\*.pch
	-if exist $(OBJS)\*.ilk del $(OBJS)\*.ilk
	-if exist $(OBJS)\*.pdb del $(OBJS)\*.pdb
	-if exist $(OBJS)\$(PROGRAM).exe del $(OBJS)\$(PROGRAM).exe

$(EXECFILE): $(ALL_OBJECTS)
 	$(LINK) /OUT:$@ $(LDFLAGS) $** $(LDPATH) $(LDLIBS)

data: 
$(OBJS)\test.obj: .\test.cpp
	$(CC) $(CCFLAGS) /Fo$@ $(EXTRAFLAGS) $**

$(OBJS)\engine.obj: .\engine.cpp
	$(CC) $(CCFLAGS) /Fo$@ $(EXTRAFLAGS) $**

$(OBJS)\mytask.obj: .\mytask.cpp
	$(CC) $(CCFLAGS) /Fo$@ $(EXTRAFLAGS) $**

$(OBJS)\Req_TimeOut_Handler.obj: .\Req_TimeOut_Handler.cpp
	$(CC) $(CCFLAGS) /Fo$@ $(EXTRAFLAGS) $**

$(OBJS)\connector.obj: .\connector.cpp
	$(CC) $(CCFLAGS) /Fo$@ $(EXTRAFLAGS) $**

$(OBJS)\client.obj: .\client.cpp
	$(CC) $(CCFLAGS) /Fo$@ $(EXTRAFLAGS) $**

$(OBJS)\test.res: .\test.rc
	$(RC) /fo$@ $(RCFLAGS) $(EXTRAFLAGS) $**

!ENDIF 

### End of the Makefile ## All rights reserved ###
##################################################
