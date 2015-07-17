// Copyright(c) 2015, Datalogics, Inc.All rights reserved.
//
//****************************************************************************
// Sample: AddPageLabels-Numbers. Adds numbered labels onto a pdf's pages  
//
// Note: This program creates page labels with numberings viewable by looking
//         at a documents thumbnails.
//
// Steps:
//  1) Open the Document that will have labels added to.
//  2) Create label's for different sets of pages
//  3) Save and exit
//****************************************************************************
//
// This agreement is between Datalogics, Inc. 101 N.Wacker Drive, Suite 1800,
// Chicago, IL 60606 ("Datalogics") and you, an end user who downloads
// source code examples for integrating to the Adobe PDF Library
// ("the Example Code"). By accepting this agreement you agree to be bound
// by the following terms of use for the Example Code.
//
// LICENSE
// -------
// Datalogics hereby grants you a royalty - free, non - exclusive license to
// download and use the Example Code for any lawful purpose.There is no charge
// for use of Example Code.
//
// OWNERSHIP
// ---------
// The Example Code and any related documentation and trademarks are and shall
// remain the sole and exclusive property of Datalogics and are protected by
// the laws of copyright in the U.S.and other countries.
//
// Datalogics is a trademark of Datalogics, Inc.
//
// TERM
// ----
// This license is effective until terminated.You may terminate it at any
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
// NON - INFRINGEMENT, ACCURACY OR COMPLETENESS OF RESPONSES, RESULTS, AND / OR
// LACK OF WORKMANLIKE EFFORT.THE PROVISIONS OF THIS SECTION SET FORTH
// SUBLICENSEE'S SOLE REMEDY AND DATALOGICS'S SOLE LIABILITY WITH RESPECT
// TO THE WARRANTY SET FORTH HEREIN.NO REPRESENTATION OR OTHER AFFIRMATION
// OF FACT, INCLUDING STATEMENTS REGARDING PERFORMANCE OF THE EXAMPLE CODE,
// WHICH IS NOT CONTAINED IN THIS AGREEMENT, SHALL BE BINDING ON DATALOGICS.
// NEITHER DATALOGICS WARRANT AGAINST ANY BUG, ERROR, OMISSION, DEFECT,
// DEFICIENCY, OR NONCONFORMITY IN ANY EXAMPLE CODE. 

#include "InitializeLibrary.h"
#include "APDFLDoc.h"
#include "ASExtraCalls.h"
#include <iostream>

int main(int argc, char** argv)
{

    APDFLib libInit;                        //Initialize the APDFL.
    ASErrorCode errCode = 0;                //Error code initially is 0.

    if (libInit.isValid() == false)         //Check for errors in initialization.
        errCode = libInit.getInitError();   //If there was an error set the code.

    DURING

//******************************************************************************************************************
// Step 1) Open the Document that will have labels added to.
//******************************************************************************************************************

        APDFLDoc document(L"../Input/toNumberLabel.pdf", true);    //Open a document and repair if damaged

        std::wcout << L"Document was sucessfully opened." << std::endl;

//******************************************************************************************************************
// Step 2) Create label's for different sets of pages
//******************************************************************************************************************
        
        PDDoc inDoc = document.getPDDoc();

        //Set the first page's label to "Cover" with the number counter to 1
        PDPageLabel coverLabel = PDPageLabelNew(inDoc, ASAtomFromString("D"), "Cover ", sizeof("Cover "), 1);
        PDDocSetPageLabel(inDoc, 0, coverLabel);

        std::wcout << L"Cover Label Added" << std::endl;

        //Set the label of second page and up to "preface" with the number counter starting at 2
        PDPageLabel prefaceLabel = PDPageLabelNew(inDoc, ASAtomFromString("r"), "preface ", sizeof("preface "), 2);
        PDDocSetPageLabel(inDoc, 1, prefaceLabel);

        std::wcout << L"Preface Labels Added" << std::endl;

        //Starting from the 5th page onwards, display pages numbers starting with 1 and up
        PDPageLabel pageLabel = PDPageLabelNew(inDoc, ASAtomFromString("D"), "", 0, 1);
        PDDocSetPageLabel(inDoc, 5, pageLabel);

        std::wcout << L"Normal Page Number Labels Added" << std::endl;

        

//******************************************************************************************************************
// Step 3) Save and exit
//******************************************************************************************************************

        //Save the document, with output path, and save flags
        document.saveDoc(L"labelled.pdf", PDSaveFull | PDSaveLinearized);

        PDDocRelease(inDoc);

        HANDLER

            //If there was an exception generate an error code 
            errCode = ERRORCODE;

            //Display the error code
            libInit.displayError(errCode); 

        END_HANDLER

    return errCode;
}
