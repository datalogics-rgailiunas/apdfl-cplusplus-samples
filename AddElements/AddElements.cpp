//# Copyright (c) 2015, Datalogics, Inc. All rights reserved.

//====================================================================================
// Project: addElements - Modifying existing page by adding elements to it
//
// Note: By default, this example program opens a file called addElements.pdf in the
//      source directory.  It adds several different elements to the page and
//      saves it as out.pdf
//
//Steps:
// Step 1) Create the system font object. It will later be applied to a PDEText object.
// Step 2) Set the graphics state to default values and the location of the text on the page.
// Step 3) Create the PDEText and PDEPath objects that will be added to the page.
// Step 4) Acquire PDEContent and add elements to the page.      
//====================================================================================

// Sample addElements/ Adds a series of elements onto a pdf
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

#include <iostream>
#undef LITTLE_ENDIAN 
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"
#include "MyPDFLibUtils.h"
#include "PSFCalls.h"
#include "ASCalls.h"
#include "ASExtraCalls.h"
#include "InitializeLibrary.h"
#include "APDFLDoc.h"

//Function used to create a rectangle, parameter description in function definition.
PDEPath PathRect(ASFixed, ASFixed, ASFixed, ASFixed, int, int, int, int);

int main(int argc, char** argv)
{
   
    APDFLib libInit;                        //Initialize the APDFL.
    ASErrorCode errCode = 0;                //Error code initially is 0.

    if (libInit.isValid() == false)         //Check for errors in initialization.
        return libInit.getInitError();   //If there was an error set the code.

    DURING

        wchar_t  pathToOrig[] = L"../Input/addElementsTo.pdf";  // WideString filename used by PDDocOpen() 
        wchar_t  pathToOut[] = L"out.pdf";                      // The text for the pathname
        ASUnicodeFormat uniFormat = NULL;                       // Format object to be used to hold the Unicode format for path access

        APDFLDoc document(pathToOrig, true);                    //Open a document and repair if damaged

//================================================================================================================================================
// Step 1) Create the system font object. It will later be applied to a PDEText object.                                
//================================================================================================================================================

        //Text that will be displayed
        std::string textToDisplay = "Here is some text in the Verdana font, using both PDEText and PDEFont. Below is a PDEPath rectangle. ";

        PDEFontAttrs attrs;         //Font attributes

        PDETextState tState;        //Text state for rendering

        PDEFont verdanaFont = NULL; //Font element that represents a Embedded OpenType font

        memset(&attrs, 0, sizeof(attrs));           //Clear out PDEFontAttrs struct
           
        attrs.name = ASAtomFromString("Verdana");   //Set PDEFontAttrs name
       
        attrs.type = ASAtomFromString("TrueType");  //Set PDEFontAttrs type
       
        PDSysFont sysFont = PDFindSysFont(&attrs, sizeof(attrs), kPDSysFontMatchFontType); //Get the corresponding sys font.
          
        PDSysFontGetAttrs(sysFont, &attrs, sizeof(PDEFontAttrs)); //Get font embedding policy 

        //Check font embedding policy 
        if (attrs.cantEmbed != 0)
            std::wcerr << L"Font " << ASAtomGetString(attrs.name) << L"can't be embedded";
        else
            verdanaFont = PDEFontCreateFromSysFont(sysFont, kPDEFontCreateEmbedded); //Fully embedded font

//================================================================================================================================================
// Step 2) Set the graphics state to default values and the location of the text on the page.
//================================================================================================================================================
     
        PDEGraphicState gState;                      //Graphic state to apply to operation

        PDEDefaultGState(&gState, 0);                //Set the grahpics state to its default values

        ASDoubleMatrix textMatrix;                   //Transformation matrix for text

        memset(&textMatrix, 0, sizeof(textMatrix));  //clear structure 
        textMatrix.a = 10;                           //Set font width and height 
        textMatrix.d = 10;                           //to 10 point size       
        textMatrix.h = 72*1;                         //x coordinate on page starting from bottom (72 pixels = 1 inch)
        textMatrix.v = 72*8;                         //y coordinate on page starting from bottom (72 pixels = 1 inch)

//================================================================================================================================================
//Step 3) Create the PDEText and PDEPath objects that will be added to the page.           
//================================================================================================================================================

        PDEText pdeText = PDETextCreate();  //Create new text run

                                            //Adding the text run to the PDE text object
        PDETextAddEx(pdeText,               //Text container to add to  
            kPDETextRun,                    //kPDETextRun, kPDETextChar 
            0,                              //Index 
            (Uns8 *)textToDisplay.c_str(),  //Text to add    
            textToDisplay.length(),         //Length of text 
            verdanaFont,                    //Font to apply to text 
            &gState, 0,                     //Graphic state to apply to text  
            &tState, 0,                     //Text state and size of structure
            &textMatrix,                    //Transformation matrix for text  
            NULL);                          //Stroke matrix  

        std::wcout << L"Created verdanaFont. " << std::endl;

        PDEPath rect = PathRect(3 * ASInt32ToFixed(72), 4 * ASInt32ToFixed(72), ASInt32ToFixed(72 * 2), ASInt32ToFixed(72 * 2), 36, 0, 0, 1);

        std::wcout << L"Created PDEPath in the form of a rectangle. " << std::endl;

//================================================================================================================================================
//Step 4) Acquire PDEContent and add elements to the page.           
//================================================================================================================================================

        PDPage pdPage = PDDocAcquirePage(document.pdDoc, 0);             //Get the PDPage

        PDEContent pdeContent = PDPageAcquirePDEContent(pdPage, 0);      //Get content on the page
    
        PDEContentAddElem(pdeContent, kPDEAfterLast, (PDEElement)rect);  //Add the newly created PDEPath object to the page content 
  
        PDEContentAddElem(pdeContent, kPDEAfterLast, reinterpret_cast<PDEElement> (pdeText)); //Insert text into page content

        std::wcout << L"Added all Elements" << std::endl;
  
        PDPageSetPDEContentCanRaise(pdPage, NULL); //Set the PDEContent for the page

        //Release the page, content and elements.
        PDPageReleasePDEContent(pdPage, NULL);
        PDPageRelease(pdPage);
        PDERelease((PDEObject)pdeText);
        PDERelease((PDEObject)rect);
        
        //Determine the needed flags for embedding and call the appropriate routines for doing so
        PDEFontEmbedNow(verdanaFont, PDDocGetCosDoc(document.pdDoc));

        PDERelease((PDEObject)verdanaFont);
   
        document.saveDoc(pathToOut, PDSaveFull | PDSaveLinearized); //Save the document 

        //Release all remaining objects
        PDERelease(reinterpret_cast<PDEObject>(gState.strokeColorSpec.space));
        PDERelease(reinterpret_cast<PDEObject>(gState.fillColorSpec.space));
    
    HANDLER
        
        errCode = libInit.getInitError();   //If there was an error set the code.

        libInit.displayError(errCode);      //Display the error code

    END_HANDLER

    return errCode;           //Return program status
}

//================================================================================================================================================
// Function that transforms PDEPath to rectangle of xPosition, yPosition, width, height, lineWidth, r, g, b (RGB color values)
//================================================================================================================================================

PDEPath PathRect(ASFixed  x, ASFixed  y, ASFixed  width, ASFixed  height, int  lineWidth, int  r, int  g, int  b)
{
    PDEPath path = PDEPathCreate();                 //Create the PDEPath object that will be used to draw a rectangle.

    PDEPathSetPaintOp(path, kPDEStroke);            //Set the fill and stroke attribute

    PDEGraphicState  gState;                        //Graphics state
    PDEColorSpec  strokeClrSpec, fillClrSpec;       //Structure describing color specification, space and value 
    PDEColorSpace  clrSpace;                        //Color scheme
    PDEColorValue  strokeClrValue, fillClrValue;    //A structure describing a color value.

    memset(&strokeClrValue, 0, sizeof (PDEColorValue));
    memset(&fillClrValue, 0, sizeof (PDEColorValue));

    //PDEColorValue color component. For example, a Gray color space has one component, 
    //a RGB color space has three components, a CMYK has four components
    strokeClrValue.color[0] = ASInt32ToFixed(r);
    strokeClrValue.color[1] = ASInt32ToFixed(g);
    strokeClrValue.color[2] = ASInt32ToFixed(b);

    //Set the color for the fill operation. 
    fillClrValue.color[0] = fixedOne - ASInt32ToFixed(r);
    fillClrValue.color[1] = fixedOne - ASInt32ToFixed(g);
    fillClrValue.color[2] = fixedOne - ASInt32ToFixed(b);
 
    clrSpace = PDEColorSpaceCreateFromName(ASAtomFromString("DeviceRGB")); //Use RGB color space

    //Assign fill/stroke values to the appropriate PDEColorSpec
    strokeClrSpec.space = fillClrSpec.space = clrSpace;
    strokeClrSpec.value = strokeClrValue;
    fillClrSpec.value = fillClrValue;

    //Set up graphics state along with color specs
    memset(&gState, 0, sizeof (PDEGraphicState));
    gState.fillColorSpec = fillClrSpec;
    gState.strokeColorSpec = strokeClrSpec;
    gState.lineWidth = ASInt32ToFixed(lineWidth + 10);
    gState.miterLimit = fixedTen;
    gState.flatness = fixedOne;

    PDEElementSetGState((PDEElement)path, &gState, sizeof (PDEGraphicState)); //Set graphics state to the Path

    //Array structure for pathData needed for a rectangle
    ASFixed  pathData[5];
    pathData[0] = kPDERect;
    pathData[1] = x;
    pathData[2] = y;
    pathData[3] = width;
    pathData[4] = height;
 
    PDEPathSetData(path, pathData, sizeof (pathData)); //Assign the pathData to the path to form rectangle
  
    PDERelease((PDEObject)clrSpace); //Released objects no longer in need
   
    return  path;                    //Return the path shaped as a rectangle
}
