#  Makefile to build DL samples 

ifeq ($(PLATFORM), )
  PLATFORM = $(shell uname)
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
          ContentModification/AttachMimeToPDF \
          ContentModification/MergeAcroforms \
          ContentModification/MergeDocuments \
          ContentModification/PDFMakeOCGVisible \
          ContentModification/PDFUncompress \
          ContentModification/SplitPDF \
          ContentModification/ImportPages \
          ContentModification/AddWatermark \
          ContentModification/EmbedFonts \
          Display/DisplayPDEContent \
          DocumentOptimization/WebOptimizedPDF \
          DocumentConversion/ConvertPDFtoEPS \
          DocumentConversion/ConvertToPDFA \
          DocumentConversion/ConvertToPDFX \
          DocumentConversion/ConvertPDFtoPostscript \
          FileSystem/AlternateFileSystem \
          Images/AddThumbnailsToPDF \
          Images/RenderPage \
          Images/CreateImageWithTransparency \
          Images/CreateSeparations \
          Images/CalcImageDPI \
          InformationExtraction/CountColorsInDoc \
          InformationExtraction/ExtractDocumentInfo \
          Printing/PostScriptInjection \
          Security/AESEncryption \
          Security/AddPassword \
          Security/AddRedaction \
          Security/EncryptDocument \
          Security/LockDocument \
          Security/SetUniquePermissions \
          Security/OpenEncrypted \
          Security/ValidateSignatures \
          Text/InsertHeadFoot \
          Text/ExtractText \
          Text/AddText \
          Text/HelloJapan \
          Text/TextSearch

ifeq ($(OS), i80386linux)
	# SAMPLES += FlattenTransparency XPStoPDF PDFOptimizer
	SAMPLES += ContentModification/FlattenTransparency  DocumentOptimization/PDFOptimizer
endif

ifeq ($(OS), sparcsolaris)
    ifneq ($(BUILD_64_BIT), true)
        SAMPLES +=  ContentModification/FlattenTransparency DocumentConversion/XPStoPDF Images/FindImageResolutions Text/UnicodeText
    endif
else
    SAMPLES += Images/FindImageResolutions Text/UnicodeText
endif

ifeq ($(OS), intelsolaris)
	SAMPLES += ContentModification/FlattenTransparency
endif

build_pdfl_samples: 
	for target in $(SAMPLES); do \
		(cd ../$$target && $(MAKE)); \
	done;

ifeq ($(LIBPATHS),)
    LIBPATHS=../../../Binaries
endif

run_pdfl_samples:
	set -e; \
	export LD_LIBRARY_PATH=$(LIBPATHS):$$LD_LIBRARY_PATH; \
	export LIBPATH=$(LIBPATHS):$$LIBPATH; \
	export SHLIB_PATH=$(LIBPATHS):$$SHLIB_PATH; \
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
