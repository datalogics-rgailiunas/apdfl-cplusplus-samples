# DLADD MattK 10June2014: these samples are not designed
# to be built with a multi-process make invocation.
.NOTPARALLEL:	dummy

ifeq ($(OS), )
$(error Need to set the OS environment variable)
endif

UTIL = ../dlutils
# 26Aug2009 - Give each sample a copy of the common modules
# since plugin samples compile these differently
COMMON_OBJS = PDFLInitCommon.o PDFLInitHFT.o InitializeLibrary.o APDFLDoc.o

INCLUDE = ../../../APDFL/Include/Headers
DLI_INCLUDE = ../../../DLI/Include
SOURCE=../../../APDFL/Include/Source
COMMON=../_Common

include $(UTIL)/$(OS).mak
include ../../../APDFL/Samples_Adobe/All/paths.rel

default: $(SAMPNAME)

CPPFLAGS = -I. -I$(INCLUDE) -I$(DLI_INCLUDE) -I$(UTIL) -I$(COMMON)


$(SAMPNAME) : $(COMMON_OBJS) $(OTHER_OBJS)
	$(CXX) -o $@ $(COMMON_OBJS) $(OTHER_OBJS) $(LDFLAGS) $(LIBS) $(EXTRA_LIBS)

##
# The files have the source and object in different directories, explicit rules
# are needed.  The '.c' files have to be compiled using the C++ compiler.
##

PDFLInitCommon.o : $(SOURCE)/PDFLInitCommon.c
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

PDFLInitHFT.o : $(SOURCE)/PDFLInitHFT.c
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

InitializeLibrary.o : $(COMMON)/InitializeLibrary.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

APDFLDoc.o : $(COMMON)/APDFLDoc.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) *.o $(UTIL)/*.o core out.* $(SAMPNAME) 

