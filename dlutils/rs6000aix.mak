# Change these to match your local environment
ifeq ($(USE_GCC), true)
CC=/opt/freeware/bin/gcc
CXX=/opt/freeware/bin/g++
EXTRA_LIBS=-ldl
else
# DLADD RobB 16Jun2016 - Use IBM XL C/C++ 13.1 for APDFL 15
CC=/opt/IBM/xlC/13.1.3/bin/xlc_r
CXX=/opt/IBM/xlC/13.1.3/bin/xlC_r
endif

ifeq ($(STAGE), debug)
    DEBUG=-DDEBUG -D_DEBUG
else
    DEBUG=-DNDEBUG
endif

LD = $(CXX)

CPP_DEFINES = $(PDF_FDIR_DEF) -DUNIX_PLATFORM=1 -DUNIX_ENV=1 -DPRODUCT=\"HFTLibrary.h\" -DPLATFORM=\"UnixPlatform.h\" \
 -DNO_PRAGMA_ONCE -DPDFL_SDK_SAMPLE -DPI_ACROCOLOR_VERSION=AcroColorHFT_VERSION_6

ifeq ($(USE_GCC), true)

CCFLAGS = -g -Wno-multichar -DAIX_GCC_COMPAT $(CPP_DEFINES) -D_ALL_SOURCE -D_POSIX_SOURCE $(DEBUG) -DRS6000AIX -pthread
CXXFLAGS = -g -Wno-multichar -DAIX_GCC_COMPAT $(CPP_DEFINES) -D_ALL_SOURCE -D_POSIX_SOURCE $(DEBUG) -pthread

# -bnoipath tells the loader to strip the absolute or relative path
# information from the shared library name when it is added to the
# loader section of the object file.  This makes it easier for the
# run time loader to find the shared libraries.
# -brtl enables run time linking for the output file.
LDFLAGS = -g -pthread -Wl,-bnoipath -Wl,-brtl

ifeq ($(BUILD_64_BIT), true)
CCFLAGS += -maix64
CXXFLAGS += -maix64
LDFLAGS += -maix64
endif

else
# supress xlC warnings:
#	1540-1401 (W) An unknown "pragma once" is specified.
#	1540-0804 (W) The characters "/*" are detected in a comment.
#	1506-342  (W) "/*" detected in comment.
#	1540-0802 (W) The character literal 'xxxx' contains more than one character.
#	1506-137  (E) Declaration must declare at least one declarator, tag, or the members of an enumeration.
#   1506-224  (I) Incorrect pragma ignored.
CCFLAGS  = -g $(CPP_DEFINES) $(DEBUG) -qstaticinline -qsuppress=1540-0802
# -qsuppress=1506-224 -qsuppress=1540-0804:1506-342
# DLADD agriffin 15Apr2009 Enable C++style comments in C code
CCFLAGS += -qcpluscmt

# YuriG 01Feb2012
CCFLAGS += -DAIX_VACPP

CXXFLAGS = $(CCFLAGS) -qrtti -+
#Linking using xlC_r or xlC is required.
LD = $(CXX)
LDFLAGS = -g -brtl 

# DLADD MattK 20Nov09: add 64-bit support
ifeq ($(BUILD_64_BIT), true)
CCFLAGS += -q64
# CXXFLAGS set by setting CCFLAGS, as it refers to the variable which results in a use-time resolution
LDFLAGS += -q64
endif

endif

LIBS = -L$(PDFL_PATH) -lDL180pdfl -lDL180CoolType -lDL180AGM -lDL180BIB -lDL180ACE -lDL180ARE -lDL180BIBUtils -lDL180JP2K -lDL180AdobeXMP -lDL180AXE8SharedExpat -lpthread
