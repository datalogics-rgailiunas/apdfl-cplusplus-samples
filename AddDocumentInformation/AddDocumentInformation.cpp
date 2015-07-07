//# Copyright(c) 2015, Datalogics, Inc.All rights reserved.
//
//************************************************************************
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
//************************************************************************
//
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
#include "MyPDFLibUtils.h"
#include "ASExtraCalls.h"

int main()
{
    //Initialize the APDFL prior to making any APDFL calls
    ASErrorCode errorCode = MyPDFLInit();

    //Check for errors upon initialization of APDFL
    if (errorCode != 0)
    {
        std::cerr << "Initialization error. See \"AcroErr.h\" for more info.\n" << std::endl;
        std::cerr << "Error system: " << ErrGetSystem(errorCode) << std::endl;
        std::cerr << "Error Severity: " << ErrGetSeverity(errorCode) << std::endl;
        std::cerr << "Error Code: " << ErrGetCode(errorCode) << std::endl;

        return errorCode;
    }

    DURING
//*************************************************************************
//Step 1) Open the PDF Document
//*************************************************************************

        //File path used to open the PDF Document
        wchar_t * inputFilePath = L"../Input/AddDocumentInformation.pdf";

        //Argument passed to function ASTextFromUnicode
        ASUnicodeFormat unicodeFormat;

        //Determine host computers unicode format	
        if (sizeof(wchar_t) == 2)
            unicodeFormat = kUTF16HostEndian;
        else
            unicodeFormat = kUTF32HostEndian;

        //ASText is the unicode string used to create ASPathName for opening the PDDoc
        ASText asText = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(inputFilePath), unicodeFormat);

        //Create the asPathName to open the input file
        ASPathName asPathName = ASFileSysCreatePathFromDIPathText(NULL, asText, NULL);

        //Free up resources
        ASTextDestroy(asText);
        asText = NULL;

        //Open the Document
        PDDoc pdDoc = PDDocOpen(asPathName, NULL, NULL, true);

        std::wcout << L"Document was sucessfully opened." << std::endl;

        //Free up resources
        ASFileSysReleasePath(NULL, asPathName);
        asPathName = NULL;

//*************************************************************************
//Step 2) Insert document information into Document
//        Title and Author are demonstrated here.
//
//Note:   Standard Key values contained in Document Information Dictionary: 
//        {"Title, Author, Subject, Keywords, Creator, Producer, Trapped"}  
//
//Note:   "key" refers to the document information field will be inserted into
//        "value" is the data being inserted
//*************************************************************************

        //Create unicode strings for inserting the document's Title into the document.
        ASText key = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"Title"), unicodeFormat);
        ASText value = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"Sample Title"), unicodeFormat);

        //Insert the document information
        PDDocSetInfoAsASText(pdDoc, key, value);

        //Free up the resources
        ASTextDestroy(key);
        ASTextDestroy(value);

        std::wcout << L"The documents title was inserted." << std::endl;

        //Create unicode strings for inserting the document's Author into the document.
        key = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"Author"), unicodeFormat);
        value = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"Sample Author"), unicodeFormat);

        //Insert the document information
        PDDocSetInfoAsASText(pdDoc, key, value);

        //Free up the resources
        ASTextDestroy(key);
        ASTextDestroy(value);

        std::wcout << L"The documents author was inserted." << std::endl;

//*************************************************************************
//Step 3) Save the Document and release Resources
//*************************************************************************

        //Create unicode string for output file
        asText = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"out.pdf"), unicodeFormat);

        //Create the asPathName to open the input file
        asPathName = ASFileSysCreatePathFromDIPathText(NULL, asText, NULL);

        //Save the new document
        PDDocSave(pdDoc, PDSaveFull, asPathName, NULL, NULL, NULL);

        std::wcout << L"The PDF Document has been saved and now contains Document Information." << std::endl;

        //Release remaining resources
        ASTextDestroy(asText);
        ASFileSysReleasePath(NULL, asPathName);
        PDDocClose(pdDoc);

        std::wcout << L"All remaining resources have been released" << std::endl;

    HANDLER
        //If there was an exception generate an error code 
        errorCode = ERRORCODE;

        char buf[256];

        ASGetErrorString(ERRORCODE, buf, sizeof(buf));

        //Print out error code
        std::cerr << "Error Code: " << errorCode << "Error Message: " << buf << std::endl;
        END_HANDLER

    MyPDFLTerm();       //Terminate the library
    
    return errorCode;   
}
