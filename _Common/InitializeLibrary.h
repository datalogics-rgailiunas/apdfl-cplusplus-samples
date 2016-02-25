// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//========================================================================
// Sample - Initialize: This class defines an object used for the
// initialization and termination of the Adobe PDF Library. It will also
// report initialization errors.
//
// InitializeLibrary.cpp: Contains the method implementations.
// InitializeLibrary.h: Contains the class definition.
//========================================================================

#ifndef INITLIB_H
#define INITLIB_H

#define NUM_FONTS 2          //The number of font directories we'll include during initialization.    
#define NUM_COLOR_PROFS 1    //The number of color profile directories we'll include during initialization.
#define NUM_PLUGIN_DIRS 1    //The number of plugin directories we'll include during initialization.

#include <iostream>
#include "PDFLCalls.h"
#include "ASCalls.h"

class APDFLib
{
public:
    APDFLib(wchar_t* dl100Dir = NULL);                //Constructor initializes APDFL and sets the path to DL100PDFL.dll to dl100Dir. If NULL is passed, defaults to ../../Libs. dl100Dir should be a relative path.
    ~APDFLib();                                       //Destructor terminates APDFL.

    ASInt32 getInitError();                           //Reports whether an error happened during initialization and returns that error.
    ASBool isValid() { return initValid; };           //Returns true if the library initialized successfully.
    void displayError(ASErrorCode);                   //Utility method, may be used to print APDFL errors to the terminal.

private:
    PDFLDataRec pdflData;                             //A struct containing information that APDFL initializes with.
    ASInt32 initError;                                //Used to record initialization errors.
    ASBool initValid;                                 //Set to true if the library initializes successfully.

    void fillDirectories();                           //Sets directory information for our PDFLDataRec.
#if WIN_PLATFORM
    HINSTANCE loadDFL100PDFL(wchar_t* relativeDir);   //Loads the DL100PDFL library dynamically.
#endif

    ASUTF16Val* fontDirList[NUM_FONTS];               //List of font directories we'll include during initialization.              //TODO: platform divergences
    ASUTF16Val* colorProfDirList[NUM_COLOR_PROFS];    //List of color profile directories we'll include during initialization.     //TODO: platform divergences
    ASUTF16Val* pluginDirList[NUM_PLUGIN_DIRS];       //List of plugin directories we'll include during initialization.            //TODO: platform divergences
};

#endif //INITLIB_H
