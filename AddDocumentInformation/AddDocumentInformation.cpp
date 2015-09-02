// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//=============================================================================
// Sample AddDocumentInformation: Opens a document, inserts document 
// information into it and saves it.
//
//Steps:
// 1) Open the Document that the document information will be inserted into.
// 2) Insert the document information.
// 3) Save the document and release resources.
//=============================================================================

#include <iostream>
#include "InitializeLibrary.h"
#include "ASExtraCalls.h"
#include "PDCalls.h"

int main(int argc, char** argv)
{
    APDFLib lib(argv[1]);                               //Initialize the Adobe PDF Library.

    if (lib.isValid() == false)                //Check to see if the Adobe PDF Library initialized.
        return lib.getInitError();             //If it failed, return the error code.

    ASErrorCode errCode = 0;                   //Will catch error codes thrown during library usage.
		
    DURING

//==================================================================================================================================================
//Step 1) Open the Document that the document information will be inserted into.
//==================================================================================================================================================

        wchar_t* inputFilePath = L"../_Input/AddDocumentInformation.pdf";    //File path of the PDF Document.

        //Determine host computer's unicode format.
        ASUnicodeFormat hostUnicodeFormat;
        if (sizeof(wchar_t) == 2)
            hostUnicodeFormat = kUTF16HostEndian;
        else
            hostUnicodeFormat = kUTF32HostEndian;

        //Create the ASPathName of the input file path.
        ASText asText = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(inputFilePath), hostUnicodeFormat);
        ASPathName asPathName = ASFileSysCreatePathFromDIPathText(NULL, asText, NULL);

        ASTextDestroy(asText);                                               //Free up resources.

        PDDoc pdDoc = PDDocOpen(asPathName, NULL, NULL, true);               //Open the input Document.

        std::wcout << L"Document was sucessfully opened." << std::endl;

        ASFileSysReleasePath(NULL, asPathName);                              //Free up resources.

//==================================================================================================================================================
//Step 2) Insert the document information.
//
//Inserting values for Title and Author are demonstrated here.
//
// Note: This program inserts standard document information contained in the
// "Document Information Dictionary." See “Document Information Dictionary” (section 14.3.3)in "ISO 32000-1:2008, Document   
// Management-Portable Document Format-Part 1: PDF 1.7, page 549" at:
// http://www.adobe.com/content/dam/Adobe/en/devnet/acrobat/pdfs/PDF32000_2008.pdf#page=557.

//Note:   "key" refers to the document information field will be inserted into
//        "value" is the data being inserted
//==================================================================================================================================================

        //Create unicode strings for inserting the document's Title into the document.
        ASText key = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"Title"), hostUnicodeFormat);
        ASText value = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"Sample Title"), hostUnicodeFormat);

        PDDocSetInfoAsASText(pdDoc, key, value);    //Insert the document information.

        std::wcout << L"The document's title was inserted." << std::endl;

        //Create unicode strings for inserting the document's Author into the document.
        key = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"Author"), hostUnicodeFormat);
        value = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"Sample Author"), hostUnicodeFormat);

        PDDocSetInfoAsASText(pdDoc, key, value);    //Insert the document information.

        //Free up resources.
        ASTextDestroy(key);
        ASTextDestroy(value);

        std::wcout << L"The document's author was inserted." << std::endl;

//==================================================================================================================================================
//Step 3) Save the Document and release Resources.
//==================================================================================================================================================

        asText = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"out.pdf"), hostUnicodeFormat);    //Create unicode path for output file.

        asPathName = ASFileSysCreatePathFromDIPathText(NULL, asText, NULL);                          //Create the ASPathName of the output file.
  
        PDDocSave(pdDoc, PDSaveFull, asPathName, NULL, NULL, NULL);                                  //Save the new document.

        std::wcout << L"The new PDF Document has been saved and now contains the new Document Information." << std::endl;

        //Release remaining resources.
        ASTextDestroy(asText); 
        ASFileSysReleasePath(NULL, asPathName);
        PDDocClose(pdDoc);

        std::wcout << L"All remaining resources have been released." << std::endl;

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);                                                                   //If there was an error, display it.

    END_HANDLER

    return errCode;                                                                                  //lib's destructor terminates the library.
}
