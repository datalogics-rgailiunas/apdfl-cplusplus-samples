// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
//===============================================================
// Sample: PlaceText, places text on a newly created pdf document
//       
// Steps: 
//  1) Create new pdf document with core attributes
//  2) Setup and add text
//  3) Save result as textPlaced.pdf     
//===============================================================
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

#include "PSFCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"
#include "ASExtraCalls.h"
#include "APDFLDoc.h"
#include "InitializeLibrary.h"
#include <iostream>

int main(int argc, char** argv)
{

    APDFLib libInit;            //Initialize the APDFL.
    ASErrorCode errCode = 0;    //Variable that represents errors

    if (libInit.isValid() == false)         //Check for errors in initialization.
        errCode = libInit.getInitError();   //If there was an error set the code.
    
    DURING

//============================================================================================================
// Step 1) Create new pdf document with core attributes
//============================================================================================================

        //Create a new document
        APDFLDoc doc;

        //Set up the 4" by 4" bounds for the page
        doc.insertPage((4 * 72), (4 * 72), PDBeforeFirstPage);
        
        //Initialize page from source document, where to place, and bound rectangle
        PDPage page = doc.getPageNumber(0);

        //Grab contenet from the page
        PDEContent content = PDPageAcquirePDEContent(page, NULL);

//============================================================================================================
// Step 2) Setup up and add text
//============================================================================================================

        PDEFontAttrs fontAttrs;    //Font attributes object

        //Initialize font Attributes
        memset(&fontAttrs, 0, sizeof(fontAttrs));

        fontAttrs.name = ASAtomFromString("CourierStd");    //Font style
        fontAttrs.type = ASAtomFromString("Type1");         //Font type

        //Create system font using font attributes, its size, and flags
        PDSysFont sysFont = PDFindSysFont(&fontAttrs, sizeof(fontAttrs), 0);

        //Create the pdeFont with the sysFont and PDEFontCreateFlags      
        PDEFont pdeFont = PDEFontCreateFromSysFont(sysFont, kPDEFontCreateEmbedded);

        PDEGraphicState gState;    //State of graphics used for rendering

        //Set the grahpics state to its default values 
        PDEDefaultGState(&gState,0);

        ASDoubleMatrix textMatrix;    //Matrix for sizing and setting location of text

        //Setting up the matrix for setting text size and placement location
        memset(&textMatrix, 0, sizeof(textMatrix));

        textMatrix.a = 12;          //Character width (matrix element size)
        textMatrix.d = 12;          //Character width (matrix element size)
        textMatrix.h = 1 * 72.0;    //Place at a x-val of 1 inch
        textMatrix.v = 2 * 72.0;    //Place at a y-val of 2 inches
          
        PDEText textObj = PDETextCreate();    //Create object for holding text 

        char* placeStr = "This text was placed!";    //String for text object
        PDETextState tState;                         //State of text used for rendering

        //Adding the features of text run to the PDE text object
        PDETextAddEx(textObj,    //The text object
            kPDETextRun,         //kPDETextRun/kPDETextChar
            0,                   //The index after where to add the character or text run
            (Uns8*)placeStr,     //String converter to Unsigned 8-bit form
            strlen(placeStr),    //String length
            pdeFont,             //The used font
            &gState,0,           //PDEGraphicState and its size. Holds graphical attributes of the text object
            &tState,0,           //Text state and its size, Holds textual attributes of the text object
            &textMatrix,         //Matrix for the text object
            NULL);               //the matrix for the line width when stroking text

        std::wcout << L"Text element created and set." << std::endl;

        //Add the text element to the page's content
        PDEContentAddElem(content, kPDEAfterLast, reinterpret_cast<PDEElement>(textObj));

        //Set the content back into the page
        PDPageSetPDEContentCanRaise(page, NULL);

        //Release objects no longer in use
        PDERelease(reinterpret_cast<PDEObject>(gState.strokeColorSpec.space));
        PDERelease(reinterpret_cast<PDEObject>(gState.fillColorSpec.space));
        PDERelease(reinterpret_cast<PDEObject>(pdeFont));
        PDERelease(reinterpret_cast<PDEObject>(textObj));
        PDPageReleasePDEContent(page, NULL);
        PDPageRelease(page);
    
//============================================================================================================
// Step 3) Save result as textPlaced.pdf 
//============================================================================================================
     
        //Save document with the given output path, and proper saving flags
        doc.saveDoc(L"textPlaced.pdf", PDSaveFull | PDSaveLinearized);

        std::wcout << L"textPlaced.pdf saved with text to be placed." << std::endl << std::endl;

    HANDLER

            //If an exception was raised generate error code
            errCode = ERRORCODE;           

            //If there was an error code, display
            libInit.displayError(errCode);

    END_HANDLER

    return errCode;

}
