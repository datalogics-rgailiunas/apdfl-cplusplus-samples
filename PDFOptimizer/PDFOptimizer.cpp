// Copyright (c) 2016, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//

#include "InitializeLibrary.h"
#include "APDFLDoc.h"
#include "DLExtrasCalls.h"

int main(int argc, char **argv)
{
    //Paths to input and output documents.
    wchar_t* inPath = L"../_Input/Ulysses.pdf";
    wchar_t* outPath = L"out.pdf";

    APDFLib lib;                                      // Initialize the Adobe PDF Library
    ASErrorCode errCode = 0;                          // This will catch error codes thrown during library usage

    if (lib.isValid() == false)                       // If it failed to initialize, return the error code
        return lib.getInitError();

    DURING

//=====================================================================================================================
// Step 1) Open the input PDF
//=====================================================================================================================

    std::wcout << L"Opening the input document - " << inPath << std::endl;

    APDFLDoc inAPDoc(inPath, true);                   // Open the input document, repairing it if necessary.
    PDDoc inDoc = inAPDoc.getPDDoc();


    // Set defaults
    PDFOptimizationParams optParams = PDDocOptimizeDefaultParams();

//=====================================================================================================================
//Step 2: Optimize and save the document and release resources. 
//        ASPathName must be created in order to call the PDDocumentOptimize method.
//=====================================================================================================================

    std::wcout << L"Creating path to output file..." << std::endl;

    ASText textToCreatePath = NULL;    //ASText object is used to create the ASPathName object.

    //Determine the size of wchar_t on the system, and set the ASTextObject to hold the path for the output document.
    if (sizeof(wchar_t) == 2)
        textToCreatePath = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*> (outPath), kUTF16HostEndian);
    else
        textToCreatePath = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(outPath), kUTF32HostEndian);

    //Create the ASPathName object from the ASText. This will be used to save the PDDoc.
    ASPathName outPathName = ASFileSysCreatePathFromDIPathText(NULL, textToCreatePath, NULL);

    std::wcout << L"Saving document and freeing resources..." << std::endl;

    PDDocumentOptimize(inDoc, outPathName, NULL, optParams);

//=====================================================================================================================
// Step 3) Release all objects that are still in use.
//=====================================================================================================================

    ASTextDestroy(textToCreatePath);
    ASFileSysReleasePath(NULL, outPathName);    

    std::wcout << L"Closing input doc." << std::endl;
    PDDocClose(inDoc);                                // Close the input document

    HANDLER
        errCode = ERRORCODE;
        lib.displayError(errCode);                    // If there was an error, display it
    END_HANDLER

    return errCode;                                   // APDFLib's destructor terminates the APDFL
};
