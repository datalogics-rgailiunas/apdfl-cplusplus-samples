

//# Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//# Sample placeText/ Places text onto a pdf
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
//
// Project: placeText
//
// Note: By default, this example program create a pdf file 
//       and adds text to the page in CourierSTD font. Then this 
//       file is saved as out.pdf in the current directory
//       
//
// Steps: 
//
// * Create new pdf
// * Setup up and add text
// * Save result as out.pdf         


#include "PSFCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"
#include "MyPDFLibUtils.h"
#include "ASCalls.h"
#include "ASExtraCalls.h"
#include <iostream>

int main(int argc, char **argv)
{

    int initErr = MyPDFLInit(); //Initialize the pdf library

    //Check for errors upon initialization 
    if (initErr != 0)
    {
        std::cerr << "Initialization error. See \"AcroErr.h\" for more info.\n" << std::endl;
        std::cerr << "Error system: " << ErrGetSystem(initErr) << std::endl;
        std::cerr << "Error Severity: " << ErrGetSeverity(initErr) << std::endl;
        std::cerr << "Error Code: " << ErrGetCode(initErr) << std::endl;
        return 0;
    }

    //Initialize selected
    //variables. If these variable are left uninitialized,
    //and an exception may occur early in the execution of the
    //placetext sample   

    PDDoc doc = NULL;                           //the new document
    PDPage page = NULL;                         //single page   
    PDEContent content;                         //content within page   
    ASFixedRect rect;                           //rectangle bound for sizing    
    PDSysFont sysFont;                          //system font   
    PDEFont pdeFont = NULL;                     //PDE used font     
    PDEFontAttrs fontAttrs;                     //font attributes
    PDEText textObj = NULL;                     //object for holding text   
    ASDoubleMatrix textMatrix;                  //matrix for sizing and setting location of text
    PDEColorSpace colorSpace = NULL;            //the type of color scale
    char *placeStr = "This text was placed!";   //String for text object
    ASText outPathName = NULL;                  //Path to save to
    ASErrorCode errCode = 0;                    //errCode check variable
    ASPathName asOutPath = NULL;                //Pathname for saving
    wchar_t  wideStringOut[] = L"out.pdf";      //The text for the pathname
    PDEColorSpace  pdeColorSpace = NULL;        //ColorSpace representing color scheme 
    PDEGraphicState gState;                     //Graphic state for rendering 
    PDETextState tState;                        //Text state for rendering

    DURING

        doc = PDDocCreate();    //Creates a new document

        //Set up the 4" by 4" bounds for the page
        rect.left = fixedZero;
        rect.top = Int16ToFixed(4 * 72);
        rect.right = Int16ToFixed(4 * 72);
        rect.bottom = fixedZero;

        //Initialize page from source document, where to place, and bound rectangle
        page = PDDocCreatePage(doc, PDBeforeFirstPage, rect);

        //Grab contenet from the page
        content = PDPageAcquirePDEContent(page, NULL);

        //Initialize font Attributes
        memset(&fontAttrs, 0, sizeof(fontAttrs));
        fontAttrs.name = ASAtomFromString("CourierStd");    //font style
        fontAttrs.type = ASAtomFromString("Type1");         //font type

        //Create system font using font attributes, its size, and flags
        sysFont = PDFindSysFont(&fontAttrs, sizeof(fontAttrs), 0);

        //Create the pdeFont with the sysFont and PDEFontCreateFlags      
        pdeFont = PDEFontCreateFromSysFont(sysFont, kPDEFontCreateEmbedded);

        //Set the grahpics state to its default values 
        PDEDefaultGState(&gState,0);

        //Setting up a matrix for setting text size and placement location
        memset(&textMatrix, 0, sizeof(textMatrix));
        textMatrix.a = 12; //Character width (matrix element size)
        textMatrix.d = 12; //Character width (matrix element size)
        textMatrix.h = 1 * 72.0; //Place at a x-val of an inch
        textMatrix.v = 2 * 72.0; //Place at a y-val of 2 inches

        textObj = PDETextCreate();

        PDETextAddEx(textObj,           //PDEText
            kPDETextRun,                //kPDETextRun/kPDETextChar
            0,                          //index
            (Uns8*)placeStr,            //String converter to Unsigned 8-bit form
            strlen(placeStr),           //string length
            pdeFont,                    //the used font
            &gState,                    //PDEGraphicState
            0,                          //^ its size
            &tState,                    //text state
            0,                          //^ its size
            &textMatrix,                //the ASDoubleMatix
            NULL);                      //stroke matrix

        std::cout << "Text element created and set." << std::endl;

        //Add the text element to the page's content
        PDEContentAddElem(content, kPDEAfterLast, reinterpret_cast<PDEElement>(textObj));

        //Set the content back into the page
        PDPageSetPDEContentCanRaise(page, NULL);


        //Save document

        if (sizeof(wchar_t) == 2)
            outPathName = ASTextFromUnicode((ASUTF16Val *)wideStringOut, kUTF16HostEndian);
        else
            outPathName = ASTextFromUnicode((ASUTF16Val *)wideStringOut, kUTF32HostEndian);

        asOutPath = ASFileSysCreatePathFromDIPathText(NULL, outPathName, NULL);

        PDDocSave(doc, PDSaveFull | PDSaveLinearized, asOutPath, ASGetDefaultFileSys(), NULL, NULL);
        std::cout << "out.pdf saved with text to be placed." << std::endl << std::endl;

    HANDLER
        errCode = ERRORCODE;
    END_HANDLER

    //Free up used objects  

    PDERelease(reinterpret_cast<PDEObject>(gState.strokeColorSpec.space));
    PDERelease(reinterpret_cast<PDEObject>(gState.fillColorSpec.space));

    if (outPathName)    ASTextDestroy(outPathName);
    if (asOutPath)      ASFileSysReleasePath(NULL, asOutPath);

    if (pdeFont)        PDERelease(reinterpret_cast<PDEObject>(pdeFont));
    if (textObj)        PDERelease(reinterpret_cast<PDEObject>(textObj));
    if (page)
    {
        PDPageReleasePDEContent(page, NULL);
        PDPageRelease(page);
    }

    if (doc)  PDDocRelease(doc);

    if (errCode)
        DisplayError(errCode);

    MyPDFLTerm();   //Terminate the pdf library
    return errCode;
}



