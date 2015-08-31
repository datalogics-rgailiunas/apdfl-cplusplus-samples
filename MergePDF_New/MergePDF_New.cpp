// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//======================================================================
// Sample: MergePDF_New: Opens two documents, merges them and saves the
//             resulting document in the working directory.
//======================================================================

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

int main(int argc, char** argv)
{
    APDFLib libInit;                                               //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;                                       //Variable used to report any exceptions/errors if they occured.

    if (libInit.isValid() == false)                                //If there was a problem in initialization, return the error code.
        return libInit.getInitError(); 

    DURING  

        std::wcout << L"Opening input files..." << std::endl;

        APDFLDoc doc1(L"../_Input/merge1.pdf", true);              //Open both of the documents that will be merged together.
        APDFLDoc doc2(L"../_Input/merge2.pdf", true);

        std::wcout << L"Inserting doc2's pages into doc1..." << std::endl;

        //Insert doc2's pages into doc1. If PDBeforeFirstPage is used doc2's pages will be inserted into doc1's.
        PDDocInsertPages(doc1.getPDDoc(), PDLastPage, doc2.getPDDoc(), 0, PDAllPages, PDInsertAll, NULL, NULL, NULL, NULL);

        std::wcout << L"Saving the output file in the working directory..." << std::endl;

        doc1.saveDoc(L"out.pdf", PDSaveFull | PDSaveLinearized);    //Save the output file as out.pdf in the working directory.

    HANDLER                             

        errCode = ERRORCODE;

        libInit.displayError(errCode);                              //If there was an error, display the error that occured.

    END_HANDLER

    return errCode;                                                 //APDFLib's destructor terminates the APDFL.                         
}
