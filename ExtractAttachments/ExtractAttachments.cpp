// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//=============================================================================================
// Sample: ExtractAttachments - Opens a file called extractFrom.pdf in the
//         Input folder. It looks at the nametree and annotations to extract
//         and save the attachments out as individual files
//
// Steps:
//  1) Iterate through the annotations in a page to extract and save embedded files within it.
//  2) Iterate through the nametree to extract and save embedded files within it.
//  3) Display errors and exit
//=============================================================================================

#include "PERCalls.h"
#include "CosCalls.h"
#include "ASExtraCalls.h"
#include "InitializeLibrary.h"
#include "APDFLDoc.h"
#include <iostream>

//Function that goes through the nametree and extracts and saves nametree attachments.
ACCB1 ASBool ACCB2 extractor(CosObj obj, CosObj value, void *clientData);

int main(int argc, char** argv)
{

    APDFLib libInit(argv[1]);                     //Initialize the APDFL.
    ASErrorCode errCode = 0;             //Error code initially is 0.

    if (libInit.isValid() == false)      //Check for errors in initialization.
        return libInit.getInitError();   //If there was an error return the code.

    DURING

        APDFLDoc document(L"../_Input/extractFrom.pdf", true);    //Open a document and repair if damaged.

//=========================================================================================================================================================
// Step 1) Iterate through the annotations in a page to extract and save embedded files within it.                                 
//=========================================================================================================================================================
        
        PDPage pdPage = document.getPage(0);                                                           //Get the PDPage .
       
        int annotTotal = PDPageGetNumAnnots(pdPage);                                                   //Find the total number of annotations. 

        //Loop that goes through annotations and extracts and saves them
        for (int i = 0; i < annotTotal; i++)
        {
             
            PDAnnot annot = PDPageGetAnnot(pdPage, i);                                                 //Access the annotations based on index on page.
            
            ASAtom typeName = PDAnnotGetSubtype(annot);

            //If an annotation is a FileAttachment perform extraction, and if not ignore
            if (!strcmp(ASAtomGetString(PDAnnotGetSubtype(annot)), "FileAttachment"))
            {

                CosObj obj = PDAnnotGetCosObj(annot);                                                      //Get CosObj from annotation.

                //Get the cos dictionary object from the CosObj, using the File Specification key
                CosObj dictObj = CosDictGet(obj, ASAtomFromString("FS"));

                //Construct the accessed attachemnt using the dictionary
                PDFileAttachment fileAttachment = PDFileAttachmentFromCosObj(dictObj);

                ASTCount temp = 0;                                                                         //String length variable that will be auto filled.

                //Grab the file's name using the cos object dictionary 
                char* fileName = CosStringValue(CosDictGet(dictObj, ASAtomFromString("F")), &temp);

                std::wcout << L"Accessed File: " << fileName << std::endl;

                ASFile outFile = NULL;                                                                     //Declare an output file for an attachment.

                //Convert to wideString format
                const size_t cSize = strlen(fileName) + 1;
                wchar_t* outPathWideString = new wchar_t[cSize];
                mbstowcs(outPathWideString, fileName, cSize);

                //The output file path text object, made with the output file path and uniCode format
                ASText outPathText;

                if (sizeof(wchar_t) == 2)
                    outPathText = ASTextFromUnicode((ASUTF16Val*)outPathWideString, kUTF16HostEndian);
                else
                    outPathText = ASTextFromUnicode((ASUTF16Val*)outPathWideString, kUTF32HostEndian);

                delete outPathWideString;                                                                 //Delete the wideString since it is no longer used.

                //Create the output file path name
                ASPathName outPathName = ASFileSysCreatePathFromDIPathText(NULL, outPathText, NULL);

                //Open a new created ASFile, using the path name 
                ASFileSysOpenFile(ASGetDefaultFileSys(), outPathName, ASFILE_CREATE, &outFile);

                PDFileAttachmentSaveToFile(fileAttachment, outFile);                                      //Save the file attach out to outFile.

                //Safely close outFile
                ASFileFlush(outFile);
                ASFileClose(outFile);

                //Release used objects no longer needed
                if (outPathText)    ASTextDestroy(outPathText);
                if (outPathName)    ASFileSysReleasePath(NULL, outPathName);
            }
        }
        
        PDPageRelease(pdPage);                                                                       //Release the page since it is no longer in use.

        std::wcout << L"Finished annotation attachment extractions" << std::endl << std::endl;

//=========================================================================================================================================================
// Step 2) Iterate through the nametree to extract and save embedded files within it.                                    
//=========================================================================================================================================================

        //Create the nametree
        PDNameTree nameTree = PDDocCreateNameTree(document.getPDDoc(), ASAtomFromString("EmbeddedFiles"));

        //Apply the enum function to the nametree so it can iterate through, extracting the attachments.
        PDNameTreeEnum(nameTree, &extractor, NULL);

        std::wcout << L"Finished nametree attachment extractions" << std::endl << std::endl;

//=========================================================================================================================================================
// Step 3) Displays errors and exit
//=========================================================================================================================================================

    HANDLER

        errCode = ERRORCODE;

        libInit.displayError(errCode);    //If there was an error, display it.

        END_HANDLER

        return errCode;                   //Return program status.
}

//=========================================================================================================================================================
// Enum Function: Goes through the nametree to extracts and saves attachements
//=========================================================================================================================================================
ACCB1 ASBool ACCB2 extractor(CosObj obj, CosObj value, void *clientData)
{
   
    PDFileAttachment fileAttachment = PDFileAttachmentFromCosObj(value);                            //Convert the CosObj to a file attachment.
  
    ASTCount temp = 0;                                                                              //String length variable that will be auto filled.

    //Grab the file's name using the cos object dictionary and the File Specifcation String key. 
    char* fileName = CosStringValue(CosDictGet(value, ASAtomFromString("F")), &temp);
    std::wcout << L"Accessed file: " << fileName << std::endl;

    //Coversion to wideString format
    const size_t cSize = strlen(fileName) + 1;
    wchar_t* outPathWideString = new wchar_t[cSize];
    mbstowcs(outPathWideString, fileName, cSize);

    //Construct the text for the path name based on the right Unicode format
    ASText outPathText = NULL;
    if (sizeof(wchar_t) == 2)
        outPathText = ASTextFromUnicode((ASUTF16Val*)outPathWideString, kUTF16HostEndian);
    else
        outPathText = ASTextFromUnicode((ASUTF16Val*)outPathWideString, kUTF32HostEndian);
   
    delete[] outPathWideString;                                                                    //Delete the wideString since it is no longer used.
   
    ASPathName outPathName = ASFileSysCreatePathFromDIPathText(NULL, outPathText, NULL);           //The output file path name.
   
    ASFile outFile = NULL;                                                                         //Declare an output file for an attachment.

    //Open a new created ASFile under 2GB, using the path name 
    ASFileSysOpenFile(ASGetDefaultFileSys(), outPathName, ASFILE_CREATE, &outFile);

    
    PDFileAttachmentSaveToFile(fileAttachment, outFile);                                           //Save the file attachment out to outFile.

    //Close outFile
    ASFileFlush(outFile);
    ASFileClose(outFile);

    //Release used objected no longer needed
    if (outPathText)    ASTextDestroy(outPathText);
    if (outPathName)    ASFileSysReleasePath(NULL, outPathName);

    return true;
}
