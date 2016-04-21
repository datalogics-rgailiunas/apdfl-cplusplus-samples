/*	
    RenderPage - Sample for the Adobe PDF Library distributed by Datalogics.
    Copyright (c) 2007-2015, Datalogics, Inc. All rights reserved.

    This sample code is licensed under the terms listed at
    http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/

    This PDF Library sample demonstrates the process of rasterizing a PDF page
    and placing the resulting raster as an image into a different PDF document.

    This file contains declarations for the RenderPage class.
*/

#ifndef MAC_PLATFORM
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#endif
#ifdef MAC_ENV
#include "MacUtils.h"
#endif

#include <iostream>
#include <string>

#include "PDFInit.h"
#include "CosCalls.h"
#include "ASCalls.h"
#include "PDCalls.h"
#include "PSFCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"
#include "PDFLExpT.h"
#include "PDFLCalls.h"

static ASAtom sDeviceRGB_K, sDeviceCMYK_K, sDeviceGray_K; //to hold colorspace ASAtoms

class RenderPage 
{
private:
    PDPage              pdPage;
    PDEImage            image; 
    PDEImageAttrs       attrs;
    PDEColorSpace       cs;
    PDEFilterArray      filterArray;
    ASFixedMatrix       matrix;
    ASFixedMatrix       scaleMatrix;
    ASFixedMatrix       imageMatrix; 
    ASFixedRect         destRect;
    ASFixedRect         scaledDestRect;
    ASFixedRect         pageRect;
    ASAtom              csAtom;
    ASInt32             nComps;
    ASInt32             bufferSize;
    ASInt32             bpc;
    char*               buffer; 
    char*               colorSpace;
    char*               filterName;
    float               resolution;

    PDEImageAttrs       SetImageAttrs(ASFixedRect scaledDestRect, ASInt32 bpc);
    PDEFilterArray      SetFilter(char *filterName);
    ASFixedMatrix       SetImageMatrix(PDEImageAttrs attrs, float resolution);
    ASFixedMatrix       SetScaleMatrix(float resolution);
    ASFixedRect         SetPageRect(ASFixedRect	destRect);
    ASAtom              SetColorSpace(char *colorSpace);
    ASInt32             SetBPC(ASInt32 bitsPerComp); 
    ASInt32             PadCompute(PDEImageAttrs attrs, ASInt32 bpc, ASInt32 nComps, char *buffer, ASInt32 bufferSize);	 
    ASInt32             SetBufferSize(PDPage pdPage, ASFixedMatrix &matrix, ASAtom csAtom, ASInt32 bpc, ASFixedRect scaledDestRect);
    float               SetResolution(float resolution);

public:
    RenderPage(PDPage &pdPage, char *colorSpace, char *filterName, ASInt32 bpc, float resolution);
    ~RenderPage();

    char*               GetImageBuffer();
    ASInt32             GetImageBufferSize();
    PDEImage            MakePDEImage();
    ASFixedRect         GetImageRect();
};
