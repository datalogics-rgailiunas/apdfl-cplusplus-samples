//
// Copyright (c) 2017, Datalogics, Inc. All rights reserved.
//
// For complete copyright information, refer to:
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
// Sample: RenderPage
//
// This file contains declarations for the RenderPage class.
//

#include <stdio.h>
#include <string.h>

#include "CosCalls.h"
#include "ASCalls.h"
#include "PDCalls.h"
#include "PSFCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"
#include "PDFLExpT.h"
#include "PDFLCalls.h"
#include "DLExtrasCalls.h"


class RenderPage 
{
private:
    PDPage              pdPage;
    PDEImage            image; 
    PDEImageAttrs       attrs;
    PDEColorSpace       cs;
    PDEFilterArray      filterArray;
    // It is "Best Practice" to use ASDouble or ASReal matrices and rectangles in rendering pages. 
    // This avoids the issue of ASFixed overflow in larger, or higher resolution, images. 
    ASDoubleMatrix      matrix;
    ASDoubleMatrix      scaleMatrix;
    ASDoubleMatrix      imageMatrix;
    ASDoubleRect        destRect;
    ASDoubleRect        scaledDestRect;
    ASDoubleRect        pageRect;
    ASAtom              csAtom;
    ASInt32             nComps;
    ASInt32             bufferSize;
    ASInt32             bpc;
    char*               buffer; 
    char*               colorSpace;
    char*               filterName;
    float               resolution;

    PDEFilterArray      SetDCTFilterParams(CosDoc cosDoc);
    ASAtom              SetColorSpace(const char *colorSpace);
    ASInt32             SetBPC(ASInt32 bitsPerComp); 

    static ASAtom       sDeviceRGB_K;
    static ASAtom       sDeviceCMYK_K;
    static ASAtom       sDeviceGray_K;

public:
    RenderPage(PDPage &pdPage, const char *colorSpace, const char *filterName, ASInt32 bpc, double resolution);
    ~RenderPage();

    char*               GetImageBuffer();
    ASInt32             GetImageBufferSize();
    PDEImage            GetPDEImage(PDDoc outDoc);
    ASFixedRect         GetImageRect();
};
