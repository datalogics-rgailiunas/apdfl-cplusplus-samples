// Copyright(c) 2015, Datalogics, Inc.All rights reserved.

//=================================================================================
// Sample: WebOptimizedPDF - Takes a non web optimized document as input and
// converts it into a web optimized document.
//
// Note: This program opens the file "nonweboptimized.pdf" in the ../Input directory,
// optimizes the document, and saves it in the working directory.
//
//Steps:
// 1) Open the input document.
// 2) Save it as a web-optimized document.
//=================================================================================

// This agreement is between Datalogics, Inc. 101 N.Wacker Drive, Suite 1800,
// Chicago, IL 60606 ("Datalogics") and you, an end user who downloads
// source code examples for integrating to the Adobe PDF Library
// ("the Example Code"). By accepting this agreement you agree to be bound
// by the following terms of use for the Example Code.
//
// LICENSE
// -------
// Datalogics hereby grants you a royalty - free, non - exclusive license to
// download and use the Example Code for any lawful purpose. There is no charge
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

#include <iostream>
#include "ASExtraCalls.h"
#include "InitializeLibrary.h"

int main(int argc, char** argv)
{
    APDFLib lib;                               //Initialize the Adobe PDF Library.

    if (lib.isValid() == false)                //Check to see if the Adobe PDF Library initialized.
        lib.getInitError();   //If it failed, return the error code.

    ASErrorCode errCode = 0;                   //Will catch error codes thrown during library usage.

    DURING

        PDDoc pdDoc = NULL;                                             //Declare pdDoc used for opening and saving the non web optimized PDF as web optimized

        wchar_t * inputPathName  = L"../_Input/NonLinearized.pdf";   //Variables holding the input/output file names
        wchar_t * outputPathName = L"WebOptimized.pdf";

        ASPathName asPathNameInputFile = NULL;                          //ASPathNames used by PDDocOpen()
        ASPathName asPathNameOutputFile = NULL;

        ASText asTextForInputFile = NULL;                               //Use ASText object to create ASPathNames for compatibility on different machines
        ASText asTextForOutputFile = NULL;

        //Determine hosts native Endian order based on size of wchar_t
        if (sizeof(wchar_t) == 2)
        {
            //Initialize ASText Object 
            asTextForInputFile = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(inputPathName), kUTF16HostEndian);
            asTextForOutputFile = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(outputPathName), kUTF16HostEndian);
        }
        else
        {
            //Initialize ASText Object 
            asTextForInputFile = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(inputPathName), kUTF32HostEndian);
            asTextForOutputFile = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(outputPathName), kUTF32HostEndian);
        }

        //Initialize ASPathNames for use by PDDocOpen() and PDDocSave()
        asPathNameInputFile = ASFileSysCreatePathFromDIPathText(NULL, asTextForInputFile, NULL);
        asPathNameOutputFile = ASFileSysCreatePathFromDIPathText(NULL, asTextForOutputFile, NULL);

        ASTextDestroy(asTextForOutputFile);                       //Release the ASTextObjects
        ASTextDestroy(asTextForInputFile);

        std::wcout << L"Opening non web optimized pdf document..." << std::endl;

        pdDoc = PDDocOpen(asPathNameInputFile, NULL, NULL, true); // Open the PDF Document

        std::wcout << L"Document open. " << std::endl;

        std::wcout << L"Optimizing document for web..." << std::endl;

        
        PDDocSave(pdDoc, PDSaveFull | PDSaveLinearized, asPathNameOutputFile, NULL, NULL, NULL);    //Save the newly opened document as a Web Optimimized PDF Document

        std::wcout << L"Document has been web optimized. " << std::endl;

        ASFileSysReleasePath(NULL, asPathNameOutputFile);   //Release the pathname objects
        ASFileSysReleasePath(NULL, asPathNameInputFile);
 
        PDDocClose(pdDoc);                                  //Close the PDDoc Object

        pdDoc = NULL;

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);                                                                   //If there was an error, display it.

    END_HANDLER

    std::wcout << L"Document has been closed. " << std::endl;

    return (errCode);                                                                           //lib's destructor terminates the library.
}
