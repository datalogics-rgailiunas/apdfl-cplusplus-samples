#
# Solaris makefile for APDFL samples
# Set the values for CC and CXX below to match your environment.
#
# DLADD MattK 14Nov05:
# NOTE: if you want to build an executable using gcc with
# a static link of libstdc++ and libgcc, you *MUST*
#   a) use gcc to drive the linker (NOT g++), and
#   b) link against the static libstdc++.a

# Failure to do (a) will cause the dynamic version of libstdc++
# to be included.  Failure to do (b) will create an executable
# which crashes on or near startup.

ifeq ($(STAGE), debug)
    DEBUG=-DDEBUG -D_DEBUG
else
    DEBUG=-DNDEBUG
endif

#DLADD wfles 10/10/2007
#Remove this defintion so we can use the definition in common.mak
#so Optional Samples will build
#UTIL = ../utils
#DLADD end

ifeq ($(BUILD_64_BIT), true)
#DLADD agriffin 05Aug2009: Switch to SunStudio 12.1 and switch -xarch=v9a to -xarch=sparcvis -m64
# DLADD: RickK 21Nov2012 APDFL10.1 - Switch to the SunStudio 12.3 compiler.
#CC = /opt/sunstudio12.1/bin/cc
#CXX = /opt/sunstudio12.1/bin/CC
CC = /opt/SS12.3/bin/cc
CXX = /opt/SS12.3/bin/CC
CCFLAGS  = -g $(PDF_FDIR_DEF) -DNO_PRAGMA_ONCE -DUNIX_PLATFORM=1 -DUNIX_ENV=1 $(DEBUG) -DPRODUCT=\"HFTLibrary.h\" -DPLATFORM=\"UnixPlatform.h\" -D_REENTRANT -xtarget=ultra2 -xarch=sparcvis -m64 -mt
CCFLAGS += -DPDFL_SDK_SAMPLE -DPI_ACROCOLOR_VERSION=AcroColorHFT_VERSION_6 -DTOOLKIT
CXXFLAGS = $(CCFLAGS)
LD = $(CXX)
LDFLAGS = -D_REENTRANT -g -xtarget=ultra2 -xarch=sparcvis -m64 -mt -L../../Libs
# DLADD bhaugen 25Jan2012 - switch to pdfl10 libs
LIBS = -lDL100pdfl -lDL100CoolType -lDL100AGM -lDL100BIB -lDL100ACE -lDL100ARE \
	   -lDL100BIBUtils -lDL100JP2K -lDL100AdobeXMP -lDL100AXE8SharedExpat \
	   -licucnv -licudata -lpthread -lsocket -lnsl -lc -lm

else  # 32-bit w/ gcc
CC = /opt/gcc-4.1.2/bin/gcc
CXX = /opt/gcc-4.1.2/bin/g++
CCFLAGS  = -g $(PDF_FDIR_DEF) -DNO_PRAGMA_ONCE -DUNIX_PLATFORM=1 -DUNIX_ENV=1 $(DEBUG) -DPRODUCT=\"HFTLibrary.h\" -DPLATFORM=\"UnixPlatform.h\" -D_REENTRANT -Wno-multichar
CCFLAGS += -DPDFL_SDK_SAMPLE -DPI_ACROCOLOR_VERSION=AcroColorHFT_VERSION_6 -DTOOLKIT
CXXFLAGS = $(CCFLAGS)
LD = $(CXX)
LDFLAGS = -D_REENTRANT -g -L../../Libs
# DLADD bhaugen 25Jan2012 - switch to pdfl10 libs
LIBS = -lDL100pdfl -lDL100CoolType -lDL100AGM -lDL100BIB -lDL100ACE -lDL100ARE \
	   -lDL100BIBUtils -lDL100JP2K -lDL100AdobeXMP -lDL100AXE8SharedExpat \
	   -licucnv -licudata -lpthread -lsocket -lnsl -lc -lm

# Suggested to statically link libstdc++ and libgcc:
# LD = $(CC)
# LDFLAGS = -D_REENTRANT -g
# DLADD bhaugen 25Jan2012 - switch to pdfl10 libs
# LIBS =  -L ../../Libs -lDL100pdfl -static-libgcc /usr/local/lib/libstdc++.a -lpthread -lc -lm

endif

INCLUDE = ../../Include/Headers
SOURCE=../../Include/Source
