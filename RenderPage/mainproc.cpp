/*    
    RenderPage - Sample for the Adobe PDF Library distributed by Datalogics.
    Copyright (c) 2007-2015, Datalogics, Inc. All rights reserved.

    This sample code is licensed under the terms listed at
    http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/

    This PDF Library sample demonstrates the process of rasterizing a PDF page
    and placing the resulting raster as an image into a different PDF document.

    This file contains the non-rasterizing portions of this sample:
    PDF document opening, creation and saving, page creation, etc.
*/

#include "APDFLDoc.h"
#include "InitializeLibrary.h"

#include "PDFInit.h"
#include "CosCalls.h"
#include "CorCalls.h"
#include "ASCalls.h"
#include "PDCalls.h"
#include "PSFCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PIExcept.h"
#include "PagePDECntCalls.h"
#include "RenderPage.h"

#ifdef MAC_ENV                                  
#include "macUtils.h"
#endif

#define INPUT_FILE      L"../_Input/RenderPage.pdf"
#define OUTPUT_FILE     L"RenderPage-out.pdf"
#define RESOLUTION      150.0 //typically 72.0, 150.0, 200.0, 300.0, or 600.0  
#define COLORSPACE      "DeviceRGB" //typically this, DeviceGray or DeviceCMYK
#define FILTER          "FlateDecode" //or ASCIIHexDecode, LZWDecode, DCTDecode
#define BPC             8 //this must be 8 for DeviceRGB & DeviceCYMK, or 1, 8, or 24 for DeviceGray

int main(int argc, char** argv)
{
    APDFLib lib;                        //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;            //This will catch error codes thrown during library usage.
    PDPage     outputPDPage = NULL;
    PDEContent content = NULL;

    if (lib.isValid() == false)         //If it failed to initialize, return the error code.
        return lib.getInitError();

    APDFLDoc inDoc(INPUT_FILE, true);   //Open the input document
    APDFLDoc outDoc;                    //Create the output document

    DURING
        PDPage pdPage = inDoc.getPage(0);                                                         //Returns the specified PDPage, the first page is 0.

        // The constructor for this class will do the scaling, position, etc. and rasterize
        // the supplied PDPage.
        RenderPage drawPage(pdPage, COLORSPACE, FILTER, BPC, RESOLUTION); 

        outputPDPage = PDDocCreatePage(outDoc.getPDDoc(), PDBeforeFirstPage, drawPage.GetImageRect());
        content      = PDPageAcquirePDEContent(outputPDPage, 0);

        // The call to MakePDEImage synthesizes a PDEImage object from the rasterized PDF page
        // created in the constructor, suitable for placing onto a PDF page.
        PDEContentAddElem(content, 0, (PDEElement) drawPage.MakePDEImage() );
        /* DLADD dtom 10Feb2010:
          Use PDPageSetPDEContentCanRaise instead of PDPageSetPDEContent.*/
        PDPageSetPDEContentCanRaise(outputPDPage, 0);
        PDPageReleasePDEContent(outputPDPage, 0);

        outDoc.saveDoc(OUTPUT_FILE, PDSaveFull | PDSaveCollectGarbage);    //Save the output file in the working directory.

        PDPageRelease(outputPDPage);
        PDPageRelease(pdPage);
        
    HANDLER
        char buf[256];
        ASGetErrorString(ERRORCODE, buf, sizeof(buf));
        fprintf(stderr, "Error code: 0x%x, Error Message: %s\n", ERRORCODE, buf);
    END_HANDLER

        inDoc.~APDFLDoc();
        outDoc.~APDFLDoc();
}
