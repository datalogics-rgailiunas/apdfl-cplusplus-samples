// Copyright (c) 2015, Datalogics, Inc. All rights reserved.

//===============================================================================
//Sample: APDFLDoc -This class is intended to assist with opeations common to 
//most samples. The class is capable of opening/creating and saving a document.
//It can also insert and retrieve pages.
//
//APDFLDoc.cpp: Contains implementations of methods.
//APDFLDoc.h: Contains class definition.
//===============================================================================

// Sample placeText/ Places text onto a pdf
// This agreement is between Datalogics, Inc. 101 N. Wacker Drive, Suite 1800,
// Chicago, IL 60606 ("Datalogics") and you, an end user who downloads
// source code examples for integrating to the Adobe PDF Library
// ("the Example Code"). By accepting this agreement you agree to be bound
// by the following terms of use for the Example Code.
//
// LICENSE
// -------
// Datalogics hereby grants you a royalty-free, non-exclusive license to
// download and use the Example Code for any lawful purpose. There is no charge
// for use of Example Code.
//
// OWNERSHIP
// ---------
// The Example Code and any related documentation and trademarks are and shall
// remain the sole and exclusive property of Datalogics and are protected by
// the laws of copyright in the U.S. and other countries.
//
// Datalogics is a trademark of Datalogics, Inc.
//
// TERM
// ----
// This license is effective until terminated. You may terminate it at any
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
// NON-INFRINGEMENT, ACCURACY OR COMPLETENESS OF RESPONSES, RESULTS, AND/OR
// LACK OF WORKMANLIKE EFFORT. THE PROVISIONS OF THIS SECTION SET FORTH
// SUBLICENSEE'S SOLE REMEDY AND DATALOGICS'S SOLE LIABILITY WITH RESPECT
// TO THE WARRANTY SET FORTH HEREIN. NO REPRESENTATION OR OTHER AFFIRMATION
// OF FACT, INCLUDING STATEMENTS REGARDING PERFORMANCE OF THE EXAMPLE CODE,
// WHICH IS NOT CONTAINED IN THIS AGREEMENT, SHALL BE BINDING ON DATALOGICS.
// NEITHER DATALOGICS WARRANT AGAINST ANY BUG, ERROR, OMISSION, DEFECT,
// DEFICIENCY, OR NONCONFORMITY IN ANY EXAMPLE CODE.

#ifndef APDFLDOC_H
#define APDFLDOC_H

#include "PDCalls.h"
#include "ASCalls.h"
#include "ASExtraCalls.h"

#include <iostream>
#include <vector>

class APDFLDoc {

private:

    static const unsigned MAX_PATH_LENGTH = 1024;                          //Private data members assosciated with the document.
    wchar_t nameOfDocument[MAX_PATH_LENGTH];

    volatile ASPathName asPathName;
    ASErrorCode errorCode;

    void initialize();                                                     //Called in constructor to initialize some data members.
    ASErrorCode printErrorHandlerMessage();                                //Prints an error message and returns the appropriate error code.
    ASErrorCode setASPathName(wchar_t* );                                  //Helper method used to create ASPathName objects for operations.

public:

    volatile PDDoc pdDoc;                                                  //Made public so it can be accessed directly.

    APDFLDoc(wchar_t*, bool doRepairDamagedFile);                          //Constructor used to open a document.
    APDFLDoc();                                                            //Constructor used to create a document.

    ASErrorCode insertPage(const ASFixed & width, const ASFixed & height, ASInt32);  //Inserts a page into the document.
    ASErrorCode insertPage(const ASInt16 & width, const ASInt16 & height, ASInt32);  //Inserts a page into the document.
    PDPage getPage(ASInt32);                                                         //Returns the specified PDPage, the first page is 0.

    volatile PDDoc& getPDDoc(){ return pdDoc; };                           //Returns a reference to the PDDoc that was created or opened.

    ASErrorCode saveDoc(wchar_t* = NULL, PDSaveFlags = PDSaveFull);        //Used to save the document, may be provided a path and PDSaveFlags.
    
    ~APDFLDoc();                                                           //Destructor frees up resources.

    APDFLDoc(const APDFLDoc& ){};                                          //Do not allow copy constructor or assignment operator to be used.
    APDFLDoc& operator=(const APDFLDoc&){};                                //in order to prevent shallow copies of objects.
};

#endif
