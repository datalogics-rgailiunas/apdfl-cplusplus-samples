CC = /opt/gcc-4.1.2/bin/gcc
CXX = /opt/gcc-4.1.2/bin/g++

ifeq ($(STAGE), debug)
    DEBUG=-DDEBUG -D_DEBUG
else
    DEBUG=-DNDEBUG
endif

ifeq ($(BUILD_64_BIT), true)
    ARCH_FLAGS = -m64
else
    ARCH_FLAGS = -m32
endif

CCFLAGS  = $(ARCH_FLAGS) -g $(PDF_FDIR_DEF) -DNO_PRAGMA_ONCE -DUNIX_PLATFORM=1 -DUNIX_ENV=1 -DPRODUCT=\"HFTLibrary.h\" $(DEBUG) -D_REENTRANT -Wno-multichar -DPDFL_SDK_SAMPLE -DPI_ACROCOLOR_VERSION=AcroColorHFT_VERSION_6 -DTOOLKIT

CXXFLAGS = $(CCFLAGS)
LD = $(CXX)

LDFLAGS = $(ARCH_FLAGS) -L../../Libs
LIBS = -lDL100pdfl -lDL100CoolType -lDL100AGM -lDL100BIB -lDL100ACE -lDL100ARE \
	   -lDL100BIBUtils -lDL100JP2K -lDL100AdobeXMP -lDL100AXE8SharedExpat \
	   -licucnv -licudata -lpthread
