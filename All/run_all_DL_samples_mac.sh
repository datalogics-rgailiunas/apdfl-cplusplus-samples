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

echo Running sample AddAttachments
cd ../AddAttachments
./AddAttachments-${stage}.app/Contents/MacOS/AddAttachments-${stage}
echo ""


echo All Datalogics samples are finished running.

