# Change these to match your local environment
ifeq ($(USE_GCC), true)
CC = /opt/gcc-4.1.2/bin/gcc
CXX = /opt/gcc-4.1.2/bin/g++
EXTRA_LIBS=-ldl
else
# DLADD YuriG 01Feb2012 - APDFL10: use XL C/C++ 11.1
CC = /opt/XLC_11/usr/vacpp/bin/xlc_r
CXX = /opt/XLC_11/usr/vacpp/bin/xlC_r
endif

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

ifeq ($(USE_GCC), true)

CCFLAGS = -g -Wno-multichar -DAIX_GCC_COMPAT $(PDF_FDIR_DEF) -D_ALL_SOURCE -D_POSIX_SOURCE -DNO_PRAGMA_ONCE -DUNIX_PLATFORM -DUNIX_ENV $(DEBUG) -DPRODUCT=\"HFTLibrary.h\" -DPLATFORM=\"UnixPlatform.h\" -DRS6000AIX -DPDFL_SDK_SAMPLE -DPI_ACROCOLOR_VERSION=AcroColorHFT_VERSION_6 -pthread
CXXFLAGS = -g -Wno-multichar -DAIX_GCC_COMPAT $(PDF_FDIR_DEF) -D_ALL_SOURCE -D_POSIX_SOURCE -DNO_PRAGMA_ONCE -DUNIX_PLATFORM -DUNIX_ENV $(DEBUG) -DPRODUCT=\"HFTLibrary.h\" -DPLATFORM=\"UnixPlatform.h\" -DPDFL_SDK_SAMPLE -DPI_ACROCOLOR_VERSION=AcroColorHFT_VERSION_6 -pthread

LD = $(CXX)
# -bnoipath tells the loader to strip the absolute or relative path
# information from the shared library name when it is added to the
# loader section of the object file.  This makes it easier for the
# run time loader to find the shared libraries.
# -brtl enables run time linking for the output file.
LDFLAGS = -g -pthread -Wl,-bnoipath -Wl,-brtl

else
# supress xlC warnings:
#	1540-1401 (W) An unknown "pragma once" is specified.
#	1540-0804 (W) The characters "/*" are detected in a comment.
#	1540-0802 (W) The character literal 'xxxx' contains more than one character.
#	1506-342  (W) "/*" detected in comment.
#	1506-137  (E) Declaration must declare at least one declarator, tag, or the members of an enumeration.
#   1506-224  (I) Incorrect pragma ignored.
#   1506-076  (W) Character constant 'xxxx' has more than one character.
CCFLAGS  = -g $(PDF_FDIR_DEF) -DUNIX_PLATFORM=1 -DUNIX_ENV=1 $(DEBUG) -DPRODUCT=\"HFTLibrary.h\" -DPLATFORM=\"UnixPlatform.h\" -brtl -qstaticinline -qsuppress=1540-1401:1506-224:1506-342:1506-076 -qsuppress=1540-0804:1540-0802
CCFLAGS += -DNO_PRAGMA_ONCE -DPDFL_SDK_SAMPLE -DPI_ACROCOLOR_VERSION=AcroColorHFT_VERSION_6 -DTOOLKIT

# DLADD kam 20Apr2007 Some enumerations in the headers require 4-byte values
CCFLAGS += -qenum=4

# DLADD agriffin 15Apr2009 Enable C++style comments in C code
CCFLAGS += -qcpluscmt

# YuriG 01Feb2012
CCFLAGS += -DAIX_VACPP

CXXFLAGS = $(CCFLAGS) -qrtti -+
#Linking using xlC_r or xlC is required.
LD = $(CXX)
LDFLAGS = -g

# DLADD MattK 20Nov09: add 64-bit support
ifeq ($(BUILD_64_BIT), true)
CCFLAGS += -q64
# CXXFLAGS set by setting CCFLAGS, as it refers to the variable which results in a use-time resolution
LDFLAGS += -q64
endif

endif

LIBS = ../../Libs/libDL100pdfl.so ../../Libs/libDL100CoolType.so ../../Libs/libDL100AGM.so ../../Libs/libDL100BIB.so ../../Libs/libDL100ACE.so ../../Libs/libDL100ARE.so ../../Libs/libDL100BIBUtils.so ../../Libs/libDL100JP2K.so ../../Libs/libDL100AdobeXMP.so ../../Libs/libDL100AXE8SharedExpat.so -lpthread

INCLUDE = ../../Include/Headers
SOURCE=../../Include/Source
