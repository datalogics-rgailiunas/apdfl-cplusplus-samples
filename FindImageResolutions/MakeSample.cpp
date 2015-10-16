#include "APDFLDoc.h"
#include "ASExpT.h"
#include "ASCalls.h"
#include "CosCalls.h"
#include "PagePDECntCalls.h"
#include "PEWCalls.h"
#include "PERCalls.h"
#include "DLExtrasCalls.h"

typedef struct imagedesc
{
    char            *name;
    PDEImageAttrs    attrs;
    CosObj           cosImage;

}ImageDesc;

ImageDesc ImageTable[] = {
    { "DuckyFiles\\ducky_300.jpg", { 0, 2083, 2275, 8 } },
    { "DuckyFiles\\ducky_438.jpg", { 0, 3041, 3321, 8 } },
    { "DuckyFiles\\ducky_600.jpg", { 0, 4166, 4550, 8 } },
    { "DuckyFiles\\ducky_1200.jpg", { 0, 8333, 9100, 8 } },
    { "DuckyFiles\\ducky_2000.jpg", { 0, 13888, 15166, 8 } },
    { "DuckyFiles\\ducky_2400.jpg", { 0, 16666, 18199, 8 } },
    { "DuckyFiles\\ducky_300_Wbg.jpg", { 0, 2083, 2275, 8 } },
    { "DuckyFiles\\ducky_300_mask.bmp", { 0, 2083, 2275, 8 } },
    { NULL}, {NULL}, { NULL } };
#define ImageTableSize (sizeof(ImageTable)/sizeof(ImageDesc))

typedef enum duckytypes
{
    DPI300 = 0,
    DPI438,
    DPI600,
    DPI1200,
    DPI2000,
    DPI2400,
    DPI300bg,
    DPI300Mask,
    DPI300bgStencil,
    DPI300sMask,
    DPI300bgSmask
} DuckyTypes;



void  	doubelmatrixrotate (ASDoubleMatrix *M, ASDouble Angle);

#ifndef WIN_ENV
#pragma pack(2)
// These are defined in wingdi.h for windows. They are
// included here as an aid to unix systems
//
typedef ASInt8 CHAR;
typedef ASInt16 SHORT;
typedef ASInt32 LONG;
typedef ASUns32       DWORD;
typedef ASUns32       BOOL;
typedef ASUns8       BYTE;
typedef ASUns16      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;

typedef ASInt32                 INT;
typedef ASUns32        UINT;
typedef ASUns32        *PUINT;

/* constants for the biCompression field */
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#define BI_JPEG       4L
#define BI_PNG        5L

typedef struct tagBITMAPFILEHEADER /* Describes the bit map file */
{
    WORD    bfType;                 /* only "BM" will be handled */
    DWORD   bfSize;                 /* Size in bytes of the File */
    WORD    bfReserved1;
    WORD    bfReserved2;
    DWORD   bfOffBits;              /* Displacement to start of map */
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER /* Describes the bit map contents */
{
    DWORD   biSize;             /* Size of This structure
                                ** The color table immediately follows
                                ** this structure! */
    LONG    biWidth;            /* In Pixels */
    LONG    biHeight;           /* In Pixels */
    WORD    biPlanes;           /* Always 1 */
    WORD    biBitCount;         /* 1 = Monochrome image  2 color entries */
    /* 4 =                  16 Color entries */
    /* 8 =                 256 Color Entries */
    /* 24 =                  0 Color Entries */
    DWORD   biCompression;      /* BI_RGB  (0) = uncompressed
                                ** BI_RLE8 (1) = Run length encoded 8 BitsPerPixel
                                ** BI_RLE4 (2) = Run Length Encoded 4 BitsPerPixel */
    DWORD   biSizeImage;        /* Bytes in map, May be zero for BI_RGB */
    LONG    biXPelsPerMeter;    /* Desired original resolution */
    LONG    biYPelsPerMeter;    /* Desired original resolution */
    DWORD   biClrUsed;          /* If zero, Number of entries is via
                                ** biBitCount, else, this is number of entries */
    DWORD   biClrImportant;     /* Ised only if lowering color count */
} BITMAPINFOHEADER;
#endif

void CompleteImageTable (CosDoc doc)
{

    for (size_t count = 0; count < ImageTableSize; count++)
    {
        ImageDesc *current = &ImageTable[count];
        memset (current->attrs.decode, 0, sizeof (current->attrs.decode));
        current->attrs.intent = 0;
        current->attrs.flags = kPDEImageExternal;

        if (current->name == NULL)
            break;

        ASPathName imagePath = ASFileSysCreatePathFromCString (NULL, current->name);
        ASFile imageFile;
        ASFileSysOpenFile64 (NULL, imagePath, ASFILE_READ, &imageFile);

        ASSize_t fileSize = ASFileGetEOF (imageFile);

        if (strstr (current->name, ".jpg"))
        {

            ASStm fileStm = ASFileStmRdOpen (imageFile, 4096);

            CosObj jpegAttributes = CosNewDict (doc, false, 6);
            CosDictPutKeyString (jpegAttributes, "Type", CosNewNameFromString (doc, false, "XObject"));
            CosDictPutKeyString (jpegAttributes, "Subtype", CosNewNameFromString (doc, false, "Image"));
            CosDictPutKeyString (jpegAttributes, "Width", CosNewInteger (doc, false, current->attrs.width));
            CosDictPutKeyString (jpegAttributes, "Height", CosNewInteger (doc, false, current->attrs.height));
            CosDictPutKeyString (jpegAttributes, "ColorSpace", CosNewNameFromString (doc, false, "DeviceRGB"));
            CosDictPutKeyString (jpegAttributes, "BitsPerComponent", CosNewInteger (doc, false, current->attrs.bitsPerComponent));
            CosDictPutKeyString (jpegAttributes, "Filter", CosNewNameFromString (doc, false, "DCTDecode"));

            current->cosImage = CosNewStream (doc, true, fileStm, 0, false, jpegAttributes, CosNewNull (), fileSize);

            ASStmClose (fileStm);
        }
        else
        {
            ASUns8 *buffer = (ASUns8 *)ASmalloc (fileSize);
            ASFileRead (imageFile, (char *)buffer, fileSize);

            BITMAPFILEHEADER *header = (BITMAPFILEHEADER*)buffer;
            BITMAPINFOHEADER *infoHeader = (BITMAPINFOHEADER*)&buffer[sizeof (BITMAPFILEHEADER)];
            ASUns8 *data = (ASUns8 *)&buffer[header->bfOffBits];

            current->attrs.height = infoHeader->biHeight;
            current->attrs.width = infoHeader->biWidth;
            current->attrs.bitsPerComponent = infoHeader->biBitCount;

            CosObj bmpAttributes = CosNewDict (doc, false, 6);
            CosDictPutKeyString (bmpAttributes, "Type", CosNewNameFromString (doc, false, "XObject"));
            CosDictPutKeyString (bmpAttributes, "Subtype", CosNewNameFromString (doc, false, "Image"));
            CosDictPutKeyString (bmpAttributes, "Width", CosNewInteger (doc, false, current->attrs.width));
            CosDictPutKeyString (bmpAttributes, "Height", CosNewInteger (doc, false, current->attrs.height));
            CosDictPutKeyString (bmpAttributes, "BitsPerComponent", CosNewInteger (doc, false, current->attrs.bitsPerComponent));
            CosDictPutKeyString (bmpAttributes, "ImageMask", CosNewBoolean (doc, false, true));
            CosObj decodeArray = CosNewArray (doc, false, 2);
            CosArrayPut (decodeArray, 0, CosNewDouble (doc, false, 1.0));
            CosArrayPut (decodeArray, 1, CosNewDouble (doc, false, 0.0));
            CosDictPutKeyString (bmpAttributes, "Decode", decodeArray);

            // We may need to compress the rows, if the row size is not an even multiple of 32.
            // And we will need to invert the image, so it is bottom up, rather than top down
            ASUns32 rowSize = ((current->attrs.width + 7) / 8);
            ASUns32 unpackedRowSize = infoHeader->biSizeImage / current->attrs.height;
            ASUns8 *newImage = (ASUns8 *)ASmalloc (rowSize * current->attrs.height);
            
            infoHeader->biSizeImage = rowSize * current->attrs.height;
            for (int count = 0; count < current->attrs.height; count++)
                memmove (&newImage[count * rowSize], &data[(current->attrs.height - count - 1) * unpackedRowSize], rowSize);

            ASStm bmpStm = ASMemStmRdOpen ((char *)newImage, infoHeader->biSizeImage);

            current->cosImage = CosNewStream (doc, true, bmpStm, 0, false, bmpAttributes, CosNewNull (), infoHeader->biSizeImage);


            ASStmClose (bmpStm);
            ASfree (buffer);
            ASfree (newImage);
        }
        ASFileClose (imageFile);
        ASFileSysReleasePath (NULL, imagePath);

    }

    // This will manufacture 3 more images.
    // The first will be the 300 dpi ducky with a radial background with the 300 DPI mask applied as a stencil mask. 
    // The second will be the 300 DPI mask as a soft mask.
    // The third will the 300 DPI ducky with a radial background, with the 300 DPI mask applied as a soft mask.

    ImageDesc *current = &ImageTable[DPI300bgStencil];
    ImageDesc *base = &ImageTable[DPI300bg];
    memmove (&current->attrs, &base->attrs, sizeof(ImageDesc));
    current->cosImage = CosObjCopy (base->cosImage, doc, true);
    CosDictPutKeyString (current->cosImage, "Mask", ImageTable[DPI300Mask].cosImage);

    current = &ImageTable[DPI300sMask];
    base = &ImageTable[DPI300Mask];
    memmove (&current->attrs, &base->attrs, sizeof(ImageDesc));
    current->cosImage = CosObjCopy (base->cosImage, doc, true);
    CosDictPutKeyString (current->cosImage, "ColorSpace", CosNewNameFromString (doc, false, "DeviceGray"));
    CosDictRemoveKeyString (current->cosImage, "ImageMask");
    CosDictRemoveKeyString (current->cosImage, "Decode");

    current = &ImageTable[DPI300bgSmask];
    base = &ImageTable[DPI300bg];
    memmove (&current->attrs, &base->attrs, sizeof(ImageDesc));
    current->cosImage = CosObjCopy (base->cosImage, doc, true);
    CosDictPutKeyString (current->cosImage, "SMask", ImageTable[DPI300sMask].cosImage);


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

// Create the image inside a form.
// The form will be set 1/2 size, the image twice size
// The form with rotate twice angle, and the form will rotate -angle.
void InsertImageInForm (CosDoc doc, PDEContent content, ASInt32 imageIndex, double X, double Y, double wide, double deep, double angle)
{
    PDEContent formContent = PDEContentCreate ();

    PDEColorSpace rgbSpace = PDEColorSpaceCreateFromName (ASAtomFromString ("DeviceRGB"));

    // Position the image in the form with it's center in the middle of the form.
    // Scale up by 2 (The form will be 1/2 size). 
    ASDoubleMatrix imageMatrix = { wide * 2, 0, 0, deep * 2, wide, deep};

    // The form will be rotated -angle, so rotate the image double the angle
    doubelmatrixrotate (&imageMatrix, angle * 2);
    ASDoubleMatrix trans = { 1, 0, 0, 1, 0, 0 };
    doubelmatrixrotate (&trans, angle*2);
    ASDoublePoint place = { -wide, -deep };
    ASDoubleMatrixTransform (&place, &trans, &place);
    imageMatrix.h += place.h;
    imageMatrix.v += place.v;

    PDEImage newImage = PDEImageCreateFromCosObjEx (&ImageTable[imageIndex].cosImage,
        &imageMatrix, rgbSpace, NULL);
    PDEContentAddElem (formContent, kPDEAfterLast, (PDEElement)newImage);

    PDERelease ((PDEObject)newImage);
    PDERelease ((PDEObject)rgbSpace);

    CosObj formCos, formResources;
    PDEContentToCosObjEx (formContent, kPDEContentToForm, NULL, sizeof (PDEContentAttrs), 
                          doc, NULL, &formCos, &formResources);

    ASDoubleMatrix formMatrix = { 0.5, 0, 0, 0.5, X + wide/2, Y + deep/2 };
    doubelmatrixrotate (&formMatrix, -angle);
    ASDoubleMatrix trans2 = { 1.0, 0, 0, 1.0, 0, 0 };
    doubelmatrixrotate (&trans2, -angle);
    ASDoublePoint place2 = { -wide/2, -deep/2 };
    ASDoubleMatrixTransform (&place2, &trans2, &place2);
    formMatrix.h += place2.h;
    formMatrix.v += place2.v;

    PDEForm form = PDEFormCreateFromCosObjEx (&formCos, &formResources, &formMatrix);
    PDEFormCalcBBox (form);

    PDEContentAddElem (content, kPDEAfterLast, (PDEElement)form);
    PDERelease ((PDEObject)form);
}

// Create the image inside a form inside a form
// The form will be set double size, the second form 1/4 size, the image twice size
// The form with rotate twice angle, and the form will rotate 3 * -angle, the image at double angle.
void InsertImageInFormInForm (CosDoc doc, PDEContent content, ASInt32 imageIndex, double X, double Y, double wide, double deep, double angle)
{
    PDEContent formContent = PDEContentCreate ();

    PDEColorSpace rgbSpace = PDEColorSpaceCreateFromName (ASAtomFromString ("DeviceRGB"));

    // Position the image in the form with it's center in the middle of the form.
    // Scale up by 2 (The form will be 1/2 size). 
    ASDoubleMatrix imageMatrix = { wide * 2, 0, 0, deep * 2, wide, deep };

    // The form will be rotated -angle, so rotate the image double the angle
    doubelmatrixrotate (&imageMatrix, angle * 2);
    ASDoubleMatrix trans = { 1, 0, 0, 1, 0, 0 };
    doubelmatrixrotate (&trans, angle * 2);
    ASDoublePoint place = { -wide, -deep };
    ASDoubleMatrixTransform (&place, &trans, &place);
    imageMatrix.h += place.h;
    imageMatrix.v += place.v;

    PDEImage newImage = PDEImageCreateFromCosObjEx (&ImageTable[imageIndex].cosImage,
        &imageMatrix, rgbSpace, NULL);
    PDEContentAddElem (formContent, kPDEAfterLast, (PDEElement)newImage);

    PDERelease ((PDEObject)newImage);
    PDERelease ((PDEObject)rgbSpace);

    CosObj formCos, formResources;
    PDEContentToCosObjEx (formContent, kPDEContentToForm, NULL, sizeof (PDEContentAttrs),
        doc, NULL, &formCos, &formResources);

    ASDoubleMatrix formMatrix = { 0.25, 0, 0, 0.25, X + wide / 2, Y + deep / 2 };
    doubelmatrixrotate (&formMatrix, -angle * 3);
    ASDoubleMatrix trans2 = { 1.0, 0, 0, 1.0, 0, 0 };
    doubelmatrixrotate (&trans2, -angle * 3);
    ASDoublePoint place2 = { -wide / 2, -deep / 2 };
    ASDoubleMatrixTransform (&place2, &trans2, &place2);
    formMatrix.h += place2.h;
    formMatrix.v += place2.v;

    PDEForm form = PDEFormCreateFromCosObjEx (&formCos, &formResources, &formMatrix);
    PDEFormCalcBBox (form);
    PDERelease ((PDEObject)content);

    PDEContent form2Content = PDEFormGetContent (form);
    CosObj form2Cos, form2Resources;
    PDEContentToCosObjEx (form2Content, kPDEContentToForm, NULL, sizeof (PDEContentAttrs),
        doc, NULL, &form2Cos, &form2Resources);

    ASDoubleMatrix form2Matrix = { 2, 0, 0, 2, X + wide / 2, Y + deep / 2 };
    doubelmatrixrotate (&formMatrix, angle * 2);
    ASDoubleMatrix trans3 = { 1.0, 0, 0, 1.0, 0, 0 };
    doubelmatrixrotate (&trans3, angle * 3);
    ASDoublePoint place3 = { -wide / 2, -deep / 2 };
    ASDoubleMatrixTransform (&place3, &trans3, &place3);
    formMatrix.h += place3.h;
    formMatrix.v += place3.v;

    PDEForm form2 = PDEFormCreateFromCosObjEx (&form2Cos, &form2Resources, &form2Matrix);
    PDEFormCalcBBox (form2);

    PDERelease ((PDEObject)form2Content);

    PDEContentAddElem (content, kPDEAfterLast, (PDEElement)form2);
    PDERelease ((PDEObject)form);
    PDERelease ((PDEObject)form2);
}
void MakeSample ()
{
    // Make a sample file for find image resolutions
    APDFLDoc sample;
    
    // Make actual PDEImages
    CosDoc cosDoc = PDDocGetCosDoc (sample.getPDDoc ());
    CompleteImageTable (cosDoc);

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

    // Page 2 contains 25 duckies. All using the 300 DPI Ducky
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
            InsertImage (content, DPI300, X, Y, 72.0, 78.6365, angle);
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

    // Page 3 contains 25 duckies. All using the 300 DPI Ducky with a Background Radial
    // Stencil Masked with the 300 DPI image mask, applied as a stencil mask
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
            InsertImage (content, DPI300bgStencil, X, Y, 72.0, 78.6365, angle);
            angle += 15;
            X += 1.5 * 72;
            if (X >= 8.25 * 72)
            {
                X = 0.75 * 72;
                Y -= 2.0 * 72.0;
            }
        }
    }
    PDPageSetPDEContent (page, 0);
    PDPageReleasePDEContent (page, 0);
    PDPageRelease (page);

    // Page 4 contains 25 duckies. All using the 300 DPI Ducky with a Background Radial
    // Soft Masked with the 300 DPI image mask, applied as a stencil mask
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
            InsertImage (content, DPI300bgSmask, X, Y, 72.0, 78.6365, angle);
            angle += 15;
            X += 1.5 * 72;
            if (X >= 8.25 * 72)
            {
                X = 0.75 * 72;
                Y -= 2.0 * 72.0;
            }
        }
    }
    PDPageSetPDEContent (page, 0);
    PDPageReleasePDEContent (page, 0);
    PDPageRelease (page);

    // Page 5 contains 25 duckies. All using the 300 DPI Ducky
    // image, each is 72 points by 78.6365. The first image is erect, 
    // each successive image is rotated 15 degrees counter clockwise
    // from the previous, through 360 derees.
    //
    // But here, each image is twice as large, and double rotated, centered 
    // in a form, which is itself scaled to 1/2 size, and counter rotated.
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
            InsertImageInForm (cosDoc, content, DPI300, X, Y, 72.0, 78.6365, angle);
            angle += 15;
            X += 1.5 * 72;
            if (X >= 8.25 * 72)
            {
                X = 0.75 * 72;
                Y -= 2.0 * 72.0;
            }
        }
    }
    PDPageSetPDEContent (page, 0);
    PDPageReleasePDEContent (page, 0);
    PDPageRelease (page);

    // Page 2 contains 25 duckies. All using the 300 DPI Ducky
    // image, each is 72 points by 78.6365. The first image is erect, 
    // each successive image is rotated 15 degrees counter clockwise
    // from the previous, through 360 derees.
    //
    // But here, each image is twice as large, and double rotated, centered
    // in a form, which is itself scaled to 1/4 size, and counter rotated angle * 3
    // which is lcoated within a form scaled by 2, rotated angle * 2
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
            InsertImageInForm (cosDoc, content, DPI300, X, Y, 72.0, 78.6365, angle);
            angle += 15;
            X += 1.5 * 72;
            if (X >= 8.25 * 72)
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