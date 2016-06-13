// Copyright (c) 2016, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//=====================================================================================================================
// Sample: ConvertPDFtoPostscript - Convert the contents of the input PDF 
//              file into a new PS file. 
//
//
// Steps:
// 1) Open the input PDF
// 2) Open the output stream
// 3) Initialize PS parameters
// 4) Write to PS file, close it, and clean up
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
    wchar_t* outPath = L"Ulysses.ps";

    APDFLib lib;                                      // Initialize the Adobe PDF Library
    ASErrorCode errCode = 0;                          // This will catch error codes thrown during library usage

    if (lib.isValid() == false)                       // If it failed to initialize, return the error code
        return lib.getInitError();

    DURING

//=====================================================================================================================
// Step 1) Open the input PDF
//=====================================================================================================================

        std::wcout << L"Opening the input document - " << inPath << std::endl;

        APDFLDoc inAPDoc(inPath, true);               // Open the input document, repairing it if necessary.
        PDDoc inDoc = inAPDoc.getPDDoc();

//=====================================================================================================================
// Step 2) Initialize print parameters
//=====================================================================================================================

        std::wcout << L"Initializing print parameters." << std::endl;

        // Set defaults (see SetupPrintParams files in Common folder)
        PDPrintParamsRec psParams;
        SetupPDPrintParams(&psParams);

        PDFLPrintUserParamsRec userParams;
        SetupPDFLPrintUserParams(&userParams);

        // Override defaults with specifics for this sample
        userParams.emitToFile = true;                 // Print to file

        psParams.emitPS = true;                       // Create PS file

//=====================================================================================================================
// Step 3) Open the output stream and set print param for stream
//=====================================================================================================================

        std::wcout << L"Opening the output stream." << std::endl;

        ASFile outFile = NULL;
        ASStm printStm = NULL;

        ASText outPathText = NULL;                    // ASText object is used to create the ASPathName object

        // Determine the size of wchar_t on the system
        // and set the ASTextObject to hold the path for the output document
        if (sizeof(wchar_t) == 2)
            outPathText = ASTextFromUnicode((ASUTF16Val*)outPath, kUTF16HostEndian);
        else
            outPathText = ASTextFromUnicode((ASUTF16Val*)outPath, kUTF32HostEndian);

        // Create the ASPathName object from the ASText. This will be used to save the PDDoc
        ASPathName outPathName = ASFileSysCreatePathFromDIPathText(NULL, outPathText, NULL);

        // Create  writeable ProcStm to handle the print stream
        ASFileSysOpenFile(ASGetDefaultFileSys(), outPathName, ASFILE_WRITE | ASFILE_CREATE, &outFile);
        printStm = ASFileStmWrOpen(outFile, 0);

        userParams.printStm = printStm;                    // Send output to the writeable stream 
        userParams.printParams = &psParams;                // Connect the two structures

//=====================================================================================================================
// Step 4) Write to PS file, close it, and clean up
//=====================================================================================================================

        std::wcout << L"Writing to the output stream." << std::endl;

        DURING
            PDFLPrintDoc(inDoc, &userParams);
        HANDLER
            errCode = ERRORCODE;
            lib.displayError(errCode);                // If there was an error, display it
        END_HANDLER

        std::wcout << L"Closing documents and cleaning up." << std::endl;

        // Cleanup and free memory
        DisposePDPrintParams(&psParams);
        DisposePDFLPrintUserParams(&userParams);

        ASStmClose(printStm);                         // Close the file stream
        PDDocClose(inDoc);                            // Close the input document

        ASFileFlush(outFile);                         // Safely close outFile
        ASFileClose(outFile);

        ASTextDestroy(outPathText);                   // Release all objects that are still in use
        ASFileSysReleasePath(NULL, outPathName);

    HANDLER
        errCode = ERRORCODE;
        lib.displayError(errCode);                    // If there was an error, display it
    END_HANDLER

    return errCode;                                   // APDFLib's destructor terminates the APDFL
};
