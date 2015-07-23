// Copyright (c) 2015, Datalogics, Inc. All rights reserved.

//===============================================================================
//Sample: APDFLDoc -This class is intended to assist with operations common to 
//most samples. The class is capable of opening/creating and saving a document.
//It can also insert and retrieve pages.
//
//APDFLDoc.cpp: Contains the method implementations.
//APDFLDoc.h: Contains the class definition.
//===============================================================================

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

//========================================================================================================================
// Default Constructor - This creates a new PDDoc object. This object will be automatically freed in the APDFLDoc's destructor.
//========================================================================================================================

APDFLDoc::APDFLDoc()
{

    initialize();                 //Helper method sets some of the data members to NULL values.

    DURING
  
        pdDoc = PDDocCreate();    //Initialize the PDDoc data member.

    HANDLER

        printErrorHandlerMessage();

        RERAISE();                //Pass exception to the next HANDLER on the stack.

    END_HANDLER
}

//========================================================================================================================
// Constructor - This constructor opens an existing PDF document. nameOfDocument is the relative path for the PDF document 
// and the bool repairDamagedFile determines whether to repair (true) or not (false) a damaged file.
//========================================================================================================================

APDFLDoc::APDFLDoc(wchar_t * nameOfDocument, bool repairDamagedFile)
{

    initialize();                                                        

    wcscpy(this->nameOfDocument, nameOfDocument);                        //Set the nameOfDocument data member.

    DURING
        
        setASPathName(this->nameOfDocument);                             //Set the ASPathName data member.

        pdDoc = PDDocOpen(asPathName, NULL, NULL, repairDamagedFile);    //Open the PDF document.

        ASFileSysReleasePath(NULL, asPathName);                          //Release the ASPathName that was just created.
        asPathName = NULL;

   HANDLER

       printErrorHandlerMessage();                                       //Report any exceptions that occured.
   
       RERAISE();                                                        //Pass the exception to the next handler on the stack.

   END_HANDLER
}

//========================================================================================================================
// initialize() - Helper method used to initialize data members to NULL values.
//========================================================================================================================

void APDFLDoc::initialize()
{
    pdDoc = NULL;
    asPathName = NULL;
    errorCode = 0;
    nameOfDocument[0] = L'\0';
}

//========================================================================================================================
// saveDoc() - This method saves the PDDoc. If pathToSaveDoc is supplied it will save to the location specified. If it is 
// not supplied it will overwrite the documents original location. The document will do a complete save by default, but other 
// flags may be specified.
//========================================================================================================================

ASErrorCode APDFLDoc::saveDoc(wchar_t * pathToSaveDoc, PDSaveFlags saveFlags)
{

    DURING

        //Error checking: Ensure a name has been set before saving the document.
        if (pathToSaveDoc == NULL && nameOfDocument[0] == L'\0')
        {
            std::wcerr << L"Failed to save document ensure PDDoc has a valid name before saving. " << std::endl;
            errorCode = -1;
            return errorCode;
        }

        //Error checking: Ensure that the ASPathName has been set before saving the document.
        if (pathToSaveDoc != NULL)
            setASPathName(pathToSaveDoc);           //Use the path specified in saveDoc if it's been set.
        else
            setASPathName(nameOfDocument);          //Overwrite the original document if it hasn't been set.
        
        //Error Checking: Ensure document has a page before saving.
        if (PDDocGetNumPages(pdDoc) > 0)
            PDDocSave(pdDoc, saveFlags, asPathName, NULL, NULL, NULL);
        else
        {
            std::wcerr << L"Failed to save document ensure PDDoc has pages. " << std::endl;
            errorCode = -2;
        }

        ASFileSysReleasePath(NULL, asPathName);    //Release ASPathName object and set to NULL.
        asPathName = NULL;

    HANDLER

        return printErrorHandlerMessage();         //Return the error code that was generated by the exception.
    
    END_HANDLER
 
        return errorCode;                          
}

//========================================================================================================================
// setASPAthName() - Helper method used to create an ASPathName. This is called by the saveDoc and open document constructor.
//========================================================================================================================

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

    ASText textToCreatePath = NULL;         //Text object to create ASPathName

    DURING

        //Determine size of wchar_t on system and get the ASText
        if (sizeof(wchar_t) == 2)
            textToCreatePath = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*> (nameOfDocument), kUTF16HostEndian);
        else
            textToCreatePath = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(nameOfDocument), kUTF32HostEndian);

        //Create the path for output file
        asPathName = ASFileSysCreatePathFromDIPathText(NULL, textToCreatePath, NULL);    

    HANDLER
         
       return printErrorHandlerMessage();   //Return error code that was generated by exception 
    
    END_HANDLER

    ASTextDestroy(textToCreatePath);        //Release text object
    
    return errorCode;
}

//========================================================================================================================
// insertPage() - Inserts a page into the PDDoc when provided ASFixed values for width, height and the location where the
// page will be inserted. The PDPage created is deallocated at the end of this method.
//========================================================================================================================

ASErrorCode APDFLDoc::insertPage(const ASFixed & width, const ASFixed & height, ASInt32 afterPageNum)
{

    DURING

        PDPage pdPage;

        //Set the page dimensions before creating the PDPage.
        ASFixedRect mediaBox;
        mediaBox.left = fixedZero;
        mediaBox.right = width;
        mediaBox.bottom = fixedZero;
        mediaBox.top = height;

        pdPage = PDDocCreatePage(pdDoc, afterPageNum, mediaBox);    //Create and insert a page into the PDDoc.

        PDPageRelease(pdPage);                                      //Release the PDPage object.
        pdPage = NULL;

    HANDLER

        return printErrorHandlerMessage();                          //Return the error code that was generated by the exception.

    END_HANDLER

        return errorCode;
}

//========================================================================================================================
// insertPage() - This is the overloaded method that takes integer arguments instead of ASFixed values.
//========================================================================================================================

ASErrorCode APDFLDoc::insertPage(const ASInt16 & width, const ASInt16 & height, ASInt32 afterPageNum)
{
    return insertPage(Int16ToFixed(width), Int16ToFixed(height), afterPageNum);
}

//========================================================================================================================
// getPage() - Accessor method for pages in the PDDoc. The argument is the page index with 0 being the first page.
//========================================================================================================================

PDPage APDFLDoc::getPage(ASInt32 pageNumber)
{

    PDPage pdPage = NULL;

    DURING
  
        pdPage = PDDocAcquirePage(pdDoc, pageNumber);    //Get the page from the PDDoc object.

    HANDLER

        printErrorHandlerMessage();                      //If an exception occured print the error.

    END_HANDLER

        return pdPage;
}

//========================================================================================================================
// printErrorHandlerMessage() - Helper method that reports errors and returns an error code.
//========================================================================================================================

ASErrorCode APDFLDoc::printErrorHandlerMessage()
{
   
    errorCode = ERRORCODE;                            //Get the error code that caused the exception.

    char buf[256];             

    ASGetErrorString(ERRORCODE, buf, sizeof(buf));    //Get the error message that coreesponds to the error code.

    std::cerr << "Error Code: " << errorCode << "Error Message: " << buf << std::endl;

    return errorCode;
}

//========================================================================================================================
// ~APDFLDoc() - Releases resources if they haven't already been freed.
//========================================================================================================================

APDFLDoc::~APDFLDoc()
{
    DURING

        if (pdDoc != NULL)                          //Close the PDDoc
            PDDocClose(pdDoc);

        if (asPathName != NULL)                     //Close the pathname
            ASFileSysReleasePath(NULL, asPathName);

    HANDLER

        printErrorHandlerMessage();

        RERAISE();                                  //Pass exception to the next handler on the stack.

    END_HANDLER
}
