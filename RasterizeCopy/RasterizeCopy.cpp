// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//======================================================================================================================
// Sample: RasterizeCopy - This sample makes a rasterized copy of the first page of the input document, and draws the
//     image onto a new document at the specified resolution.
//
//Steps: 
// Step 1) Determine the input page's content matrix and create a scaled output matrix based on the resolution we want.
// Step 2) Determine the color space for the output image.
// Step 3) Render the page's contents to a buffer in memory.
// Step 4) Create an image out of the buffer.
// Step 5) Draw the image to an output document.
// Step 6) Save and close.
//======================================================================================================================

#include "APDFLDoc.h"
#include "InitializeLibrary.h"

#include "PERCalls.h"
#include "PEWCalls.h"
#include "AcroColorCalls.h"
#include "PDFLExpT.h"
#include "PagePDECntCalls.h"
#include "DLExtrasCalls.h"

int main(int agc, char** argv)
{
    APDFLib lib;                                         //Initialize the Adobe PDF Library.

    if (lib.isValid() == false)                          //If it failed to initialize, return the error code.
        return lib.getInitError();

    wchar_t* inPath  = L"../_Input/RasterizeCopy.pdf";   //The path to the input document.
    std::string colorSpace = "DeviceRGB";                //The color space we want the output image to be in. This sample supports DeviceRGB, DeviceCMYK, and DeviceGray.
    const char* filterName = "FlateDecode";              //How we want the output image decoded. Typically FlateDecode, ASCIIHexDecode, LZWDecode, or DCTDecode.
    float resolution = 200.0;                            //The resolution of the image we'll render. Typically 72.0, 150.0, 200.0, 300.0, or 600.0.
    wchar_t* outPath = L"RasterizeCopied.pdf";           //The path to the output document we'll create.

    ASErrorCode errCode = 0;                             //This will catch error codes thrown during library usage.

    DURING

    APDFLDoc inDoc(inPath, true);                        //Open the input document, repairing if necessary.
    PDPage inPage = inDoc.getPage(0);                    //We'll be rasterizing the first page.

//==================================================================================================================================================================================================================
//Step 1) Determine the input page's content matrix and create a scaled output matrix based on the resolution we want.
//==================================================================================================================================================================================================================

    ASFixedRect destRect;                                                 //The scaled rectangle of our destination bitmap, which we will fill presently.
    ASFixedMatrix userMatrix;                                             //We will scale this by our resolution to get a matrix which will be concatenated with the default input page matrix during rendering.
    ASFixedRect inPageRect;                                               //After transformation by the resolution scale matrix, this will define the printed area of the input page.

    PDPageGetFlippedMatrix(inPage, &userMatrix);                          //Get the matrix which transforms user-space coordinates to rotated and cropped coordinates.
    PDPageGetCropBox(inPage, &inPageRect);                                //The crop box of a page is the area which prints.

    ASFixedMatrix scaleMatrix;                                            //This scale matrix will be used to scale the user matrix to the resolution we want.
    scaleMatrix.a = scaleMatrix.d = FloatToASFixed(resolution / 72.0);    //We define the scale as a factor of 72.0.
    scaleMatrix.b = scaleMatrix.c = scaleMatrix.h = scaleMatrix.v = 0;

    //Create the destination rectangle and source rectangle with correct scaling and positioning.
    ASFixedMatrixConcat(&userMatrix, &scaleMatrix, &userMatrix);          //Multipliy the user matrix by the scale, and store the result in userMatrix.
    ASFixedMatrixTransformRect(&destRect, &scaleMatrix, &inPageRect);     //The inPageRect rectangle is transformed through scaleMatrix, and its smallest bounding box is stored in destRect.

//==================================================================================================================================================================================================================
//Step 2) Determine the color space for the output image.
//==================================================================================================================================================================================================================

    ASAtom colorSpaceAtom;                                          //The atom we'll use to get the desired color space.
    PDEColorSpace outColorSpace;                                    //The color space itself. This won't be used until we draw the image onto the output.
    ASInt32 nComps;                                                 //The number of components each color has in this color space.
    ASInt32 bitsPerComp;                                            //The number of bits per component in this color space.
    unsigned char backgroundColor;                                  //In this color space, set all components to this value to achieve the desired background color.
    AC_Profile acProfile;                                           //PDPageDrawContentsToMemoryWithParams requires this to match up with the color space.

    colorSpaceAtom = ASAtomFromString(colorSpace.c_str());
    outColorSpace = PDEColorSpaceCreateFromName(colorSpaceAtom);

    if (colorSpace == "DeviceRGB")
    {
        nComps = 3;
        bitsPerComp = 8;                                            //Only 8 is valid for DeviceRGB.
        backgroundColor = 0xFF;                                     //All components are set to FF to represent white in RGB.
         ACProfileFromCode(&acProfile, AC_Profile_sRGB);
    }
    else if (colorSpace == "DeviceGray")
    {
        nComps = 1;
        bitsPerComp = 1;                                            //PDFL supports both 1-bit monochrome grayscale, 8, and 24-bit Grayscale. If you choose 1-bit, the image will be dithered automatically.
        backgroundColor = 0xFF;                                     //All components are set to 00 to represent white in Grayscale.
         ACProfileFromCode(&acProfile, AC_Profile_SystemGray);
    }
    else if (colorSpace == "DeviceCMYK")
    {
        nComps = 4;
        bitsPerComp = 8;                                            //Only 8 is valid for DeviceCMYK.
        backgroundColor = 0x00;                                     //All components are set to FF to represent white in CMYK.
         ACProfileFromCode(&acProfile, AC_Profile_SystemCMYK);
    }
    else
    {
        std::wcout << L"Undefined color space specifier: ''" << colorSpace.c_str() << L"''." << std::endl;
        return -1;
    }

//==================================================================================================================================================================================================================
//Step 3) Render the page's contents to a buffer in memory.
//==================================================================================================================================================================================================================

    std::wcout << L"Allocating memory." << std::endl;

    //These parameters specify all the details of how we want the page rendered.
    PDPageDrawMParamsRec drawParams;
    memset((char*) &drawParams, 0, sizeof (PDPageDrawMParamsRec));                      //Initialize all elements of drawParams to 0.
    drawParams.size = sizeof(PDPageDrawMParamsRec);

    //The destination, source, and transformation rectangles, respectively.
    drawParams.destRect = &destRect;
    drawParams.matrix = &userMatrix;

    drawParams.csAtom = colorSpaceAtom;                                                //The atom which defines the color space.
    drawParams.bpc = bitsPerComp;                                                      //The bits each component requires in this color space.

    drawParams.iccProfile = acProfile;                                                 //Specifies the characteristics of the supplied color space.
    drawParams.renderIntent = AC_Perceptual;                                           //Try to preserve the visual relationship between colors while rendering.

    //Bitfields specifying how we want the page rendered.
    drawParams.smoothFlags = kPDPageDrawSmoothText                                     //Anti-alias text.
                             | kPDPageDrawSmoothLineArt                                //Anti-alias line art.
                             | kPDPageDrawSmoothImage;                                 //Anti alias images.
    drawParams.flags = kPDPageDoLazyErase;                                             //Erase while rendering only when needed.

    ASInt32 bufferSize = PDPageDrawContentsToMemoryWithParams(inPage, &drawParams);    //Since the buffer is null, this method only calculates the required buffersize for the specified rendering.
    char* buffer = new char[bufferSize];                                               //Now that we know how much memory we need to render the page, we allocate it.
    memset(buffer, backgroundColor, bufferSize);                                       //In effect, this makes the background color of the image {backgroundColor,backgroundColor,...}.

    drawParams.buffer = buffer;
    drawParams.bufferSize = bufferSize;
    drawParams.size = sizeof(drawParams);

    std::wcout << L"Drawing the page's contents to memory." << std::endl;

    PDPageDrawContentsToMemoryWithParams(inPage, &drawParams);                         //Now that the buffer is set, the method renders the page to the buffer.

    //Free used resources.
    PDPageRelease(inPage);

//==================================================================================================================================================================================================================
//Step 4) Create an image out of the buffer.
//==================================================================================================================================================================================================================

    //Here we set the image attributes of the image we'll draw.
    PDEImageAttrs imageAttrs; 
    memset(&imageAttrs, 0, sizeof(PDEImageAttrs));
    imageAttrs.flags  = kPDEImageExternal;            //Indicates that this image is an XObject.
    imageAttrs.height = abs(ASFixedRoundToInt16(destRect.top) - ASFixedRoundToInt16(destRect.bottom));
    imageAttrs.width  = abs(ASFixedRoundToInt16(destRect.right) - ASFixedRoundToInt16(destRect.left));
    imageAttrs.bitsPerComponent = bitsPerComp;

    //The bitmap data generated by PDPageDrawContentsToMemoryEx is 32-bit aligned. The PDECreateImageEx expects,
    //however, 8-bit aligned image data. To resolve this difference, we check to see if the 32-bit aligned width
    //is different from the 8-bit aligned width. If so, we fix the image data by stripping off the padding at the end.
    //PDF consumer applications ignore the padding bits.
    ASUns32 unpaddedRowLength = (((imageAttrs.width * bitsPerComp * nComps) + 31) / 32) * 4;
    ASUns32 paddedRowLength = (imageAttrs.width * bitsPerComp * nComps) / 8;
    if ( unpaddedRowLength != paddedRowLength)
    {
        char *src, *dest;    //Temporary pointers to the bitmap data buffer created by PDPageDrawContentsToMemory.
        int sw, dw;
        sw = ((((imageAttrs.width * bitsPerComp * nComps) + 31) / 32) * 4);

        dw = ((imageAttrs.width * bitsPerComp * nComps) + 7) / 8;

        //Copy the source bytes to the destination.
        src = dest = buffer;
        for (int i = 0; i < imageAttrs.height; i++)
        {
            for (int j = 0; j < dw; j++)
                dest[j] = src[j];
            src += sw; dest += dw;
        }
        //Recalculate buffer size.
        bufferSize = dw * imageAttrs.height;
    }
    //The image data is now 8-bit aligned.

    //Here we create the matrix our image will fill.
    ASDoubleMatrix imageMatrix;
    imageMatrix.a = (ASDouble)(imageAttrs.width / (resolution / 72.0));
    imageMatrix.d = (ASDouble)(imageAttrs.height / (resolution / 72.0));
    imageMatrix.b = imageMatrix.c = 0;
    imageMatrix.h = imageMatrix.v = 0;

    //This filter array defines how we want the image encoded.
    PDEFilterArray filterArray;
    memset(&filterArray, 0, sizeof(PDEFilterArray));
    filterArray.spec[0].name = ASAtomFromString(filterName);    //PDEFilterArrays use one spec by default.

    std::wcout << L"Rendering the image from memory." << std::endl;

    PDEImage img = PDEImageCreateEx(&imageAttrs, sizeof(imageAttrs), &imageMatrix, 0, outColorSpace, NULL, &filterArray, 0, (unsigned char*)buffer, bufferSize);

    //Free used resources.
    delete[](buffer);
    PDERelease(reinterpret_cast<PDEObject>(outColorSpace));

//==================================================================================================================================================================================================================
//Step 5) Draw the image to an output document.
//==================================================================================================================================================================================================================

    std::wcout << L"Drawing the image onto a new document." << std::endl;

    APDFLDoc outDoc;                                                                                            //Create our output document.
    outDoc.insertPage(inPageRect.right-inPageRect.left, inPageRect.top-inPageRect.bottom,PDBeforeFirstPage);    //Give it a page big enough to hold the image.
    PDPage outPage = outDoc.getPage(0);
    PDEContent outPageCont = PDPageAcquirePDEContent(outPage, 0);                                               //We'll add the image to its contents.

    PDEContentAddElem(outPageCont, 0, (PDEElement)img);                                                         //Add the image to the page.
    PDPageSetPDEContentCanRaise(outPage, 0);                                                                    //This sets the content back into the page.

    //Free used resources.
    PDERelease(reinterpret_cast<PDEObject>(img));

//==================================================================================================================================================================================================================
//Step 6) Save and close.
//==================================================================================================================================================================================================================

    std::wcout << L"Saving..." << std::endl;

    PDPageReleasePDEContent(outPage, 0);                       //Must be released before the page can be released.
    PDPageRelease(outPage);                                    //Must be released before the document can be saved.
    outDoc.saveDoc(outPath);                                   //APDFLDoc's destructor will handle closing both documents.

    std::wcout << L"Success!" << std::endl;

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);                             //If there was an error, display it.

    END_HANDLER

    return errCode;                                            //lib's destructor terminates the library.
}
