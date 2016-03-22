// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//========================================================================
// Sample: Create a list of all images in a document and their parameters. 
//         Provide a sublist of instances where these images are displayed and 
//         their effective resolutions.
//	
// This sample ignores mages that are used in pattern color spaces.
//
// Steps:
// 1)   Search through the document to find all images and all references.
//      This will locate all images, either in-line or as XObjects, referenced
//      in the document. The process will not find images that are present in the 
//      document but never referenced. Nor will it locate images in the document
//      that are used as GState implied Soft Masks.
//
// 2) Display the list
//========================================================================

#include <iostream>
#include <vector>
using namespace std;
#include <math.h>
#include "InitializeLibrary.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"
#include "CosCalls.h"

#include "APDFLDoc.h"

// Input file to use for example
#if WIN_PLATFORM
wchar_t Input_File[1024] = L"..\\_Input\\FindImageResolutions.pdf";
#else
wchar_t Input_File[1024] = L"../_Input/FindImageResolutions.pdf";
#endif

// These values are used to calculate the rotation specified in a Matrix
#define degrees_to_radians (3.1415926535897932385 / 180.0)

// Define a structure to describe one reference to an image
typedef struct imageReference
{
    ASUns32         page;               // Page number this reference occurs on
    PDEImageAttrs   attrs;              // The image attributes used at reference time
    PDEImage        reference;          // The PDEImage Object which references this image
    ASDoubleMatrix  matrix;             // The matrix in effect at the time of reference
    ASDouble        hRes, vRes;         // Effective Horizontal and Vertical resolutions
    ASDouble        Rotation;           // Rotation angle of the image (in degrees)
    ASDouble        Shear;              // Shear of horizontal/vertical (in degrees)
}ImageRef;

// Define a list of such references 
typedef vector<ImageRef> ImageRefList;

// Define a structure to describe one image defintion, noting the 
// places the structure is referenced in
typedef struct imagedef
{
	ASBool              inLine;                 // This will be "True" if this is an InLine image.
                                                // If this is True, there will be only one reference, 
                                                // and there will be no CosObj.
    CosObj              imageObject;            // The COS Object which defines this image
    ASUns32             imageWide, imageDeep;   // The width and depth of the image in pixels
    ImageRefList       *references;             // A list of places where this image is referenced
    ASBool              isMask;                 // Image is a mask image, applied to another image
    ASBool              isSMask;                // Image is a Soft Mask, applied to another image
} ImageDef;

// Define a list of structures
typedef vector<ImageDef *> ImageList;

// This is a utility routine to rotate a matrix N degrees counterclockwise
void doublematrixrotate (ASDoubleMatrix *M, ASDouble Angle)
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

// This routine displays the list of images just created
void DisplayImageList (ImageList *list, size_t references, FILE *log)
{

    // Display the total count of images and references
    fprintf (log, "Found %01d image data streams, %01d total image references.\n\n", list->size(), references);

    // Display the information for each image
    for (size_t count = 0; count < list->size (); count++)
    {
        ImageDef *current = list->at (count);
        fprintf (log, "Image data stream %s%01d is an %s image %01d samples wide, and %01d samples deep. It is referenced %01d time%s.\n",
            current->isSMask ? "(Soft Mask) " : current->isMask ? "(Mask) " : "",
            count + 1, current->inLine ? "InLine" : "XObject", current->imageWide, current->imageDeep,
            current->references->size(), current->references->size() > 1 ? "s" : "");
        
        // Display all of the references to this image 
        for (size_t count2 = 0; count2 < current->references->size(); count2++)
        {
            ImageRef *currentRef = &current->references->at (count2);
            fprintf (log, "-- Reference %01d is on page %01d and has a resolution of %01g horiziontal DPI, %01g vertical DPI.\n",
                count2 + 1, currentRef->page+1, currentRef->hRes, currentRef->vRes);
            if (currentRef->Rotation)
                fprintf (log, "         Reference is rotated %01.15g degrees\n", currentRef->Rotation);
            if (currentRef->Shear)
                fprintf (log, "         Reference is sheared %01.15g degrees\n", currentRef->Shear);
        }
        // Put a blank line between images
        fprintf (log, "\n");
    }
}

// This routine calculates the horizontal and vertical resolution of 
// each reference to each image. Note that different references to a given
// image may have different effective resolutions.
void CalculateResolution (ImageDef *image, ImageRef *reference)
{
    // The horizontal resolution of the image is the width of the image
    // in pixels divided by the displayed width of that image. The displayed 
    // width is the "matrix.a" value. Likewise, the vertical resolution depth,
    // the image in pixels divided by display depth (matrix.d). 
    //
    // When the image is shown rotated, the actual display size is somewhat
    // harder to determine. Here, we will locate any rotation of shear created
    // when the image is rotated, and create a new matrix, with that rotation
    // removed. They we can simply use the A and D values of the de-rotated matrix.

    // Discover the rotation and horizontally and vertically
    double theta1 = atan2 (reference->matrix.a, -reference->matrix.c);
    double theta2 = atan2 (reference->matrix.d, reference->matrix.b);

    // Convert these to degrees, as an aid in understanding the actual angles used.
    // Note that a 90 degree angle from horizontal to vertical is "erect".
    double degrees1 = -floor((((theta1) / degrees_to_radians) - 90) + 0.5);
    if (degrees1 < 0)
        degrees1 = 360.0 + degrees1;
    double degrees2 = -floor((((theta2) / degrees_to_radians) - 90) + 0.5);
    if (degrees2 < 0)
        degrees2 = 360.0 + degrees2;

    double shear = fabs (fabs (degrees1) - fabs (degrees2));

    reference->Rotation = max (degrees1, degrees2);
    reference->Shear = shear;

    // Shear of 180% is most likely a mirroring, in H or V
    // Subtract shear from angle, and zero shear
    if (shear == 180.0)
    {
        reference->Shear = 0;
        reference->Rotation -= 180.0;
        if (reference->Rotation < 0)
            reference->Rotation += 360.0;
    }

    // "derotate" the image matrix
    ASDoubleMatrix derotating = { 1, 0, 0, 1, 0, 0 };
    doublematrixrotate (&derotating, -reference->Rotation);
    ASDoubleMatrix erect;
    ASDoubleMatrixConcat (&erect, &reference->matrix, &derotating);

    // We use the absolute largest of of each of the horizontal components to find
    // horizontal resolution, and of each fo the vertical components to find vertical
    // resolution. In essence, we are finding the width of any horizontal 1 pixel 
    // "slice" of the image, as it intersects a row of the render media, and the same 
    // for a vertical slice as it intersects a column. We use absolute values, as we 
    // do not care which "direction" the lines are drawn in.
    double  hScale = max (fabs (erect.a), fabs (erect.c));
    double  vScale = max (fabs (erect.d), fabs (erect.b));
    reference->hRes = fabs (image->imageWide / hScale) * 72.0;
    reference->vRes = fabs (image->imageDeep / vScale) * 72.0;

    // Round both resolutions to a whole number
    reference->hRes = floor (reference->hRes + 0.5);
    reference->vRes = floor (reference->vRes + 0.5);

    return;
}

// This routine creates an entry in the image list, and or an existingimages reference list.
// This also checks for masks on an image, and calls itself recursively to process the mask, if there is one.
void CreateImageEntry (ASUns32 pageNo, PDEImage image, ASDoubleMatrix matrix, ImageList *imageList, ASUns32 *imageCount, ASBool mask, ASBool sMask)
{ 
    ImageDef *newImage = (ImageDef *)malloc (sizeof(ImageDef));
    ImageRef newImageRef;

    PDEImageGetAttrs (image, &newImageRef.attrs, sizeof (PDEImageAttrs));
    newImageRef.matrix = matrix;
    newImageRef.reference = image;
    newImageRef.page = pageNo;

    newImage->references = new ImageRefList;

    newImage->isMask = mask;
    newImage->isSMask = sMask;
    PDEImageGetCosObj (image, &newImage->imageObject);

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
            ASFixedMatrix unity = { fixedOne, 0, 0, fixedOne, 0, 0 };
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
        // This is an XObject image. There may be multiple entries, and there MUST be a CosObj
        newImage->inLine = false;
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
        // This is an InLine image. There may be only a single reference to it and there is no CosObj
        newImage->inLine = true;
        newImage->imageObject = CosNewNull ();
        newImage->imageWide = newImageRef.attrs.width;
        newImage->imageDeep = newImageRef.attrs.height;

        CalculateResolution (newImage, &newImageRef);

        newImage->references->push_back (newImageRef);
        imageList->push_back (newImage);
    }

}

// This is a PDE tree walk through a content block. It will always be called with the page content,
// and may recurse to include the contents of elements which are containers.
void FindImagesInContent (ASUns32 pageNumber, PDEContent content, ASDoubleMatrix matrix, ImageList *imageList, ASUns32 *imageCount, ASBool inSoftMask)
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
                (*imageCount)++;
                ASDoubleMatrix imageMatrix;
                PDEElementGetMatrixEx (elem, &imageMatrix);
                ASDoubleMatrixConcat (&imageMatrix, &imageMatrix, &matrix);
                CreateImageEntry (pageNumber, (PDEImage)elem, imageMatrix, imageList, imageCount, false, inSoftMask);
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
                FindImagesInContent (pageNumber, local, localMatrix, imageList, imageCount, inSoftMask);

                // NOTE: PDEFormGetContent "acquires" the content, so it must be 
                //  released. Other elements type that get content do not acquire them.
                PDERelease ((PDEObject)local);
                break;
            }

            // In the case of a container, we parse the container content.
            // In this case, we do NOT concatenate the matrices
            case kPDEContainer:
            {
                PDEContent local = PDEContainerGetContent ((PDEContainer)elem);
                FindImagesInContent (pageNumber, local, matrix, imageList, imageCount, inSoftMask);
                break;
            }

            // In the case of a Group, we parse the group content.
            // In this case, we do NOT concatenate the matrices
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

int main(int argc, char** argv)
{
    APDFLib libInit;                   //Initialize the Adobe PDF Library
    ASErrorCode errCode = 0;           //Variable used to report any exceptions/errors if they occur 
    ImageList imageList;                   // The list of all images in this document
    ASUns32  totalImageReferences = 0;     // A count of all references to image in this document

    if (libInit.isValid() == false)    //If a problem appears with APDFL initializing, return the error code
        return libInit.getInitError();           

    DURING
        //=====================================================================================================================
        // Step 1: Locate all of the images in the document, and all of the references to them.
        //=====================================================================================================================
        APDFLDoc document (Input_File, true);             //Open the document to be analyzed

        // Loop through each page. Find the images on that page 
        // and add them to the image list.
        for (ASUns32 pageNo = 0; pageNo < document.numPages(); pageNo++)
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
        libInit.displayError (errCode);          //If an error occurs display the error message
        return (errCode);
    END_HANDLER

        return (0);                              //APDFLib's destructor terminates the Library
}
