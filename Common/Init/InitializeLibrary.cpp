// Copyright(c) 2015, Datalogics, Inc.All rights reserved.

//=========================================================================
// Sample: Initialize - Initializes the APDFLibrary.
//=========================================================================

// This agreement is between Datalogics, Inc. 101 N.Wacker Drive, Suite 1800,
// Chicago, IL 60606 ("Datalogics") and you, an end user who downloads
// source code examples for integrating to the Adobe PDF Library
// ("the Example Code"). By accepting this agreement you agree to be bound
// by the following terms of use for the Example Code.
//
// LICENSE
// -------
// Datalogics hereby grants you a royalty - free, non - exclusive license to
// download and use the Example Code for any lawful purpose.There is no charge
// for use of Example Code.
//
// OWNERSHIP
// ---------
// The Example Code and any related documentation and trademarks are and shall
// remain the sole and exclusive property of Datalogics and are protected by
// the laws of copyright in the U.S.and other countries.
//
// Datalogics is a trademark of Datalogics, Inc.
//
// TERM
// ----
// This license is effective until terminated.You may terminate it at any
// other time by destroying the Example Code.
//
// WARRANTY DISCLAIMER
// -------------------
// THE EXAMPLE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
// EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// DATALOGICS DISCLAIM ALL OTHER WARRANTIES, CONDITIONS, UNDERTAKINGS OR
// TERMS OF ANY KIND, EXPRESS OR IMPLIED, WRITTEN OR ORAL, BY OPERATION OF
// LAW, ARISING BY STATUTE, COURSE OF DEALING, USAGE OF TRADE OR OTHERWISE,
// INCLUDING, WARRANTIES OR CONDITIONS OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, SATISFACTORY QUALITY, LACK OF VIRUSES, TITLE,
// NON - INFRINGEMENT, ACCURACY OR COMPLETENESS OF RESPONSES, RESULTS, AND / OR
// LACK OF WORKMANLIKE EFFORT.THE PROVISIONS OF THIS SECTION SET FORTH
// SUBLICENSEE'S SOLE REMEDY AND DATALOGICS'S SOLE LIABILITY WITH RESPECT
// TO THE WARRANTY SET FORTH HEREIN.NO REPRESENTATION OR OTHER AFFIRMATION
// OF FACT, INCLUDING STATEMENTS REGARDING PERFORMANCE OF THE EXAMPLE CODE,
// WHICH IS NOT CONTAINED IN THIS AGREEMENT, SHALL BE BINDING ON DATALOGICS.
// NEITHER DATALOGICS WARRANT AGAINST ANY BUG, ERROR, OMISSION, DEFECT,
// DEFICIENCY, OR NONCONFORMITY IN ANY EXAMPLE CODE.

#include "InitializeLibrary.h"

//====================================================
//Sets the PDFLDataRec and initializes the library 
//with that data.
//====================================================
APDFLib::APDFLib()
{
    initValid = false;

    memset(&pdflData, 0, sizeof(PDFLDataRec));

    pdflData.size = sizeof(PDFLDataRec);
    pdflData.allocator = NULL;          //Use default memory allocation procedures.
    fillDirectories();                  //Sets the PDFLData datamember prior to calling PDFLInitHFT()

    initError = PDFLInitHFT(&pdflData); //Initialize the library

    if (initError == 0)                 //If initError is 0, initialization success
        initValid = true;              
}

//====================================================
//Reports whether an error happened and returns that 
//error.
//====================================================
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

//====================================================
//Sets directory information for our PDFLDataRec.
//====================================================
void APDFLib::fillDirectories()
{
    fontDirList[0] = (ASUTF16Val*)L"..\\..\\..\\APDFL\\Resource\\Font";
    fontDirList[1] = (ASUTF16Val*)L"..\\..\\..\\APDFL\\Resource\\CMap";
    pdflData.dirList = fontDirList;
    pdflData.listLen = NUM_FONTS;


    colorProfDirList[0] = (ASUTF16Val*)L"..\\..\\..\\APDFL\\Resource\\Color\\Profiles";
    pdflData.colorProfileDirList = colorProfDirList;
    pdflData.colorProfileDirListLen = NUM_COLOR_PROFS;


    pdflData.cMapDirectory = fontDirList[1];
    pdflData.unicodeDirectory = (ASUTF16Val*)L"..\\..\\..\\APDFL\\Resource\\Unicode";


    pluginDirList[0] = (ASUTF16Val*)L"..\\..\\..\\APDFL\\Libs";
    pdflData.pluginDirList = pluginDirList;
    pdflData.pluginDirListLen = NUM_PLUGIN_DIRS;
}

//====================================================
//Destructor terminates the library when program ends.
//====================================================
APDFLib::~APDFLib()
{
    PDFLTermHFT();
}
