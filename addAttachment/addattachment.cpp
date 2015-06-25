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

    //Sample variables
    wchar_t* p_attachment1 = L"../Input/attachment1.xlsx";  //Path to attachment 1 (name tree)
    wchar_t* p_attachment2 = L"../Input/attachment2.docx";  //Path to attachment 2 (annotation)

    wchar_t* p_inputpdf = L"../Input/noattachment.pdf";  //Path to input pdf
    wchar_t* p_attached = L"../Input/attachment.pdf";    //Path to new PDF

    //PDFL variables
    ASErrorCode  errCode = 0;          //Tracks errors.

    //Sample variables
    PDDoc inputpdf = NULL;             //Reference to input pdf
        //For attachment 1
    ASFile attach1_as = NULL;           //For opening the attachment file
    PDFileAttachment attach1_pfa;       //APDFL object for attachment
    PDFileSpec attach1_fs;
    PDNameTree files_tree;             //EmbeddedFiles name tree of the input pdf
        //For attachment 2
    ASFile attach2_as = NULL;           //For opening the attachment file
    PDFileAttachment attach2_pfa;       //APDFL object for attachment
    PDFileSpec attach2_fs;
        //Link stuff re: attachment 2
    ASFixedRect annot_location;
    PDLinkAnnot attachment_annot;
    PDAction annot_action;
    PDPage page1 = NULL;

    DURING

        //  \\\\\\\\\\\\\\\\\\\\\
        //   Initialize variables
        //  /////////////////////

        //Open the input pdf
        std::wcout << L"Opening the input PDF." << std::endl;
        inputpdf = util.openPDFNoSecurity(p_inputpdf);

        //Open attachment 1 file
        std::wcout << L"Opening attachment 1." << std::endl;
        attach1_as = util.openASFile(p_attachment1);

        //Open attachment 2 file
        std::wcout << L"Opening attachment 2." << std::endl;
        attach2_as = util.openASFile(p_attachment2);

        //  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
        //   Embed Attachment 1 via the name tree
        //  /////////////////////////////////////

        std::wcout << L"Embedding attachment 1 through name tree." << std::endl;

        //Create PDFileAttachment 1 and its file specification dictionary
        attach1_pfa = PDFileAttachmentNewFromFile(
            PDDocGetCosDoc(inputpdf),     //Input pdf, as cosdoc
            attach1_as,                    //ASFile to attach
            NULL, (ASUns32)0,             //No filters for the file attachment stream
            CosNewNull(),                 //No filter parameters
            NULL, NULL, NULL                //No ASProgressMonitor
            );
        attach1_fs = PDFileAttachmentGetCosObj(attach1_pfa);

        //Retrieve name tree from inputpdf to put the attachment in.
        //EmbeddedFiles has not yet been used, so we must "create" it.
        //Normally you would test this name tree with PDNameTreeIsValid.
        files_tree = PDDocCreateNameTree(
            inputpdf,                              //The input pdf
            ASAtomFromString("EmbeddedFiles"));    //Which name tree we want

        //Put the attachment in the name tree.
        PDNameTreePut(
            files_tree,                                //The tree to add to
            CosNewString(                              //The key value for the name tree (totally arbitrary)
                PDDocGetCosDoc(inputpdf),    //The relevant cosdoc
                true,                        //Create an indirect object; needs saving
                "TheSpreadsheet",14),        //The string itself
            attach1_fs);                               //The file specification for the attachment.

        //  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
        //   Embed Attachment 2 as an annotation
        //  ////////////////////////////////////

        std::wcout << L"Embedding attachment 2 w/ annotation." << std::endl;

        //Create PDFileAttachment 2 and its file specification dictionary
        attach2_pfa = PDFileAttachmentNewFromFile(
            PDDocGetCosDoc(inputpdf),     //Input pdf, as cosdoc
            attach2_as,                    //ASFile to attach
            NULL, (ASUns32)0,             //No filters for the file attachment stream
            CosNewNull(),                 //No filter parameters
            NULL, NULL, NULL                //No ASProgressMonitor
            );

        attach2_fs = PDFileAttachmentGetCosObj(attach2_pfa);
        
        //The annotation will go on page 1...
        page1 = PDDocAcquirePage(inputpdf, (ASInt32)0);

        //Set the bounding rect to place the attachment TODO: this is f'd up
        annot_location.left = Int16ToFixed(5 * 72);
        annot_location.top = Int16ToFixed(0 * 72);
        annot_location.right = Int16ToFixed(6 * 72);
        annot_location.bottom = Int16ToFixed(9.25 * 72);

        PDNameTreePut(
            files_tree,                                //The tree to add to
            CosNewString(                              //The key value for the name tree (totally arbitrary)
            PDDocGetCosDoc(inputpdf),    //The relevant cosdoc
            true,                        //Create an indirect object; needs saving
            "TheDocx", 14),        //The string itself
            attach2_fs);                               //The file specification for the attachment.

        //Create the annotation
        attachment_annot = CastToPDLinkAnnot(
                                PDPageCreateAnnot(
                                    page1, 
                                    ASAtomFromString("FileAttachment"),
                                    &annot_location));
        /*
        //Make an action for it
        annot_action = PDActionNewFromFileSpec(
                            inputpdf,
                            ASAtomFromString("Launch"), //??????GoToE??????????????Launch???????????????????????
                            attach2_fs);
        
        //Set that action
        PDLinkAnnotSetAction(attachment_annot, annot_action);
        */
        //Add the annot to the page.
        PDPageAddAnnot(
            page1,
            (ASInt32)-2,
            attachment_annot);
        


        //  \\\\\\\\\\\\\\\
        //   Save and close
        //  ///////////////

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
    if (attach1_as) ASFileClose(attach1_as);

    //Display errors
    if (errCode)
        DisplayError(errCode);

    //Terminate the PDF library.
    MyPDFLTerm();

    //End with elegance
    return errCode;
}