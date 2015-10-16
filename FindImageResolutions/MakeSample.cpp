#include "APDFLDoc.h"
#include "ASExpT.h"
#include "CosCalls.h"
#include "PagePDECntCalls.h"
#include "PEWCalls.h"
#include "PERCalls.h"

typedef struct imagedesc
{
    char            *name;
    PDEImageAttrs    attrs;
    PDEImage         unMaskedImage;
    CosObj           cosImage;

}ImageDesc;

ImageDesc ImageTable[] = {
    { "DuckyFiles\\ducky_300.jpg", { 0, 2083, 2275, 8 } },
    { "DuckyFiles\\ducky_438.jpg", { 0, 3041, 3321, 8 } },
    { "DuckyFiles\\ducky_600.jpg", { 0, 4166, 4550, 8 } },
    { "DuckyFiles\\ducky_1200.jpg", { 0, 8333, 9100, 8 } },
    { "DuckyFiles\\ducky_2000.jpg", { 0, 13888, 15166, 8 } },
    { "DuckyFiles\\ducky_2400.jpg", { 0, 16666, 18199, 8 } } };
#define ImageTableSize (sizeof(ImageTable)/sizeof(ImageDesc))

void  	doubelmatrixrotate (ASDoubleMatrix *M, ASDouble Angle);

void CompleteImageTable (CosDoc doc)
{

    PDEColorSpace rgbSpace = PDEColorSpaceCreateFromName (ASAtomFromString ("DeviceRGB"));

    for (size_t count = 0; count < ImageTableSize; count++)
    {
        ImageDesc *current = &ImageTable[count];
        memset (current->attrs.decode, 0, sizeof (current->attrs.decode));
        current->attrs.intent = 0;
        current->attrs.flags = kPDEImageExternal;

        ASPathName imagePath = ASFileSysCreatePathFromCString (NULL, current->name);
        ASFile imageFile;
        ASFileSysOpenFile64 (NULL, imagePath, ASFILE_READ, &imageFile);

        ASSize_t fileSize = ASFileGetEOF (imageFile);
        ASStm fileStm = ASFileStmRdOpen (imageFile, 4096);

        CosObj jpegAttributes = CosNewDict (doc, false, 6);
        CosDictPutKeyString (jpegAttributes, "Type", CosNewNameFromString (doc, false, "XObject"));
        CosDictPutKeyString (jpegAttributes, "Subtype", CosNewNameFromString (doc, false, "Image"));
        CosDictPutKeyString (jpegAttributes, "Width", CosNewInteger (doc, false, current->attrs.width));
        CosDictPutKeyString (jpegAttributes, "Height", CosNewInteger (doc, false, current->attrs.height));
        CosDictPutKeyString (jpegAttributes, "ColorSpace", CosNewNameFromString (doc, false, "DeviceRGB"));
        CosDictPutKeyString (jpegAttributes, "BitsPerComponent", CosNewInteger (doc, false, 8));
        CosDictPutKeyString (jpegAttributes, "Filter", CosNewNameFromString (doc, false, "DCTDecode"));

        current->cosImage = CosNewStream (doc, true, fileStm, 0, false, jpegAttributes, CosNewNull (), fileSize);

        ASStmClose (fileStm);
        ASFileClose (imageFile);
        ASFileSysReleasePath (NULL, imagePath);

    }

    PDERelease ((PDEObject)rgbSpace);
    return;
}

void InsertImage (PDEContent content, ASInt32 imageIndex, double X, double Y, double wide, double deep, double angle)
{
    PDEColorSpace rgbSpace = PDEColorSpaceCreateFromName (ASAtomFromString ("DeviceRGB"));
    ASDoubleMatrix imageMatrix = { wide, 0, 0, deep, X + (wide/2.0), Y + (deep/2.0)};
    doubelmatrixrotate (&imageMatrix, angle);
    ASDoubleMatrix trans = { 1, 0, 0, 1,0,0 };
    doubelmatrixrotate (&trans, angle);
    ASDoublePoint place = { -wide / 2.0, -deep / 2.0 };
    ASDoubleMatrixTransform (&place, &trans, &place);
    imageMatrix.h += place.h;
    imageMatrix.v += place.v;


    PDEImage newImage = PDEImageCreateFromCosObjEx (&ImageTable[imageIndex].cosImage,
                                                    &imageMatrix, rgbSpace, NULL);
    PDEContentAddElem (content, kPDEAfterLast, (PDEElement)newImage);

    PDERelease ((PDEObject)newImage);
    PDERelease ((PDEObject)rgbSpace);

}

void MakeSample ()
{
    // Make a sample file for find image resolutions
    APDFLDoc sample;
    
    // Make actual PDEImages
    CompleteImageTable (PDDocGetCosDoc (sample.getPDDoc()));

    /* Create a page with 6 images
    .. All images are Ducky, at various resolutions
       0.75 left and right bounds, 2 inch images, with 0.5 between
       top row is 1 inch below page top, and 157.273 points deep (To retain aspect ratio
       Second row is 4 inches below top and 157.273 points deep.
    */
    sample.insertPage (ASFloatToFixed (8.5 * 72), ASFloatToFixed (11.0 * 72), PDBeforeFirstPage);
    PDPage page = sample.getPage (0);
    PDEContent content = PDPageAcquirePDEContent (page, 0);

    ASDouble X = 0.75 * 72.0;
    ASDouble Y = 7 * 72.0;
    for (int count = 0; count < 6; count++)
    {
        InsertImage (content, count, X, Y, 144.0, 157.273, 0);
        X += 2.5 * 72;
        if (count == 2)
        {
            X = 0.75 * 72;
            Y -= 3.0 * 72.0;
        }
    }

    PDPageSetPDEContent (page, 0);
    PDPageReleasePDEContent (page, 0);
    PDPageRelease (page);

    // Page 2 contains 25 duckies. All using the 300 DPI
    // image, each is 72 points by 78.6365. The first image is erect, 
    // each successive image is rotated 15 degrees counter clockwise
    // from the previous, through 360 derees.
    sample.insertPage (ASFloatToFixed (8.5 * 72), ASFloatToFixed (11.0 * 72),
        sample.numPages () - 1);
    page = sample.getPage (sample.numPages () - 1);
    content = PDPageAcquirePDEContent (page, 0);
    X = 0.75 * 72.0;
    Y = 9.5 * 72.0;
    for (double angle = 0; angle < 360;)
    {
        for (int count = 0; count < 25; count++)
        {
            InsertImage (content, 0, X, Y, 72.0, 78.6365, angle);
            angle += 15;
            X += 1.5 * 72;
            if (X >= 8.25 * 72 )
            {
                X = 0.75 * 72;
                Y -= 2.0 * 72.0;
            }
        }
    }
    PDPageSetPDEContent (page, 0);
    PDPageReleasePDEContent (page, 0);
    PDPageRelease (page);


    sample.saveDoc (L"FindImageResolutions.pdf", PDSaveFull | PDSaveCollectGarbage);
    return;
}