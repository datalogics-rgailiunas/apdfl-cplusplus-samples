//# Copyright(c) 2015, Datalogics, Inc.All rights reserved.

//==============================================================================
// Sample: AddDocumentInformation opens a document, inserts document 
// information and saves
//
// Note: This program inserts standard document information contained in the
// "Document Information Dictionary" see the PDF Reference section 10.2 for 
// more information
//
//Steps:
// 1) Open the Document that the document information will be inserted into
// 2) Insert the document information 
// 3) Save the document and release resources
//==============================================================================

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

#include <iostream>
#include "ASExtraCalls.h"
#include "InitializeLibrary.h"

int main(int argc, char** argv)
{
    APDFLib libInit;                               //Initialize the APDFL.
    ASErrorCode errCode = 0;                       //Set error code to 0.

    if (libInit.isValid() == false)                  //Check to see if the APDFL initialized.
        return errCode = libInit.getInitError();   //If it failed return the error code.

    DURING
//==========================================================================================================================================
//Step 1) Open the PDF Document
//==========================================================================================================================================
      
        wchar_t* inputFilePath = L"../_Input/AddDocumentInformation.pdf";   //File path used to open the PDF Document.
 
        ASUnicodeFormat unicodeFormat;                                      //Argument passed to function ASTextFromUnicode.

        //Determine host computers unicode format	
        if (sizeof(wchar_t) == 2)
            unicodeFormat = kUTF16HostEndian;
        else
            unicodeFormat = kUTF32HostEndian;

        //ASText is the unicode string used to create ASPathName for opening the PDDoc.
        ASText asText = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(inputFilePath), unicodeFormat);

        //Create the asPathName to open the input file.
        ASPathName asPathName = ASFileSysCreatePathFromDIPathText(NULL, asText, NULL);
  
        ASTextDestroy(asText);  //Free up resources.
        asText = NULL;
     
        PDDoc pdDoc = PDDocOpen(asPathName, NULL, NULL, true);  //Open the Document.

        std::wcout << L"Document was sucessfully opened." << std::endl;
  
        ASFileSysReleasePath(NULL, asPathName); //Free up resources.
        asPathName = NULL;

//==========================================================================================================================================
//Step 2) Insert document information into Document
//        Title and Author are demonstrated here.
//
//Note:   Standard Key values contained in Document Information Dictionary: 
//        {"Title, Author, Subject, Keywords, Creator, Producer, Trapped"}  
//
//Note:   "key" refers to the document information field will be inserted into
//        "value" is the data being inserted
//==========================================================================================================================================

        //Create unicode strings for inserting the document's Title into the document.
        ASText key = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"Title"), unicodeFormat);
        ASText value = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"Sample Title"), unicodeFormat);
   
        PDDocSetInfoAsASText(pdDoc, key, value); //Insert the document information.
 
        ASTextDestroy(key); //Free up the resources.
        ASTextDestroy(value);

        std::wcout << L"The document's title was inserted." << std::endl;

        //Create unicode strings for inserting the document's Author into the document.
        key = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"Author"), unicodeFormat);
        value = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"Sample Author"), unicodeFormat);

        PDDocSetInfoAsASText(pdDoc, key, value); //Insert the document information.

        ASTextDestroy(key);     //Free up resources.
        ASTextDestroy(value);

        std::wcout << L"The document's author was inserted." << std::endl;

//==========================================================================================================================================
//Step 3) Save the Document and release Resources
//==========================================================================================================================================
     
        asText = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"out.pdf"), unicodeFormat); //Create unicode string for output file.
     
        asPathName = ASFileSysCreatePathFromDIPathText(NULL, asText, NULL);                   //Create the asPathName to open the input file.
  
        PDDocSave(pdDoc, PDSaveFull, asPathName, NULL, NULL, NULL);                           //Save the new document.

        std::wcout << L"The PDF Document has been saved and now contains Document Information." << std::endl;

        ASTextDestroy(asText); //Release remaining resources.
        ASFileSysReleasePath(NULL, asPathName);
        PDDocClose(pdDoc);

        std::wcout << L"All remaining resources have been released" << std::endl;

    HANDLER

            libInit.displayError(errCode);  //Display any errors/exceptions that may have occured.

    END_HANDLER

    return errCode;   
}
