// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//==============================================================================
// Sample: AddWatermark - This sample adds two watermarks to the first two
//             pages of the input PDF. There are two ways of adding a watermark
//             to a document. One can add a text watermark, or a watermark
//             that is the image of a page of a PDF document. This sample
//             adds both kinds to both pages.
//
// Steps:
// 1) Set the watermark parameters struct.
// 2) Set the text watermark parameters struct.
// 3) Add the page and text watermarks.
//==============================================================================


#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#include "PagePDECntCalls.h"
#include "PERCalls.h"
#include "PSFCalls.h"
#include "PEWCalls.h"

int main(int argc, char** argv)
{

    APDFLib libInit;                                            //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;                                    //Variable used to report any exceptions/errors if they occured.

    if (libInit.isValid() == false)                             //If there was a problem in initialization, return the error code.
        return libInit.getInitError();

    DURING

    APDFLDoc inDoc(L"../_Input/AddWatermark.pdf", true);        //Open the input document, repairing it if it's damaged.
    APDFLDoc watermarkDoc(L"../_Input/Watermark.pdf", true);    //We'll take the watermark image from the first page of this PDF.

//========================================================================================================================================================================================================================
//Step 1) Set the watermark parameters struct.
//
// Note: This struct is used by both our text and page watermarks.
//========================================================================================================================================================================================================================

    std::wcout << L"Preparing the watermark settings." << std::endl;

    PDDocAddWatermarkParamsRec watermarkOptions;
    memset(&watermarkOptions, 0, sizeof(PDDocAddWatermarkParamsRec));
    watermarkOptions.size = sizeof(PDDocAddWatermarkParamsRec);

    ////////////////////////////
    //Display options.        //
    ////////////////////////////
	PDPageRange	placeHolder = {0, 1, PDAllPages};
	watermarkOptions.targetRange = placeHolder;							 //A list-initialized PDPageRange. The first page to watermark is 0, the last page is 1, and we'll add a watermark to every page in the range.


    watermarkOptions.zOrderTop    = false;                               //Watermarks will be added to the background of the page, not on top.
    
    watermarkOptions.showOnScreen = true;                                //Watermarks will be visible in a PDF viewer...
    watermarkOptions.showOnPrint  = false;                               //..but will not show up if the PDF is printed.
    watermarkOptions.fixedPrint   = false;                               //Watermarks will not be a fixed print watermark, meaning it changes its size and position based on the target media's dimensions, if necessary.

    ////////////////////////////
    //Placement options.      //
    ////////////////////////////
    watermarkOptions.horizAlign = kPDHorizCenter;                        //Watermarks will be horizontally aligned at the center of the page.
    watermarkOptions.horizValue = 0.0f;                                  //No horizontal offset.

    watermarkOptions.vertAlign  = kPDVertCenter;                         //Vertically aligned at the top.
    watermarkOptions.vertValue  = 0.0f;                                  //No vertical offset either.

    watermarkOptions.percentageVals = false;                             //Whether horizValue and vertValues are percentages of the page size. If not, they are in user units.

    ////////////////////////////
    //Transformation options. //
    ////////////////////////////
    watermarkOptions.scale    =  0.5f;                                   //The scale the watermark should be drawn at, with 1.0f representing 100%.
    watermarkOptions.rotation = -25.0f;                                  //Counterclockwise rotation added to the watermark, in degrees.
    watermarkOptions.opacity  =  0.5f;                                   //The opacity set to the watermark, with 1.0f representing 100%.

    ////////////////////////////
    //Optional callbacks.     //
    ////////////////////////////
    watermarkOptions.progMon     = NULL;                                 //Optional progress monitor, for monitoring the watermark adding process.
    watermarkOptions.progMonData = NULL;                                 //Optional data to send to progress monitor.

    watermarkOptions.cancelProc     = NULL;                              //Optional cancel procedure function checked while adding the watermark.
    watermarkOptions.cancelProcData = NULL;                              //Optional data to send to cancel procedure.

//=======================================================================================================================================================================================================================
//Step 2) Set the text watermark parameters struct.
//
// Note: This struct is only used by the text watermark. This will be a little more involved, as we have resource creation to do. We must:
//     a) Create an ASText object for our watermark text.
//     b) Load a font for it.
//     c) Create a PDColorValueRec which specifies the color of the text.
//=======================================================================================================================================================================================================================

    PDDocWatermarkTextParamsRec textWatermarkOptions;
    memset(&textWatermarkOptions, 0, sizeof(PDDocWatermarkTextParamsRec));
    textWatermarkOptions.size = sizeof(PDDocWatermarkTextParamsRec);

    ////////////////
    //The text.   //
    ////////////////
    ASText text = ASTextFromUnicode((ASUTF16Val*)"Copyright (c) 2015, Datalogics, Inc.", kUTF8);    //This is possible because ASCII is equivalent to UTF-8.
    textWatermarkOptions.srcText = text;
    textWatermarkOptions.textAlign = kPDHorizCenter;                                                //The text will be horizontally aligned with the page.

    ////////////////
    //The font.   //
    ////////////////
    PDEFontAttrs fontAttrs;
    memset(&fontAttrs, 0, sizeof(fontAttrs));

    fontAttrs.name = ASAtomFromString("CourierStd");
    fontAttrs.type = ASAtomFromString("Type1");
    
    PDSysFont sysFont = PDFindSysFont(&fontAttrs, sizeof(fontAttrs), 0);
    PDEFont pdeFont = PDEFontCreateFromSysFont(sysFont, kPDEFontDoNotEmbed);
    
    textWatermarkOptions.pdeFont = pdeFont;
    textWatermarkOptions.sysFontName = fontAttrs.name;
    textWatermarkOptions.fontSize = 14.0f;

    ////////////////
    //The color.  //
    ////////////////
    PDColorValueRec color;
    color.space = PDDeviceRGB;
    color.value[0] = fixedZero;  //Red value.
    color.value[1] = fixedZero;  //Green value.
    color.value[2] = fixedZero;  //Blue value. I've gone with the color black.
    textWatermarkOptions.color = color;

//=======================================================================================================================================================================================================================
//Step 3) Add the page and text watermarks.
//=======================================================================================================================================================================================================================

    std::wcout << L"Adding the page watermark." << std::endl;

    PDPage pageWatermarkSource = watermarkDoc.getPage(0);                                     //This page is our watermark image.
    PDDocAddWatermarkFromPDPage(inDoc.getPDDoc(), pageWatermarkSource, &watermarkOptions);    //Add the page watermark.
    PDPageRelease(pageWatermarkSource);

    //We're going to modify our watermark parameters a bit for the text watermark.
    watermarkOptions.vertAlign = kPDVertBottom;
    watermarkOptions.scale = 1.0f;
    watermarkOptions.rotation = 0.0f;
    watermarkOptions.vertValue = textWatermarkOptions.fontSize;                               //This vertical offset ensures the text will be visible on the page.

    std::wcout << L"Adding the text watermark." << std::endl;

    PDDocAddWatermarkFromText(inDoc.getPDDoc(), &textWatermarkOptions, &watermarkOptions);    //Add the text watermark.
    ASTextDestroy(text);
    PDERelease((PDEObject)pdeFont);

    inDoc.saveDoc(L"AddedWatermarks.pdf");                                                    //Save the document. APDFLDoc defaults to using the "PDSaveFull" flag while saving. 
                                                                                              //(APDFLDoc's destructor takes care of closing the documents and releasing the rest of their resources.)
    HANDLER

        errCode = ERRORCODE;
        libInit.displayError(errCode);                                                        //If there was an error, display it.

    END_HANDLER

    if (!errCode)
        std::wcout << L"Success!" << std::endl;

    return errCode;                                                                           //APDFLib's destructor terminates the library.
}
