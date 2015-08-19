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


PDEForm contentToForm(PDEContent content, ASInt32 formType, PDDoc document);

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

        ASFixed pageLength   = ASFloatToFixed(10.0 * 72.0);    //12 inches wide.
        ASFixed pageHeight   = ASFloatToFixed(4.25  * 72.0);    //5 inches high.

        int NUM_PAGES = blendModes.size();
        for (int i = 0; i < NUM_PAGES; ++i){
            doc.insertPage(pageLength, pageHeight, PDBeforeFirstPage);      //Give it a page of 7.5 inches square dimensions.
        }

        //Add the titles now, why not.
        ///Titles.
        PDEFontAttrs fontAttrs;                             //Struct that will contain font name and type.
        memset(&fontAttrs, 0, sizeof(fontAttrs));           //Ensure any "garbage" data is cleared out.
        fontAttrs.name = ASAtomFromString("CourierStd");    //Set the font name and type. 
        fontAttrs.type = ASAtomFromString("Type1");          
        //Locate the system font that corresponds to the PDEFontAttrs struct we just set.
        PDSysFont sysFont = PDFindSysFont(&fontAttrs, sizeof(fontAttrs), 0);
        //Create the CourierStd Type1 font with embed flag set.       
        PDEFont pdeFont = PDEFontCreateFromSysFont(sysFont, kPDEFontCreateEmbedded);
        ASDoubleMatrix textMatrix;                     //Struct that determines the size and location of text on page.
        memset(&textMatrix, 0, sizeof(textMatrix));    //Clear out any "garbage" the struct may contain.
        textMatrix.a = 24.0;                           //Character width.
        textMatrix.d = 24.0;                           //Character height.
        textMatrix.h = 0.25*72.0;                                  //Place at a x-val of a quarter of an inch from the left side of the page.
        textMatrix.v = ASFixedToFloat(pageHeight) - (0.20 *  72.00) - 24.0;    //Place a fifth of an inch from the top of the page, adjusting for font size.
        
        for (int i = 0; i < NUM_PAGES; i++)
        {
            PDEText textObj = PDETextCreate();    //PDEText will be set, and then added into the PDEContent.
            PDPage outPage = doc.getPage(i);
            PDEContent pagecontent = PDPageAcquirePDEContent(outPage, 0);

            std::stringstream title;
            title << "Blend Mode: " << blendModes[i];

            PDETextAddEx(textObj,                 //The PDEText object we just created.
                kPDETextRun,                      //kPDETextRun and kPDETextChar specify whether a string or character will be inserted.
                0,                                //The index after which to add the character or text run.
                (Uns8*)title.str().c_str(),                  //The string that will be added should be type-cast as a pointer to Uns8.
                strlen(title.str().c_str()),                 //Length of the string.
                pdeFont,                          //The PDEFont we created holding information such as font name, type and whether it's embedded or not.
                NULL,0,           //We use the gState that's supposed to be for the colors, because it makes no difference.
                NULL,0,                        //Default the text state.
                &textMatrix,                      //Matrix containing size and location for the text.
                NULL);                            //The matrix for the line width when stroking text.

            PDEContentAddElem(pagecontent,kPDEBeforeFirst, reinterpret_cast<PDEElement>(textObj));    //Add the text element to the page's content.
            PDPageSetPDEContentCanRaise(outPage, NULL);                                             //Set the content back into the page.

            PDERelease(reinterpret_cast<PDEObject>(textObj));
            PDPageReleasePDEContent(outPage, 0);
            PDPageRelease(outPage);
        }
            PDERelease(reinterpret_cast<PDEObject>(pdeFont));








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

        PDEForm singleShape = contentToForm(singleShapeContent, 1, pdoc);

        //STEP 2: DEFINE TWO TRI-SQUARES MADE UP OF SINGLESHAPES.
        //ONE FOR CMYK, ONE FOR RGB.

        ///The container which holds the final prototypical form, with all three shapes.
        PDEContent cmykContent = PDEContentCreate();
        PDEContent rgbContent = PDEContentCreate();

        for (PDEContent* nextContent : { &cmykContent, &rgbContent })
        {
            char* colorSpace = (nextContent == &cmykContent ? "DeviceCMYK" : "DeviceRGB");
            int symmetry = (nextContent == &cmykContent ? -1 : 1);    //This is used to achieve symmetry between the two triads on the page.

            for (int i = 0; i < NUM_SHAPES; ++i)
            {
                PDEForm nextShape = PDEFormCreateClone(singleShape);

                //Set the position for this shape.
                ASFixedMatrix shapePosition = { fixedOne, 0, 0, fixedOne, 0, 0 };
                shapePosition.h = symmetry*delta_x[i];
                shapePosition.v = delta_y[i];

                //Set the graphics state of this shape.
                //This conditional branching makes three shapes of each color.
                shapeGState.fillColorSpec.space = PDEColorSpaceCreateFromName(ASAtomFromString((colorSpace)));
                shapeGState.fillColorSpec.value.color[0] = (i == 0 ? fixedOne : fixedZero); //Red/Cyan value.
                shapeGState.fillColorSpec.value.color[1] = (i == 1 ? fixedOne : fixedZero); //Green/Magenta value.
                shapeGState.fillColorSpec.value.color[2] = (i == 2 ? fixedOne : fixedZero); //Blue/Yellow value.
                //CMYK has a fourth component, Key (Black). But we don't need to use it.

                shapeGState.wasSetFlags = kPDEFillCSpaceWasSet | kPDEFillCValueWasSet;

                PDEElementSetMatrix((PDEElement)nextShape, &shapePosition);
                PDEElementSetGState((PDEElement)nextShape, &shapeGState, sizeof(shapeGState));
                PDEContentAddElem(*nextContent, kPDEBeforeFirst, (PDEElement)nextShape);

                PDERelease((PDEObject)nextShape);
            }
        }
        PDERelease((PDEObject)singleShape);

        PDEForm rgbTriad = contentToForm(rgbContent,1,pdoc);
        PDEForm cmykTriad = contentToForm(cmykContent,1,pdoc);

        PDERelease((PDEObject)rgbContent);
        PDERelease((PDEObject)cmykContent);

        for (int i = 0; i < NUM_PAGES; ++i) {
            PDPage outPage = doc.getPage(i);
            PDEContent pagecontent = PDPageAcquirePDEContent(outPage, 0);

            //The position of the triad.
            ASFixedMatrix finalposition = { fixedOne, 0, 0, fixedOne, 0, 0 };
            
            //The PDEExtGState determines the blending of the triads.
            PDEExtGState shapeExtGState = PDEExtGStateCreateNew(PDDocGetCosDoc(pdoc));
            PDEExtGStateSetOpacityFill(shapeExtGState, fixedThreeQuarters);
            PDEExtGStateSetBlendMode(shapeExtGState, ASAtomFromString(blendModes[i]));

            shapeGState.extGState = shapeExtGState;
            shapeGState.wasSetFlags |= kPDEExtGStateWasSet;

            //Set the RGB triad.
            finalposition.h = rightHalfCenter_X - squareLength/2;
            finalposition.v = rightHalfCenter_Y - squareLength/2;

            PDEElementSetGState((PDEElement)rgbTriad, &shapeGState, sizeof(PDEGraphicState));
            PDEElementSetMatrix((PDEElement)rgbTriad, &finalposition);
            PDEContentAddElem(pagecontent, kPDEBeforeFirst, (PDEElement)rgbTriad);

            //Set the CMYK triad.
            finalposition.h = leftHalfCenter_X - squareLength/2;
            finalposition.v = leftHalfCenter_Y - squareLength / 2;

            PDEElementSetGState((PDEElement)cmykTriad, &shapeGState, sizeof(PDEGraphicState));
            PDEElementSetMatrix((PDEElement)cmykTriad, &finalposition);
            PDEContentAddElem(pagecontent, kPDEBeforeFirst, (PDEElement)cmykTriad);

            //Set the content into the page and release the page.
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

PDEForm contentToForm(PDEContent content, ASInt32 formType, PDDoc document)
{
    PDEContentAttrs contentAttrs;
    memset((char *)&contentAttrs, 0, sizeof (PDEContentAttrs));
    
    contentAttrs.formType = formType;
    PDEElementGetBBox((PDEElement)content, &contentAttrs.bbox);
    contentAttrs.matrix.a = contentAttrs.matrix.d = fixedOne; //b,c,h, and v were set to 0 by memset.

    CosObj cosContent, cosResources;
    PDEContentToCosObj(content, kPDEContentToForm, &contentAttrs, sizeof(PDEContentAttrs), PDDocGetCosDoc(document), NULL, &cosContent, &cosResources);

    ASFixedMatrix unity = { fixedOne, 0, 0, fixedOne, 0, 0 };
    return PDEFormCreateFromCosObj(&cosContent, &cosResources, &unity);
}
