// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//============================================================================
// Sample: ExtractDocumentInformation opens a document, extracts document 
// information and saves it in viewable format in a new pdf
//
// Note: This program uses standard document information contained in the
// "Document Information Dictionary" see the PDF Reference section 10.2 for 
// more information
//
// Steps:
//  1) Extract document information from input document 
//  2) Add information to a new pdf, save and exit
//============================================================================

#include <string>
#include <iostream>
#include "ASCalls.h"
#include "InitializeLibrary.h"
#include "ASExtraCalls.h"
#include "PagePDECntCalls.h"
#include "PSFCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "APDFLDoc.h"

int main(int argc, char** argv)
{
    APDFLib libInit(argv[1]);                     //Initialize the Adobe PDF library.
    ASErrorCode errCode = 0;             //Variable used to report any exceptions/errors if they occured. 

    if (libInit.isValid() == false)      //Check for errors in initialization.
        return libInit.getInitError();    //If it failed, return the error code.

    DURING

        APDFLDoc inDoc(L"../_Input/extractInfoFrom.pdf", true);              //Open the input document and repair if damaged

        std::wcout << L"Document was successfully opened." << std::endl;

//=========================================================================================================================================================================
// Step 1) Extract document information from input document
//=========================================================================================================================================================================

        APDFLDoc outDoc;                                                                                //Create a new document

        //Create page to the source document with where to place, and bound rectangle
        outDoc.insertPage(Int32ToFixed(4 * 72), Int32ToFixed(4 * 72), PDBeforeFirstPage);

        PDPage page = outDoc.getPage(0);                                                                //Grab the first page from the source PDF                                       

        PDEContent content = PDPageAcquirePDEContent(page, NULL);                                       //Grab content from the page

        PDEFontAttrs fontAttrs;                                                                         //Structure holding the attributes of a PDEFont.

        //Initialize font Attributes
        memset(&fontAttrs, 0, sizeof(fontAttrs));
        fontAttrs.name = ASAtomFromString("CourierStd");                                                //Set font style
        fontAttrs.type = ASAtomFromString("Type1");                                                     //Set font type

        //Create system font using font attributes, its size, and flags
        PDSysFont sysFont = PDFindSysFont(&fontAttrs, sizeof(fontAttrs), 0);

        //Create the pdeFont with the sysFont and a PDEFontCreateFlags      
        PDEFont pdeFont = PDEFontCreateFromSysFont(sysFont, kPDEFontCreateEmbedded);

        PDEGraphicState gState;                                                                         //Structure holding the graphic attributes of the text object.

        PDEDefaultGState(&gState, 0);                                                                   //Set to default since the text is being displayed plainly.

        PDETextState tState;                                                                            //Structure holding the attributes of a PDEText.   
        PDEText textObj[8];                                                                             //Array holding text objects with text to be displayed onto a PDF

        //Array of key names, used to access specific information about the document
        std::string keyNames[7] = { "Title", "Author", "Subject", "Keywords", "Creator", "Producer", "Trapped" };

        ASText keyText;                                                                                 //Text object holding a keys name

        ASText infoText = ASTextNew();                                                                  //Text object to hold information about the source pdf

        std::string concatString;                                                                       //String that contains the final output text for the output PDF

        //Loop that will iterate through the keys applying each of them, and extracting the correct 
        //information.This information is then taken and placed into a newly created pdf as  
        //individual text elements
        for (int i = 0; i < 8; i++)
        {
            //In this special case a description is used to display at the top
            if (i == 0)
            {
                concatString = "<<<Exracted Document Information>>>";
            }

            //In all other cases construct a string to be displayed 
            //which holds the decription's title and description 
            else
            {

                keyText = ASTextFromPDText(keyNames[i - 1].c_str());                                    //Access the key's text from it's string

                PDDocGetInfoASText(inDoc.getPDDoc(), keyText, infoText);                                //Obtain the description using the key, put into value

                char * infoString = ASTextGetPDTextCopy(infoText, 0);

                //Concatenate the resulting string
                concatString = keyNames[i - 1] + ": " + std::string(infoString);

                //Delete text objects, no longer in use
                ASTextDestroy(keyText);
                ASfree(infoString);

            }

            ASDoubleMatrix textMatrix;                                                                  //Matrix for sizing and setting location of text

            //Setting up a matrix for setting text size and placement location
            memset(&textMatrix, 0, sizeof(textMatrix));
            textMatrix.a = 6;                                                                           //Character width (matrix element size)          
            textMatrix.d = 6;                                                                           //Character width (matrix element size)         
            textMatrix.h = .5 * 72.0;                                                                   //Place at a x-value of a half inch (where 72 signifies an inch)

            //Place at y-value's starting at 3.5 inches and decrementing down by .25 inches 
            textMatrix.v = (3.5 * 72) - ((i / 4.0) * 72);

            textObj[i] = PDETextCreate();                                                               //Initialize and set the text object to hold text

            PDETextAddEx(textObj[i],                                                                    //The current text object
                kPDETextRun,                                                                            //kPDETextRun or kPDETextChar for text runs or text characters. 
                0,                                                                                      //The index after which to add the text run.
                (Uns8*)concatString.c_str(),                                                            //String to add, converted to Unsigned 8-bit form
                concatString.length(),                                                                  //String length
                pdeFont,                                                                                //The used font
                &gState, 0,                                                                             //Graphic state and its size. 
                &tState, 0,                                                                             //Text state and its size.
                &textMatrix,                                                                            //Transformation matrix for text. 
                NULL);                                                                                  //Stroke matrix, the matrix for the line width when stroking text
        }

        std::wcout << L"Text elements created and set." << std::endl;

        //Release the objects no longer in use
        PDERelease(reinterpret_cast<PDEObject>(pdeFont));
        PDERelease(reinterpret_cast<PDEObject>(gState.strokeColorSpec.space));
        PDERelease(reinterpret_cast<PDEObject>(gState.fillColorSpec.space));
        ASTextDestroy(infoText);
    

//=========================================================================================================================================================================
// Step 2) Add information to a new pdf, save and exit
//=========================================================================================================================================================================

        //Add the text elements to the page's content and then release
        for (int i = 0; i < 8; i++)
        {
            PDEContentAddElem(content, kPDEAfterLast, reinterpret_cast<PDEElement>(textObj[i]));
            PDERelease(reinterpret_cast<PDEObject>(textObj[i]));
        }


        PDPageSetPDEContentCanRaise(page, NULL);                                                      //Set the content back into the page

        //Release the page and content as it is no longer used
        PDPageReleasePDEContent(page, NULL);
        PDPageRelease(page);

        //Save document with the given output path, and proper saving flags
        outDoc.saveDoc(L"DocumentInfo.pdf", PDSaveFull | PDSaveLinearized);

        std::wcout << L"DocumentInfo.pdf saved with text to be placed." << std::endl << std::endl;    

    HANDLER

        errCode = ERRORCODE;

    libInit.displayError(errCode);                                                                    //If there was an error, display it.

    END_HANDLER

    return errCode;                                                                                   //Return program status.
}
