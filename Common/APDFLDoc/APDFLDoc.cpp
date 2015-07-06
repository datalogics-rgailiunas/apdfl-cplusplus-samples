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

#include "APDFLDoc.h"

//************************************************************************
// Constructor opens an APDFL document when supplied a file name and
// bool to determine whether to repair (true) or not (false) a damaged file.
//************************************************************************

APDFLDoc::APDFLDoc(wchar_t * nameOfDocument, bool repairDamagedFile)
{
    //Initially should be NULL
    pdDoc = NULL;
    asPathName = NULL;
    pdPage = NULL;
    errorCode = 0;

    //Inititialize name of document data member
    setDocumentName(nameOfDocument);

    DURING
        //Set the path name data member
        setASPathName(this->nameOfDocument);

        //Open the document
        pdDoc = PDDocOpen(asPathName, NULL, NULL, repairDamagedFile);

    HANDLER

        //Pass exception to the next handler on the stack.
        RERAISE();

    END_HANDLER
}

//************************************************************************
//This constructor creates a new PDF Document
//width and height are OPTIONAL arguments, if both are supplied then this
//method will create a new page and insert it into the document with the
//dimensions (in pixels) specified.
//************************************************************************

APDFLDoc::APDFLDoc(unsigned width, unsigned height)
{
    //Initially should be NULL
    pdDoc = NULL;
    asPathName = NULL;
    pdPage = NULL;
    errorCode = 0;
    
    //When calling this method must specify name explicitly before saving
    //using setDocumentName
    setDocumentName(L"\0");

    DURING
        //Create the pdDoc
        pdDoc = PDDocCreate();
        
        //If arguments were all supplied create and insert the page
        if (width != 0 && height != 0)
            insertPage(width, height);

    HANDLER

        //set data members to null
        pdDoc = NULL;
        pdPage = NULL;

        //Pass exception to the next handler on the stack.
        RERAISE();

    END_HANDLER
}

//************************************************************************
//saveWebOptimized() Saves a web optimized PDF Document
//This method will create a page if one doesnt exist before saving
//************************************************************************

ASErrorCode APDFLDoc::saveWebOptimized()
{
    DURING

        //Return if name has not been set.
        if ((wcscmp(nameOfDocument, L"\0") == 0) && asPathName == NULL)
        {
            errorCode = -1;
            std::wcerr << L"Failed to save document, please us setDocumentName() before saving";
            E_RETURN(errorCode);
        }

        //If pages dont exist when saving, insert a page
        if (PDDocGetNumPages(pdDoc) <= 0)
            insertPage((72 * 8.5), (72 * 11));

        //Set the path name for the document
        setASPathName(nameOfDocument);

        //Save the document as web optimized
        PDDocSave(pdDoc, PDSaveFull | PDSaveLinearized, asPathName, NULL, NULL, NULL);

        //release pathname
        ASFileSysReleasePath(NULL, asPathName);
        asPathName = NULL;

    HANDLER

        //Return error code that was generated by exception
        return printErrorHandlerMessage();	

    END_HANDLER

        //errorcode = 0 if no exceptions raised
        return errorCode;
}

//************************************************************************
//saveDoc() Saves a non web optimized PDF Document
//This method will create a page if one doesnt exist before saving
//************************************************************************

ASErrorCode APDFLDoc::saveDoc()
{

    DURING

        //Return if name has not been set.
        if (wcscmp(nameOfDocument, L"\0") == 0)
        {
            errorCode = -1;
            std::wcerr << L"Failed to save document, please us setDocumentName() before saving";
            E_RETURN(errorCode);
        }

        //Set the ASPathName
        setASPathName(nameOfDocument);

        //If pages dont exist when saving, insert a page
        if (PDDocGetNumPages(pdDoc) <= 0)
            insertPage((72 * 8.5), (72 * 11.0));

        //Save the document
        PDDocSave(pdDoc, PDSaveFull, asPathName, NULL, NULL, NULL);

        //Release path name
        ASFileSysReleasePath(NULL, asPathName);
        asPathName = NULL;

    HANDLER

        //Return error code that was generated by exception
        return printErrorHandlerMessage();
    
    END_HANDLER

        //errorcode = 0 if no exceptions raised
        return errorCode;
}

//************************************************************************
//Sets the name of the document wchar_t * data member
//************************************************************************

void APDFLDoc::setDocumentName(const wchar_t * newDocumentName)
{
    //Check to make sure there is room for path before copy
    if (wcslen(newDocumentName) <= MAX_PATH_LENGTH)
        wcscpy(this->nameOfDocument, newDocumentName);

}

//************************************************************************
// setASPathName is a method used to create an ASPathName
//pathTocreate is the name of the path that is being created
//************************************************************************

ASErrorCode APDFLDoc::setASPathName(wchar_t * pathToCreate)
{
    //Set the pathname data member
    setDocumentName(pathToCreate);

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
// Inserts a page into the PDDoc
//************************************************************************

ASErrorCode APDFLDoc::insertPage(const int & width, const int & height)
{
    DURING
        //Create page dimensions
        ASFixedRect mediaBox;
        mediaBox.left = fixedZero;
        mediaBox.right = Int16ToFixed(width);
        mediaBox.bottom = fixedZero;
        mediaBox.top = Int16ToFixed(height);

        //Create and insert a page into the PDDoc
        pdPage = PDDocCreatePage(pdDoc, PDBeforeFirstPage, mediaBox);

    HANDLER

        //Return error code that was generated by exception
        return printErrorHandlerMessage();
    
    END_HANDLER

        return errorCode;
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
        
        if (pdDoc != NULL)                            //Close the PDDoc
            PDDocClose(pdDoc);

        if (pdPage != NULL)                           //Close the page
            PDPageRelease(pdPage);

        if (asPathName != NULL)                       //Close the pathname
            ASFileSysReleasePath(NULL, asPathName);

    HANDLER                            

        //Pass exception to the next handler on the stack.
        RERAISE();

    END_HANDLER
}
