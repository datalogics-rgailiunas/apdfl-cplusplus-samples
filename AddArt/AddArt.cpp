// Copyright(c) 2015, Datalogics, Inc.All rights reserved.

//==============================================================================
// Sample: AddArt - This sample demonstrates drawing an object by manipulating
// PDEPath objects.
//	
// Steps:
// 1) Create a PDF document with one page.
// 2) Set the graphics state for the first shape.
// 3) Draw the arrow.
// 4) Draw a second arrow and apply transformations.
// 5) Release resources and save the PDF document.
//==============================================================================

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

#include <iostream>
#include "PagePDECntCalls.h"
#include "PEWCalls.h"
#include "PERCalls.h"
#include "ASCalls.h"
#include "ASExtraCalls.h"
#include "InitializeLibrary.h"
#include "APDFLDoc.h"

int main(int argc, char** argv)
{

    APDFLib libInit;                   //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;           //Variable used to report any exceptions/errors if they occured.

    if (libInit.isValid() == false)    //If there was a problem in initialization, return the error code.
        return libInit.getInitError();

//=============================================================================================================================================================
//Step 1: Create a PDF document with one page. After the document is created a page and a list of the page's content is acquired.
//=============================================================================================================================================================

    DURING

        wchar_t* pathName = L"AddArtOut.pdf";                          //This variable holds the output file's pathname.

        APDFLDoc document;                                             //Call APDFLDoc's default constructor to create a new PDF document.

        //Insert a 10 x 10 inch page into the PDF document.
        document.insertPage(Int32ToFixed((72 * 10)), Int32ToFixed((72 * 10)), PDBeforeFirstPage);    
    
        PDPage pdPage = document.getPage(0);                           //Retrieve the page that was just created.

        PDEContent pdeContent = PDPageAcquirePDEContent(pdPage, 0);    //Acquire the content list from the PDPage.

        PDEPath pdePath = PDEPathCreate();                             //Create path object, which will be made into a shape.

//=============================================================================================================================================================
// Step 2: Set the graphics state for the first shape. This PDEGraphicsState object contains attributes about how a PDEElement will be displayed.
//============================================================================================================================================================= 

        PDEPathSetPaintOp(pdePath, kPDEStroke);                 //PDEPath will be stroked, but not filled.

        PDEGraphicState gState;                                 //Struct that will hold display attributes.
        memset(&gState, 0, sizeof(gState));                     //Clear out any "garbage data" in the struct.

        ASFixed red = ASFloatToFixed(.6);                       //Red Intensity ranges is [0.0 - 1.0].                             
        ASFixed green = ASFloatToFixed(.2);                     //Green Intensity ranges is [0.0 - 1.0].
        ASFixed blue = ASFloatToFixed(1.0);                     //Blue Intensity ranges is [0.0 - 1.0].

        PDEColorValue strokeColorValue;                         //Will be assigned to PDEGraphicState.strokeColorSpec.

        memset(&strokeColorValue, 0, sizeof(PDEColorValue));    //Initially value = 0 (Black.)
        strokeColorValue.color[0] = red;                        //Assign the color values we just set to the struct.
        strokeColorValue.color[1] = green;                      //In this case the object will be painted purple.
        strokeColorValue.color[2] = blue;

        //We are using the RGB color space in this case. Default value is "DeviceGray".
        PDEColorSpace colorSpace = PDEColorSpaceCreateFromName(ASAtomFromString("DeviceRGB"));   

        //Set the PDEColorSpec struct with the colorSpace and strokeColorValues we just set.
        PDEColorSpec strokeColorSpec = {colorSpace, strokeColorValue};    

        gState.strokeColorSpec = strokeColorSpec;               //Set to "DeviceRBG" and purple.
        gState.lineWidth = Int32ToFixed(3);                     //Line width is set to a thickness of 3.
        gState.lineCap = 1;                                     //Using rounded line caps.
        gState.lineJoin = 1;                                    //Using rounded line joins.
        gState.miterLimit = fixedTen;                           //Set this to it's default value.
        gState.flatness = fixedZero;                            //Set this to it's default value.

        //Set the PDEPath's graphic state with the structure that was just filled out.
        PDEElementSetGState(reinterpret_cast<PDEElement>(pdePath), &gState, sizeof(PDEGraphicState));

//=============================================================================================================================================================
//Step 3: Draw the arrow. This shape is drawn using a list of coordinates and calling PDEPathAddSegment to join the (x, y) coordinates with straight lines.
//=============================================================================================================================================================

        const short numberOfPoints = 7;    //Number of points required to draw an arrow

        //Arrays containing (x, y) coordinates representing points for the arrow that is being drawn. (72 pixels in one inch.)
        ASInt32 arrowXCoordinates[numberOfPoints] {(72 * 7), (72 * 6), (72 * 6), (72 * 4), (72 * 4), (72 * 3), (72 * 5)};
        ASInt32 arrowYCoordinates[numberOfPoints] {(72 * 7), (72 * 7), (72 * 3), (72 * 3), (72 * 7), (72 * 7), (72 * 9)};
        ASFixedPoint pointToInsert; 

        //Move to start point at the tip of the arrow before drawing lines.
        pointToInsert.h = ASInt32ToFixed(72 * 5);
        pointToInsert.v = ASInt32ToFixed(72 * 9);
        PDEPathAddSegment(pdePath, kPDEMoveTo, pointToInsert.h, pointToInsert.v, fixedZero, fixedZero, fixedZero, fixedZero);

        //Loop through the coordinates and draw lines connecting each (x, y) pair.
        for (unsigned i = 0; i < numberOfPoints; ++i)
        {
            pointToInsert.h = ASInt32ToFixed(arrowXCoordinates[i]);
            pointToInsert.v = ASInt32ToFixed(arrowYCoordinates[i]);

            PDEPathAddSegment(pdePath, kPDELineTo, pointToInsert.h, pointToInsert.v, fixedZero, fixedZero, fixedZero, fixedZero);
        }

        //Insert the PDEPath into the PDEContent that was acquired.
        PDEContentAddElem(pdeContent, kPDEAfterLast, reinterpret_cast<PDEElement>(pdePath));

        PDPageNotifyContentsDidChange(pdPage);               //Notify the page that the PDEContent has been changed.

        PDPageSetPDEContentCanRaise(pdPage, 0);              //Set the PDEContent back into the PDPage's Cos object.

        PDERelease(reinterpret_cast<PDEObject>(pdePath));    //Release the path object.

//=============================================================================================================================================================
// Step 4: Draw a second arrow and apply transformations. Create a new PDEPath object and apply some transformations to it using a transformation matrix. We 
// will add a fill color of yellow to this object to differentiate between the objects.
// Note: See Section 4.1 (Page 205) of the PDF reference for more detailed information on translations/rotations/scaling and skews.
//=============================================================================================================================================================

        PDEPath pdePath2 = PDEPathCreate();                       //Second path object that will be drawn to the page.

        PDEPathSetPaintOp(pdePath2, kPDEStroke | kPDEFill);       //This arrow will be stroked and filled with color.

        red = ASFloatToFixed(1.0);                                //Fill color is yellow.
        blue = ASFloatToFixed(0.0);
        green = ASFloatToFixed(1.0);
    
        //Clear out PDEColorValue struct and fill using the RBG values we just set.
        PDEColorValue fillColorValue;                               
        memset(&fillColorValue, 0, sizeof(PDEColorValue));
        fillColorValue.color[0] = red;
        fillColorValue.color[1] = green;
        fillColorValue.color[2] = blue;
        
        PDEColorSpec fillColorSpec{ colorSpace, fillColorValue };   //Fill color will also use the RGB colorspace.

        gState.fillColorSpec = fillColorSpec;                       //Graphics state now contains a fill color.

        PDEElementSetGState(reinterpret_cast<PDEElement>(pdePath2), &gState, sizeof(gState));

        //Redraw the exact same arrow we created in step 3.
        pointToInsert.h = ASInt32ToFixed(72 * 5);
        pointToInsert.v = ASInt32ToFixed(72 * 9);
        PDEPathAddSegment(pdePath2, kPDEMoveTo, pointToInsert.h, pointToInsert.v, fixedZero, fixedZero, fixedZero, fixedZero);

        for (unsigned i = 0; i < numberOfPoints; ++i)
        {
            pointToInsert.h = ASInt32ToFixed(arrowXCoordinates[i]);
            pointToInsert.v = ASInt32ToFixed(arrowYCoordinates[i]);

            PDEPathAddSegment(pdePath2, kPDELineTo, pointToInsert.h, pointToInsert.v, fixedZero, fixedZero, fixedZero, fixedZero);
        }

        ASDoubleMatrix translationMatrix{ 1, 0.0, 0.0, 1, (72.0 * 4.0), (-72.0 * 2.0) };        //Move the arrow 4 inches right and 2 inches down.
        ASDoubleMatrix scalingMatrix{ 0.75, 0.0, 0.0, 0.75, 0.0, 0.0 };                         //Scale it to 75% of its original size.
        ASDoubleMatrix translateAndScale;                                                       //Holds result of multiplying the last two matrices.

        //IMPORTANT NOTE! If applying multiple transformations to a matrix you must concatenate the matrices before applying transformations. 
        //DO NOT attempt to call PDEElementSetMatrixEx() more than one time on the same PDEElement.
        ASDoubleMatrixConcat(&translateAndScale, &translationMatrix, &scalingMatrix);      

        PDEElementSetMatrixEx(reinterpret_cast<PDEElement>(pdePath2), &translateAndScale);       //Apply the transformation matrix.

        PDEContentAddElem(pdeContent, kPDEAfterLast, reinterpret_cast<PDEElement>(pdePath2));    //Insert the PDEPath into the PDEContent that was acquired.

        PDPageNotifyContentsDidChange(pdPage);                                                   //Notify the page that the PDEContent has been changed.                                                 

        PDPageSetPDEContentCanRaise(pdPage, 0);                                                  //Set the PDEContent back into the PDPage's Cos object.

//=============================================================================================================================================================
//Step 5) Release resources and save the PDF document.
//=============================================================================================================================================================

        //Release the remaining resources.
        PDERelease(reinterpret_cast<PDEObject>(colorSpace));
        PDERelease(reinterpret_cast<PDEObject>(pdePath2));
        PDPageReleasePDEContent(pdPage, 0);
        PDPageRelease(pdPage);

        document.saveDoc(pathName, PDSaveFull | PDSaveLinearized);    //Save the document

    HANDLER

        errCode = ERRORCODE;
            
        libInit.displayError(errCode);                                //If there was an error, display the error that occured.

    END_HANDLER

    return errCode;                                                   //APDFLib's destructor terminates the library.
}
