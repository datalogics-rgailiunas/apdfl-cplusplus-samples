//# Copyright(c) 2015, Datalogics, Inc.All rights reserved.

//************************************************************************
// Sample: Create Document - Creates a document with page numbers
//
// Note: This program creates a new document and places page numbers
// 	     on each of the created pages
//	
//CreateDocument.h contains CreateDocument class definition
//CreateDocumentApp.cpp contains the driver
//CreateDocument contains method implementations
//Steps:
// 1) Create Document and add pages
// 2) Save the document and release resources
//************************************************************************

//# This agreement is between Datalogics, Inc. 101 N.Wacker Drive, Suite 1800,
//# Chicago, IL 60606 ("Datalogics") and you, an end user who downloads
//# source code examples for integrating to the Adobe PDF Library
//# ("the Example Code"). By accepting this agreement you agree to be bound
//# by the following terms of use for the Example Code.
//#
//# LICENSE
//# -------
//# Datalogics hereby grants you a royalty - free, non - exclusive license to
//# download and use the Example Code for any lawful purpose.There is no charge
//# for use of Example Code.
//#
//# OWNERSHIP
//# ---------
//# The Example Code and any related documentation and trademarks are and shall
//# remain the sole and exclusive property of Datalogics and are protected by
//# the laws of copyright in the U.S.and other countries.
//#
//# Datalogics is a trademark of Datalogics, Inc.
//#
//# TERM
//# ----
//# This license is effective until terminated.You may terminate it at any
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
//# NON - INFRINGEMENT, ACCURACY OR COMPLETENESS OF RESPONSES, RESULTS, AND / OR
//# LACK OF WORKMANLIKE EFFORT.THE PROVISIONS OF THIS SECTION SET FORTH
//# SUBLICENSEE'S SOLE REMEDY AND DATALOGICS'S SOLE LIABILITY WITH RESPECT
//# TO THE WARRANTY SET FORTH HEREIN.NO REPRESENTATION OR OTHER AFFIRMATION
//# OF FACT, INCLUDING STATEMENTS REGARDING PERFORMANCE OF THE EXAMPLE CODE,
//# WHICH IS NOT CONTAINED IN THIS AGREEMENT, SHALL BE BINDING ON DATALOGICS.
//# NEITHER DATALOGICS WARRANT AGAINST ANY BUG, ERROR, OMISSION, DEFECT,
//# DEFICIENCY, OR NONCONFORMITY IN ANY EXAMPLE CODE. */

#include <iostream>

#include "InitializeLibrary.h"
#include "ASExtraCalls.h"

#define PAGES_TO_INSERT 5

int main(int argc, char** argv)
{
    APDFLib libInit;                             //Initialize the APDFL
    ASErrorCode errCode = 0;                     //Error code is 0 if there were no issues 

    if (libInit.isValid() == false)              //If there was a problem in initializing the APDFL
        return errCode = libInit.getInitError(); //Return the error code.
        
    DURING

//==================================================================
//Step 1) Create the document and add pages
//==================================================================
        
        PDDoc pdDoc = PDDocCreate();  //Create the document

        //Set page dimensions for PDDoc (72 pixels in an inch)
        ASFixedRect mediaBox; 
        mediaBox.left = fixedZero;
        mediaBox.right = Int16ToFixed(72 * 8.5);
        mediaBox.bottom = fixedZero;
        mediaBox.top = Int16ToFixed(72 * 11);

        std::wcout << L"Inserting pages into Document..." << std::endl;

        //Create and insert pages in newly created document
        for (unsigned i = 0; i < PAGES_TO_INSERT; ++i)
            PDDocCreatePage(pdDoc, PDBeforeFirstPage, mediaBox);
  
//==================================================================
//Step 2) Save the document and release resources
//==================================================================

        std::wcout << L"Creating path to output file..." << std::endl;
        
        wchar_t * nameOfOutputFile = L"out.pdf\0";  //Name of file being created

        ASText textToCreatePath = NULL; //Text object used to create ASPathName

        //Determine size of wchar_t on the system and set the ASTextObject to hold the path name for output
        if (sizeof(wchar_t) == 2)
            textToCreatePath = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*> (nameOfOutputFile), kUTF16HostEndian);
        else
            textToCreatePath = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(nameOfOutputFile), kUTF32HostEndian);

        //Create the pathname used by PDDocSave()
        ASPathName asPathName = ASFileSysCreatePathFromDIPathText(NULL, textToCreatePath, NULL);
    
        std::wcout << L"Saving document and freeing resources..." << std::endl;
         
        PDDocSave(pdDoc, PDSaveFull | PDSaveLinearized, asPathName, ASGetDefaultFileSys(), NULL, NULL); //Save the newly created document

        //Release text object, path and pddoc
        ASTextDestroy(textToCreatePath);
        ASFileSysReleasePath(NULL, asPathName);
        PDDocClose(pdDoc);

    HANDLER

        libInit.displayError(errCode); //Display any exceptions that may have occured.

    END_HANDLER

    return errCode;

}
