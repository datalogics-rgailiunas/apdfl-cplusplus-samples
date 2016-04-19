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
#include "MyPDFLibUtils.h"
#include "RenderPage.h"

#ifdef MAC_ENV                                  
#include "macUtils.h"
#endif

#define INPUT_FILE      "../_Input/RenderPage.pdf"
#define OUTPUT_FILE     "RenderPage-out.pdf"
#define RESOLUTION		150.0 //typically 72.0, 150.0, 200.0, 300.0, or 600.0  
#define COLORSPACE		"DeviceRGB" //typically this, DeviceGray or DeviceCMYK
#define FILTER			"FlateDecode" //or ASCIIHexDecode, LZWDecode, DCTDecode
#define BPC				8 //this must be 8 for DeviceRGB & DeviceCYMK, or 1, 8, or 24 for DeviceGray

void MainProc(int argc, char **argv )
{
    ASInt32 err = 0;	
    volatile ASPathName outputPathName = NULL;
    volatile PDDoc      outputPDDoc    = NULL;
    volatile PDPage     outputPDPage   = NULL;
    volatile PDEContent content        = NULL;

    PDDoc pdDoc = MyPDDocOpen(INPUT_FILE);

    if (pdDoc)
    {
    DURING
        PDPage pdPage = PDDocAcquirePage(pdDoc, 0);

        // The constructor for this class will do the scaling, position, etc. and rasterize
        // the supplied PDPage.
        RenderPage drawPage(pdPage, COLORSPACE, FILTER, BPC, RESOLUTION); 

        outputPDDoc  = PDDocCreate();
        outputPDPage = PDDocCreatePage(outputPDDoc, PDBeforeFirstPage, drawPage.GetImageRect());
        content      = PDPageAcquirePDEContent(outputPDPage, 0);

        // The call to MakePDEImage synthesizes a PDEImage object from the rasterized PDF page
        // created in the constructor, suitable for placing onto a PDF page.
        PDEContentAddElem(content, 0, (PDEElement) drawPage.MakePDEImage() );
        /* DLADD dtom 10Feb2010:
          Use PDPageSetPDEContentCanRaise instead of PDPageSetPDEContent.*/
        PDPageSetPDEContentCanRaise(outputPDPage, 0);
        PDPageReleasePDEContent(outputPDPage, 0);

#if WIN_PLATFORM || UNIX_PLATFORM
        outputPathName = ASFileSysCreatePathName(NULL, ASAtomFromString("Cstring"), (char*)OUTPUT_FILE, 0);
#elif MAC_PLATFORM
        outputPathName = GetMacPath(OUTPUT_FILE);
#endif

        PDDocSave(outputPDDoc, PDSaveFull | PDSaveCollectGarbage, outputPathName, 0, 0, 0);
        ASFileSysReleasePath(NULL, outputPathName);

        PDPageRelease(outputPDPage);
        PDPageRelease(pdPage);
        
    HANDLER
        char buf[256];
        ASGetErrorString(ERRORCODE, buf, sizeof(buf));
        fprintf(stderr, "Error code: 0x%x, Error Message: %s\n", ERRORCODE, buf);
    END_HANDLER

        PDDocClose(outputPDDoc);
        PDDocClose(pdDoc);
    }
}

#define INCLUDE_MYPDFLIBAPP_CPP	1
#include "MyPDFLibApp.cpp"
#undef INCLUDE_MYPDFLIBAPP_CPP
