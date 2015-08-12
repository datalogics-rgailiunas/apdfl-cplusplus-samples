// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//====================================================================
// Sample: AddAttachment - Adds two attachments to the input document.
//
// Note: 
// The first attachment is embedded in the EmbeddedFiles name tree of
// the document.
// The second is embedded into the File Specification dictionary of
// an annotation that will be added to the page.
//
//Steps:
// 1) Create the first PDFileAttachment.
// 2) Embed it to the document's name tree.
// 3) Create the second PDFileAttachment.
// 4) Create an annotation.
// 5) Embed the second PDFileAttachment to the annotation.
// 6) Save and close.
//====================================================================

#include <iostream>

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#include "CosCalls.h"

//Returns a newly opened ASFile, given the file's path.
ASFile openASFile(wchar_t* filepath);

int main(int argc, char** argv)
{
    APDFLib lib;                                                 //Initialize the Adobe PDF Library.

    if (lib.isValid() == false)                                  //If it failed to initialize, return the error code.
        return lib.getInitError();

    wchar_t* pathAttachment1 = L"../_Input/attachment1.xlsx";    //Path to first attachment.
    wchar_t* pathAttachment2 = L"../_Input/attachment2.docx";    //Path to second attachment.
    wchar_t* pathInput       = L"../_Input/noattachment.pdf";    //Path to input pdf.
    wchar_t* pathOutput      = L"attached.pdf";                  //Path to output pdf we'll create.

    ASErrorCode errCode = 0;                                     //This will catch error codes thrown during library usage.

    DURING

        std::wcout << L"Opening the input PDF." << std::endl;

        APDFLDoc inputAPDoc(pathInput, true);                    //Opens the input PDF document.
        PDDoc inputDoc = inputAPDoc.getPDDoc();                  //The input PDF's PDDoc reference.
        CosDoc inputDocCD = PDDocGetCosDoc(inputDoc);            //The PDDoc's COS representation.

//===============================================================================================================================================
// 1) Create the first PDFileAttachment.
//===============================================================================================================================================

        std::wcout << L"Opening attachment 1." << std::endl;

        ASFile attach1ASFile = openASFile(pathAttachment1);           //Open the first file we want to attach.

        PDFileAttachment attach1PDFA =
              PDFileAttachmentNewFromFile(inputDocCD,                 //The PDF the file attachment will be referenced from.
                                          attach1ASFile,              //The file we want to attach.
                                          NULL, 0,                    //No filters for the file attachment stream.
                                          CosNewNull(),               //No filter parameters.
                                          NULL, NULL, NULL);          //No progress monitoring.

        CosObj attach1CO = PDFileAttachmentGetCosObj(attach1PDFA);    //Get the CosObj representation of the file attachment.

        ASFileClose(attach1ASFile);                                   //Release the ASFile. We only need the PDFileAttachment object to embed.

//===============================================================================================================================================
// 2) Embed it to the document's name tree.
//===============================================================================================================================================

        std::wcout << L"Embedding it in the name tree." << std::endl;

        //Retrieve the document's "EmbeddedFiles" name tree.
        //(This name tree has not yet been used, so we must "create" it)
        PDNameTree filesTree = PDDocCreateNameTree(inputDoc, ASAtomFromString("EmbeddedFiles"));

        //The KEY to add to the inputDocCD CosDoc. Totally arbitrary.
        CosObj EmbedKey = CosNewString(inputDocCD, true, "TheSpreadsheet", 14);

        //Adding a key/value pair to the EmbeddedFiles name tree, where the value is
        //a CosObj of a PDFileAttachment, embeds the file in the PDDoc.
        PDNameTreePut(filesTree,     //The tree to add the Key/Value pair to.
                      EmbedKey,      //the KEY.
                      attach1CO);    //The VALUE: the attachment's file specification

        std::wcout << L"Success." << std::endl;

//===============================================================================================================================================
// 3) Create the second PDFileAttachment.
//===============================================================================================================================================

        std::wcout << L"Opening attachment 2." << std::endl;

        ASFile attach2ASFile = openASFile(pathAttachment2);           //Open the second file we want to attach.

        PDFileAttachment attach2PDFA =
            PDFileAttachmentNewFromFile(inputDocCD,                   //The PDDoc CosDict that the file attachment will be referenced from.
                                        attach2ASFile,                //The file we want to attach.
                                        NULL, 0,                      //No filters for the file attachment stream.
                                        CosNewNull(),                 //No filter parameters.
                                        NULL, NULL, NULL);            //No progress monitoring.


        CosObj attach2CO = PDFileAttachmentGetCosObj(attach2PDFA);    //Get the CosObject representation of the file attachment.

        ASFileClose(attach2ASFile);                                   //Release the ASFile. We only need the PDFileAttachment CosObject to embed.

//===============================================================================================================================================
// 4) Create an annotation.
//===============================================================================================================================================

        std::wcout << L"Creating an annotation for it." << std::endl;

        //This rectangle determines the annotation's placement on the page.
        ASFixedRect annotLocation;
        annotLocation.left   = ASFloatToFixed(2.50 * 72);                           //There are 72 pixels per inch.
        annotLocation.right  = ASFloatToFixed(3.00 * 72);
        annotLocation.top    = ASFloatToFixed(8.40 * 72);
        annotLocation.bottom = ASFloatToFixed(8.90 * 72);

        PDPage page1 = PDDocAcquirePage(inputDoc, 0);                               //The annotation will go on page 1.

        //This method just creates an annotation.
        PDAnnot newAnnot = PDPageCreateAnnot(page1,                                 //The page the annotation will go on.
                                             ASAtomFromString("FileAttachment"),    //The type of annotation we're creating.
                                             &annotLocation);                       //The annotation's location on the page.

        //This method actually adds the annotation to the page.
        PDPageAddAnnot(page1, -2, newAnnot);                                        //Add the annotation as the first annotation on the page.

        CosObj newAnnotCO = PDAnnotGetCosObj(newAnnot);                             //The annotation's CosObject representation.

//===============================================================================================================================================
// 5) Embed the second PDFileAttachment to the annotation.
//===============================================================================================================================================

        std::wcout << L"Embedding the attachment in the annotation." << std::endl;

        //Like embedding to a PDDoc's name tree, adding a key/Value pair to
        //the annotation's CosDict, where the key is "FS" (the File Specification key)
        //and the value is the a PDFileAttachment cos object, will embed the file to
        //the annotation.
        CosDictPutKeyString(newAnnotCO,                    //The dictionary we want to add to.
                            "FS",                          //The KEY: we're changing the File Specification.
                            attach2CO);                    //The VALUE: The file spec we want the annot to open

//===============================================================================================================================================
// 6) Save and close.
//===============================================================================================================================================

        std::wcout << L"Success." << std::endl; 
        std::wcout << L"Saving the output document." << std::endl;

        PDPageRelease(page1);                              //PDPages must be released before closing the document.
        inputAPDoc.saveDoc(pathOutput);                    //inputAPDoc's destructor will close the document.

        std::wcout << L"Success." << std::endl;

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);                         //If there was an error, display it.

    END_HANDLER

    return errCode;                                        //lib's destructor terminates the library.
};

//===============================================================================================================================================
//ASFile function: Returns a newly opened ASFile, given the file's path.
//===============================================================================================================================================
ASFile openASFile(wchar_t* filepath)
{
    ASFile file = NULL;            //The file we want to open.
    ASText pathText = NULL;        //ASText object of its filepath.
    ASPathName pathName = NULL;    //ASPathName of its filepath.

    DURING

        //Compute the host's unicode format.
        ASUnicodeFormat hostUniFormat;
        if (sizeof(wchar_t) == 2)
            hostUniFormat = kUTF16HostEndian;
        else
            hostUniFormat = kUTF32HostEndian;

        //Create the file path name to open the file.
        pathText = ASTextFromUnicode((ASUTF16Val *)filepath, hostUniFormat);
        pathName = ASFileSysCreatePathFromDIPathText(NULL, pathText, NULL);

        //Open the file.
        ASErrorCode openErr = ASFileSysOpenFile(ASGetDefaultFileSys(),    //The file system from which the pathname was obtained.
                                                pathName,                 //The ASPathName of the file's path.
                                                ASFILE_READ,              //The mode we want to process the file with.
                                                &file);                   //File will be opened to this object.

        //Release resources.
        ASTextDestroy(pathText);
        ASFileSysReleasePath(ASGetDefaultFileSys(), pathName);

        if (openErr) ASRaise(openErr);                                    //If there was an error opening the file, throw it.

    HANDLER

        ASRaise(ERRORCODE);                                               //If there was some other error, throw it.

    END_HANDLER

    return file;
};
