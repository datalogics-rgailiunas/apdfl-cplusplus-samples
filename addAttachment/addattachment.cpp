// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// Sample addAttachment / Embeds a file to the input PDF, and saves as a new PDF.
//
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

//APDFL
#include <CosCalls.h>

//Sample
#include "SampleUtils.h"

int main(){
    //For common subroutines.
    Utilities util;

    //Initialize the PDF library.
    if (int error = util.initPDFL()) return error;

    //Sample variables
    wchar_t* p_attachment = L"../Input/forattach.xlsx";  //Path to PDF we want to attach to
    wchar_t* p_inputpdf = L"../Input/noattachment.pdf";  //Path to attachment
    wchar_t* p_attached = L"../Input/attachment.pdf";    //Path to new PDF

    //PDFL variables
    ASErrorCode  errCode = 0;          //Tracks errors.

    //Sample variables
    PDDoc inputpdf = NULL;             //PDDoc for input pdf
    ASFile attach_as = NULL;           //For opening the attachment file
    PDFileAttachment attach_pfa;       //APDFL object for attachment
    PDNameTree files_tree;             //EmbeddedFiles name tree of the input pdf

    DURING
        //Open the input pdf
        std::wcout << L"Opening the input PDF." << std::endl;
        inputpdf = util.openPDFNoSecurity(p_inputpdf);

        //Open the file we want to attach
        std::wcout << L"Opening the attachment." << std::endl;
        attach_as = util.openASFile(p_attachment);

        //Create attachment
        std::wcout << L"Attaching it." << std::endl;
        attach_pfa = PDFileAttachmentNewFromFile(
            PDDocGetCosDoc(inputpdf),     //Input pdf, as cosdoc
            attach_as,                    //ASFile to attach
            NULL, (ASUns32)0,             //No filters for the file attachment stream
            CosNewNull(),                 //No filter parameters
            NULL,NULL,NULL                //No ASProgressMonitor
            );

        //Retrieve name tree from inputpdf to put the attachment in.
        //EmbeddedFiles has not yet been used, so we must "create" it.
        //Normally you would test this name tree with PDNameTreeIsValid.
        files_tree = PDDocCreateNameTree(
            inputpdf,                              //The input pdf
            ASAtomFromString("EmbeddedFiles"));    //Which name tree we want

        //Put the attachment in the name tree.
        PDNameTreePut(
            files_tree,                                //The tree to add to
            CosNewString(                              //The key value for the name tree
                PDDocGetCosDoc(inputpdf),    //The relevant cosdoc
                true,                        //Create an indirect object; needs saving
                "attn",4),                   //The string itself
            PDFileAttachmentGetCosObj(attach_pfa));    //The file specification for the attachment.

        //Save the document as a new document.
        std::wcout << L"Attached successfully. Saving the new document." << std::endl;
        PDDocSave(
            inputpdf,                           //Document to save
            PDDocNeedsSave | PDDocIsOpen        //PDDocSaveFlags
                | PDSaveCopy,
            util.makeASPathName(p_attached),    //ASPath to save to
            ASGetDefaultFileSys(),              //The file system
            NULL, NULL);                        //No ASProgressMonitor.
        std::wcout << L"The document was saved." << std::endl;

        //Close the input document
        PDDocClose(inputpdf);

    HANDLER
        errCode = ERRORCODE;
    END_HANDLER

    //Release resources
    if (inputpdf)  PDDocRelease(inputpdf);
    if (attach_as) ASFileClose(attach_as);

    //Display errors
    if (errCode)
        DisplayError(errCode);

    //Terminate the PDF library.
    MyPDFLTerm();

    //End with elegance
    return errCode;
}