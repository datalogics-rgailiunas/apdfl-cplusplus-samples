// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//==================================================================
// Sample: AddText - Adds text to a newly created PDF document.
//       
// Steps: 
//  1) Create a document.
//  2) Create a PDEFont object.
//  3) Set the graphic state.
//  4) Create the PDEText object and add it to the PDEContent object.
//  5) Save the output document.
//===================================================================

#include <iostream>

#include "PSFCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"
#include "ASExtraCalls.h"

#include "APDFLDoc.h"
#include "InitializeLibrary.h"

int main(int argc, char** argv)
{

    APDFLib libInit;                      //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;              //Variable used to report any exceptions/errors if they occured.

    if (libInit.isValid() == false)       //If there was a problem in initialization, return the error code.
        return libInit.getInitError();    
    
    DURING

//============================================================================================================
// Step 1: Create a document. Insert and acquire a page from the document. PDEContent is then acquired from
// the page. A PDEText object will later be added to this content.
//============================================================================================================

        APDFLDoc doc;    //Create a new empty document.

        //Insert a 4 inch x 4 inch page into the document, 0 is the first in the document.
        doc.insertPage(Int16ToFixed((4 * 72)), Int16ToFixed((4 * 72)), PDBeforeFirstPage);
        
        //Get the first page from the document. Note: caller is responsible for releasing the PDPage acquired.
        PDPage page = doc.getPage(0);

        //Acquire PDEContent, PDE objects can be added to the acquired content.
        PDEContent content = PDPageAcquirePDEContent(page, NULL);

//============================================================================================================
// Step 2: Create a PDEFont object. The PDEFont object is created from a PDSysFont object which contains
// attributes such as font name and type.
//============================================================================================================

        PDEFontAttrs fontAttrs;                             //Struct that will contain font name and type.
 
        memset(&fontAttrs, 0, sizeof(fontAttrs));           //Ensure any "garbage" data is cleared out.

        fontAttrs.name = ASAtomFromString("CourierStd");    //Set the font name and type. 
        fontAttrs.type = ASAtomFromString("Type1");          

        //Locate the system font that corresponds to the PDEFontAttrs struct we just set.
        PDSysFont sysFont = PDFindSysFont(&fontAttrs, sizeof(fontAttrs), 0);

        //Create the CourierStd Type1 font with embed flag set.       
        PDEFont pdeFont = PDEFontCreateFromSysFont(sysFont, kPDEFontCreateEmbedded);

//============================================================================================================
// Step 3: Set the graphic state. The graphics state must be set for all PDE objects that will be displayed
// on a PDPage. This object is used to set information about colors, colorspace, linewidth, etc. In this sample
// we set the values to default because it is simple text placement.
//============================================================================================================

        PDEGraphicState gState;      
                                     
        PDEDefaultGState(&gState,0);                   //Set the graphics state to default values.

        ASDoubleMatrix textMatrix;                     //Struct that determines the size and location of text on page.

        memset(&textMatrix, 0, sizeof(textMatrix));    //Clear out any "garbage" the struct may contain.

        textMatrix.a = 12.0;                           //Character width.
        textMatrix.d = 12.0;                           //Character height.
        textMatrix.h = 1 * 72.0;                       //Place at a x-val of 1 inch from the left side of the page.
        textMatrix.v = 2 * 72.0;                       //Place at a y-val of 2 inches from the bottom of the page.
          
        char* placeStr = "This text was placed!";      //Text that will be added to the PDEText object.
        PDETextState tState;                           //Text state may be adjusted for character spacing, etc. Using default values in this sample.

//============================================================================================================
// Step 4: Create the PDEText object and add it to the PDEContent object. The information we set in steps 2
// and 3 are added to the PDEText and then the PDEText is added to the PDEContent.
//============================================================================================================

        PDEText textObj = PDETextCreate();    //PDEText will be set, and then added into the PDEContent.
                                           
        PDETextAddEx(textObj,                 //The PDEText object we just created.
            kPDETextRun,                      //kPDETextRun and kPDETextChar specify whether a string or character will be inserted.
            0,                                //The index after which to add the character or text run.
            (Uns8*)placeStr,                  //The string that will be added should be type-cast as a pointer to Uns8.
            strlen(placeStr),                 //Length of the string.
            pdeFont,                          //The PDEFont we created holding information such as font name, type and whether it's embedded or not.
            &gState,0,                        //PDEGraphicState and its size. Contains graphical attributes of the text object.
            &tState,0,                        //Text state and its size. Contains textual attributes of the text object.
            &textMatrix,                      //Matrix containing size and location for the text.
            NULL);                            //The matrix for the line width when stroking text.

        std::wcout << L"Text element created and set." << std::endl;

        PDEContentAddElem(content, kPDEAfterLast, reinterpret_cast<PDEElement>(textObj));    //Add the text element to the page's content.

        PDPageSetPDEContentCanRaise(page, NULL);                                             //Set the content back into the page.

//============================================================================================================
// Step 5: Save the output document. Release any resources that are still in use, save the document and exit
// the program.
//============================================================================================================

        //Release objects that are no longer in use.
        PDERelease(reinterpret_cast<PDEObject>(gState.strokeColorSpec.space));
        PDERelease(reinterpret_cast<PDEObject>(gState.fillColorSpec.space));
        PDERelease(reinterpret_cast<PDEObject>(pdeFont));
        PDERelease(reinterpret_cast<PDEObject>(textObj));
        PDPageReleasePDEContent(page, NULL);
        PDPageRelease(page);
    
        doc.saveDoc(L"textPlaced.pdf", PDSaveFull | PDSaveLinearized);    //Save the PDF document in the working directory.

        std::wcout << L"textPlaced.pdf saved with text to be placed." << std::endl << std::endl;

    HANDLER

            errCode = ERRORCODE;           

            libInit.displayError(errCode);    //If there was an error, display the error that occured.

    END_HANDLER

    return errCode;                           //APDFLib's destructor terminates the library.

}
