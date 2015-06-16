
//# Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
//# Sample splitPdf/ Splits a pdf into seperate pdf files 
//
//# This agreement is between Datalogics, Inc. 101 N. Wacker Drive, Suite 1800,
//# Chicago, IL 60606 ("Datalogics") and you, an end user who downloads
//# source code examples for integrating to the Adobe PDF Library
//# ("the Example Code"). By accepting this agreement you agree to be bound
//# by the following terms of use for the Example Code.
//#
//# LICENSE
//# -------
//# Datalogics hereby grants you a royalty-free, non-exclusive license to
//# download and use the Example Code for any lawful purpose. There is no charge
//# for use of Example Code.
//#
//# OWNERSHIP
//# ---------
//# The Example Code and any related documentation and trademarks are and shall
//# remain the sole and exclusive property of Datalogics and are protected by
//# the laws of copyright in the U.S. and other countries.
//#
//# Datalogics is a trademark of Datalogics, Inc.
//#
//# TERM
//# ----
//# This license is effective until terminated. You may terminate it at any
//# other time by destroying the Example Code.
//#
//# WARRANTY DISCLAIMER
//# -------------------
//# THE EXAMPLE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
//# EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES
//# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//#
//# DATALOGICS DISCLAIM ALL OTHER WARRANTIES, CONDITIONS, UNDERTAKINGS OR
//# TERMS OF ANY KIND, EXPRESS OR IMPLIED, WRITTEN OR ORAL, BY OPERATION OF
//# LAW, ARISING BY STATUTE, COURSE OF DEALING, USAGE OF TRADE OR OTHERWISE,
//# INCLUDING, WARRANTIES OR CONDITIONS OF MERCHANTABILITY, FITNESS FOR A
//# PARTICULAR PURPOSE, SATISFACTORY QUALITY, LACK OF VIRUSES, TITLE,
//# NON-INFRINGEMENT, ACCURACY OR COMPLETENESS OF RESPONSES, RESULTS, AND/OR
//# LACK OF WORKMANLIKE EFFORT. THE PROVISIONS OF THIS SECTION SET FORTH
//# SUBLICENSEE'S SOLE REMEDY AND DATALOGICS'S SOLE LIABILITY WITH RESPECT
//# TO THE WARRANTY SET FORTH HEREIN. NO REPRESENTATION OR OTHER AFFIRMATION
//# OF FACT, INCLUDING STATEMENTS REGARDING PERFORMANCE OF THE EXAMPLE CODE,
//# WHICH IS NOT CONTAINED IN THIS AGREEMENT, SHALL BE BINDING ON DATALOGICS.
//# NEITHER DATALOGICS WARRANT AGAINST ANY BUG, ERROR, OMISSION, DEFECT,
//# DEFICIENCY, OR NONCONFORMITY IN ANY EXAMPLE CODE.

//  
//
// Project: splitPdf
//
// Note: By default, this example program opens a file called toBeSplit.pdf
//    which can be any length, and splits it into seperate pdf's each being 
//    a single page of the original. These split documents are then saved to
//    the current directory
//
// Steps: 
//
// * Open split pdf
// * Create, name and save each page as a new pdf
//          in the baseDocument_Page<PAGE NUMBER>.pdf
// 



#include "MyPDFLibUtils.h"
#include <vector>
#include <string>
#include "ASCalls.h"
#include "ASExtraCalls.h"
#include <iostream>


int main(int argc, char **argv)
{
    //Initialize the pdf library
    int  err = MyPDFLInit();

    //Check for errors upon initialization 
    if (err != 0)
    {
        std::cerr << "Initialization error. See \"AcroErr.h\" for more info.\n" << std::endl;
        std::cerr << "Error system: " << ErrGetSystem(err) << std::endl;
        std::cerr << "Error Severity: " << ErrGetSeverity(err) << std::endl;
        std::cerr << "Error Code: " << ErrGetCode(err) << std::endl;
        return 0;
    }


    //Initialize selected variables.  If these variable are 
    //left uninitialized, and an exception occurs early in the 
    //execution of the sample, a Segmentation Violation crash is
    //likely.  Properly initializing these variables to NULL
    //avoids the crash.

    wchar_t  pathToOrig[] = L"toBeSplit.pdf";                           // C-String filename used by MyPDDocOpen() 
    PDDoc pdDocOrig = NULL;                                             // The source pdf from which the split pdfs will orignate from             
    ASErrorCode errCode = 0;                                            // Used by HANDLER to report exceptions 
    std::wstring pageNameString = L"";                                  // Name of page used for saving
    ASText origPathText = NULL;                                         // The input file path text object for the splits
    ASPathName origPathName = NULL;                                     // The input file path name for the splits
    ASText outPathText = NULL;                                          // The output file path test object for the splits
    ASPathName outPathName = NULL;                                      // The output file path name for the splits
    const std::wstring pageNameWStringBase = L"baseDocument_Page";      // The first portion of the output files' names
    const std::wstring pageNameWStringEnd = L".pdf";                    // The ending portion of the output files' names
    ASUnicodeFormat uniFormat = NULL;                                   // Format object to be used to hold the Unicode format for path access




    /*==================================================================*\
                                    Open Doc
    \*==================================================================*/

    //Exception handler must be used for all APDFL Calls
    DURING

        //Open the document to be split 
        if (sizeof(wchar_t) == 2)
            uniFormat = kUTF16HostEndian;
        else
            uniFormat = kUTF32HostEndian;

        origPathText = ASTextFromUnicode((ASUTF16Val *)pathToOrig, uniFormat);

        origPathName = ASFileSysCreatePathFromDIPathText(NULL, origPathText, NULL);

        pdDocOrig = PDDocOpen(origPathName, NULL, NULL, true);

        //Ensure document opened
        if (!pdDocOrig)
        {
            std::cerr << "Unable to open file to be split" << pathToOrig << "$$$" << std::endl;
            E_RETURN(0);
        }


        /*===========================================================================*\
             Extract each page from orignal pdf and save as seperate file; Splitting
        \*===========================================================================*/

        //A vector of the PDDoc type that will hold the individual pages of a document
        std::vector<PDDoc> splitDocs(PDDocGetNumPages(pdDocOrig));

        //An iterator used to get track of and access the above splitDocs vector
        std::vector<PDDoc>::iterator iter = splitDocs.begin();

        //Tracks the index of the iterator
        int tracker;    

        //Advance the iterator through the splitDocs vector
        for (iter = splitDocs.begin(); iter < splitDocs.end(); iter++)
        {

            //Derefernce to iterator to get the page it's meant to point to and create it as a PDDoc 
            *iter = PDDocCreate();
            tracker = iter - splitDocs.begin(); //set to current index

            //Insert the right page from the source pdf, based on the index of the iterator
            PDDocInsertPages(*iter, PDBeforeFirstPage, pdDocOrig, tracker, 1, NULL, NULL, NULL, NULL, NULL);

            //Set the output file name according to what page number is currently accessed              
            pageNameString = pageNameWStringBase + std::to_wstring(tracker) + pageNameWStringEnd;

            outPathText = ASTextFromUnicode((ASUTF16Val *)(wchar_t*)(pageNameString.c_str()), uniFormat);

            //Use the string to form ASPathName
            outPathName = ASFileSysCreatePathFromDIPathText(NULL, outPathText, NULL);

            //Save file using the ASPathName
            PDDocSave(*iter, PDSaveFull | PDSaveLinearized, outPathName, ASGetDefaultFileSys(), NULL, NULL);
            std::wcout << std::endl << pageNameString << " was created and saved";

            //Release the document after saving
            PDDocRelease(*iter);

            //Release the path so that it can be reused within the loop
            ASFileSysReleasePath(NULL, outPathName);
        }

    HANDLER
        errCode = ERRORCODE;
    END_HANDLER

    //Release/close all all objects that were used
    if (outPathName) ASFileSysReleasePath(NULL, outPathName);
    if (origPathName) ASFileSysReleasePath(NULL, origPathName);
    if (pdDocOrig)  PDDocClose(pdDocOrig);

    //If there was an error print, release used objects and exit
    if (errCode)
    {
        DisplayError(errCode);  //Display the error
    }


    MyPDFLTerm();   //Terminate the pdf library

    std::cout << std::endl << std::endl;

    return 0;
}






