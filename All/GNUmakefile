#  Makefile to build DL samples

ifeq ($(OS), )
  PLATFORM = $(shell uname)
  MACHINE = $(shell uname -m)
  BUILD_64_BIT=true
  ifeq ($(PLATFORM), Linux)
    ifeq ($(MACHINE), x86_64)
      OS=i80386linux
    endif
    ifeq ($(MACHINE), aarch64)
      OS=armv8linux
    endif
  endif
  ifeq ($(PLATFORM), Darwin)
    ifeq ($(MACHINE), x86_64)
      OS=mac-x86-64
    endif
    ifeq ($(MACHINE), aarch64)
      OS=armv8mac
    endif
  endif
  ifeq ($(PLATFORM), AIX)
    OS=rs6000aix
  endif
  ifeq ($(PLATFORM), sun4v)
    OS=sparcsolaris
  endif
endif


do_pdfl_samples: build_pdfl_samples run_pdfl_samples $(REDIRECT2)

SAMPLES = Annotations/CreateAnnotations \
          Annotations/FlattenAnnotations \
          ContentCreation/AddArt \
          ContentCreation/AddAttachments \
          ContentCreation/AddContent \
          ContentCreation/CreateBookmarks \
          ContentCreation/CreateDocument \
          ContentCreation/CreateLayers \
          ContentCreation/CreateTransparency \
          ContentExtraction/CopyContent \
          ContentExtraction/ExtractAttachments \
          ContentExtraction/ExtractFonts \
          ContentModification/AddBookmarks \
          ContentModification/AddDocumentInformation \
          ContentModification/AddLinks \
          ContentModification/AddPageNumbers \
          ContentModification/AddTextWatermark \
          ContentModification/AttachMimeToPDF \
          ContentModification/EmbedFonts \
          ContentModification/FlattenTransparency \
          ContentModification/ImportPages \
          ContentModification/MergeAcroforms \
          ContentModification/MergeDocuments \
          ContentModification/PDFMakeOCGVisible \
          ContentModification/PDFUncompress \
          ContentModification/SplitPDF \
          Display/DisplayPDEContent \
          DocumentConversion/ConvertPDFtoEPS \
          DocumentConversion/ConvertPDFtoPostscript \
          DocumentConversion/ConvertToPDFA \
          DocumentConversion/ConvertToPDFX \
          DocumentOptimization/PDFOptimizer \
          FileSystem/AlternateFileSystem \
          Images/AddThumbnailsToPDF \
          Images/CalcImageDPI \
          Images/CreateImageWithTransparency \
          Images/CreateSeparations \
          Images/FindImageResolutions \
          Images/OutputPreview \
          Images/RenderPage \
          InformationExtraction/CountColorsInDoc \
          InformationExtraction/ExtractDocumentInfo \
          Printing/PostScriptInjection \
          Security/AddPassword \
          Security/AddRedaction \
          Security/AddRegexRedaction \
          Security/AddTriangularRedaction \
          Security/AESEncryption \
          Security/EncryptDocument \
          Security/LockDocument \
          Security/OpenEncrypted \
          Security/SetUniquePermissions \
          Text/AddText \
          Text/ExtractText \
          Text/ExtractAcroFormFieldData \
          Text/ExtractTextFromAnnotations \
          Text/ExtractTextByRegion \
          Text/RegexTextSearch \
          Text/RegexExtractText \
          Text/ExtractTextByPatternMatch \
          Text/ExtractCJKTextByPatternMatch \
          Text/HelloJapan \
          Text/InsertHeadFoot \
          Text/TextSearch \
          Text/TextSelectEnum \
          Text/UnicodeText

ifneq ($(findstring i80386linux,$(OS)), )
ifeq ($(BUILD_64_BIT), true)
    SAMPLES += Text/ExtractTextFromMultiRegions
endif
endif


ifeq ($(findstring rs6000aix,$(OS)), )
ifeq ($(findstring mac,$(OS)), )
    SAMPLES +=  DocumentConversion/XPStoPDF
endif
endif

build_pdfl_samples:
	$(info OS=$(OS))
	$(info BUILD_64_BIT=$(BUILD_64_BIT))
	set -e; \
	export OS=$(OS) \
	export BUILD_64_BIT=$(BUILD_64_BIT) \
	export; \
	for target in $(SAMPLES); do \
		(cd ../$$target && $(MAKE)); \
	done;

ifeq ($(LIBPATHS),)
    LIBPATHS=../../../Binaries
endif

run_pdfl_samples:
	set -e; \
	export LD_LIBRARY_PATH=$(LIBPATHS):$$LD_LIBRARY_PATH; \
	export; \
	for target in $(SAMPLES); do \
		echo "Running DL Sample $$target"; \
		export NAME=`echo "$$target" | cut -d'/' -f 2`; \
		(cd ../$$target && (./$$NAME $(REDIRECT))) ;\
		echo ; \
	done; \
	echo ;

clean:
	rm -f *.o *.out out.* ../dlutils/*.o; \
	for target in $(SAMPLES); do \
		(cd ../$$target && $(MAKE) clean); \
	done;
