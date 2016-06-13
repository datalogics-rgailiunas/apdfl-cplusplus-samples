// Copyright (c) 2016, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//=====================================================================================================================
// Sample: ConvertPDFtoEPS - Convert the contents of each page of the input PDF 
//              file into a new EPS file. 
//
//
// Steps:
// 1) Open the input PDF
// 2) Initialize print parameters
// 3) Open the output stream for each page, set print param for stream, and write to file
//=====================================================================================================================

/* Printing Support */
#include "InitializeLibrary.h"
#include "APDFLDoc.h"
#include "PDFLPrint.h"
#include "SetupPrintParams.h"

int main(int argc, char **argv)
{
    //Paths to input and output documents.
    wchar_t* inPath = L"../_Input/Ulysses.pdf";
    wchar_t* outPath = L"Ulysses_page_";

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

//=====================================================================================================================
// Step 2) Initialize print parameters
//=====================================================================================================================

    std::wcout << L"Initializing print parameters." << std::endl;

    // Set defaults
    PDPrintParamsRec psParams;
    SetupPDPrintParams(&psParams);

    PDFLPrintUserParamsRec userParams;
    SetupPDFLPrintUserParams(&userParams);

    // Override defaults with specifics for this sample
    userParams.emitToFile = true;                     // Print to file

    psParams.emitPS = true;                           // Create file
    psParams.outputType = PDOutput_EPSNoPrev;         // Create an EPS file with no preview

//=====================================================================================================================
// Step 3) Open the output stream for each page, set print param for stream, and write to file
//=====================================================================================================================

    std::wcout << L"Opening the output stream." << std::endl;

    ASFile outFile = NULL;
    ASStm printStm = NULL;

    wchar_t pageNameString[100];                      // Name of the next page to save

    ASText outPathText = NULL;                        // ASText object is used to create the ASPathName object

    //Advance the page index through all the document pages
    for (int pageIndex = 0; pageIndex < PDDocGetNumPages(inDoc); pageIndex++)
    {
        // Create the file name based on the outPath specified and the page index
        swprintf(pageNameString, 100, L"%ls%ld.eps", outPath, pageIndex + 1);

        // Determine the size of wchar_t on the system
        // and set the ASTextObject to hold the path for the output document
        if (sizeof(wchar_t) == 2)
            outPathText = ASTextFromUnicode((ASUTF16Val*)(pageNameString), kUTF16HostEndian);
        else
            outPathText = ASTextFromUnicode((ASUTF16Val*)(pageNameString), kUTF32HostEndian);

        // Create the ASPathName object from the ASText. This will be used to save the PDDoc
        ASPathName outPathName = ASFileSysCreatePathFromDIPathText(NULL, outPathText, NULL);

        // Create writeable ProcStm to handle the print stream
        ASFileSysOpenFile(ASGetDefaultFileSys(), outPathName, ASFILE_WRITE | ASFILE_CREATE, &outFile);
        printStm = ASFileStmWrOpen(outFile, 0);

        userParams.printStm = printStm;                    // Send output to a writeable stream

        psParams.ranges[0].startPage = pageIndex;          // Specify starting page via the current page index
        psParams.ranges[0].endPage = pageIndex;            // Specify ending page (also current page index)

        userParams.printParams = &psParams;                // Connect the two structures

        // Create EPS document from the current page
        std::wcout << L"Writing to " << pageNameString << std::endl;
        PDFLPrintDoc(inDoc, &userParams);

        std::wcout << L"Closing output document and cleaning up." << std::endl;

        ASStmClose(printStm);                         // Close the file stream

        ASFileFlush(outFile);                         // Safely close outFile
        ASFileClose(outFile);

        ASTextDestroy(outPathText);                   // Release all objects that are still in use
        ASFileSysReleasePath(NULL, outPathName);
    }

    // Cleanup and free memory
    DisposePDPrintParams(&psParams);
    DisposePDFLPrintUserParams(&userParams);

    std::wcout << L"Closing input doc." << std::endl;
    PDDocClose(inDoc);                                // Close the input document

    HANDLER
        errCode = ERRORCODE;
        lib.displayError(errCode);                    // If there was an error, display it
    END_HANDLER

    return errCode;                                   // APDFLib's destructor terminates the APDFL
};
