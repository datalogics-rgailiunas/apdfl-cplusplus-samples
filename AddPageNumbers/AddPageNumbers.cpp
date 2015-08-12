// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//============================================================================
// Sample: AddPageLabels-Numbers. This program creates page labels and adds 
//         them to a PDF with numberings. They are viewable by looking at a 
//         documents thumbnails.
//
// Steps:
//  1) Open the Document that will have labels added to.
//  2) Create labels for different sets of pages
//  3) Save and exit
//============================================================================

#include "InitializeLibrary.h"
#include "APDFLDoc.h"
#include "ASExtraCalls.h"
#include <iostream>

int main(int argc, char** argv)
{

    APDFLib libInit;                        //Initialize the APDFL.
    ASErrorCode errCode = 0;                //Error code initially is 0.

    if (libInit.isValid() == false)         //Check for errors in initialization.
        return libInit.getInitError();      //If there was an error return the code.

    DURING

//==================================================================================================================================
// Step 1) Open the Document that will have labels added to.
//==================================================================================================================================

        APDFLDoc document(L"../_Input/toNumber.pdf", true);            //Open a document and repair if damaged

        std::wcout << L"Document was sucessfully opened." << std::endl;

//==================================================================================================================================
// Step 2) Create label's for different sets of pages
// Note: PDPageLabel takes in a style key : "R" for upper - case Roman numbers, "r" for lower - case Roman numbers,
//                                          "A" for upper-case alphabetic numbers, or "a" for lower-case alphabetic numbers
//==================================================================================================================================

        //Set the first page's label to "Cover" with the number counter to 1
        PDPageLabel coverLabel = PDPageLabelNew(document.pdDoc, ASAtomFromString("D"), "Cover ", sizeof("Cover "), 1);
        PDDocSetPageLabel(document.pdDoc, 0, coverLabel);

        std::wcout << L"Cover Label Added" << std::endl;

        //Set the label of second page and up to "preface" with the number counter starting at 2
        PDPageLabel prefaceLabel = PDPageLabelNew(document.pdDoc, ASAtomFromString("r"), "preface ", sizeof("preface "), 2);
        PDDocSetPageLabel(document.pdDoc, 1, prefaceLabel);

        std::wcout << L"Preface Labels Added" << std::endl;

        //Starting from the 5th page onwards, display pages numbers starting with 1 and upwards
        PDPageLabel pageLabel = PDPageLabelNew(document.pdDoc, ASAtomFromString("D"), "", 0, 1);
        PDDocSetPageLabel(document.pdDoc, 5, pageLabel);

        std::wcout << L"Normal Page Number Labels Added" << std::endl;
       
//==================================================================================================================================
// Step 3) Save and exit
//==================================================================================================================================
      
        document.saveDoc(L"labelled.pdf", PDSaveFull | PDSaveLinearized);    //Save the document, with output path, and save flags

        HANDLER

            errCode = ERRORCODE;

            libInit.displayError(errCode);                                   //If there was an error, display it.

        END_HANDLER

    return errCode;
}
