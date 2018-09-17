//
// Copyright (c) 2007-2018, Datalogics, Inc. All rights reserved.
//
// For complete copyright information, refer to:
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
// The RenderPage sample program shows how to render a PDF document page to memory.
//
// For more detail see the description of the RenderPage sample program on our Developer’s site, 
// http://dev.datalogics.com/adobe-pdf-library/sample-program-descriptions/c1samples#renderpage

#include "RenderPage.h"
#include <math.h>

//Statics
ASAtom RenderPage::sDeviceRGB_K;
ASAtom RenderPage::sDeviceCMYK_K;
ASAtom RenderPage::sDeviceGray_K;

// These are utility routines to convert Rects and Matrices between ASDouble and
// ASReal, and ASFixed.
// ASFixed was the original method of specifing "real" numbers in APDFL. It is still widly present in APDFL interfaces, though it is 
//   limited by both it's resolution (0.0001 typically) and it range (+- 32767). There is a full complement of methods for combining
//   matrices, and transforming and comparing rectangles. Internal to APDFL, ASFixed is now seldom used.
// ASReal was introduced into APDFL later. It is implemented as a float, and it does not have the full 
//   complement of transform methods. It is used here because it is needed in the interface to PDPageDrawContentsToMemoryWithParams.
// ASDouble was introduced most recently. It has a full complement of transformation methods. Many interfaces to APDFL have been updated
//   (Generally by the addition of "Ex" to the interface name) to provide/accepts such values.
//
//  However, conversion between these forms is not always supplied. These routines provided the conversions needed for this sample.
void ASDoubleRectToASReal (ASRealRect &out, ASDoubleRect &in)
{
    out.left = in.left;
    out.right = in.right;
    out.top = in.top;
    out.bottom = in.bottom;
}

void ASDoubleMatrixToASReal (ASRealMatrix &out, ASDoubleMatrix &in)
{
    out.a = in.a;
    out.b = in.b;
    out.c = in.c;
    out.d = in.d;
    out.tx = in.h;
    out.ty = in.v;
}

void ASFixedRectToASDouble (ASDoubleRect &out, ASFixedRect &in)
{
    out.left = ASFixedToFloat (in.left);
    out.right = ASFixedToFloat (in.right);
    out.top = ASFixedToFloat (in.top);
    out.bottom = ASFixedToFloat (in.bottom);
}

void ASFixedMatrixToASDouble (ASDoubleMatrix &out, ASFixedMatrix &in)
{
    out.a = ASFixedToFloat (in.a);
    out.b = ASFixedToFloat (in.b);
    out.c = ASFixedToFloat (in.c);
    out.d = ASFixedToFloat (in.d);
    out.h = ASFixedToFloat (in.h);
    out.v = ASFixedToFloat (in.v);
}

void ASDoubleToFixedRect (ASFixedRect &out, ASDoubleRect &in)
{
    out.left = FloatToASFixed (in.left);
    out.right = FloatToASFixed (in.right);
    out.top = FloatToASFixed (in.top);
    out.bottom = FloatToASFixed (in.bottom);
}

// This both constructs the RenderPage object, and creates the page rendering. 
//  The rendered page can be accessed as a bitmap via the methods GetImageBuffer() and GetImageSize, or as a PDEImage, 
//  via the method GetPDEImage(). The PDEImage creation will be deferred until it is requested.
RenderPage::RenderPage(PDPage &pdPage, const char *colorSpace, const char *filterName, ASInt32 inBPC, double inResolution)
{
    // Set up the static colorspace atoms
    sDeviceRGB_K = ASAtomFromString("DeviceRGB");
    sDeviceCMYK_K = ASAtomFromString("DeviceCMYK");
    sDeviceGray_K = ASAtomFromString("DeviceGray");

    image = NULL;

    //If you are using a decode filter such as FlateDecode, the filterArray values will be set here
    memset (&filterArray, 0, sizeof (PDEFilterArray));
    if (filterName)
    {
        filterArray.numFilters = 1;
        filterArray.spec[0].name = ASAtomFromString (filterName);
    }
    
    //Set resolution.
    //  A PDF "unit" is, by default, 1/72nd of an inch. So a resolution of 72 is one PDF "unit" per pixel. 
    //  if no resolution is set, we will use 72 DPI as our image reslution. This sample does not attempt to
    //  support different horiziontal and vertical resolutions. APDFL, can easily support them by using a 
    //  different scale factor in the scale matrix "a" (horiziontal) and "d" (vertical) members. The scale 
    //  factors are simply (72.0 / resolution).
    if (inResolution <= 0.0)
        resolution = 72.0;
    else
        resolution = inResolution;

    //Get the colorspace atom, set the number of components per colorspace
    //  and store the appropriate colorspace for an output PDEImage
    //  This sample will raise an exception if the color space is not one of
    //  DeviceGray, DeviceRGB, or DeviceCMYK. APDFL supports a number of additional
    //  color spaces. The image created may also be conformed to a given ICC Profile.
    csAtom = SetColorSpace(colorSpace);

    //The size of each color component to be represented in the image.
    bpc = SetBPC(inBPC);

    //Gets the matrix that transforms user space coordinates to rotated and cropped coordinates.
    //  In PDF, the normal origin of an image is the lower left corner of the image, with 
    //  position increasing in number up and to the right. This would be obtained using 
    //  PDPageGetDefaultMatrix. However, most image formats prefer an origin of the top left 
    //  corner. Drawing the image left to right, top to bottom. The interface PDPageGetFlippedMatrix
    //  will obtain a matrix to draw the page in this order.
    ASFixedMatrix pageFixedMatrix;
    PDPageGetFlippedMatrix (pdPage, &pageFixedMatrix);
    ASFixedMatrixToASDouble (matrix, pageFixedMatrix);


    //Gets the media box for a page. The Media Box reflects the entire contents of the page.
    ASFixedRect pageFixedRect;
    PDPageGetMediaBox (pdPage, &pageFixedRect);
    ASFixedRectToASDouble (pageRect, pageFixedRect);

    // We want to display the page upright, regardless of how it was imaged. 
    // The matrix returned by PDPageGetFlippedMatrix will include the rotation
    // applied to the page (as will PDPageGetDefaultMatrix). However, the rectangle
    // returned by PDPageGetMediaBox will not be rotated. So if the page is rotated
    // 90 or 270 degrees, we need to swap those sides here.
    PDRotate rotation = PDPageGetRotate (pdPage);
    if ((rotation == 90) || (rotation == 270))
    {
        // If the page is rotated 90 or 270 degrees,
        // Swap width and depth.
        ASFixed saveLeft = pageRect.left;
        ASFixed saveRight = pageRect.right;
        pageRect.left = pageRect.bottom;
        pageRect.right = pageRect.top;
        pageRect.bottom = saveLeft;
        pageRect.top = saveRight;
    }

    //Set page coordinates/rectangle to crop box for PDDocCreatePage
    // The destination rectangle should be considered to be in pixels.
    // Here, we will round up to include the last partial pixel
    destRect.left = destRect.bottom = 0;
    destRect.right = floor ((pageRect.right - pageRect.left) + 0.5);
    destRect.top = floor ((pageRect.top - pageRect.bottom) + 0.5);

    //Set the scale matrix that will be concatenated to the user space matrix
    scaleMatrix.a = scaleMatrix.d = resolution / 72.0;
    scaleMatrix.b = scaleMatrix.c = scaleMatrix.h = scaleMatrix.v = 0;

    //Apply the scale to the default matrix
    ASDoubleMatrixConcat(&matrix, &scaleMatrix, &matrix);
    ASDoubleMatrixTransformRect(&scaledDestRect, &scaleMatrix, &destRect);

    // "Best Practice" is to use PDPageDrawContentsToMemoryWithParams, as it allows
    // the matrix and rects to be specified in floating point, eliminating the need
    // to test for ASFixed Overflows.
    PDPageDrawMParamsRec drawParams;

    // Many of the values in the params record will not be used for this simple rendering. 
    // so we set them all to zeros initally
    memset (&drawParams, 0, sizeof (PDPageDrawMParamsRec));
    drawParams.size = sizeof (PDPageDrawMParamsRec);
    drawParams.csAtom = csAtom;
    drawParams.bpc = bpc;

    // For this example we will smooth (anti-alias) all of the marks. For a given application,
    // this may or may not be desireable. See the enumeration PDPageDrawSmoothFlags for the full set of options.
    drawParams.smoothFlags = kPDPageDrawSmoothText | kPDPageDrawSmoothLineArt | kPDPageDrawSmoothImage;

    // The DoLazyErase flag is usually, if not always turned on, UseAnnotFaces will cause annotations
    // in the page to be displayed, and kPDPageDsiplayOverprintPreview will display the page showing 
    // overprinting. The precise meaning of these flags, as well as others that may be used here, can be
    // seen in the defintion of PDPageDrawFlags
    drawParams.flags = kPDPageDoLazyErase | kPDPageUseAnnotFaces | kPDPageDisplayOverPrintPreview;

    // This is a bit clumsy, because features were added over time. The matrices and rectangles in this
    // interface use ASReal as thier base, rather than ASDouble. But there is not a complete set of 
    // concatenation and transformation methods for ASReal. So we generally generate the matrix and 
    // rectangle values using ASDouble, and convert to ASReal. 
    ASRealRect realDestRect;
    ASDoubleRect doubleUpdateRect;
    ASRealRect realUpdateRect;
    ASRealMatrix realMatrix;
    ASDoubleRectToASReal (realDestRect, scaledDestRect);
    ASFixedRectToASDouble (doubleUpdateRect, pageFixedRect);    // This rectangle, the portion of the page to display, 
    ASDoubleRectToASReal (realUpdateRect, doubleUpdateRect);    // Must NOT be swapped to reflect page rotation.
    ASDoubleMatrixToASReal (realMatrix, matrix);
    drawParams.asRealDestRect = &realDestRect;               // This is where the image is drawn on the resultant bitmap.
                                                             //   It is generally set at 0, 0 and width/height in pixels.
    drawParams.asRealUpdateRect = &realUpdateRect;           // This is the portion of the document to be drawn. If omitted, 
                                                             // it will be the document media box, which is generally what is wanted.
    drawParams.asRealMatrix = &realMatrix;                   // This is the scale factor of the page from points to pixels, and the
                                                             // displacement of the lower left hand corner of the area of the page
                                                             // to be rendered.

    // Additional values in this record control such features as drawing separations, 
    // specifiying a desired output profile, selecting optional content, and providing for 
    // a progress reporting callback.

    //Allocate the buffer for storing the rendered page content
    // It is important that ALL of the flags and options used in the actual draw be set the same here!
    // Calling this interface with drawParms.bufferSize or drawParams.buffer equal to zero will return the size of the buffer
    //   needed to contain this image. This is the most certain way to get the correct buffer size. If we call this routine with a buffer
    //   that is not large enough to contain the image, we will not draw the image, but will simply, silently, return the size of the buffer
    //   needed!
    bufferSize = PDPageDrawContentsToMemoryWithParams (pdPage, &drawParams);   // This call, with a NULL buffer pointer, returns needed buffer size
    
    //  One frequent failure point in rendering images is being unable to allocate sufficient contigious space 
    //  for the bitmap buffer. Here, that will be indicated by a zero value for drawParams.buffer after the 
    //  call to malloc. If the buffer size is larger than the internal limit of malloc, it may also raise an
    //  interupt! Catch these conditions here, and raise an out of memory error to the caller.
    try
    {
        buffer = (char *)ASmalloc (bufferSize);
        if (!buffer)
            ASRaise (genErrNoMemory);
    }
    catch (...)
    {
        ASRaise (genErrNoMemory);
    }

    // With these values in place, the next call to PDPageDrawContentsToMemoryWithParams() will fill the bitmap.
    drawParams.bufferSize = bufferSize;
    drawParams.buffer = buffer;

    // Render page content to the bitmap buffer
    PDPageDrawContentsToMemoryWithParams (pdPage, &drawParams);

    // Set up attributes for the PDEImage to be made by GetPDEImage
    memset (&attrs, 0, sizeof (PDEImageAttrs));
    attrs.flags = kPDEImageExternal;
    attrs.height = floor (abs ((scaledDestRect.top - scaledDestRect.bottom) + 0.5));
    attrs.width = floor (abs ((scaledDestRect.right - scaledDestRect.left) + 0.5));
    attrs.bitsPerComponent = bpc;

    // The bitmap data generated by PDPageDrawContentsToWindow uses 32-bit aligned rows. 
    // The PDF image operator expects, however, 8-bit aligned image rows. 
    // To remedy this difference, we check to see if the 32-bit aligned width
    // is different from the 8-bit aligned width. If so, we fix the image data by 
    // stripping off the padding at the end of each row.
    ASInt32 createdWidth = ((((attrs.width * bpc * nComps) + 31) / 32) * 4);
    ASInt32 desiredWidth = ((attrs.width * bpc * nComps) / 8);

    if (createdWidth != desiredWidth)
    {
        for (int row = 1; row < attrs.height; row++)
            memmove (&buffer[row * desiredWidth], &buffer[row * createdWidth], desiredWidth);
        drawParams.bufferSize = desiredWidth * attrs.height;
    }

}

RenderPage::~RenderPage() 
{ 
    if(buffer)
        ASfree (buffer);
    buffer = NULL;

    if (image != NULL)
        PDERelease(reinterpret_cast<PDEObject>(image));

    PDERelease(reinterpret_cast<PDEObject>(cs));
}

char * RenderPage::GetImageBuffer()
{
    return buffer;
}

ASInt32 RenderPage::GetImageBufferSize()
{
    return bufferSize;
}

ASFixedRect RenderPage::GetImageRect()
{
    ASFixedRect fixedRect;
    ASDoubleToFixedRect (fixedRect, pageRect);
    return fixedRect;
}

PDEImage RenderPage::GetPDEImage(PDDoc outDoc)
{
    // If we have already created the image, simply return it.
    if (image != NULL)
        return image;

    // When we are encoding in DCT, we need some additional information 
    // in the filter. This information was not available when we original created
    // the filter. So we add this now.
    if (filterArray.spec[0].name == ASAtomFromString("DCTDecode"))
        SetDCTFilterParams(PDDocGetCosDoc(outDoc));

    //Create the image matrix using the height/width attributes and apply the resolution.
    
    imageMatrix.a = attrs.width / (resolution / 72.0);
    imageMatrix.d = attrs.height / (resolution / 72.0);
    imageMatrix.b = imageMatrix.c = imageMatrix.h = imageMatrix.v = 0;

    // Create an image XObject from the bitmap buffer to embed in the output document
    image = PDEImageCreateInCosDocEx ( &attrs, 
                                        sizeof(attrs),
                                        &imageMatrix, 
                                        0,
                                        cs, 
                                        NULL,
                                        &filterArray, 
                                        0,
                                        (unsigned char*) buffer, 
                                        bufferSize,
                                        PDDocGetCosDoc (outDoc));

    return image;
}


PDEFilterArray RenderPage::SetDCTFilterParams(CosDoc cosDoc)
{
    // Create a new Cos dictionary
    CosObj dictParams = CosNewDict(cosDoc, false, 4);

    // Populate the dictionary with required entries to do JPEG compression
    // (only Columns, Rows, and the number of color dimensions needed for sraight JPEG)
    CosDictPut(dictParams, ASAtomFromString("Columns"),
        CosNewInteger(cosDoc, false, attrs.width));
    CosDictPut(dictParams, ASAtomFromString("Rows"),
        CosNewInteger(cosDoc, false, attrs.height));
    CosDictPut(dictParams, ASAtomFromString("Colors"),
        CosNewInteger(cosDoc, false, nComps));

    filterArray.numFilters = 1;
    filterArray.spec[0].encodeParms = dictParams;
    filterArray.spec[0].decodeParms = CosNewNull();

    return filterArray;
}


// Validate colorspace selection and set the channels per color
ASAtom RenderPage::SetColorSpace(const char *colorSpace)
{    
    csAtom = ASAtomFromString ( colorSpace );
    if ( csAtom == sDeviceGray_K )
    {
        nComps = 1;
    }
    else if ( csAtom == sDeviceRGB_K )
    {
        nComps = 3;
    }
    else if ( csAtom == sDeviceCMYK_K )
    {
        nComps = 4;
    }
    else
    {
        // Not a valid colorspace
        ASRaise(genErrBadParm);
    }

    // initialize the output colorspace for the PDEImage we'll generate in MakePDEImage
    cs = PDEColorSpaceCreateFromName(csAtom);

    return csAtom;
}

// Set the BPC depending on the colorspace being used
ASInt32 RenderPage::SetBPC(ASInt32 bitsPerComp)
{   
    bpc = bitsPerComp;
    if(csAtom==sDeviceRGB_K || csAtom==sDeviceCMYK_K)
    {
        if(bitsPerComp!=8)
        {
            // Reset to 8
            bpc = 8;
        }
    }
    if(csAtom==sDeviceGray_K)
    {
        if((bitsPerComp != 1) && (bitsPerComp != 8) && (bitsPerComp != 24))
        {
            // Reset to 8
            bpc = 8;
        }
    }
    return bpc;
}
