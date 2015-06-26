// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// Sample addAttachment / Embeds two files to the input PDF:
//                          one is embedded via name tree, the other via annotation.
//                          The PDF is then saved as a new file.
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

    //  \\\\\\\\\\\\\
    //   Declarations
    //  /////////////

    //Sample variables
    wchar_t* path_attachment1 = L"../Input/attachment1.xlsx";  //Path to attachment 1 (attach via name tree)
    wchar_t* path_attachment2 = L"../Input/attachment2.docx";  //Path to attachment 2 (attach via annotation)

    wchar_t* path_inputpdf = L"../Input/noattachment.pdf";     //Path to input pdf
    wchar_t* path_attached = L"../Input/attachment.pdf";       //Path to new PDF to create

    //PDFL variables
    ASErrorCode  errCode = 0;          //Tracks errors.

    //Sample variables
    PDDoc inputpdf = NULL;             //Reference to input pdf
        //For attachment 1
    ASFile attach1_as;                 //ASFile for attachment 1
    PDFileAttachment attach1_pfa;      //PDFileAttachment for attachment 1
    PDNameTree files_tree;             //EmbeddedFiles name tree of the input pdf
        //For attachment 2
    ASFile attach2_as;                 //ASFile for attachment 2
    PDFileAttachment attach2_pfa;      //PDFileAttachment for attachment 2
    ASFixedRect annot_location;        //Defines the location for the annotation
    PDLinkAnnot attachment_annot;      //The annotation in which we'll embed attachment 2
    PDPage page1 = NULL;               //Reference to page 1, where the annotation goes

    DURING
        //Open the input pdf
        std::wcout << L"Opening the input PDF." << std::endl;
        inputpdf = util.openPDFNoSecurity(path_inputpdf);

        //  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
        //   Embed Attachment 1 via the name tree
        //  /////////////////////////////////////

        //Create PDFileAttachment 1
        std::wcout << L"Opening attachment 1." << std::endl;
        attach1_as = util.openASFile(path_attachment1);
        attach1_pfa = PDFileAttachmentNewFromFile(
            PDDocGetCosDoc(inputpdf),     //Input pdf, as cosdoc
            attach1_as,                   //ASFile to attach
            NULL, (ASUns32) 0,            //No filters for the file attachment stream
            CosNewNull(),                 //No filter parameters
            NULL, NULL, NULL);            //No ASProgressMonitor

        //Retrieve name tree from inputpdf to put the attachment in
        //(The name tree has not yet been used, so we must "create" it)
        files_tree = PDDocCreateNameTree(
            inputpdf,                              //The pdf whose name tree we want
            ASAtomFromString("EmbeddedFiles"));    //The name tree in which embedded files go

        //Embed the attachment in the name tree
        std::wcout << L"Placing it in the name tree." << std::endl;
        PDNameTreePut(
            files_tree,                                   //The tree to add to
            CosNewString(                                 //The key for the name tree (totally arbitrary)
                PDDocGetCosDoc(inputpdf),  //The relevant cosdoc
                true,                      //Create an indirect object; needs saving
                "TheSpreadsheet",14),      //The string itself
            PDFileAttachmentGetCosObj(attach1_pfa));      //The value: the attachment's file specification

        //  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
        //   Embed Attachment 2 as an annotation
        //  ////////////////////////////////////

        //Create PDFileAttachment 2
        std::wcout << L"Opening attachment 2." << std::endl;
        attach2_as = util.openASFile(path_attachment2);
        attach2_pfa = PDFileAttachmentNewFromFile(
            PDDocGetCosDoc(inputpdf),      //Input pdf, as cosdoc
            attach2_as,                    //ASFile to attach
            NULL, (ASUns32)0,              //No filters for the file attachment stream
            CosNewNull(),                  //No filter parameters
            NULL, NULL, NULL);             //No ASProgressMonitor
        
        //Create the annotation
        std::wcout << L"Embedding it through an annotation." << std::endl;

        //Our rect determines the annotation's placement on the page
        annot_location.left   = Int16ToFixed(2.50 * 72);
        annot_location.right  = Int16ToFixed(3.00 * 72);
        annot_location.top    = Int16ToFixed(8.40 * 72);
        annot_location.bottom = Int16ToFixed(8.90 * 72);

        //The annotation will go on page 1...
        page1 = PDDocAcquirePage(inputpdf, (ASInt32)0);

        //Create the annotation (it still must be edited and officially added to the page)
        attachment_annot = PDPageCreateAnnot(page1, ASAtomFromString("FileAttachment"), &annot_location);
        
        //Put the file specification into the annotation's cos dictionary.
        //This effectively sets the annotation's action to open the document,
        //and properly embeds the document.
        CosDictPutKeyString(
            PDAnnotGetCosObj(attachment_annot),      //The annotation dictionary we want to edit
            "FS",                                    //the key for the dictionary: we're editing the File Specification
            PDFileAttachmentGetCosObj(attach2_pfa)); //the value for the dictionary: The file spec we want the annot to open

        //Add the annotation to the page
        PDPageAddAnnot(page1,   //The page to add it to
            (ASInt32)-2,        //The placement in the page's annotation array (before first)
            attachment_annot);  //The annotation to add

        //  \\\\\\\\\\\\\\\
        //   Save and close
        //  ///////////////

        //Save the document as a new document.
        std::wcout << L"All embeddings successful. Saving the output document." << std::endl;

        PDPageRelease(page1);                     //Release your pages before saving

        PDDocSave(
            inputpdf,                             //Document to save
            PDDocNeedsSave | PDDocIsOpen          //PDDocSaveFlags
                | PDSaveCopy,
            util.makeASPathName(path_attached),   //ASPath to save to
            ASGetDefaultFileSys(),                //The file system
            NULL, NULL);                          //No ASProgressMonitor

        std::wcout << L"The document was saved." << std::endl;

        //Close the input document
        PDDocClose(inputpdf);

    HANDLER
        errCode = ERRORCODE;
    END_HANDLER

    //Release resources
    if (inputpdf)   PDDocRelease(inputpdf);
    if (attach1_as) ASFileClose(attach1_as);
    if (attach2_as) ASFileClose(attach2_as);

    //Display errors
    if (errCode)
        DisplayError(errCode);

    //Terminate the PDF library
    MyPDFLTerm();

    //End.
    return errCode;
}