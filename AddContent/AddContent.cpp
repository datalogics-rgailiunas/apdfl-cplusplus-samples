// Copyright (c) 2015, Datalogics, Inc. All rights reserved.

//====================================================================================
// Sample: AddContent - This samples opens a file called AddContent.pdf in the input 
//                      directory. It adds several different elements to the page and 
//                      saves it as AddedContent.pdf.
//      
// Steps:
//  1) Set up the font for the text to be displayed.    
//  2) Set up the content to be added to the document.
//  3) Acquire PDEContent and add elements to the page.       
//====================================================================================

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
   
    APDFLib libInit;                                             //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;                                     //Variable used to report any exceptions/errors if they occured. 

    if (libInit.isValid() == false)                              //Check for errors upon initialization.
        return libInit.getInitError();                           //If it failed, return the error code.

    DURING

        APDFLDoc document(L"../_Input/AddContent.pdf", true);    //Open input document with path and repair if damaged.

//================================================================================================================================================
// Step 1) Set up the font for the text to be displayed.                            
//================================================================================================================================================

        PDEFontAttrs attrs;                                                                   //Structure holding the attributes of a PDEFont.
        PDEFont verdanaFont = NULL;                                                           //Font element that will represent the font Verdana.
       
        memset(&attrs, 0, sizeof(attrs));                                                     //Clear out PDEFontAttrs struct.                                                                     
        attrs.name = ASAtomFromString("Verdana");                                             //Set font attribute's name.                                                          
        attrs.type = ASAtomFromString("TrueType");                                            //Set font attribute's type.                                         
       
        PDSysFont sysFont = PDFindSysFont(&attrs, sizeof(attrs), kPDSysFontMatchFontType);    //Get the corresponding system font.
          
        PDSysFontGetAttrs(sysFont, &attrs, sizeof(PDEFontAttrs));                             //Get font embedding policy. 

                                                                                              //Check if font is embeddable. 
        if (attrs.cantEmbed != 0)
            std::wcerr << L"Font " << ASAtomGetString(attrs.name) << L" can not be embedded";
        else
            verdanaFont = PDEFontCreateFromSysFont(sysFont, kPDEFontCreateEmbedded);          //Create font from the system font and embed.

        std::wcout << L"Created verdanaFont. " << std::endl;

        //Determine the needed flags for embedding and call the appropriate routines for doing so.
        PDEFontEmbedNow(verdanaFont, PDDocGetCosDoc(document.pdDoc));

        
//================================================================================================================================================
// Step 2) Set up the content to be added to the document. Elements are added to the page at X and Y values starting from the bottom left corner.
//================================================================================================================================================
        
        //Text that will be displayed on page
        std::string textToDisplay = "Here is some text in the Verdana font, using both PDEText and PDEFont. Below is a PDEPath rectangle. ";

        PDEGraphicState gState;                         //Structure holding the graphic attributes of the text object.

        PDEDefaultGState(&gState, 0);                   //Set the graphics state to its default values since the text is being displayed plainly.
        
        PDETextState tState;                            //Structure holding the attributes of a PDEText.

        ASDoubleMatrix textMatrix;                      //Transformation matrix for text which determines location of the text on page.

        memset(&textMatrix, 0, sizeof(textMatrix));     //Clear structure. 
        textMatrix.a = 10;                              //Set font width and height. 
        textMatrix.d = 10;                              //Set font point size.     
        textMatrix.h = 72 * 0.6;                        //x coordinate on page (72 pixels = 1 inch).
        textMatrix.v = 72 * 8;                          //y coordinate on page.         

        PDEText pdeText = PDETextCreate();              //Create a new text run.

        //Adding the text run to the PDE text object
        PDETextAddEx(pdeText,                           //Text container to add to. 
            kPDETextRun,                                //kPDETextRun or kPDETextChar for text runs or text characters. 
            0,                                          //The index after which to add the text run.
            (Uns8 *)textToDisplay.c_str(),              //Text to add.    
            textToDisplay.length(),                     //Length of text 
            verdanaFont,                                //Font to apply to text. 
            &gState, 0,                                 //Graphic state and its size.  
            &tState, 0,                                 //Text state and its size.
            &textMatrix,                                //Transformation matrix for text.  
            NULL);                                      //Stroke matrix for the line width when stroking text.  

        std::wcout << L"Created text object for display. " << std::endl;

        //Call to PathRect() function to design a blue rectangle, passing in xPosition, yPosition, width, height, lineWidth, RGB color values.
        PDEPath rect = PathRect(ASFloatToFixed(72 * 3.25), ASInt32ToFixed(72 * 4), ASInt32ToFixed(72 * 2), ASInt32ToFixed(72 * 2), 46, 0, 0, 1);

        std::wcout << L"Created PDEPath in the form of a rectangle. " << std::endl;

//================================================================================================================================================
//Step 3) Acquire PDEContent and add elements to the page.           
//================================================================================================================================================

        PDPage pdPage = PDDocAcquirePage(document.pdDoc, 0);                      //Get the page.

        PDEContent pdeContent = PDPageAcquirePDEContent(pdPage, 0);               //Get the page's content.
    
                                                                                  //Add the rectangle to the page content. 
        PDEContentAddElem(pdeContent, kPDEAfterLast, (PDEElement)rect);                          
   
                                                                                  //Add the text into page content.
        PDEContentAddElem(pdeContent, kPDEAfterLast, (PDEElement)pdeText);

        std::wcout << L"Added all Elements" << std::endl;
  
                                                                                 
        PDPageSetPDEContentCanRaise(pdPage, NULL);                                //Set the content ack into the page              

                                                                                  
        document.saveDoc(L"AddedContent.pdf", PDSaveFull | PDSaveLinearized);     //Save the document with proper save flags.

        //Release all objects
        PDPageReleasePDEContent(pdPage, NULL);
        PDPageRelease(pdPage);
        PDERelease((PDEObject)pdeText);
        PDERelease((PDEObject)rect);
        PDERelease((PDEObject)verdanaFont);
        PDERelease(reinterpret_cast<PDEObject>(gState.strokeColorSpec.space));    
        PDERelease(reinterpret_cast<PDEObject>(gState.fillColorSpec.space));
    
    HANDLER
        
        errCode = ERRORCODE;   

        libInit.displayError(errCode);                                            //If there was an error, display it.
        
    END_HANDLER

                                                                                  //Return program status.
    return errCode;         

}

//================================================================================================================================================
// PDEPath Function: Transforms PDEPath to rectangle of xPosition, yPosition, width, height, lineWidth, r, g, b (RGB color values).
//================================================================================================================================================
PDEPath PathRect(ASFixed  x, ASFixed  y, ASFixed  width, ASFixed  height, int  lineWidth, int  r, int  g, int  b)
{

    //Create the PDEPath object that will be used to draw a rectangle.
    PDEPath rectangle = PDEPathCreate();                                        
                                                                       
    PDEPathSetPaintOp(rectangle, kPDEStroke);                                           //Set the paint operation to Stroke for the path.

    PDEGraphicState gState;                                                             //Holds the vewiable attributes of the rectangle.
    PDEColorSpec strokeClrSpec;                                                         //Structure describing stroke specifications.
    PDEColorSpace clrSpace;                                                             //The used color scheme.
    PDEColorValue strokeClrValue;                                                       //Structure describing stroke color values

    memset(&strokeClrValue, 0, sizeof (PDEColorValue));

    //PDEColorValue color components for the RGB color space.
    strokeClrValue.color[0] = ASInt32ToFixed(r);
    strokeClrValue.color[1] = ASInt32ToFixed(g);
    strokeClrValue.color[2] = ASInt32ToFixed(b);

    //Use RGB color space
    clrSpace = PDEColorSpaceCreateFromName(ASAtomFromString("DeviceRGB")); 

    //Assign fill/stroke values to the appropriate PDEColorSpec.
    strokeClrSpec.space  = clrSpace;
    strokeClrSpec.value = strokeClrValue;

    //Set up graphics state along with color specs so that the rectangle displays with the specified color and line width.
    memset(&gState, 0, sizeof (PDEGraphicState));
    gState.strokeColorSpec = strokeClrSpec;
    gState.lineWidth = ASInt32ToFixed(lineWidth);
    gState.miterLimit = fixedTen;
    gState.flatness = fixedOne;

    //Set graphics state to the path to give it the color and size of the rectange.
    PDEElementSetGState((PDEElement)rectangle, &gState, sizeof (PDEGraphicState));

    //Array structure for pathData needed for a rectangle.
    ASFixed  pathData[5];
    pathData[0] = kPDERect;
    pathData[1] = x;
    pathData[2] = y;
    pathData[3] = width;
    pathData[4] = height;
 
                                                                                      
    PDEPathSetData(rectangle, pathData, sizeof (pathData));                             //Assign the pathData to the path to form rectangle.
                                                                                       
    PDERelease((PDEObject)clrSpace);                                                    //Released objects.
                                                                                     
    return rectangle;                                                                   //Return the rectangle.
}
