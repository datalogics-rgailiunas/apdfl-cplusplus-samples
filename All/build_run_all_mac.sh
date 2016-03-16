#!/bin/bash
## ***
## ***  Copyright (c) 2015-2016, Datalogics, Inc. All rights reserved.
## ***

## **********************************************************************************************************************
## *** Sample: All - Builds and runs each APDFL sample, and outputs the results.
## ***
## *** By default, this occurs with the debug configuration.
## *** Pass in "release" as an argument to use the release configuration.
## *** It is important to note that this program assumes: 
## ***    1. A sample was built successfully <-> Its exe is located in <samplefolder>/<arch>/<stage>/<samplename>.exe
## ***    2. Above, <samplename> is equal to <samplefolder>.
## ***
## *** MAINTENANCE:
## ***   Step 3 is where you want to be if you want to add or ##ove samples from this script.
## ***   Add or ##ove items from DL_SAMPLE_LIST or AD_SAMPLE_LIST, making sure to update NUM_SAMPLES,
## ***   NUM_AD_SAMPLES, NUM_DL_SAMPLES, and NUM_PL_SAMPLES accordingly. Bear in mind that each item in the list must
## ***   be, simultaneously, the name of the sample's folder in ../, the name of the sample's .sln,
## ***   and the name of the executable it builds...
## ***   If necessary, you can also specify arguments for samples. Follow the examples therein.
## ***
## *** USAGE:
## ***   All arguments are case-insensitive.
## ***
## ***   ARGUMENT      EFFECT
## ***   -release      Build Release configuration instead of Debug configuration.
## ***   -64-bit       Build the 64-bit version instead of 32-bit version.
## ***
## *** Steps:
## *** 1) Initialize.
## *** 2) Build each sample.
## *** 3) Decide which samples to run.
## *** 4) Run the samples.
## *** 4) Output the results.
## **********************************************************************************************************************

# Exit immediately if there is an error.
set -e
# initialize variable

NUM_SUCCEED_RUN=0
NUM_FAIL_RUN=0


# Assume some default setting
STAGE=Debug
ARCH=i386

for var in "$@"
do 
	if [ "$var" == "-release" ]
	then
		STAGE=Release
	fi
  if [ "$var" == "-64-bit" ]
  then
    ARCH==x86_64
  fi  
done

echo "Configuration: "$STAGE
echo "Archs: "$ARCH
xcodebuild -project All_Datalogics.xcodeproj -target All_Datalogics -configuration $STAGE -arch=$ARCH
echo $?
if [ "$?" -eq "0" ]; then
    echo "All Sample" $STAGE $ARCH "Build Success"
else
    echo "All Sample" $STAGE $ARCH "Build Failed"
fi

# Sample Name List
declare -a DL_SAMPLE_LIST=( \
	"AddArt" "AddAttachments" "AddBookmarks" \
	"AddContent" "AddDocumentInformation" "AddLinks" \
  "AddPageNumbers" "AddPassword" "AddRedaction" \
  "AddText" "AddWatermark" "CopyContent" \
  "CreateAnnotations" "CreateDocument" "CreateLayers" \
  "CreateTransparency" "EncryptDocument" "ExtractAttachments" \
  "ExtractDocumentInfo" "ExtractText" "FindImageresolutions" \
  "FlattenAnnotations" "FlattenTransparency" "LockDocument" \
  "MergeDocuments" "OpenEncrypted" "RasterizeCopy" \
  "SetUniquePermissions" "SplitPDF" "TextSearch" \
  "UnicodeText" "WebOptimizedPDF"
	)

## now loop through the DL Samples
for i in "${DL_SAMPLE_LIST[@]}"
do
   echo "$i"
   cd ../"$i"
   ./"$i"-${STAGE}.app/Contents/MacOS/"$i"-${STAGE}
   # check the status
   if [ "$?" -eq "0" ]; then
   	   NUM_SUCCEED_RUN=$(( $NUM_SUCCEED_RUN + 1 ))
   else
   	   NUM_FAIL_RUN=$(( $NUM_FAIL_RUN + 1))
   fi
done

echo "Succeed Run number: " $NUM_SUCCEED_RUN

echo "Failed Run number: " $NUM_FAIL_RUN

echo All samples are finished running.

exit $NUM_FAIL_RUN
