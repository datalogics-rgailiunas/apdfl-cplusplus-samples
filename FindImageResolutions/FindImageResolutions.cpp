// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//========================================================================
// Sample: Create a list of all images in a document, and thier parameters, 
//         With a sublist of instances where this images are displayed, and 
//         thier effective resolutions.
//	
//-> For this case, we are going to ignore images that are used in pattern color spaces!
//
// Steps:
// 1) Walk the document, finding all images and all references
// 2) Display the list
//========================================================================
#define MakeSampleFile 1
#if MakeSampleFile
void MakeSample ();
#endif

#include <iostream>
#include <vector>
using namespace std;

#include "InitializeLibrary.h"
#include "ASExtraCalls.h"
#include "PDCalls.h"
#include "PERCalls.h"
#include "PEWcalls.h"
#include "PagePDECntCalls.h"
#include "CosCalls.h"

#include "APDFLDoc.h"

#ifndef M_PI
#define M_PI       3.1415926535897932385E0  /*Hex  2^ 1 * 1.921FB54442D18 */
#endif

#define degrees_to_radians (M_PI / 180.0)


// Define a structure to describe one reference to an image
typedef struct imageReference
{
    ASSize_t        page;               // Page n umber this reference occurs on.
    PDEImageAttrs   attrs;              // The image attributes used at reference time.
    PDEImage        reference;          // The PDEImage Object which references this image
    ASDoubleMatrix  matrix;             // The matrix in effect at the time of reference
    ASDoubleRect    bbox;               // Bounding box of the image
    ASDoublePoint   center;             // center of image
    ASDouble        hRes, vRes;         // Effective Horiziontal and Vertical resolutions
    ASDouble        Rotation;           // Rotation angle of the image (In Degrees)
    ASDouble        Shear;              // Shear of horiz/vertical (In Degrees)
}ImageRef;

// Define a list of such references 
typedef vector<ImageRef> ImageRefList;

// Define a structure to desribe one image defintion, noting the 
// places the structure is referenced in
typedef struct imagedef
{
    ASBool              inLine;                 // This will be true is this is an InLine image. 
                                                //  If this is the case, then there will be only one reference, 
                                                //  and there will be no CosObj.
    CosObj              imageObject;            //  The COS Object which defines this image
    ASUns32             imageWide, imageDeep;   //  The width and depth of the image in pixels
    ImageRefList       *references;             // A list of places where this image is referenced.
    ASBool              isMask;                 // Image is a mask image, applied to another image
    ASBool              isSMask;                // Image is a Soft Mask, applied to another image
} ImageDef;

// Define a list of such structures
typedef vector<ImageDef *> ImageList;

// This is a utility routine to roate a matrix N degrees counterclockwise
void  	doubelmatrixrotate (ASDoubleMatrix *M, ASDouble Angle)
{
    double	Ad = Angle;
    double	Sina, Cosa;
    double	Ma, Mb, Mc, Md;

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

// This routine just displays the list of images that we created
void DisplayImageList (ImageList *list, size_t references, FILE *log)
{

    // Display the total count of images and references
    fprintf (log, "We found a total of %01d image, referenced %01d times.\n\n", list->size (), references);

    // Display each images information once
    for (size_t count = 0; count < list->size (); count++)
    {
        ImageDef *current = list->at (count);
        fprintf (log, "   Image %s%01d is an %s image %01d pixels wide, and %01d pixels deep. It is referenced %01d times.\n",
            current->isSMask ? "(Soft Mask) " : current->isMask ? "(Mask) " : "",
            count + 1, current->inLine ? "InLine" : "XObject", current->imageWide, current->imageDeep, current->references->size ());
        
        // Display all of the references to this image 
        for (size_t count2 = 0; count2 < current->references->size(); count2++)
        {
            ImageRef *currentRef = &current->references->at (count2);
            fprintf (log, "      Reference %01d is on page %01d and has a resolution of %01g Horiziontal, and %01g vertical. (Matrix [%01g %01g %01g, %01g %01g %01g]\n",
                count2 + 1, currentRef->page+1, currentRef->hRes, currentRef->vRes, currentRef->matrix.a, currentRef->matrix.b,
                currentRef->matrix.c, currentRef->matrix.d, currentRef->matrix.h, currentRef->matrix.v);
            fprintf (log, "        Image is bounded by [%01g %01g %01g %01g], and centered at [%01g %01g].\n",
                currentRef->bbox.left, currentRef->bbox.bottom, currentRef->bbox.right, currentRef->bbox.top, 
                currentRef->center.h, currentRef->center.v);
            if (currentRef->Rotation)
                fprintf (log, "         Image is rotated %01.15g degrees\n", currentRef->Rotation);
            if (currentRef->Shear)
                fprintf (log, "         Image is sheared %01.15g degrees\n", currentRef->Shear);
        }

        // Put a blank line between images
        fprintf (log, "\n");
    }


}

// This routine calculates the horiziontal and vertical resolution of 
// each reference to each image. Note that different references may have
// different effective resolutions
void CalculateResolution (ImageDef *image, ImageRef *reference)
{
    // There are four cases of resolution to handle
    //    Case 1, the image is errect, or inverted
    //    Case 2, the image is rotated 90 degrees colckwise or counterclockwise
    //    Case 3, The image is rotated at some angle other than 90 degrees
    //    Case 4, The image is sheared

    //  Case 1. The image is errect, or inverted
    //     In this case, the b and c values of the matrix are zero
    if ((reference->matrix.b == 0) && (reference->matrix.c == 0))
    {
        // An image that is rotated 0 or 180 degrees is simple. Divide the
        //  size of the image in points (ImageWide or Image Deep) by it's size in 
        //  pixels, and multiple by 72. We do this using absolute values, and then
        //  we get the same value if the image is upright, or inverted, or if it is 
        //  "mirrored" in either or both planes
        reference->hRes = fabs (image->imageWide / reference->matrix.a) * 72.0;
        reference->vRes = fabs (image->imageDeep / reference->matrix.d) * 72.0;
        reference->Rotation = 0;
        reference->Shear = 0;

    }
    else
    {
        // Case 2. The image is rotated 90 degrees
        //   In this case, the matrix a and d values will be zero
        if ((reference->matrix.a == 0) && (reference->matrix.d == 0))
        {
            // An image that is rotated 90 or 270 degrees is also simple. Divide the
            //  size of the image in points (ImageWide or Image Deep) by it's size in 
            //  pixels, and multiple by 72. But "swap" the horiziontal and vertical planes.
            //  We do this using absolute values, and then we get the same value if the 
            // image is upright, or inverted, or if it is "mirrored" in either or both planes
            reference->hRes = fabs (image->imageDeep / reference->matrix.b) * 72.0;
            reference->vRes = fabs (image->imageWide / reference->matrix.c) * 72.0;
            reference->Rotation = 90;
            reference->Shear = 0;

        }
        else
        {

            // Case 3 and 4 We know already that we have a rotation or shear, if we reach here.
            //   If the angle arctan (a,b) is not the same as the angle arctan (c, d), then 
            //   the image is sheared, as well as rotate.
            double theta1 = atan2 (reference->matrix.a, -reference->matrix.b);
            double theta2 = atan2 (reference->matrix.d, reference->matrix.c);

            // Convert these to degrees, as an aid in understanding the actual angles used.
            double degrees1 = fabs (theta1) / degrees_to_radians;
            double degrees2 = fabs (theta2) / degrees_to_radians;

            // When we check that they are the same angle, allow a little "slop"
            if (fabs (fabs (theta1) - fabs (theta2)) < 0.001)
            {
                // If they both represent the same angle, then we are rotated, rather than sheared.
                // The "hieght" and "width" will vary by degree of rotation
                reference->Rotation = degrees1;
                reference->Shear = 0;
            }
            else
            {
                // If they both represent different angles, then we are sheared, And maybe also rotated.
                // The "hieght" and "width" will vary by degree of rotation
                reference->Rotation = min (degrees1, degrees2);
                reference->Shear = fabs (degrees1 - degrees2);
            }

            // Shearing does not really effect resolution. So we can treat both the same
            //
            // "derotate" the image matrix
            ASDoubleMatrix derotating = { 1, 0, 0, 1, 0, 0 };
            doubelmatrixrotate (&derotating, reference->Rotation);
            ASDoubleMatrix erect;
            ASDoubleMatrixConcat (&erect, &reference->matrix, &derotating);

            // We use the absolute largest of of each of the horiziontal components to find
            // horiziontal resolution, and of each fo the vertical components to find vertical
            // resolution. In essence, we are finding the width of any horiziontal, 1 pixel, 
            // "slice" of the image, as it intersects a row of the render media, and the same 
            // for a vertical slice as it intersects a column. We use absolute values, as we 
            // do not care which "direction" the lines are drawn in.
            double  hScale = max (fabs (erect.a), fabs (erect.c));
            double  vScale = max (fabs (erect.d), fabs (erect.b));
            reference->hRes = fabs (image->imageDeep / hScale) * 72.0;
            reference->vRes = fabs (image->imageWide / vScale) * 72.0;
        }
    }

    // Round both resolutions to a whole number
    reference->hRes = floor (reference->hRes + 0.5);
    reference->vRes = floor (reference->vRes + 0.5);

    return;

}


// This routine creates an entry in the image list, and or an existingimages reference list.
// This also checks for maks on an image, and calls itself recursively to process the mask, if there is one.
void CreateImageEntry (ASSize_t pageNo, PDEImage image, ASDoubleMatrix matrix, ImageList *imageList, size_t *imageCount, ASBool mask, ASBool sMask)
{ 
    ImageDef *newImage = (ImageDef *)malloc (sizeof(ImageDef));
    ImageRef newImageRef;

    PDEImageGetAttrs (image, &newImageRef.attrs, sizeof (PDEImageAttrs));
    newImageRef.matrix = matrix;
    newImageRef.reference = image;
    newImageRef.page = pageNo;
    ASFixedRect bbox;
    PDEElementGetBBox ((PDEElement)image, &bbox);
    newImageRef.bbox.left = ASFixedToFloat (bbox.left);
    newImageRef.bbox.right = ASFixedToFloat (bbox.right);
    newImageRef.bbox.top = ASFixedToFloat (bbox.top);
    newImageRef.bbox.bottom = ASFixedToFloat (bbox.bottom);
    newImageRef.center.h = newImageRef.bbox.left + ((newImageRef.bbox.right - newImageRef.bbox.left) / 2.0);
    newImageRef.center.v = newImageRef.bbox.bottom + ((newImageRef.bbox.top - newImageRef.bbox.bottom) / 2.0);

    newImage->references = new ImageRefList;

    newImage->isMask = mask;
    newImage->isSMask = sMask;

    // Check if there is a "stencil mask" applied to this image
    if (((CosObjGetType (newImage->imageObject)) != NULL) &&
        (CosDictKnownKeyString (newImage->imageObject, "Mask")))
    {
        // There is a Mask applied to the image.
        // It may be a "Stencil" mask, or a "Chroma" mask. The former
        // is all we care about. It will be a stencil if the object is a stream
        CosObj mask = CosDictGetKeyString (newImage->imageObject, "Mask");
        if (CosObjGetType (mask) == CosStream)
        {
            ASFixedMatrix unity = { 1, 0, 0, 1, 0, 0 };
            PDEImage imageMask = PDEImageCreateFromCosObj (&mask, &unity, NULL, NULL);
            (*imageCount)++;
            CreateImageEntry (pageNo, imageMask, matrix, imageList, imageCount, true, false);
            PDERelease ((PDEObject)imageMask);
        }
    }

    // Check to see if there is a soft mask image
    PDEImage softMask = PDEImageGetSMask (image);
    if (softMask)
    {
        // If there is a soft mask, then add it to the image list
        (*imageCount)++;
        CreateImageEntry (pageNo, softMask, matrix, imageList, imageCount, false, true);
        PDERelease ((PDEObject)softMask);
    }

    if (newImageRef.attrs.flags & kPDEImageExternal)
    {
        // This is an XObject image. There may be multiple entries, and there MUST be a 
        // CosObj
        newImage->inLine = false;
        PDEImageGetCosObj (image, &newImage->imageObject);
        newImage->imageWide = CosIntegerValue (CosDictGetKeyString (newImage->imageObject, "Width"));
        newImage->imageDeep = CosIntegerValue (CosDictGetKeyString (newImage->imageObject, "Height"));

        CalculateResolution (newImage, &newImageRef);

        // See if we already have an image entry for this image
        for (ASSize_t count = 0; count < imageList->size (); count++)
        {
            if (CosObjEqual (imageList->at(count)->imageObject, newImage->imageObject))
            {
                imageList->at(count)->references->push_back (newImageRef);
                free (newImage->references);
                free (newImage);
                return;
            }
        }
        newImage->references->push_back (newImageRef);
        imageList->push_back (newImage);
    }
    else
    {
        // This is an InLine image. There may be only a single reference too it, and there is no
        // CosObj
        newImage->inLine = true;
        newImage->imageObject = CosNewNull ();
        newImage->imageWide = newImageRef.attrs.width;
        newImage->imageDeep = newImageRef.attrs.height;

        CalculateResolution (newImage, &newImageRef);

        newImage->references->push_back (newImageRef);
        imageList->push_back (newImage);
    }

}

// This is a PDE tree walk through a content block. It will alwyas be called with the page content,
// and may recurse to include the contents of elements which are containers.
void FindImagesInContent (ASSize_t pageNumber, PDEContent content, ASDoubleMatrix matrix, ImageList *imageList, size_t *imageCount, ASBool inSoftMask)
{ 
    for (ASSize_t count = 0; count < PDEContentGetNumElems (content); count++)
    {
        PDEElement elem = PDEContentGetElem (content, count);
        PDEType elemType = (PDEType)PDEObjectGetType ((PDEObject)elem);

        // Some types of PDE Elements may have a GState
        // associated with them, and that gState may specify a 
        // soft mask. If there is one, we want to add it to the list.
        //
        if ((elemType == kPDEForm) || (elemType == kPDEImage) || (elemType == kPDEPath))
        {
            PDEGraphicState gState;
            PDEElementGetGState (elem, &gState, sizeof (PDEGraphicState));
            if ((gState.extGState) && (PDEExtGStateHasSoftMask (gState.extGState)))
            {
                PDESoftMask softMask = PDEExtGStateAcquireSoftMask (gState.extGState);
                if (softMask != NULL)
                {
                    if (PDEObjectGetType ((PDEObject)softMask) == kPDESoftMask)
                    {
                        ASDoubleMatrix softMatrix, formMatrix;
                        PDEForm softForm = PDESoftMaskAcquireFormEx (softMask, &softMatrix);
                        if (softForm != NULL)
                        {
                            PDEContent local = PDEFormGetContent (softForm);
                            PDEFormGetMatrixEx (softForm, &formMatrix);
                            ASDoubleMatrixConcat (&softMatrix, &formMatrix, &softMatrix);
                            ASDoubleMatrixConcat (&softMatrix, &matrix, &softMatrix);
                            FindImagesInContent (pageNumber, local, softMatrix, imageList, imageCount, true);
                            PDERelease ((PDEObject)local);
                            PDERelease ((PDEObject)softForm);
                        }
                    }
                    PDERelease ((PDEObject)softMask);
                }
            }
        }

        // Locate images, and elements that contain contents.
        switch (elemType)
        {
            // In the case of a PDEImage, we create an image entry
            case kPDEImage:
            {
                (*imageCount)++;
                ASDoubleMatrix imageMatrix;
                PDEElementGetMatrixEx (elem, &imageMatrix);
                ASDoubleMatrixConcat (&imageMatrix, &matrix, &imageMatrix);
                CreateImageEntry (pageNumber, (PDEImage)elem, imageMatrix, imageList, imageCount, false, inSoftMask);
                break;
            }

            //In the case of a PDEForm, we parse the forms content, 
            // concatenating the forms matrix to the current matrix
            case kPDEForm:
            {
                PDEContent local = PDEFormGetContent ((PDEForm)elem);
                ASDoubleMatrix localMatrix;
                PDEFormGetMatrixEx ((PDEForm)elem, &localMatrix);
                ASDoubleMatrixConcat (&localMatrix, &matrix, &localMatrix);
                FindImagesInContent (pageNumber, local, localMatrix, imageList, imageCount, inSoftMask);

                // NOTE: PDEFormGetContent "acquires" the content, so it must be 
                //  released. Other elements type that get content do not acquire them.
                PDERelease ((PDEObject)local);
                break;
            }

            //In the case of a container, we parse the container content.
            //  In this case, we do NOT concatenate the matrices
            case kPDEContainer:
            {
                PDEContent local = PDEContainerGetContent ((PDEContainer)elem);
                FindImagesInContent (pageNumber, local, matrix, imageList, imageCount, inSoftMask);
                break;
            }

            // In the case of a Group, we parse the group content.
            //   In this case, we do NOT concatenat the matrices
            case kPDEGroup:
            {
                PDEContent local = PDEGroupGetContent ((PDEGroup)elem);
                FindImagesInContent (pageNumber, local, matrix, imageList, imageCount, inSoftMask);
                break;
            }

            // All other objects, we simply ignore
            default:
                break;

        }
    }
}

void CleanupImageList (ImageList *list)
{ 
    for (ASSize_t count = 0; count < list->size (); count++)
    {
        ImageDef *current = list->at (count);
        free (current->references);
        free (current);
    }
}


int wmain(int argc, wchar_t** argv)
{
    APDFLib libInit;                   //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;           //Variable used to report any exceptions/errors if they occured. 


    ImageList imageList;                    // The list of all images in this document
    ASSize_t  totalImageReferences = 0;     // A count of all references to image in this document

    if (libInit.isValid() == false)    //If there was a problem in initialization, return the error code.
        return libInit.getInitError();           
        

#if MakeSampleFile
    MakeSample ();
#endif

    DURING

//=====================================================================================================================
// Step 1: Locate all of the images in the document, and all of the references too them.
//=====================================================================================================================
        
        APDFLDoc document (argv[1], true);;             //Open the document to be analyzed

        // Loop through each page, findig the images on that page, and 
        // adding them to the image list
        for (ASSize_t pageNo = 0; pageNo < document.numPages(); pageNo++)
        {
            
            PDPage page = document.getPage (pageNo);                    // Acquire the page
            PDEContent content = PDPageAcquirePDEContent (page, 0);     // Acquire the page content

            ASDoubleMatrix unity = { 1.0, 0, 0, 1.0, 0, 0 };            // Initial matrix is Unity

            // This call will find all images on the current page, and add them to the image list
            FindImagesInContent (pageNo, content, unity, &imageList, &totalImageReferences, false);

            PDPageReleasePDEContent (page, 0);                          // Release the page content
            PDPageRelease (page);                                       // Release the page
        }

//=====================================================================================================================
// Step 2: Display the list of images, and references
//=====================================================================================================================
        FILE *log = fopen ("Images.log", "w");
        DisplayImageList (&imageList, totalImageReferences, log);
        fclose (log);


//=====================================================================================================================
// Step 3: Cleanup
//=====================================================================================================================

        CleanupImageList (&imageList);

        HANDLER

            errCode = libInit.getInitError ();

            libInit.displayError (errCode);    //If there was an error, display the error that occured.

            return (errCode);

        END_HANDLER

        return (0);                              //APDFLib's destructor terminates the library.

}
