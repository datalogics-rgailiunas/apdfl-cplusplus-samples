# Copyright (c) 2015-2016, Datalogics, Inc. All rights reserved.
#
# http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
#
# *********************************************************************
.NOTPARALLEL:	dummy

ifeq ($(OS), )
$(error Need to set the OS environment variable)
endif

UTIL = ../utils
# Give each sample a copy of the common modules
# since plugin samples compile these differently
COMMON_OBJS = PDFLInitCommon.o PDFLInitHFT.o InitializeLibrary.o APDFLDoc.o

INCLUDE = ../../../APDFL/Include/Headers
DLI_INCLUDE = ../../../DLI/Include
SOURCE=../../../APDFL/Include/Source
COMMON=../_Common

include $(UTIL)/$(OS).mak
include ../../../APDFL/Samples/All/paths.rel

default: $(SAMPNAME)

INCDIRS = -I. -I$(INCLUDE) -I$(DLI_INCLUDE) -I$(UTIL) -I$(COMMON)


$(SAMPNAME) : $(COMMON_OBJS) $(OTHER_OBJS)
	$(LD) -o $@ $(COMMON_OBJS) $(OTHER_OBJS) $(LDFLAGS) $(LIBS) $(EXTRA_LIBS)

##
# Default rules to build source files into object files listed in ${OTHER_OBJS)
##

%.o : %.cpp
	$(CXX) $(INCDIRS) $(CXXFLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(INCDIRS) $(CCFLAGS) -c $< -o $@


##
# The files below have the source and object in different directories, explicit rules
# are needed.  The first two are '.c' files which have to be compiled using the C++ compiler.
##

PDFLInitCommon.o : $(SOURCE)/PDFLInitCommon.c
	$(CXX) $(INCDIRS) $(CXXFLAGS) -c $< -o $@

PDFLInitHFT.o : $(SOURCE)/PDFLInitHFT.c
	$(CXX) $(INCDIRS) $(CXXFLAGS) -c $< -o $@

InitializeLibrary.o : $(COMMON)/InitializeLibrary.cpp
	$(CXX) $(INCDIRS) $(CXXFLAGS) -c $< -o $@
	
APDFLDoc.o : $(COMMON)/APDFLDoc.cpp
	$(CXX) $(INCDIRS) $(CXXFLAGS) -c $< -o $@
	
clean:
	rm -f *.o $(UTIL)/*.o core out.* $(SAMPNAME) $(CLEAN_UP_FILES)
