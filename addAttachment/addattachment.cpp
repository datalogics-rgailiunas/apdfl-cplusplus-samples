// Copyright (c) 2015, Datalogics, Inc. All rights reserved.

//===============================================================
// Sample: AddAttachment - Adds two attachments to a document.
//
// This sample adds to attachments to a document.
// One by embedding it in the EmbeddedFiles name tree of
// the document,
// the other by embedded it into an annotation we add.
//
//Steps:
// 1) Create and embed an attachment to the name tree
// 2) Create and embed an attachment to an annotation
// 3) Save and close
//===============================================================

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

#include "SampleUtils.h"
#include "../Common/Init/InitializeLibrary.h"

int main()
{
    Utilities util;                                            //For common subroutines

    //Initialize the APDF libary
    APDFLib lib;
    int err = lib.getInitError(); //Will display errors, if any
    if (err) return err;

    //APDFL variables
    ASErrorCode  errCode = 0;                                  //Tracks errors.

    //Sample variables
    wchar_t* path_attachment1 = L"../Input/attachment1.xlsx";  //Path to attachment name tree
    wchar_t* path_attachment2 = L"../Input/attachment2.docx";  //Path to attachment for annotation
    wchar_t* path_inputpdf = L"../Input/noattachment.pdf";     //Path to input pdf
    wchar_t* path_attached = L"attached.pdf";       //Path to output pdf

    DURING

        std::wcout << L"Opening the input PDF." << std::endl;
        PDDoc inputpdf = util.openPDFNoSecurity(path_inputpdf);

//===========================================================================================
//Step 1) Create and embed an attachment to the name tree.
//===========================================================================================

        //Create PDFileAttachment 1
        std::wcout << L"Opening attachment 1." << std::endl;
        ASFile attach1_as = util.openASFile(path_attachment1);
        PDFileAttachment attach1_pfa = PDFileAttachmentNewFromFile(
                                        PDDocGetCosDoc(inputpdf),              //The relevant pdf
                                        attach1_as,                            //The relevant ASFile
                                        NULL, (ASUns32) 0,                     //No filters for the file attachment stream
                                        CosNewNull(),                          //No filter parameters
                                        NULL, NULL, NULL);                     //No ASProgressMonitor

        ASFileClose(attach1_as);


        //Retrieve the proper file embedding name tree
        //(This name tree has not yet been used, so we must "create" it)
        PDNameTree files_tree = PDDocCreateNameTree(inputpdf,ASAtomFromString("EmbeddedFiles"));

        //Embed the attachment in the name tree
        std::wcout << L"Placing it in the name tree." << std::endl;
        PDNameTreePut(
            files_tree,                                        //The tree to put it in
            CosNewString(PDDocGetCosDoc(inputpdf),             //The KEY (totally arbitrary)
                true,"TheSpreadsheet",14),
            PDFileAttachmentGetCosObj(attach1_pfa));           //The VALUE: the attachment's file specification


//===========================================================================================
//Step 2) Create and embed an attachment to an annotation
//===========================================================================================

        //Create PDFileAttachment 2
        std::wcout << L"Opening attachment 2." << std::endl;
        ASFile attach2_as = util.openASFile(path_attachment2);
        PDFileAttachment attach2_pfa = PDFileAttachmentNewFromFile(
                                        PDDocGetCosDoc(inputpdf),              //The relevant pdf
                                        attach2_as,                            //The relevant ASFile
                                        NULL, (ASUns32)0,                      //No filters for the file attachment stream
                                        CosNewNull(),                          //No filter parameters
                                        NULL, NULL, NULL);                     //No ASProgressMonitor

        ASFileClose(attach2_as);
        attach2_as = NULL;

        //Create the annotation
        std::wcout << L"Embedding it through an annotation." << std::endl;

        //This rect determines the annotation's placement on the page
        ASFixedRect annot_location;
        annot_location.left   = Int16ToFixed(2.50 * 72);
        annot_location.right  = Int16ToFixed(3.00 * 72);
        annot_location.top    = Int16ToFixed(8.40 * 72);
        annot_location.bottom = Int16ToFixed(8.90 * 72);

        PDPage page1 = PDDocAcquirePage(inputpdf, (ASInt32)0);
        PDAnnot attachment_annot = PDPageCreateAnnot(page1, 
                            ASAtomFromString("FileAttachment"), &annot_location);
        CosDictPutKeyString(                                   //Embed the file specification into the annotation's cos dictionary.
            PDAnnotGetCosObj(attachment_annot),                //The dictionary we want to edit
            "FS",                                              //The KEY for the dictionary: we're editing the File Specification
            PDFileAttachmentGetCosObj(attach2_pfa));           //The VALUE for the dictionary: The file spec we want the annot to open

        PDPageAddAnnot(page1,(ASInt32)-2,attachment_annot);    //Add the annotation as the first annotation on the page


//===========================================================================================
//Step 3) Save and close
//===========================================================================================

        std::wcout << L"All embeddings successful. Saving the output document." << std::endl;

        PDPageRelease(page1);

        PDDocSave(inputpdf, PDDocNeedsSave | PDDocIsOpen | PDSaveCopy,
            util.makeASPathName(path_attached), ASGetDefaultFileSys(), NULL, NULL);

        std::wcout << L"The document was saved." << std::endl;

        PDDocClose(inputpdf);

    HANDLER

        errCode = ERRORCODE;

    END_HANDLER

    if (errCode) lib.displayError(errCode);    //If there was an error, display it
    return errCode;                            //End. lib's destructor terminates the library.
}
