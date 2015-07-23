// Copyright(c) 2015, Datalogics, Inc.All rights reserved.

//========================================================================
// Sample: Create Document - Creates a new document and inserts pages.
//	
// Steps:
// 1) Create the document and add pages.
// 2) Save the document and release resources.
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
// DEFICIENCY, OR NONCONFORMITY IN ANY EXAMPLE CODE. */

#include <iostream>

#include "InitializeLibrary.h"
#include "ASExtraCalls.h"

#define PAGES_TO_INSERT 5

int main(int argc, char** argv)
{
    APDFLib libInit;                   //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;           //Variable used to report any exceptions/errors if they occured. 

    if (libInit.isValid() == false)    //If there was a problem in initialization, return the error code.
        return libInit.getInitError();           
        
    DURING

//=====================================================================================================================
// Step 1: Create the document and add pages.) ASFixedRect will hold the page dimensions. 
// Note: 72 pixels == 1 Inch
//=====================================================================================================================
        
        PDDoc pdDoc = PDDocCreate();    //Create a new PDF document.

        //Set the dimensions for the page. In this case 8.5 x 11 inches.
        ASFixedRect mediaBox; 
        mediaBox.left = fixedZero;
        mediaBox.right = FloatToASFixed(72.0 * 8.5);
        mediaBox.bottom = fixedZero;
        mediaBox.top = FloatToASFixed(72.0 * 11.0);

        std::wcout << L"Inserting pages into Document..." << std::endl;

        //Create and insert 5 pages into the PDF document.
        for (unsigned i = 0; i < PAGES_TO_INSERT; ++i)
            PDDocCreatePage(pdDoc, PDBeforeFirstPage, mediaBox);
  
//=====================================================================================================================
//Step 2: Save the document and release resources.) ASPathName must be created in order to call the PDDocSave method.
//=====================================================================================================================

        std::wcout << L"Creating path to output file..." << std::endl;
        
        wchar_t * nameOfOutputFile = L"out.pdf";    

        ASText textToCreatePath = NULL;    //ASText object is used to create the ASPathName object.

        //Determine the size of wchar_t on the system, and set the ASTextObject to hold the path for the output document.
        if (sizeof(wchar_t) == 2)
            textToCreatePath = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*> (nameOfOutputFile), kUTF16HostEndian);
        else
            textToCreatePath = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(nameOfOutputFile), kUTF32HostEndian);

        //Create the ASPathName object from the ASText. This will be used to save the PDDoc.
        ASPathName asPathName = ASFileSysCreatePathFromDIPathText(NULL, textToCreatePath, NULL);
    
        std::wcout << L"Saving document and freeing resources..." << std::endl;
         
        PDDocSave(pdDoc, PDSaveFull, asPathName, ASGetDefaultFileSys(), NULL, NULL);    //Save the PDF document.

        //Release all objects that are still in use.
        ASTextDestroy(textToCreatePath);
        ASFileSysReleasePath(NULL, asPathName);
        PDDocClose(pdDoc);

    HANDLER

        errCode = libInit.getInitError();

        libInit.displayError(errCode);    //If there was an error, display the error that occured.

    END_HANDLER

    return errCode;                       //APDFLib's destructor terminates the library.

}
