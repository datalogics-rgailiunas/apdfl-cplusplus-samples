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
CC = /opt/solarisstudio12.4/bin/cc
CXX = /opt/solarisstudio12.4/bin/CC
CCFLAGS  = -g $(PDF_FDIR_DEF) -DNO_PRAGMA_ONCE -DUNIX_PLATFORM=1 -DUNIX_ENV=1 $(DEBUG) -DPRODUCT=\"HFTLibrary.h\" -DPLATFORM=\"UnixPlatform.h\" -D_REENTRANT -xtarget=ultra2 -xarch=sparcvis -m64 -mt
CCFLAGS += -DPDFL_SDK_SAMPLE -DPI_ACROCOLOR_VERSION=AcroColorHFT_VERSION_6 -DTOOLKIT -erroff=multicharconst
CXXFLAGS = $(CCFLAGS)
LD = $(CXX)
LDFLAGS = -g -xtarget=ultra2 -xarch=sparcvis -m64 -mt -L../../Libs

LIBS = -lDL150pdfl -lDL150CoolType -lDL150AGM -lDL150BIB -lDL150ACE -lDL150ARE \
	   -lDL150BIBUtils -lDL150JP2K -lDL150AdobeXMP -lDL150AXE8SharedExpat \
	   -licucnv -licudata -lpthread -lsocket -lnsl -lc -lm

else  # 32-bit w/ gcc
CC = /usr/bin/gcc
CXX = /usr/bin/g++
CCFLAGS  = -g $(PDF_FDIR_DEF) -DUNIX_PLATFORM=1 -DUNIX_ENV=1 $(DEBUG) -DPRODUCT=\"HFTLibrary.h\" -DPLATFORM=\"UnixPlatform.h\" -D_REENTRANT -Wno-multichar
CCFLAGS += -DPDFL_SDK_SAMPLE -DPI_ACROCOLOR_VERSION=AcroColorHFT_VERSION_6 -DTOOLKIT
CXXFLAGS = $(CCFLAGS)
LD = $(CXX)
LDFLAGS = -D_REENTRANT -g -L../../Libs

LIBS = -lDL150pdfl -lDL150CoolType -lDL150AGM -lDL150BIB -lDL150ACE -lDL150ARE \
	   -lDL150BIBUtils -lDL150JP2K -lDL150AdobeXMP -lDL150AXE8SharedExpat \
	   -licucnv -licudata -lpthread -lsocket -lnsl -lc -lm

endif

INCLUDE = ../../Include/Headers
SOURCE=../../Include/Source
