// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//*******************************************************************************
//Helper App. for Samples: APDFLDoc is intended to assist with common PDDoc
//operations. This class contains methods that open and create documents and 
//performs other common operations.
//APDFLDoc.cpp: Contains implementations of methods.
//APDFLDoc.h: Contains class definition.
//*******************************************************************************
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

#include "MyPDFLibUtils.h"
#include "APDFLDoc.h"
#include "PDCalls.h"
#include "ASCalls.h"
#include "ASExtraCalls.h"

#include <iostream>
#include <vector>

class APDFLDoc {
    
public:
    static const unsigned MAX_PATH_LENGTH = 260;                    //Data members assosciated with document
    wchar_t nameOfDocument[MAX_PATH_LENGTH];
    volatile ASPathName asPathName;
    PDPage pdPage;
    volatile PDDoc pdDoc;
    ASErrorCode errorCode;

    
    APDFLDoc(wchar_t*, bool doRepairDamagedFile);                   //Constructor used to open a document
    APDFLDoc(unsigned width = 0, unsigned height = 0);              //Constructor used to create a document
    void setDocumentName(const wchar_t *);                          //Used to set the name of the document
    ASErrorCode setASPathName(wchar_t *);                           //Used to set ASPathNameObjects
    ASErrorCode insertPage(const int & width, const int & height);  //Inserts a page into the document
    ASErrorCode saveWebOptimized();                                 //saves doc as weboptimized
    ASErrorCode saveDoc();                                          //fully saves non web optimized doc
    ASErrorCode printErrorHandlerMessage();                         //Prints/Returns the error message within the handler block
    ~APDFLDoc();                                                    //Destructor frees up resources

    APDFLDoc(const APDFLDoc &){};                                   //Do not allow copy constructor or assignment operator to be used
    APDFLDoc& operator=(const APDFLDoc&){};                         //in order to prevent shallow copies of objects
};

#endif
