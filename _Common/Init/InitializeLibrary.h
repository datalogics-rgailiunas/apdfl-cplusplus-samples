// Copyright(c) 2015, Datalogics, Inc.All rights reserved.

//========================================================================
// Sample - Initialize: This class defines an object used for the
// initialization and termination of the Adobe PDF Library. It will also
// report initialization errors.
//
// InitializeLibrary.cpp: Contains the method implementations.
// InitializeLibrary.h: Contains the class definition.
//========================================================================

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

#ifndef INITLIB_H
#define INITLIB_H

#define NUM_FONTS 2          //The number of font directories we'll include during initialization.    
#define NUM_COLOR_PROFS 1    //The number of color profile directories we'll include during initialization.
#define NUM_PLUGIN_DIRS 1    //The number of plugin directories we'll include during initialization.

#include <iostream>
#include "PDFLCalls.h"

class APDFLib
{
public:
    APDFLib();                                        //Constructor initializes APDFL.
    ~APDFLib();                                       //Destructor terminates APDFL.

    ASInt32 getInitError();                           //Reports whether an error happened during initialization and returns that error.
    ASBool isValid() { return initValid; };           //Returns true if the library initialized successfully.
    void displayError(ASErrorCode);                   //Utility method, may be used to print APDFL errors to the terminal.

private:
    PDFLDataRec pdflData;                             //A struct containing information that APDFL initializes with.
    ASInt32 initError;                                //Used to record initialization errors.
    ASBool initValid;                                 //Set to true if the library initializes successfully.

    void fillDirectories();                           //Sets directory information for our PDFLDataRec.

    ASUTF16Val* fontDirList[NUM_FONTS];               //List of font directories we'll include during initialization.              //TODO: platform divergences
    ASUTF16Val* colorProfDirList[NUM_COLOR_PROFS];    //List of color profile directories we'll include during initialization.     //TODO: platform divergences
    ASUTF16Val* pluginDirList[NUM_PLUGIN_DIRS];       //List of plugin directories we'll include during initialization.            //TODO: platform divergences
};

#endif //INITLIB_H
