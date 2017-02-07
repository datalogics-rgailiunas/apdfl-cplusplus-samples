//
// Copyright (c) 2015-2017, Datalogics, Inc. All rights reserved.
//
// For complete copyright information, refer to:
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
// Sample: FindImageResolutions - Create a list of all images in a document, and their parameters, 
//         with a sublist of instances where these images are displayed, and 
//         their effective resolutions.
//      
// For simplicity, we are going to ignore images that are used in pattern color spaces!
//
// Steps:
// 0) [Optional, performed when no file name is specified on command line:] Perhaps the more
//      edifying part of this sample is the production of a meaty sample file.  This is
//      performed by the code in the second source file, which produces a multi-page document
//      with oodles of images in all sorts of orientations and positions.  The program 
//      then performs its analytics on the newly created file.
// 1) Walk the document, finding all images and all references
//      This will locate all images, either in-line, or XObjects, referenced
//      in the document. It will not locate images that are present in the 
//      document but never referenced, nor will it locate images in the document
//      that are used as GState implied Soft Masks.
// 2) Print the list to an output file.
//
// Command line arguments:  <InputFileName>   (optional - defaults to SAMPLE_FILE)
//

#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <algorithm>

#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"
#include "CosCalls.h"

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#include "FindImageResolutions.h"

#define SAMPLE_FILE  "FindImageResolutions.pdf"
#define OUTPUT_FILE "FindImageResolutions-out.txt"


int main(int argc, char* argv[])
{
    APDFLib libInit;                   //Initialize the Adobe PDF Library.
    if (libInit.isValid() == false)    //If there was a problem in initialization, return the error code.
    {
        int errCode = libInit.getInitError();
        APDFLib::displayError ( errCode );
        return errCode;
    }    

    // If a file name was specified on the command line, we will process that file.
    // Without a file name specified, we will produce the canned sample file and process it.
    bool fMakeSample ( false);
    std::string csInputFile; 
    if ( argc > 1 )
    {
        csInputFile = argv[1];
    }
    else
    {
        csInputFile = SAMPLE_FILE;
        fMakeSample = true;
    }
    std::string csOutputFileName ( OUTPUT_FILE );

    // Step 0: Produce the input file is requested
    if ( fMakeSample )
    {
        std::cout << "Producing sample file " << SAMPLE_FILE << std::endl;
        if ( MakeSample() )
        {
            std::cout << "Failed creating the sample file." << std::endl;
            return -1;
        }
    }

    ImageList imageList;                    // The list of all images in this document

DURING

    std::cout << "Analyzing file " << csInputFile.c_str() << "; writing results to " 
              << csOutputFileName.c_str() << std::endl;
    APDFLDoc document (csInputFile.c_str(), true);                  //Open the document to be analyzed

    // Step 1: Locate all images by walking the document and saving images and references
    for (ASSize_t pageNo = 0; pageNo < document.numPages(); pageNo++)
    {
        PDPage page = document.getPage (pageNo);                    // Acquire the page
        PDEContent content = PDPageAcquirePDEContent (page, 0);     // Acquire the page content

        ASDoubleMatrix unity = { 1.0, 0, 0, 1.0, 0, 0 };            // Initial matrix is Unity

        // This call will find all images on the current page, and add them to the image list
        FindImagesInContent (pageNo, content, unity, &imageList, false);

        PDPageReleasePDEContent (page, 0);                          // Release the page content
        PDPageRelease (page);                                       // Release the page
    }

    // Step 2: Output the results
    std::ofstream ofs ( csOutputFileName.c_str() );
    ofs << "Processing file " << csInputFile.c_str() << std::endl
        << "We found a total of " << imageList.size() << " images, referended " 
        << std::accumulate ( imageList.begin(), imageList.end(), 0, RefAccumulator() ) << " times\n\n";
    int cnt;
    std::vector<ImageDef>::const_iterator it, itE = imageList.end();
    for ( cnt = 1, it = imageList.begin(); it != itE; ++it, ++cnt )
    {
        ofs << "   Image " << cnt << *it;
    }

HANDLER
    APDFLib::displayError(ERRORCODE);
    return ERRORCODE;
END_HANDLER

    return 0;                              //APDFLib's destructor terminates the library.
}

// This routine calculates the horizontal and vertical resolution of 
// each reference to each image. Note that different references may have
// different effective resolutions
void CalculateResolution (ImageDef *image, ImageRef *reference)
{
    // We want to find the resolution of the image as if it were not rotated.
    // To that end, we need to discover if it IS rotated, and create a Matrix
    // as it would be if the image were not rotated.

    // Discover the rotation and horizontally, and vertically
    double theta1 = atan2 (reference->m_matrix.a, -reference->m_matrix.c);
    double theta2 = atan2 (reference->m_matrix.d, reference->m_matrix.b);

    // Convert these to degrees, as an aid in understanding the actual angles used.
    // Note that a 90 degree angle from horizontal to vertical is "erect".
    double degrees1 = -floor((((theta1) / degrees_to_radians) - 90) + 0.5);
    if (degrees1 < 0)
        degrees1 = 360.0 + degrees1;
    double degrees2 = -floor((((theta2) / degrees_to_radians) - 90) + 0.5);
    if (degrees2 < 0)
        degrees2 = 360.0 + degrees2;

    double shear = fabs (fabs (degrees1) - fabs (degrees2));

    reference->m_rotation = (std::max) (degrees1, degrees2);
    reference->m_shear = shear;

    // m_shear of 180% is most likely a mirroring, in H or V
    // Subtract shear from angle, and zero shear
    if (shear == 180.0)
    {
        reference->m_shear = 0;
        reference->m_rotation -= 180.0;
        if (reference->m_rotation < 0)
            reference->m_rotation += 360.0;
    }

    // "de-rotate" the image matrix
    ASDoubleMatrix derotating = { 1, 0, 0, 1, 0, 0 };
    DoubleMatrixRotate (&derotating, -reference->m_rotation);
    ASDoubleMatrix erect;
    ASDoubleMatrixConcat (&erect, &reference->m_matrix, &derotating);

    // We use the absolute largest of each of the horizontal components to find
    // horizontal resolution, and of each of the vertical components to find vertical
    // resolution. In essence, we are finding the width of any horizontal, 1 pixel, 
    // "slice" of the image, as it intersects a row of the render media, and the same 
    // for a vertical slice as it intersects a column. We use absolute values, as we 
    // do not care which "direction" the lines are drawn in.
    double  hScale = (std::max) (fabs (erect.a), fabs (erect.c));
    double  vScale = (std::max) (fabs (erect.d), fabs (erect.b));
    reference->m_hRes = fabs (image->m_width / hScale) * 72.0;
    reference->m_vRes = fabs (image->m_depth / vScale) * 72.0;


    // Round both resolutions to a whole number
    reference->m_hRes = floor (reference->m_hRes + 0.5);
    reference->m_vRes = floor (reference->m_vRes + 0.5);

    return;
}

// This routine creates an entry in the image list, and/or an existing images reference list.
// This also checks for masks on an image, and calls itself recursively to process the mask, if there is one.
void CreateImageEntry (ASSize_t pageNo, PDEImage image, ASDoubleMatrix matrix, ImageList *imageList, ASBool mask, ASBool smask)
{ 
    ImageDef newImage ( image, mask, smask );

    // Check if there is a "stencil mask" applied to this image
    if (((CosObjGetType (newImage.m_cos_object)) != 0 ) &&
        (CosDictKnownKeyString (newImage.m_cos_object, "Mask")))
    {
        // There is a Mask applied to the image.  It may be a "Stencil" mask, or a "Chroma" mask. 
        // The former is all we care about. It will be a stencil if the object is a stream.
        CosObj mask = CosDictGetKeyString (newImage.m_cos_object, "Mask");
        if (CosObjGetType (mask) == CosStream)
        {
            ASFixedMatrix unity = { 1, 0, 0, 1, 0, 0 };
            PDEImage imageMask = PDEImageCreateFromCosObj (&mask, &unity, NULL, NULL);
            CreateImageEntry (pageNo, imageMask, matrix, imageList, true, false);
            PDERelease ((PDEObject)imageMask);
        }
    }

    // Check to see if there is a soft mask image
    PDEImage softMask = PDEImageGetSMask (image);
    if (softMask)
    {
        // If there is a soft mask, then add it to the image list
        CreateImageEntry (pageNo, softMask, matrix, imageList, false, true);
        PDERelease ((PDEObject)softMask);
    }

    ImageRef newImageRef;
    PDEImageGetAttrs (image, &newImageRef.m_attrs, sizeof (PDEImageAttrs));
    newImageRef.m_matrix = matrix;
    newImageRef.m_reference = image;
    newImageRef.m_page = pageNo;

    if (newImageRef.m_attrs.flags & kPDEImageExternal)
    {
        // This is an XObject image. There may be multiple entries, and there MUST be a CosObj
        newImage.m_inline = false;
        newImage.m_width = CosIntegerValue (CosDictGetKeyString (newImage.m_cos_object, "Width"));
        newImage.m_depth = CosIntegerValue (CosDictGetKeyString (newImage.m_cos_object, "Height"));

        CalculateResolution (&newImage, &newImageRef);

        // See if we already have an image entry for this image
        for (ASSize_t count = 0; count < imageList->size (); count++)
        {
            if (CosObjEqual (imageList->at(count).m_cos_object, newImage.m_cos_object))
            {
                imageList->at(count).m_refs.push_back (newImageRef);
                return;
            }
        }
        newImage.m_refs.push_back (newImageRef);
        imageList->push_back (newImage);
    }
    else
    {
        // This is an InLine image. There may be only a single reference too it, and there is no CosObj
        newImage.m_inline = true;
        newImage.m_cos_object = CosNewNull ();
        newImage.m_width = newImageRef.m_attrs.width;
        newImage.m_depth = newImageRef.m_attrs.height;

        CalculateResolution (&newImage, &newImageRef);

        newImage.m_refs.push_back (newImageRef);
        imageList->push_back (newImage);
    }
}

// This is a PDE tree walk through a content block. It will always be called with the page content,
// and may recurse to include the contents of elements which are containers.
void FindImagesInContent (ASSize_t pageNumber, PDEContent content, ASDoubleMatrix matrix, ImageList *imageList, ASBool inSoftMask)
{ 
    for (ASInt32 count = 0; count < PDEContentGetNumElems (content); count++)
    {
        PDEElement elem = PDEContentGetElem (content, count);

        // Locate images, and elements that contain contents.
        switch (PDEObjectGetType ((PDEObject)elem))
        {
            // In the case of a PDEImage, we create an image entry
            case kPDEImage:
            {
                ASDoubleMatrix imageMatrix;
                PDEElementGetMatrixEx (elem, &imageMatrix);
                ASDoubleMatrixConcat (&imageMatrix, &imageMatrix, &matrix);
                CreateImageEntry (pageNumber, (PDEImage)elem, imageMatrix, imageList, false, inSoftMask);
                break;
            }

            // In the case of a PDEForm, we parse the forms content, 
            // concatenating the forms matrix to the current matrix
            case kPDEForm:
            {
                PDEContent local = PDEFormGetContent ((PDEForm)elem);
                ASDoubleMatrix localMatrix;
                PDEFormGetMatrixEx ((PDEForm)elem, &localMatrix);
                ASDoubleMatrixConcat (&localMatrix, &localMatrix, &matrix);
                FindImagesInContent (pageNumber, local, localMatrix, imageList, inSoftMask);

                // NOTE: PDEFormGetContent "acquires" the content, so it must be 
                // released. Other elements type that get content do not acquire them.
                PDERelease ((PDEObject)local);
                break;
            }

            // In the case of a container, we parse the container content.
            // In this case, we do NOT concatenate the matrices
            case kPDEContainer:
            {
                PDEContent local = PDEContainerGetContent ((PDEContainer)elem);
                FindImagesInContent (pageNumber, local, matrix, imageList, inSoftMask);
                break;
            }

            // In the case of a group, we parse the group content.
            // In this case, we do NOT concatenate the matrices
            case kPDEGroup:
            {
                PDEContent local = PDEGroupGetContent ((PDEGroup)elem);
                FindImagesInContent (pageNumber, local, matrix, imageList, inSoftMask);
                break;
            }

            // All other objects, we simply ignore
            default:
                break;

        }
    }
}

// This is a utility routine to rotate a matrix N degrees counterclockwise
void DoubleMatrixRotate (ASDoubleMatrix *M, ASDouble Angle)
{
    double      Ad = Angle;
    double      Sina, Cosa;
    double      Ma, Mb, Mc, Md;

    while (Ad < 0)
        Ad += 360;

    while (Ad > 360.0)
        Ad = Ad - 360.0;

    if (Ad < 0.0001)
        return;

    Ad *= degrees_to_radians;
    Ma = M->a;
    Mb = M->b;
    Mc = M->c;
    Md = M->d;
    Sina = sin (Ad);
    Cosa = cos (Ad);

    M->a = (Cosa * Ma) + (Sina * Mc);
    M->b = (Cosa * Mb) + (Sina * Md);
    M->c = (Cosa * Mc) - (Sina * Ma);
    M->d = (Cosa * Md) - (Sina * Mb);

    return;
}
