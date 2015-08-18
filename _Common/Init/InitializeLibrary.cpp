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

#include "InitializeLibrary.h"

//========================================================================================================
//Constructor:
//Sets the PDFLDataRec and initializes the library with that data.
//========================================================================================================
APDFLib::APDFLib()
{
    initValid = false;                            //Whether the initialization succeeded.

    memset(&pdflData, 0, sizeof(PDFLDataRec));    //Clear the data struct so we can set its data.

	//Set PDFLDataRec's data.
    pdflData.size = sizeof(PDFLDataRec);          //Give it its size.
    pdflData.allocator = NULL;                    //Use default memory allocation procedures.
    fillDirectories();                            //Set the directory inclusion data.

    initError = PDFLInitHFT(&pdflData);           //Initialize the library.

    if (initError == 0)                           //If initError is 0, initialization succeeded.
        initValid = true;              
}

//========================================================================================================
//ASInt32 function:
//Reports whether an error happened during initialization and returns that error.
//========================================================================================================
ASInt32 APDFLib::getInitError()
{
    if (initValid == false)
    {
        std::wcerr << L"Initialization error. See \"AcroErr.h\" for more info.\n" << std::endl;
        std::wcerr << L"Error system: " << ErrGetSystem(initError) << std::endl;
        std::wcerr << L"Error Severity: " << ErrGetSeverity(initError) << std::endl;
        std::wcerr << L"Error Code: " << ErrGetCode(initError) << std::endl;
    }

    return initError;
}

//========================================================================================================
//Void function:
//Sets directory information for our PDFLDataRec.
//========================================================================================================
void APDFLib::fillDirectories()
{
	//Set the font directory list and its length.
    fontDirList[0] = (ASUTF16Val*)L"..\\..\\..\\APDFL\\Resource\\Font";
    fontDirList[1] = (ASUTF16Val*)L"..\\..\\..\\APDFL\\Resource\\CMap";
    pdflData.dirList = fontDirList;
    pdflData.listLen = NUM_FONTS;

	//Set the color profile directory list and its length.
    colorProfDirList[0] = (ASUTF16Val*)L"..\\..\\..\\APDFL\\Resource\\Color\\Profiles";
    pdflData.colorProfileDirList = colorProfDirList;
    pdflData.colorProfileDirListLen = NUM_COLOR_PROFS;

	//Set the Unicode directory.
    pdflData.cMapDirectory = fontDirList[1];
    pdflData.unicodeDirectory = (ASUTF16Val*)L"..\\..\\..\\APDFL\\Resource\\Unicode";

	//Set the plugin directory and its length.
    pluginDirList[0] = (ASUTF16Val*)L"..\\..\\..\\APDFL\\Libs";
    pdflData.pluginDirList = pluginDirList;
    pdflData.pluginDirListLen = NUM_PLUGIN_DIRS;
}

//========================================================================================================
//Void function:
//Utility method, may be used to print APDFL errors to the terminal.
//========================================================================================================
void APDFLib::displayError(ASErrorCode errCode)
{
    if (errCode == 0) return;

    char errStr[250];
    std::fprintf(stderr, "[Error %lx] %s\n", errCode, ASGetErrorString(errCode, errStr, sizeof(errStr)));
}

//========================================================================================================
//Destructor:
//Terminates the library when program ends.
//========================================================================================================
APDFLib::~APDFLib()
{
    PDFLTermHFT();
}
