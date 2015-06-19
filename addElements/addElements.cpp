//# Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
//# Sample addElements/ Adds a series of elements onto a pdf
//
//# This agreement is between Datalogics, Inc. 101 N. Wacker Drive, Suite 1800,
//# Chicago, IL 60606 ("Datalogics") and you, an end user who downloads
//# source code examples for integrating to the Adobe PDF Library
//# ("the Example Code"). By accepting this agreement you agree to be bound
//# by the following terms of use for the Example Code.
//#
//# LICENSE
//# -------
//# Datalogics hereby grants you a royalty-free, non-exclusive license to
//# download and use the Example Code for any lawful purpose. There is no charge
//# for use of Example Code.
//#
//# OWNERSHIP
//# ---------
//# The Example Code and any related documentation and trademarks are and shall
//# remain the sole and exclusive property of Datalogics and are protected by
//# the laws of copyright in the U.S. and other countries.
//#
//# Datalogics is a trademark of Datalogics, Inc.
//#
//# TERM
//# ----
//# This license is effective until terminated. You may terminate it at any
//# other time by destroying the Example Code.
//#
//# WARRANTY DISCLAIMER
//# -------------------
//# THE EXAMPLE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
//# EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES
//# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//#
//# DATALOGICS DISCLAIM ALL OTHER WARRANTIES, CONDITIONS, UNDERTAKINGS OR
//# TERMS OF ANY KIND, EXPRESS OR IMPLIED, WRITTEN OR ORAL, BY OPERATION OF
//# LAW, ARISING BY STATUTE, COURSE OF DEALING, USAGE OF TRADE OR OTHERWISE,
//# INCLUDING, WARRANTIES OR CONDITIONS OF MERCHANTABILITY, FITNESS FOR A
//# PARTICULAR PURPOSE, SATISFACTORY QUALITY, LACK OF VIRUSES, TITLE,
//# NON-INFRINGEMENT, ACCURACY OR COMPLETENESS OF RESPONSES, RESULTS, AND/OR
//# LACK OF WORKMANLIKE EFFORT. THE PROVISIONS OF THIS SECTION SET FORTH
//# SUBLICENSEE'S SOLE REMEDY AND DATALOGICS'S SOLE LIABILITY WITH RESPECT
//# TO THE WARRANTY SET FORTH HEREIN. NO REPRESENTATION OR OTHER AFFIRMATION
//# OF FACT, INCLUDING STATEMENTS REGARDING PERFORMANCE OF THE EXAMPLE CODE,
//# WHICH IS NOT CONTAINED IN THIS AGREEMENT, SHALL BE BINDING ON DATALOGICS.
//# NEITHER DATALOGICS WARRANT AGAINST ANY BUG, ERROR, OMISSION, DEFECT,
//# DEFICIENCY, OR NONCONFORMITY IN ANY EXAMPLE CODE.


//
// Project: addElements - Modifying existing page by adding elements to it
//
// Note: By default, this example program opens a file called addElements.pdf in the
//      source directory.  It adds several different elements to the page and
//      saves it as out.pdf
//
// Steps:
//
//-Open an existing PDF document.
//-Add some PDE elements to the document.
//-Save the document as a new PDF (don't alter the original).
//-Close.



#include <iostream>
#undef LITTLE_ENDIAN 
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"
#include "MyPDFLibUtils.h"
#include "PSFCalls.h"
#include "ASCalls.h"
#include "ASExtraCalls.h"


PDEPath PathRect(ASFixed, ASFixed, ASFixed, ASFixed, int, int, int, int);


int main(int argc, char **argv)
{
    //Initialize the pdf library
    int initErr = MyPDFLInit(); 

    //Check for errors upon initialization
    if (initErr != 0)
    {
        std::cerr << "Initialization error. See \"AcroErr.h\" for more info.\n" << std::endl;
        std::cerr << "Error system: " << ErrGetSystem(initErr) << std::endl;
        std::cerr << "Error Severity: " << ErrGetSeverity(initErr) << std::endl;
        std::cerr << "Error Code: " << ErrGetCode(initErr) << std::endl;
        return 0;
    }

   
    //Initialize selected variables. If these variable
    //are left uninitialized, and an exception may occur early
    //in the execution of the placetext sample

    //The Elements that will appear on the page
    //*
    PDEText pdeText = NULL;                                 // Element to display text 
    PDEPath rect = NULL;                                    // Path element that will be formed into a rectangle 
    PDEFont verdanaFont = NULL;                             // Font element that represents a Embedded OpenType font
    //*
  
    wchar_t  pathToOrig[] = L"../Input/addElementsTo.pdf";  // WideString filename used by PDDocOpen() 
    PDDoc pdDocOrig = NULL;                                 // A PDF document object
    PDPage pdPage = NULL;                                   // A page in document object
    PDEContent pdeContent = NULL;                           // Container for page content 
    PDEFontAttrs attrs;                                     // Font attributes    
    ASDoubleMatrix textMatrix;                              // Transformation matrix for text 
    PDEGraphicState gState;                                 // Graphic state to apply to operation 
    PDEColorSpace pdeColorSpace = NULL;                     // Object holding the ColorSpace or color scale used 
    PDSysFont sysFont;                                      // System font object
    ASErrorCode errCode = 0;                                // Used to catch errors
    ASInt32 err = 0;                                        //
    ASText origPathText = NULL;                             // The input file path text object
    ASPathName origPathName = NULL;                         // The input file path name 
    ASText outPathText = NULL;                              // Path to save to
    ASPathName outPathName = NULL;                          // Pathname for saving
    wchar_t  pathToOut[] = L"out.pdf";                      // The text for the pathname
    ASUnicodeFormat uniFormat = NULL;                       // Format object to be used to hold the Unicode format for path access
    PDETextState tState;                                    //Text state for rendering

    //Text that will be displayed
    std::string textToDisplay = "Here is some text in the Verdana font, using both PDEtext and PDEFont. Below is a PDEPath rectangle ";

    DURING

        //Set up path and open input document and check see if it opened sucessfully 
        if (sizeof(wchar_t) == 2)
            uniFormat = kUTF16HostEndian;
        else
            uniFormat = kUTF32HostEndian;

        origPathText = ASTextFromUnicode((ASUTF16Val *)pathToOrig, uniFormat);

        origPathName = ASFileSysCreatePathFromDIPathText(NULL, origPathText, NULL);

        pdDocOrig = PDDocOpen(origPathName, NULL, NULL, true);

        //If the document did not exist, we exit the DURING block and enter HANDLER
        

        //=================================================================//
        //             Creating a PDEFont from a system font               //                     
        //=================================================================//
        
        memset(&attrs, 0, sizeof(attrs));
        
        //Set PDEFontAttrs name 
        attrs.name = ASAtomFromString("Verdana");
       
        //Set PDEFontAttrs type 
        attrs.type = ASAtomFromString("TrueType");
       
        //Get the corresponding sys font. 
        sysFont = PDFindSysFont(&attrs, sizeof(attrs), kPDSysFontMatchFontType);
        
        //Checking to see if the system font was retrieved. 
        if (sysFont)
        {
            //Get font embedding policy 
            PDSysFontGetAttrs(sysFont, &attrs, sizeof(PDEFontAttrs));

            //Check font embedding policy 
            if (attrs.cantEmbed != 0)
                std::cerr << "Font " << ASAtomGetString(attrs.name) << "can't be embedded";
            else
                //Fully embedded font 
                verdanaFont = PDEFontCreateFromSysFont(sysFont, kPDEFontCreateEmbedded);
        }

        //If sysFont is not valid exit program
        else E_RETURN(0);

        //=================================================================//
        // Setting up the default graphics state. This is done in order to //
        // free PDEColor objects                                           //
        //=================================================================//

        //Set the grahpics state to its default values 
        PDEDefaultGState(&gState, 0);

        //===============================================================================//
        //  Text matrix determines where the text will appear on the page and what size. //
        //===============================================================================//
        memset(&textMatrix, 0, sizeof(textMatrix));   // clear structure 
        textMatrix.a = 9.6;                           // set font width and height 
        textMatrix.d = 9.6;                           // to 10 point size       
        textMatrix.h = 72*1;                          // x
        textMatrix.v = 72*8;                          //,y coordinate on page starting from bottom left
                                                      // where 72 represents an inch

        //=================================================================//
        //               Add PDEText Elements into the document            //
        //=================================================================//

        //Create new text run 
        pdeText = PDETextCreate();

        //adding the text run to the PDE text object
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

        std::cout << "Created verdanaFont" << std::endl;  

                                                                //|---in inches--|
        //Call PathRect to transform PDEPath in a rectangle of xPos,yPos,height,width, lineWidth, r, g, b
        rect = PathRect(3 * ASInt32ToFixed(72), 4 * ASInt32ToFixed(72), ASInt32ToFixed(72 * 2), ASInt32ToFixed(72 * 2), 36, 0, 0, 1);
        std::cout << "Created PDEPath in the form of a rectangle" << std::endl;

        //Get the PDPage 
        pdPage = PDDocAcquirePage(pdDocOrig, 0);

        //Get content on the page 
        pdeContent = PDPageAcquirePDEContent(pdPage, 0);

        //Add the newly created PDEPath object to the page content 
        PDEContentAddElem(pdeContent, kPDEAfterLast, (PDEElement)rect);

        //Insert text into page content 
        PDEContentAddElem(pdeContent, kPDEAfterLast, reinterpret_cast<PDEElement> (pdeText));
        std::cout << "Added all Elements" << std::endl;

        //Set the PDEContent for the page 
        PDPageSetPDEContentCanRaise(pdPage, NULL);

        //Release the page and its contents
        PDPageReleasePDEContent(pdPage, NULL);
        PDPageRelease(pdPage);
        pdPage = NULL;

        //Determine the needed flags for embedding and call the appropriate routines for doing so
        PDEFontEmbedNow(verdanaFont, PDDocGetCosDoc(pdDocOrig));

        //=================================================================//
        //              Save Output and Release Used Objects               //
        //=================================================================//

        //Save document to a file with the unicode format determined prior
        outPathText = ASTextFromUnicode((ASUTF16Val *)pathToOut, uniFormat);

        outPathName = ASFileSysCreatePathFromDIPathText(NULL, outPathText, NULL);

        //Save file using the ASPathName
        PDDocSave(pdDocOrig, PDSaveFull | PDSaveLinearized, outPathName, ASGetDefaultFileSys(), NULL, NULL);

        ASFileSysReleasePath(NULL, outPathName);
    
    HANDLER
        err = ERRORCODE;
    END_HANDLER

    //Release used objects 

    

    if (outPathText) 
        ASTextDestroy(outPathText);
    if (outPathName)   
        ASFileSysReleasePath(NULL, outPathName);
    if (pdeText)
        PDERelease((PDEObject)pdeText);
    if (pdeColorSpace)
        PDERelease((PDEObject)pdeColorSpace);
    if (verdanaFont)
        PDERelease((PDEObject)verdanaFont);
    if (rect)
        PDERelease((PDEObject)rect);    
    if (pdDocOrig)
    {
        PDDocRelease(pdDocOrig);
        PDERelease(reinterpret_cast<PDEObject>(gState.strokeColorSpec.space));
        PDERelease(reinterpret_cast<PDEObject>(gState.fillColorSpec.space));
    }

    //If there was an error display 
    if (err)
    {
        std::cout << "erroo";
        DisplayError(errCode);
       
        if (pdDocOrig == NULL)
        {
            std::cerr << "Failed to open input file " << std::endl;
        }
    }

    MyPDFLTerm();
    return 0;
}


//Function that transforms PDEPath to rectangle of xPosition,yPosition,height,width, lineWidth, r, g, b
PDEPath PathRect(ASFixed  x, ASFixed  y, ASFixed  width, ASFixed  height, int  lineWidth, int  r, int  g, int  b)
{
    PDEPath path = PDEPathCreate();

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

    //Use complement colors for the fill operation 
    fillClrValue.color[0] = fixedOne - ASInt32ToFixed(r);
    fillClrValue.color[1] = fixedOne - ASInt32ToFixed(g);
    fillClrValue.color[2] = fixedOne - ASInt32ToFixed(b);

    //Use RGB color space 
    clrSpace = PDEColorSpaceCreateFromName(ASAtomFromString("DeviceRGB"));

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

    //Set graphics state to the Path
    PDEElementSetGState((PDEElement)path, &gState, sizeof (PDEGraphicState));

    //Array structure for pathData needed for a rectangle
    ASFixed  pathData[5];
    pathData[0] = kPDERect;
    pathData[1] = x;
    pathData[2] = y;
    pathData[3] = width;
    pathData[4] = height;

    //Assign the pathData to the path to form rectangle
    PDEPathSetData(path, pathData, sizeof (pathData));

    //Return the path shaped as a rectangle
    return  path;
}




