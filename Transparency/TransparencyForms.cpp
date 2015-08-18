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

        ASFixed pageLength = ASFloatToFixed(12.0 * 72);    //12 inches wide.
        ASFixed pageHeight = ASFloatToFixed(6.0 * 72);      //6 inches high.
        ASFixed squareLength = FloatToASFixed(2.0 * 72); //The length of a square, in inches. Here, one inch.

        int NUM_PAGES = blendModes.size();
        for (int i = 0; i < NUM_PAGES; ++i){
            doc.insertPage(pageLength, pageHeight, PDBeforeFirstPage);      //Give it a page of 7.5 inches square dimensions.
        }

        //The origin of a rectangle is at its bottom-left corner, thus the adjustment by squareLength.
        ASFixed pageCenter_X = (pageLength / 2);
        ASFixed pageCenter_Y = pageHeight / 2;

        //The center point of the left half of the page.
        ASFixed leftHalfCenter_X = (pageCenter_X / 2)- 0.25*squareLength;
        ASFixed leftHalfCenter_Y = pageCenter_Y - 0.25*squareLength;
        //The center point of the right half of the page.
        ASFixed rightHalfCenter_X = pageCenter_X + leftHalfCenter_X - 0.25*squareLength;
        ASFixed rightHalfCenter_Y = pageCenter_Y - 0.25*squareLength;






        // Code re: shapes begins below              V
        //
        //"ss" stands for "single shape"

        //  1  //

        //The content containing just our first square.
        PDEContent ss_Content = PDEContentCreate();

        //Make the path a rectangle (circles are way too hard)
        PDEPath ss_Path = PDEPathCreate();
        PDEPathSetPaintOp(ss_Path, kPDEFill);
        PDEPathAddSegment(ss_Path, kPDERect, fixedZero, fixedZero, squareLength, squareLength, //x2,y1,width,height.
            0, 0);                                                                             //unused for kPDERect

        //position (just in case)
        ASFixedMatrix unity = { fixedOne, 0, 0, fixedOne, 0, 0 };
        PDEElementSetMatrix((PDEElement)ss_Path, &unity);

        //gstate
        PDEGraphicState ss_gS;
        PDEDefaultGState(&ss_gS, sizeof(PDEGraphicState));
        ss_gS.wasSetFlags = 0;
        PDEElementSetGState((PDEElement)ss_Path, &ss_gS, sizeof(ss_gS));

        //Done, add the elem.
        PDEContentAddElem(ss_Content, kPDEBeforeFirst, (PDEElement)ss_Path);

        //  2  //

        //Turn the content into a cosobj?
        CosObj ss_cos_cont, ss_cos_res;
        PDEContentToCosObj(ss_Content, kPDEContentToForm, NULL, NULL, PDDocGetCosDoc(pdoc), NULL, &ss_cos_cont, &ss_cos_res);
        CosDictPut(ss_cos_cont, ASAtomFromString("Resources"), ss_cos_res); //I did this off the recommendation of the documentation of the above macro, but it, of course, does not get me obviously closer to the goal.
        PDERelease((PDEObject)ss_Content);

        //Turn the cosobj into a form
        PDEForm ss_shapef = PDEFormCreateFromCosObj(&ss_cos_cont, &ss_cos_res, &unity);

        //set up gstate
        ss_gS.wasSetFlags = kPDEFillCSpaceWasSet | kPDEFillCValueWasSet | kPDEExtGStateWasSet;
        PDERelease((PDEObject)ss_gS.fillColorSpec.space);
        ss_gS.fillColorSpec.space = PDEColorSpaceCreateFromName(ASAtomFromString(("DeviceRGB")));
        PDEExtGState eags = PDEExtGStateCreateNew(PDDocGetCosDoc(pdoc));
        PDEExtGStateSetOpacityFill(eags, fixedThreeQuarters);
        PDEExtGStateSetBlendMode(eags,ASAtomFromString(blendModes[5]));
        ss_gS.extGState = eags;

        ///The container which holds the final prototypical form, with all three shapes.
        PDEContent rgbContent = PDEContentCreate();

        ///We would add three shapes, but just one for development right now.
        ASFixedMatrix rgbposition = { fixedOne, 0, 0, fixedOne, 0, 0 };
        rgbposition.h = pageCenter_X;
        rgbposition.v = pageCenter_Y;
        ss_gS.fillColorSpec.value.color[0] = fixedOne;  //r
        ss_gS.fillColorSpec.value.color[1] = fixedZero; //g
        ss_gS.fillColorSpec.value.color[2] = fixedZero; //b.

        PDEElementSetMatrix((PDEElement)ss_shapef, &rgbposition);
        PDEElementSetGState((PDEElement)ss_shapef, &ss_gS, sizeof(ss_gS));
        PDEContentAddElem(rgbContent, kPDEBeforeFirst, (PDEElement)ss_shapef);

        CosObj rgb_cos_cont, rgb_cos_res;
        PDEContentToCosObj(rgbContent, kPDEContentToForm, NULL, NULL, PDDocGetCosDoc(pdoc), NULL, &rgb_cos_cont, &rgb_cos_res);
        CosDictPut(rgb_cos_cont, ASAtomFromString("Resources"), rgb_cos_res); //See comment of same macro call earlier.
        PDERelease((PDEObject)rgbContent);

        ///The final PDEForm!
        PDEForm rgbf = PDEFormCreateFromCosObj(&rgb_cos_cont, &rgb_cos_res, &unity);

        for (int i = 0; i < NUM_PAGES; ++i) {

            PDPage outPage = doc.getPage(i);
            PDEContent pagecontent = PDPageAcquirePDEContent(outPage, 0);
            ASFixedMatrix finalposition = { fixedOne, 0, 0, fixedOne, 0, 0 };
            finalposition.h = 0;
            finalposition.v = 0;

            PDEElementSetMatrix((PDEElement)rgbf, &finalposition);

            ///***/// PDEForm is added here. It's put here for debug purposes.
            PDEContentAddElem(pagecontent, kPDEBeforeFirst, (PDEElement)rgbf);
            ///***///


            PDPageSetPDEContentCanRaise(outPage, 0);
            PDPageReleasePDEContent(outPage, 0);
            PDPageRelease(outPage);
        }

        PDERelease((PDEObject)ss_Path);
        doc.saveDoc(L"Transparency.pdf");


    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);             //If there was an error, display it.

    END_HANDLER

    return errCode;
};
