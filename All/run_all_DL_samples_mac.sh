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

echo Running sample CopyContent
cd ../CopyContent
./CopyContent-${stage}.app/Contents/MacOS/CopyContent-${stage}
echo ""
echo All Datalogics samples are finished running.

