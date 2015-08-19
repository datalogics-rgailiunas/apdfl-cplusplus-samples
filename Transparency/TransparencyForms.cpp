// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//===========================================================================
// Sample: Transparency - ??????????
//
//Steps:
// 1) ?????????????
//===========================================================================

#include <sstream>
#include <string>
#include <vector>
#include <sstream>

#include "APDFLDoc.h"
#include "InitializeLibrary.h"

#include "PERCalls.h"
#include "PSFCalls.h"
#include "PagePDECntCalls.h"
#include "PDExpT.h"
#include "ASCalls.h"
#include "PEWCalls.h"
#include "ASCalls.h"
#include "ASExtraCalls.h"
#include "CosCalls.h"

int main(int argc, char** argv)
{
    APDFLib lib;                                                  //Initialize the Adobe PDF Library.
    if (lib.isValid() == false)                                   //If it failed to initialize, return the error code.
        return lib.getInitError();

    ASErrorCode errCode = 0;                                      //Tracks runtime errors in the application.

    //All twelve blend modes available in APDFL. See the Adobe PDF Reference, sixth edition, section 7.2.4, table 7.2 for a description of each.
    //Available here: http://www.adobe.com/content/dam/Adobe/en/devnet/acrobat/pdfs/pdf_reference_1-7.pdf
    std::vector<char*> blendModes {
        "Normal","Multiply","Screen","Overlay",
        "Darken","Lighten","ColorDodge","ColorBurn",
        "HardLight","SoftLight","Difference","Exclusion"
    };

    DURING

        APDFLDoc doc;                                                                              //Creates a new, blank page.
        PDDoc pdoc = doc.getPDDoc();

        ASFixed pageLength   = ASFloatToFixed(12.0 * 72.0);    //12 inches wide.
        ASFixed pageHeight   = ASFloatToFixed(6.0  * 72.0);    //6 inches high.

        int NUM_PAGES = blendModes.size();
        for (int i = 0; i < NUM_PAGES; ++i){
            doc.insertPage(pageLength, pageHeight, PDBeforeFirstPage);      //Give it a page of 7.5 inches square dimensions.
        }

        //The origin of a rectangle is at its bottom-left corner, thus the adjustment by squareLength.
        ASFixed pageCenter_X = (pageLength / 2);
        ASFixed pageCenter_Y = pageHeight / 2;

        //The center point of the left half of the page.
        ASFixed leftHalfCenter_X = (pageCenter_X / 2);
        ASFixed leftHalfCenter_Y = pageCenter_Y;
        //The center point of the right half of the page.
        ASFixed rightHalfCenter_X = pageCenter_X + leftHalfCenter_X;
        ASFixed rightHalfCenter_Y = pageCenter_Y;





        /* Shape definitions */
        const int NUM_SHAPES = 3;
        ASFixed squareLength = FloatToASFixed(2.0  * 72.0);    //The length of a square, in inches. Here, two inches.
        //The position deltas for our overlapping shapes off of the center of the grouping.
        ASFixed delta = squareLength / 3;
        //                  shape 1       shape 2        shape 3
        ASFixed delta_x[NUM_SHAPES] { 1.25 * delta,  0.00 * delta, -1.25 * delta };
        ASFixed delta_y[NUM_SHAPES] { 0.25 * delta, -1.25 * delta, -0.25 * delta };
        //     (Color can be determined via modular arithmetic.)

        /*                   */

        // Code re: shapes begins below              V
        //
        //"ss" stands for "single shape"


        //STEP 1: DEFINE A SINGLE SQUARE AS A PDEFORM.

        //The content containing just our first square.
        PDEContent singleShapeContent = PDEContentCreate();

        //Make the path a rectangle (circles are way too hard)
        PDEPath shapePath = PDEPathCreate();
        PDEPathSetPaintOp(shapePath, kPDEFill);
        PDEPathAddSegment(shapePath, kPDERect, fixedZero, fixedZero, squareLength, squareLength, //x2,y1,width,height.
            0, 0);                                                                               //unused for kPDERect

        //Give it its GRAFX ST4T3
        PDEGraphicState shapeGState;
        PDEDefaultGState(&shapeGState, sizeof(PDEGraphicState));
        shapeGState.wasSetFlags = 0;
        PDEElementSetGState((PDEElement)shapePath, &shapeGState, sizeof(shapeGState));

        //Done, add the elem.
        PDEContentAddElem(singleShapeContent, kPDEBeforeFirst, (PDEElement)shapePath);

        //Turn the content into a cosobj?
        CosObj singleShapeCosContent, singleShapeCosResources; 
        PDEContentAttrs contentAttrs;   //This objet will be re-used.
        memset((char *)&contentAttrs, 0, sizeof (PDEContentAttrs));
        contentAttrs.formType = 1;      //Set because the content contains a form XObject, our square.
        PDEElementGetBBox((PDEElement)singleShapeContent, &contentAttrs.bbox); //The bounding box of our form is the same as the bounding box of the PDEContents for the square.
        contentAttrs.matrix.a = contentAttrs.matrix.d = fixedOne;
        //b,c,h, and v were set to 0 by memset.

        PDEContentToCosObj(singleShapeContent, kPDEContentToForm, &contentAttrs, sizeof(PDEContentAttrs), PDDocGetCosDoc(pdoc), NULL, &singleShapeCosContent, &singleShapeCosResources);
        PDERelease((PDEObject)singleShapeContent);
        //Turn the cosobj into a form
        ASFixedMatrix unity = { fixedOne, 0, 0, fixedOne, 0, 0 };

        PDEForm singleShape = PDEFormCreateFromCosObj(&singleShapeCosContent, &singleShapeCosResources, &unity);

        //STEP 2: DEFINE TWO TRI-SQUARES MADE UP OF SINGLESHAPES.
        //ONE FOR CMYK, ONE FOR RGB.
        //
        //We'll start with just rgb....

        PDEForm rgbBlendingCircles;
        //PDEForm cmykBlendingCircles;

        //although cmyk has four components and not three, we can ignore the fourth as we don't want to add any black to our blending squares.
        //            the components default to 0, which is what we want.

        ///The container which holds the final prototypical form, with all three shapes.
        PDEContent rgbContent = PDEContentCreate();

        shapeGState.fillColorSpec.space = PDEColorSpaceCreateFromName(ASAtomFromString(("DeviceRGB")));
        shapeGState.wasSetFlags = kPDEFillCSpaceWasSet | kPDEFillCValueWasSet;

        for (int i = 0; i < NUM_SHAPES; ++i)
        {
            PDEForm nextShape = PDEFormCreateClone(singleShape);

            //Set the position for this shape.
            ASFixedMatrix shapePosition = { fixedOne, 0, 0, fixedOne, 0, 0 };
            shapePosition.h = delta_x[i];
            shapePosition.v = delta_y[i];

            //Set the graphics state of this shape.
            shapeGState.fillColorSpec.value.color[0] = (i == 0 ? fixedOne : fixedZero); //Red value.
            shapeGState.fillColorSpec.value.color[1] = (i == 1 ? fixedOne : fixedZero); //Green value.
            shapeGState.fillColorSpec.value.color[2] = (i == 2 ? fixedOne : fixedZero); //Blue value.

            PDEElementSetMatrix((PDEElement)nextShape, &shapePosition);
            PDEElementSetGState((PDEElement)nextShape, &shapeGState, sizeof(shapeGState));
            PDEContentAddElem(rgbContent, kPDEBeforeFirst, (PDEElement)nextShape);

            PDERelease((PDEObject)nextShape);
        }
        PDERelease((PDEObject)singleShape);

        CosObj rgb_cos_cont, rgb_cos_res;
        memset((char *)&contentAttrs, 0, sizeof (PDEContentAttrs));
        contentAttrs.formType = 1;
        PDEElementGetBBox((PDEElement)rgbContent, &contentAttrs.bbox);
        contentAttrs.matrix.a = contentAttrs.matrix.d = fixedOne;
        //b,c,h, and v were set to 0 by memset.

        PDEContentToCosObj(rgbContent, kPDEContentToForm, &contentAttrs, sizeof(PDEContentAttrs), PDDocGetCosDoc(pdoc), NULL, &rgb_cos_cont, &rgb_cos_res);
        PDERelease((PDEObject)rgbContent);

        ///The final PDEForm!
        PDEForm rgbf = PDEFormCreateFromCosObj(&rgb_cos_cont, &rgb_cos_res, &unity);

        for (int i = 0; i < NUM_PAGES; ++i) {
            PDEForm triad = PDEFormCreateClone(rgbf);

            PDPage outPage = doc.getPage(i);
            PDEContent pagecontent = PDPageAcquirePDEContent(outPage, 0);
            ASFixedMatrix finalposition = { fixedOne, 0, 0, fixedOne, 0, 0 };
            finalposition.h = rightHalfCenter_X - squareLength/2;
            finalposition.v = rightHalfCenter_Y - squareLength/2;

            PDEExtGState shapeExtGState = PDEExtGStateCreateNew(PDDocGetCosDoc(pdoc));
            PDEExtGStateSetOpacityFill(shapeExtGState, fixedThreeQuarters);
            PDEExtGStateSetBlendMode(shapeExtGState, ASAtomFromString(blendModes[i]));
            shapeGState.extGState = shapeExtGState;
            shapeGState.wasSetFlags |= kPDEExtGStateWasSet;

            PDEElementSetGState((PDEElement)rgbf, &shapeGState, sizeof(PDEGraphicState));

            PDEElementSetMatrix((PDEElement)rgbf, &finalposition);
            PDEContentAddElem(pagecontent, kPDEBeforeFirst, (PDEElement)rgbf);

            PDPageSetPDEContentCanRaise(outPage, 0);
            PDPageReleasePDEContent(outPage, 0);
            PDPageRelease(outPage);
        }

        PDERelease((PDEObject)shapePath);
        doc.saveDoc(L"Transparency.pdf");


    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);             //If there was an error, display it.

    END_HANDLER

    return errCode;
};
