#
# Solaris makefile for APDFL samples
# Set the values for CC and CXX below to match your environment.
#

ifeq ($(STAGE), debug)
    DEBUG=-DDEBUG -D_DEBUG
else
    DEBUG=-DNDEBUG
endif


ifeq ($(BUILD_64_BIT), true)
CC = /opt/developerstudio12.6/bin/cc
CXX = /opt/developerstudio12.6/bin/CC
CCFLAGS  = -g $(PDF_FDIR_DEF) -DNO_PRAGMA_ONCE -DUNIX_PLATFORM=1 -DUNIX_ENV=1 $(DEBUG) -DPRODUCT=\"HFTLibrary.h\" -DPLATFORM=\"UnixPlatform.h\" -D_REENTRANT -xtarget=ultra2 -xarch=sparcvis -m64 -mt
CCFLAGS += -DPDFL_SDK_SAMPLE -DPI_ACROCOLOR_VERSION=AcroColorHFT_VERSION_6 -DTOOLKIT
CXXFLAGS = $(CCFLAGS)
LD = $(CXX)
LDFLAGS = -g -m64 -mt -R\$${ORIGIN}/$(PDFL_PATH) -L$(PDFL_PATH)

LIBS = -lDL180pdfl -lDL180CoolType -lDL180AGM -lDL180BIB -lDL180ACE -lDL180ARE \
	   -lDL180BIBUtils -lDL180JP2K -lDL180AdobeXMP -lDL180AXE8SharedExpat \
	   -licuuc -licudata -lpthread -lsocket -lnsl -lc -lm

else  # 32-bit w/ gcc
CC = /usr/gcc/4.8/bin/gcc
CXX = /usr/gcc/4.8/bin/g++
CCFLAGS  = -g $(PDF_FDIR_DEF) -DUNIX_PLATFORM=1 -DUNIX_ENV=1 $(DEBUG) -DPRODUCT=\"HFTLibrary.h\" -DPLATFORM=\"UnixPlatform.h\" -D_REENTRANT -Wno-multichar
CCFLAGS += -DPDFL_SDK_SAMPLE -DPI_ACROCOLOR_VERSION=AcroColorHFT_VERSION_6 -DTOOLKIT
CXXFLAGS = $(CCFLAGS)
LD = $(CXX)
LDFLAGS = -g -R\$${ORIGIN}/$(PDFL_PATH) -L$(PDFL_PATH)

LIBS = -lDL180pdfl -lDL180CoolType -lDL180AGM -lDL180BIB -lDL180ACE -lDL180ARE \
	   -lDL180BIBUtils -lDL180JP2K -lDL180AdobeXMP -lDL180AXE8SharedExpat \
	   -licuuc -licudata -lpthread -lsocket -lnsl -lc -lm

endif
