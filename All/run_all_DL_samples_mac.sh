#!/bin/bash

# Exit immediately if there is an error.
set -e

if [ "$1" != "" ]
then
	echo "$0: Parameter 1 is $1."
fi
# Accept an input parameter that chooses the build stage,
# Debug or Release.  Default to Debug like Windows.
if [ "$1" == "Release" ] || [ "$1" == "release" ]
then
	stage=Release
else
	stage=Debug
fi
echo "$0: Setting stage to $stage."
echo ""

echo Running sample AddArt
cd ../AddArt
./AddArt-${stage}.app/Contents/MacOS/AddArt-${stage}
echo ""

echo Running sample AddAttachments
cd ../AddAttachments
./AddAttachments-${stage}.app/Contents/MacOS/AddAttachments-${stage}
echo ""

echo Running sample AddBookmarks
cd ../AddBookmarks
./AddBookmarks-${stage}.app/Contents/MacOS/AddBookmarks-${stage}
echo ""

echo Running sample AddContent
cd ../AddContent
./AddContent-${stage}.app/Contents/MacOS/AddContent-${stage}
echo ""

echo Running sample AddDocumentInformation
cd ../AddDocumentInformation
./AddDocumentInformation-${stage}.app/Contents/MacOS/AddDocumentInformation-${stage}

echo Running sample AddLinks
cd ../AddLinks
./AddLinks-${stage}.app/Contents/MacOS/AddLinks-${stage}
echo ""

echo Running sample AddPageNumbers
cd ../AddPageNumbers
./AddPageNumbers-${stage}.app/Contents/MacOS/AddPageNumbers-${stage}
echo ""

echo Running sample AddPassword
cd ../AddPassword
./AddPassword-${stage}.app/Contents/MacOS/AddPassword-${stage}
echo ""

echo Running sample AddRedaction
cd ../AddRedaction
./AddRedaction-${stage}.app/Contents/MacOS/AddRedaction-${stage}
echo ""

echo Running sample AddText
cd ../AddText
./AddText-${stage}.app/Contents/MacOS/AddText-${stage}
echo ""

echo Running sample AddWatermark
cd ../AddWatermark
./AddWatermark-${stage}.app/Contents/MacOS/AddWatermark-${stage}
echo ""

echo Running sample ConvertPDFtoEPS
cd ../ConvertPDFtoEPS
./ConvertPDFtoEPS-${stage}.app/Contents/MacOS/ConvertPDFtoEPS-${stage}
echo ""

echo Running sample ConvertPDFtoPostscript
cd ../ConvertPDFtoPostscript
./ConvertPDFtoPostscript-${stage}.app/Contents/MacOS/ConvertPDFtoPostscript-${stage}
echo ""

echo Running sample CopyContent
cd ../CopyContent
./CopyContent-${stage}.app/Contents/MacOS/CopyContent-${stage}
echo ""

echo Running sample CreateAnnotations
cd ../CreateAnnotations
./CreateAnnotations-${stage}.app/Contents/MacOS/CreateAnnotations-${stage}
echo ""

echo Running sample CreateDocument
cd ../CreateDocument
./CreateDocument-${stage}.app/Contents/MacOS/CreateDocument-${stage}
echo ""

echo Running sample CreateLayers
cd ../CreateLayers
./CreateLayers-${stage}.app/Contents/MacOS/CreateLayers-${stage}
echo ""

echo Running sample CreateTransparency
cd ../CreateTransparency
./CreateTransparency-${stage}.app/Contents/MacOS/CreateTransparency-${stage}
echo ""

echo Running sample EncryptDocument
cd ../EncryptDocument
./EncryptDocument-${stage}.app/Contents/MacOS/EncryptDocument-${stage}
echo ""

echo Running sample ExtractAttachments
cd ../ExtractAttachments
./ExtractAttachments-${stage}.app/Contents/MacOS/ExtractAttachments-${stage}
echo ""

echo Running sample ExtractDocumentInfo
cd ../ExtractDocumentInfo
./ExtractDocumentInfo-${stage}.app/Contents/MacOS/ExtractDocumentInfo-${stage}
echo ""

echo Running sample ExtractText
cd ../ExtractText
./ExtractText-${stage}.app/Contents/MacOS/ExtractText-${stage}
echo ""

echo Running sample FindImageResolutions
cd ../FindImageResolutions
./FindImageResolutions-${stage}.app/Contents/MacOS/FindImageResolutions-${stage}
echo ""

echo Running sample FlattenAnnotations
cd ../FlattenAnnotations
./FlattenAnnotations-${stage}.app/Contents/MacOS/FlattenAnnotations-${stage}
echo ""

echo Running sample FlattenTransparency
cd ../FlattenTransparency
./FlattenTransparency-${stage}.app/Contents/MacOS/FlattenTransparency-${stage}
echo ""

echo Running sample LockDocument
cd ../LockDocument
./LockDocument-${stage}.app/Contents/MacOS/LockDocument-${stage}
echo ""

echo Running sample MergeDocuments
cd ../MergeDocuments
./MergeDocuments-${stage}.app/Contents/MacOS/MergeDocuments-${stage}
echo ""

echo Running sample OpenEncrypted
cd ../OpenEncrypted
./OpenEncrypted-${stage}.app/Contents/MacOS/OpenEncrypted-${stage}
echo ""

echo Running sample PDFOptimizer
cd ../PDFOptimizer
./PDFOptimizer-${stage}.app/Contents/MacOS/PDFOptimizer-${stage}
echo ""

echo Running sample RenderPage
cd ../RenderPage
./RenderPage-${stage}.app/Contents/MacOS/RenderPage-${stage}
echo ""

echo Running sample SetUniquePermissions
cd ../SetUniquePermissions
./SetUniquePermissions-${stage}.app/Contents/MacOS/SetUniquePermissions-${stage}
echo ""

echo Running sample SplitPDF
cd ../SplitPDF
./SplitPDF-${stage}.app/Contents/MacOS/SplitPDF-${stage}
echo ""

echo Running sample TextSearch
cd ../TextSearch
./TextSearch-${stage}.app/Contents/MacOS/TextSearch-${stage}
echo ""

echo Running sample UnicodeText
cd ../UnicodeText
./UnicodeText-${stage}.app/Contents/MacOS/UnicodeText-${stage}
echo ""

echo Running sample WebOptimizedPDF
cd ../WebOptimizedPDF
./WebOptimizedPDF-${stage}.app/Contents/MacOS/WebOptimizedPDF-${stage}
echo ""
echo All Datalogics samples are finished running.

