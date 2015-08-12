// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//=================================================================================
// Sample: SplitPDF - Opens the input file "PDFToBeSplit.pdf", and copies each page
//                into a new PDF document which is saved to the working directory.
//
// Note: The length of the input document is irrelevant. A document of any length
// can be split with this algorithm.
//
// Steps: 
//  1) Open PDFToBeSplit.pdf and prepare a vector of PDDocs, one for each page.
//  2) Iterate through the vector, inserting the appropriate page and then
//     saving the document.
//=================================================================================

#include <vector>
#include <string>
#include <iostream>
#include "APDFLDoc.h"
#include "InitializeLibrary.h"

int main(int argc, char** argv)
{
    APDFLib lib;                      //Initialize the Adobe PDF Library.

    if (lib.isValid() == false)       //If it failed to initialize, return the error code.
        return lib.getInitError();

    ASErrorCode errCode = 0;          //Will catch error codes thrown during library usage.

//===========================================================================================================================
// Step 1) Open PDFToBeSplit.pdf and prepare a vector of PDDocs, one for each page.
//===========================================================================================================================

    DURING

        APDFLDoc document(L"../_Input/PDFToBeSplit.pdf", true);                 //Open the input document. Repair if damaged.

        std::vector<PDDoc> splitDocs(PDDocGetNumPages(document.getPDDoc()));    //A vector for the new documents.

        std::vector<PDDoc>::iterator iter = splitDocs.begin();                  //An iterator to access splitDocs' members.

        int pageIndex;                                                          //Tracks the index of the iterator.
        std::wstring pageNameString;                                            //Name of the next page to save.

        //Determine the host's unicode format. For creating output pathnames.
        ASInt32 uniFormat;
        if (sizeof(wchar_t) == 2)
            uniFormat = kUTF16HostEndian;
        else
            uniFormat = kUTF32HostEndian;

        ASText outPathText;                                                     //Text object to create output pathname.

        ASPathName outPathName;                                                 //Pathname used to save the document.

//===========================================================================================================================
// Step 2) Iterate through the vector, inserting the appropriate page and then saving the document.
//===========================================================================================================================

        //Advance the iterator through the splitDocs vector.
        for (iter = splitDocs.begin(); iter < splitDocs.end(); std::cout<<std::endl, iter++)
        {
            //Create a new PDDoc for the next page.
            *iter = PDDocCreate();

            //Set tracker to the next page's index.
            pageIndex = iter - splitDocs.begin();

            //Insert one page from the input doc, starting with the page indexed by tracker, into the next PDDoc.
            PDDocInsertPages(*iter, PDBeforeFirstPage, document.getPDDoc(), pageIndex, 1, NULL, NULL, NULL, NULL, NULL);

            //Set the output file name according to the current page number.
            pageNameString = L"baseDocument_Page" + std::to_wstring(pageIndex + 1) + L".pdf";

            //Create the ASText object used to create the ASPathName object.
            outPathText = ASTextFromUnicode((ASUTF16Val*)(wchar_t*)(pageNameString.c_str()), uniFormat);

            //The ASPathName will be used to save the document.
            outPathName = ASFileSysCreatePathFromDIPathText(NULL, outPathText, NULL); 

            //Save file using the ASPathName.
            PDDocSave(*iter, PDSaveFull | PDSaveLinearized, outPathName, ASGetDefaultFileSys(), NULL, NULL); 

            std::wcout << pageNameString << " was created and saved";

            //Release the document after saving.
            PDDocRelease(*iter);

            //Release the path so that it can be reused within the loop.
            ASFileSysReleasePath(NULL, outPathName);
            
            //Free up the ASText object that was created.
            ASTextDestroy(outPathText);
        }

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);    //If there was an error, display it.

    END_HANDLER

    return errCode;                   //lib's destructor terminates the library.
}
