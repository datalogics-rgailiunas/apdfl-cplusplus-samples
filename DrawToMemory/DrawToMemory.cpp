// Copyright(c) 2015, Datalogics, Inc.All rights reserved.

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

// This agreement is between Datalogics, Inc. 101 N.Wacker Drive, Suite 1800,
// Chicago, IL 60606 ("Datalogics") and you, an end user who downloads
// source code examples for integrating to the Adobe PDF Library
// ("the Example Code"). By accepting this agreement you agree to be bound
// by the following terms of use for the Example Code.
//
// LICENSE
// -------
// Datalogics hereby grants you a royalty - free, non - exclusive license to
// download and use the Example Code for any lawful purpose.There is no charge
// for use of Example Code.
//
// OWNERSHIP
// ---------
// The Example Code and any related documentation and trademarks are and shall
// remain the sole and exclusive property of Datalogics and are protected by
// the laws of copyright in the U.S.and other countries.
//
// Datalogics is a trademark of Datalogics, Inc.
//
// TERM
// ----
// This license is effective until terminated.You may terminate it at any
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
// NON - INFRINGEMENT, ACCURACY OR COMPLETENESS OF RESPONSES, RESULTS, AND / OR
// LACK OF WORKMANLIKE EFFORT.THE PROVISIONS OF THIS SECTION SET FORTH
// SUBLICENSEE'S SOLE REMEDY AND DATALOGICS'S SOLE LIABILITY WITH RESPECT
// TO THE WARRANTY SET FORTH HEREIN.NO REPRESENTATION OR OTHER AFFIRMATION
// OF FACT, INCLUDING STATEMENTS REGARDING PERFORMANCE OF THE EXAMPLE CODE,
// WHICH IS NOT CONTAINED IN THIS AGREEMENT, SHALL BE BINDING ON DATALOGICS.
// NEITHER DATALOGICS WARRANT AGAINST ANY BUG, ERROR, OMISSION, DEFECT,
// DEFICIENCY, OR NONCONFORMITY IN ANY EXAMPLE CODE.

#include "APDFLDoc.h"
#include "InitializeLibrary.h"

#include "PERCalls.h"
#include "PEWCalls.h"
#include "PDFLExpT.h"
#include "PagePDECntCalls.h"

int main(int agc, char** argv)
{
    APDFLib lib;                                         //Initialize the Adobe PDF Library.

    if (lib.isValid() == false)                          //If it failed to initialize, return the error code.
        return lib.getInitError();

    wchar_t* inPath  = L"../_Input/RasterizeCopy.pdf";   //The path to the input document.
    std::string colorSpace = "DeviceRGB";                //The color space we want the output image to be in. This sample supports DeviceRGB, DeviceCMYK, and DeviceGray.
    const char* filterName = "FlateDecode";              //How we want the output image decoded. Typically FlateDecode, ASCIIHexDecode, LZEDecode, or DCTDecode.
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

    colorSpaceAtom = ASAtomFromString(colorSpace.c_str());
    outColorSpace = PDEColorSpaceCreateFromName(colorSpaceAtom);

    if (colorSpace == "DeviceRGB")
    {
        nComps = 3;
        bitsPerComp = 8;                                            //Only 8 is valid for DeviceRGB.
        backgroundColor = 0xFF;                                     //All components are set to FF to represent white in RGB.
    }
    if (colorSpace == "DeviceGray")
    {
        nComps = 1;
        bitsPerComp = 1;                                            //PDFL supports both 1-bit monochrome grayscale and 8-bit Grayscale. So you could also set this to 1; the image will be dithered automatically.
        backgroundColor = 0x00;                                     //All components are set to 00 to represent white in Grayscale.
    }
    if (colorSpace == "DeviceCMYK")
    {
        nComps = 4;
        bitsPerComp = 8;                                            //Only 8 is valid for DeviceCMYK.
        backgroundColor = 0xFF;                                     //All components are set to FF to represent white in CMYK.
    }

//==================================================================================================================================================================================================================
//Step 3) Render the page's contents to a buffer in memory.
//==================================================================================================================================================================================================================

    //For these methods, we'll need ASDouble matrices. So we translate.
    ASDoubleMatrix userMatrixD;
    userMatrixD.a = (ASDouble)ASFixedToFloat(userMatrix.a);
    userMatrixD.b = (ASDouble)ASFixedToFloat(userMatrix.b);
    userMatrixD.c = (ASDouble)ASFixedToFloat(userMatrix.c);
    userMatrixD.d = (ASDouble)ASFixedToFloat(userMatrix.d);
    userMatrixD.h = (ASDouble)ASFixedToFloat(userMatrix.h);
    userMatrixD.v = (ASDouble)ASFixedToFloat(userMatrix.v);

    ASDoubleRect destRectD;
    destRectD.bottom = (ASDouble)ASFixedToFloat(destRect.bottom);
    destRectD.top    = (ASDouble)ASFixedToFloat(destRect.top);
    destRectD.left   = (ASDouble)ASFixedToFloat(destRect.left);
    destRectD.right  = (ASDouble)ASFixedToFloat(destRect.right);

    ASCab drawFlags = ASCabNew();                                    //This cabinet holds the flags which define how we want the page drawn.
    ASCabPutBool(drawFlags, kPDPageDoLazyEraseStr, true);            //Erase the page while rendering only as needed.
    ASCabPutBool(drawFlags, kPDPageUseAnnotFacesStr, true);          //Draw annotation appearances.
    ASCabPutBool(drawFlags, kPDPageDrawSmoothTextStr, true);         //Anti-alias the text.
    ASCabPutBool(drawFlags, kPDPageDrawSmoothLineArtStr, true);      //Anti-alias the line art.
    ASCabPutBool(drawFlags, kPDPageDrawSmoothImageStr, true);        //Anti-alias the images.

    std::cout << "Allocating memory." << std::endl;

    //Calling this method with a null buffer will calculate how much memory we need to store the page's contents.
    ASInt32 bufferSize = PDPageDrawContentsToMemoryEx(inPage, drawFlags, &userMatrixD, NULL, colorSpaceAtom, bitsPerComp, &destRectD, NULL, 0, NULL, NULL);
    char* buffer = new char[bufferSize];                             //This is the memory we're drawing the page contents to.
    memset(buffer, backgroundColor, bufferSize);                     //In effect, this makes the background color of the image equal to whatever color
                                                                     //    results from setting each component equal to backgroundColor.

    std::cout << "Drawing the page's contents to memory." << std::endl;

    //Finally, the page's contents are rendered to the buffer.
    PDPageDrawContentsToMemoryEx(inPage, drawFlags, &userMatrixD, NULL, colorSpaceAtom, bitsPerComp, &destRectD, buffer, bufferSize, NULL, NULL); 

    //Free used resources.
    ASCabDestroy(drawFlags);
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
    if (((((imageAttrs.width * bitsPerComp * nComps) + 31) / 32) * 4) != ((imageAttrs.width * bitsPerComp * nComps) / 8))
    {
        char *src, *dest;    //Temporary pointers to the bitmap data buffer created by PDPageDrawContentsToMemory.
        int sw, dw;
        sw = ((((imageAttrs.width * bitsPerComp * nComps) + 31) / 32) * 4);

        if (bitsPerComp == 1)
        {
            if (imageAttrs.width / 8 + ((imageAttrs.width % 8)))
                dw = 1;
            else
                dw = 0;
        }
        else
            dw = (imageAttrs.width * bitsPerComp * nComps) / 8;

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
    else
    {
        imageAttrs.width = (((( imageAttrs.width* bitsPerComp * nComps) + 31) / 32) * 4) * 8 / (bitsPerComp * nComps);
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

    std::cout << "Rendering the image from memory." << std::endl;

    PDEImage img = PDEImageCreateEx(&imageAttrs, sizeof(imageAttrs), &imageMatrix, 0, outColorSpace, NULL, &filterArray, 0, (unsigned char*)buffer, bufferSize);

    //Free used resources.
    delete[](buffer);
    PDERelease(reinterpret_cast<PDEObject>(outColorSpace));

//==================================================================================================================================================================================================================
//Step 5) Draw the image to an output document.
//==================================================================================================================================================================================================================

    std::cout << "Drawing the image onto a new document." << std::endl;

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

    std::cout << "Saving..." << std::endl;

    PDPageReleasePDEContent(outPage, 0);                       //Must be released before the page can be released.
    PDPageRelease(outPage);                                    //Must be released before the document can be saved.
    outDoc.saveDoc(outPath);                                   //APDFLDoc's destructor will handle closing both documents.

    std::cout << "Success!" << std::endl;

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);                             //If there was an error, display it.

    END_HANDLER

    return errCode;                                            //lib's destructor terminates the library.
}
