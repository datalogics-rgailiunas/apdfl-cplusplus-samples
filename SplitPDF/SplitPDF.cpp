// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
//=================================================================================
// Sample: Opens a file called toBeSplit.pdf which can be any length, and splits 
//         it into seperate pdf's each being a single page of the original. These 
//         split documents are then saved to the current directory.
//
// Steps: 
//  1) Open toBeSplit.pdf, the source of the split pdf files
//  2) Create, and name each page as a new pdf in the form
//         baseDocument_Page<PAGE_NUMBER>.pdf
//  3) Save output files into working directory, then exit
//=================================================================================
//
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

#include "MyPDFLibUtils.h"
#include <vector>
#include <string>
#include "ASExtraCalls.h"
#include "APDFLDoc.h"
#include "InitializeLibrary.h"
#include <iostream>

int main(int argc, char** argv)
{
    APDFLib libInit;            //Initialize the APDFL.
    ASErrorCode errCode = 0;    //Variable that represents errors

    if (libInit.isValid() == false)         //Check for errors in initialization.
        errCode = libInit.getInitError();   //If there was an error set the code.

//=================================================================================================================
// Step 1) Open toBeSplit.pdf, the source of the split pdf files
//=================================================================================================================

    DURING

        APDFLDoc document(L"../Input/toBeSplit.pdf", true);    //Open a document from the and repair if damaged

//=================================================================================================================
// Step 2) Create, and name each page as a new pdf in the form baseDocument_Page<PAGE NUMBER>.pdf
//=================================================================================================================

        //A vector of the PDDoc type that will hold the individual pages of a document
        std::vector<PDDoc> splitDocs(PDDocGetNumPages(document.getPDDoc()));

        //An iterator used to get track of and access the above splitDocs vector
        std::vector<PDDoc>::iterator iter = splitDocs.begin();
       
        int tracker;                      //Tracks the index of the iterator     
        std::wstring pageNameString;      //Name of page used for saving
        ASText outPathText = NULL;        //The output file path test object for the splits
        ASPathName outPathName = NULL;    //The output file path name for the splits

        //Advance the iterator through the splitDocs vector
        for (iter = splitDocs.begin(); iter < splitDocs.end(); iter++)
        {

            //Dereference the iterator to get the page it points to and create it as a PDDoc 
            *iter = PDDocCreate();

            //Set to current index
            tracker = iter - splitDocs.begin(); 

            //Insert the right page from the source pdf, based on the index of the iterator
            PDDocInsertPages(*iter, PDBeforeFirstPage, document.getPDDoc(), tracker, 1, NULL, NULL, NULL, NULL, NULL);

            //Set the output file name according to what page number is currently accessed              
            pageNameString = L"baseDocument_Page" + std::to_wstring(tracker + 1) + L".pdf";

//=================================================================================================================
// Step 3) Save output files into working directory, then exit
//=================================================================================================================
            
            //Save the document with the given path, and save flags
            document.saveDoc((wchar_t*)(pageNameString.c_str()), PDSaveFull | PDSaveLinearized); 

            std::wcout << std::endl << pageNameString << L" was created and saved" << std::endl;

        }

    HANDLER

        //If there was an exception generate an error code 
        errCode = ERRORCODE;

        //Display the error code
        libInit.displayError(errCode); 

    END_HANDLER

    return errCode;
}
