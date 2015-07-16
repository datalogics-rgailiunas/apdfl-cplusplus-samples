// Copyright (c) 2015, Datalogics, Inc. All rights reserved.

//*******************************************************************************
//Helper App. for Samples: APDFLDoc is intended to assist with common PDDoc
//operations. This class contains methods that open and create documents and 
//performs other common operations.
//APDFLDoc.cpp: Contains implementations of methods.
//APDFLDoc.h: Contains class definition.
//*******************************************************************************

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

#include "APDFLDoc.h"

//************************************************************************
// Constructor opens an APDFL document when supplied a file name and
// bool to determine whether to repair (true) or not (false) a damaged file.
//************************************************************************

APDFLDoc::APDFLDoc(wchar_t * nameOfDocument, bool repairDamagedFile)
{

    initialize();

    //Copy name of document into data member
    wcscpy(this->nameOfDocument, nameOfDocument);

    DURING
    
        //Set the path name data member
        setASPathName(this->nameOfDocument);

        //Open the document
        pdDoc = PDDocOpen(asPathName, NULL, NULL, repairDamagedFile);

        //Release path name that was just created.
        ASFileSysReleasePath(NULL, asPathName);
        asPathName = NULL;

        HANDLER

        printErrorHandlerMessage();

        //Pass exception to the next handler on the stack.
        RERAISE();

    END_HANDLER
}

//************************************************************************
//This constructor creates a new PDF Document. 
//************************************************************************

APDFLDoc::APDFLDoc()
{

    initialize();

    DURING

        //Create the pdDoc
        pdDoc = PDDocCreate();       

    HANDLER

        printErrorHandlerMessage();

        //Pass exception to the next handler on the stack.
        RERAISE();

    END_HANDLER
}

//************************************************************************
//saveDoc() Saves a PDDoc. 
//If pathToSaveDoc is supplied it will save to the location specified. If 
//it is not supplied it will overwrite the documents original location.
//The document will do a FullSave by default, but other flags may be specified.
//
//  EX: saveDoc(L"out.pdf", PDSaveFull | PDSaveLinearized);
//************************************************************************

ASErrorCode APDFLDoc::saveDoc(wchar_t * pathToSaveDoc, PDSaveFlags saveFlags)
{

    DURING

        //Ensure a name has been set for the document before saving.
        if (pathToSaveDoc == NULL && nameOfDocument[0] == L'\0')
        {
            std::wcerr << L"Failed to save document ensure PDDoc has a valid name before saving. " << std::endl;
            errorCode = -1;
            return errorCode;
        }

        //Set the path name of the document.
        if (pathToSaveDoc != NULL)
            setASPathName(pathToSaveDoc);   //Use the path specified in saveDoc if it's been set.
        else
            setASPathName(nameOfDocument);  //Overwrite the original document if it hasn't been set.
        
        //Check to see if document has a page before saving
        if (PDDocGetNumPages(pdDoc) > 0)
            PDDocSave(pdDoc, saveFlags, asPathName, NULL, NULL, NULL);
        else
        {
            std::wcerr << L"Failed to save document ensure PDDoc has pages. " << std::endl;
            errorCode = -1;
        }

        //Release path name
        ASFileSysReleasePath(NULL, asPathName);
        asPathName = NULL;

    HANDLER

        return printErrorHandlerMessage();  //Return error code that was generated by exception
    
    END_HANDLER
 
        return errorCode;   //errorcode = 0 if no exceptions raised
}

//************************************************************************
//Sets the ASPathName data member, used when opening or saving a file.
//************************************************************************

ASErrorCode APDFLDoc::setASPathName(wchar_t * pathToCreate)
{
    //Check to make sure there is room for path before copy
    if (wcslen(pathToCreate) <= MAX_PATH_LENGTH)
        wcscpy(this->nameOfDocument, pathToCreate);
    else
    {
        std::wcerr << L"Failed to create path, please check length of path name." << std::endl;
        return -1;
    }

    //Text object to create ASPathName
    ASText textToCreatePath = NULL;

    DURING

        //Determine size of wchar_t on system and get the ASText
        if (sizeof(wchar_t) == 2)
            textToCreatePath = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*> (nameOfDocument), kUTF16HostEndian);
        else
            textToCreatePath = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(nameOfDocument), kUTF32HostEndian);

        //Create the path for output file
        asPathName = ASFileSysCreatePathFromDIPathText(NULL, textToCreatePath, NULL);

    HANDLER
        
       //Return error code that was generated by exception 
       return printErrorHandlerMessage();
    
    END_HANDLER

    //Release text object
    ASTextDestroy(textToCreatePath);
    
    return errorCode;
}

//************************************************************************
// Inserts a page into the PDDoc given a width, height and page number.
//************************************************************************

ASErrorCode APDFLDoc::insertPage(const ASInt16 & width, const ASInt16 & height, ASInt32 afterPageNum)
{
    return insertPage(Int16ToFixed(width), Int16ToFixed(height), afterPageNum);
}


ASErrorCode APDFLDoc::insertPage(const ASFixed & width, const ASFixed & height, ASInt32 afterPageNum)
{

    DURING

        PDPage pdPage;

        //Create page dimensions
        ASFixedRect mediaBox;
        mediaBox.left = fixedZero;
        mediaBox.right = width;
        mediaBox.bottom = fixedZero;
        mediaBox.top = height;

        //Create and insert a page into the PDDoc
        pdPage = PDDocCreatePage(pdDoc, afterPageNum, mediaBox);

        PDPageRelease(pdPage);
        pdPage = NULL;

    HANDLER

        //Return error code that was generated by exception
        return printErrorHandlerMessage();

    END_HANDLER

        return errorCode;
}

//************************************************************************
// Returns specified page, first page is 0
// Important note: Caller is responsible for calling PDPageRelease on page.
//************************************************************************

PDPage APDFLDoc::getPageNumber(ASInt32 pageNumber)
{

    PDPage pdPage = NULL;

    DURING
  
        pdPage = PDDocAcquirePage(pdDoc, pageNumber); //Get the page number

    HANDLER

        printErrorHandlerMessage();                   //if exception occurs print the error.

    END_HANDLER

        return pdPage;
}

//************************************************************************
// Initializes data members to NULL values, called in constructors.
//************************************************************************

void APDFLDoc::initialize()
{
    //Initially should be NULL
    pdDoc = NULL;
    asPathName = NULL;
    errorCode = 0;
    nameOfDocument[0] = L'\0';
}

//************************************************************************
// printErrorHandlerMessage prints out errors in the handler blocks and
// returns an error code based on the exception raised.
//************************************************************************

ASErrorCode APDFLDoc::printErrorHandlerMessage()
{
    //If there was an exception generate an error code
    errorCode = ERRORCODE;

    char buf[256];

    ASGetErrorString(ERRORCODE, buf, sizeof(buf));

    //Print out error code
    std::cerr << "Error Code: " << errorCode << "Error Message: " << buf << std::endl;

    return errorCode;
}

//************************************************************************
// Destructor for APDFLDoc class, releases any remaining resources
//************************************************************************

APDFLDoc::~APDFLDoc()
{
    DURING

        if (pdDoc != NULL)                          //Close the PDDoc
            PDDocClose(pdDoc);

        if (asPathName != NULL)                     //Close the pathname
            ASFileSysReleasePath(NULL, asPathName);

    HANDLER

        printErrorHandlerMessage();

        //Pass exception to the next handler on the stack.
        RERAISE();

    END_HANDLER
}
