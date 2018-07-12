##################################################
#
# Generic Makefile for ACE_Proactor Program.
#
# Author: zdx (zdxster AT gmail DOT com)
# Date: 2006/05/08

### Variables: ###
CC = cl.exe
RC = rc.exe
LINK = link.exe
PROGRAM = server
OBJS = ..\output
EXECFILE = $(OBJS)\$(PROGRAM).exe
ACEPATH = $(DEVROOT)\deps\ACE_wrappers

CMNCCFLAGS = /D "_UNICODE" /D "UNICODE" /Od /D "WIN32" /D "_CONSOLE" /EHsc /GS /W3 /nologo /c /Wp64 /TP
CMNLDFLAGS = /NOLOGO /SUBSYSTEM:CONSOLE /MACHINE:X86 /pdb:"$(OBJS)\$(PROGRAM).pdb" /out:"$(EXECFILE)" 
CMNLDLIBS = kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib

!IF "$(BUILD)" == ""
BUILD = Debug
!ENDIF

!IF "$(BUILD)" == "Release"
CCFLAGS = $(CMNCCFLAGS) /MD /FD /Zi /D "NDEBUG"
EXTRAFLAGS = /I $(ACEPATH) /I. 
LDFLAGS = $(CMNLDFLAGS) /INCREMENTAL:NO 
LDPATH = $(ACEPATH)\release
LDLIBS = $(CMNLDLIBS) ACE.lib 

!ELSEIF "$(BUILD)" == "Debug"
CCFLAGS = $(CMNCCFLAGS) /Gm /RTC1 /MDd /ZI /D "_DEBUG" 
EXTRAFLAGS = /I $(ACEPATH) /I. 
LDFLAGS = $(CMNLDFLAGS) /incremental:yes /debug 
LDPATH = $(ACEPATH)\debug
LDLIBS = $(CMNLDLIBS) ACEd.lib 
!ENDIF

!IF "$(BUILD)" == "Release" || "$(BUILD)" == "Debug"
ALL_OBJECTS = \
		$(OBJS)\mytask.obj \
		$(OBJS)\acceptor.obj \
		$(OBJS)\server.obj \
		$(OBJS)\simple.obj

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
 	$(LINK) /OUT:$@ $(LDFLAGS) $** /LIBPATH:$(LDPATH) $(LDLIBS)

data: 
$(OBJS)\mytask.obj: .\mytask.cpp
	$(CC) $(CCFLAGS) /Fo$@ $(EXTRAFLAGS) $**

$(OBJS)\acceptor.obj: .\acceptor.cpp
	$(CC) $(CCFLAGS) /Fo$@ $(EXTRAFLAGS) $**

$(OBJS)\server.obj: .\server.cpp
	$(CC) $(CCFLAGS) /Fo$@ $(EXTRAFLAGS) $**

$(OBJS)\simple.obj: .\simple.cpp
	$(CC) $(CCFLAGS) /Fo$@ $(EXTRAFLAGS) $**

!ENDIF 

### End of the Makefile ## All rights reserved ###
##################################################
