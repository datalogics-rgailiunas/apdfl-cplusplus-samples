ifeq ($(BUILD_64_BIT), true)
    CC = LD_LIBRARY_PATH=/opt/rh/llvm-toolset-7.0/root/usr/lib64 /opt/rh/llvm-toolset-7.0/root/usr/bin/clang
    CXX = LD_LIBRARY_PATH=/opt/rh/llvm-toolset-7.0/root/usr/lib64 /opt/rh/llvm-toolset-7.0/root/usr/bin/clang++
else
    CC = LD_LIBRARY_PATH=/opt/rh/llvm-toolset-7.0/root/usr/lib /opt/rh/llvm-toolset-7.0/root/usr/bin/clang
    CXX = LD_LIBRARY_PATH=/opt/rh/llvm-toolset-7.0/root/usr/lib /opt/rh/llvm-toolset-7.0/root/usr/bin/clang++
endif
            
ifeq ($(STAGE), debug)
    DEBUG=-DDEBUG -D_DEBUG
else
    DEBUG=-DNDEBUG
endif

ARCH_FLAGS = -march=armv8-a

CCFLAGS  = $(ARCH_FLAGS) -g $(PDF_FDIR_DEF) -DNO_PRAGMA_ONCE -DUNIX_PLATFORM=1 -DUNIX_ENV=1 -DPRODUCT=\"HFTLibrary.h\" $(DEBUG) -D_REENTRANT -Wno-multichar
CCFLAGS += -DPDFL_SDK_SAMPLE -DPI_ACROCOLOR_VERSION=AcroColorHFT_VERSION_6 -DTOOLKIT

CXXFLAGS = $(CCFLAGS) 
CXXFLAGS += -Wno-write-strings
CXXFLAGS += -std=c++17


LD = $(CXX)

LDFLAGS = $(ARCH_FLAGS) -Wl,-rpath,\$${ORIGIN}/$(PDFL_PATH) -L$(PDFL_PATH)
LIBS = -lDL180pdfl -lDL180CoolType -lDL180AGM -lDL180BIB -lDL180ACE -lDL180ARE \
	   -lDL180BIBUtils -lDL180JP2K -lDL180AdobeXMP -lDL180AXE8SharedExpat \
	   -licuuc -licudata -lpthread -lstdc++fs
