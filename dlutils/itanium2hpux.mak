ifeq ($(USE_GCC), true)

CC = /opt/hp-gcc/bin/gcc
CXX= /opt/hp-gcc/bin/g++
LD = $(CXX)

CCFLAGS  = -g $(PDF_FDIR_DEF) -DUNIX_PLATFORM=1 -DUNIX_ENV=1 -DPRODUCT=\"HFTLibrary.h\" -D_REENTRANT
CCFLAGS += -DPLATFORM=\"UnixPlatform.h\" -DPDFL_SDK_SAMPLE -DPI_ACROCOLOR_VERSION=AcroColorHFT_VERSION_6 -DTOOLKIT -DHPPAHPUX
CCFLAGS += -fPIC -Wno-multichar -Wno-implicit -Wno-write-strings
ifeq ($(STAGE), debug)
    CCFLAGS += -DDEBUG -D_DEBUG
else
    CCFLAGS += -DNDEBUG
endif

LDFLAGS = -g

ifeq ($(BUILD_64_BIT), true)
CCFLAGS += -DBUILD_64_BIT -mlp64
LDFLAGS += -mlp64
endif

CXXFLAGS = ${CCFLAGS} -Wno-ctor-dtor-privacy
LIBS = -L$(PDFL_PATH)/Libs -lDL150ACE -lDL150AGM -lDL150ARE \
 	   -lDL150AXE8SharedExpat -lDL150AdobeXMP -lDL150BIB \
 	   -lDL150BIBUtils -lDL150CoolType -lDL150JP2K -lDL150pdfl \
 	   -lc -lpthread -lm

INCLUDE = $(PDFL_PATH)/Include/Headers
SOURCE = $(PDFL_PATH)/Include/Source

else #USE_GCC

ifeq ($(BUILD_64_BIT), true)
TARGET_CPU_FLAGS := +DD64 +DSitanium2 
else
TARGET_CPU_FLAGS := +DD32 +DSitanium2
endif

CC = /opt/aCC.6.28/bin/aCC +e
CXX = /opt/aCC.6.28/bin/aCC -AA
#warning 2550: variable X was set but not used
CCFLAGS  = ${TARGET_CPU_FLAGS} -g -mt -D_RWSTD_MULTI_THREAD +W3422 $(PDF_FDIR_DEF) -DUNIX_PLATFORM=1 -DUNIX_ENV=1 -DPRODUCT=\"HFTLibrary.h\" -D_REENTRANT 
CCFLAGS += -DPLATFORM=\"UnixPlatform.h\" -DPDFL_SDK_SAMPLE -DPI_ACROCOLOR_VERSION=AcroColorHFT_VERSION_6 -DTOOLKIT -DITANIUM2HPUX
ifeq ($(STAGE), debug)
#warning 2186: pointless comparison of unsigned integer with zero
#        2111: statement is unreachable
    CCFLAGS += -DDEBUG -D_DEBUG +W2186,2111
else
    CCFLAGS += -DNDEBUG
endif
CXXFLAGS = $(CCFLAGS)
LD = $(CXX)
LDFLAGS = ${TARGET_CPU_FLAGS} -g -mt -Wl,+vnoshlibunsats

LIBS = -L$(PDFL_PATH)/Libs -lDL150pdfl

INCLUDE = $(PDFL_PATH)/Include/Headers
SOURCE = $(PDFL_PATH)/Include/Source

endif
