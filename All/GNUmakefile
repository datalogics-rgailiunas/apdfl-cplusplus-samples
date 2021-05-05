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
          DocumentOptimization/WebOptimizedPDF \
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
          Security/AddTriangularRedaction \
          Security/AESEncryption \
          Security/EncryptDocument \
          Security/LockDocument \
          Security/OpenEncrypted \
          Security/SetUniquePermissions \
          Security/ValidateSignatures \
          Text/AddText \
          Text/ExtractText \
          Text/HelloJapan \
          Text/InsertHeadFoot \
          Text/TextSearch \
          Text/TextSelectEnum \
          Text/UnicodeText

ifneq ($(findstring i80386linux,$(OS)), )
    SAMPLES += Security/AddRegexRedaction
    SAMPLES += Text/RegexTextSearch
    SAMPLES += Text/RegexExtractText
endif

ifeq ($(findstring rs6000aix,$(OS)), )
    SAMPLES +=  DocumentConversion/XPStoPDF
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
