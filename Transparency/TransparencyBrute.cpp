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

int brain(int argc, char** argv)
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

        PDEGraphicState gState;
        PDEDefaultGState(&gState, sizeof(PDEGraphicState));
        gState.fillColorSpec.space = PDEColorSpaceCreateFromName(ASAtomFromString(("DeviceGray"))); //Gray just for the titles.
        gState.wasSetFlags = kPDEFillCSpaceWasSet | kPDEFillCValueWasSet | kPDEExtGStateWasSet;



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
        textMatrix.h = 0.0;                       //Place at a x-val of 1 inch from the left side of the page.
        textMatrix.v = ASFixedToFloat(pageHeight)-24.0;                       //Place at a y-val of 2 inches from the bottom of the page.
        
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
                &gState,sizeof(gState),           //We use the gState that's supposed to be for the colors, because it makes no difference.
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
        

        gState.fillColorSpec.space = PDEColorSpaceCreateFromName(ASAtomFromString(("DeviceRGB"))); //Gray just for the titles.
        for (int i = 0; i < NUM_PAGES; ++i) {

            PDPage outPage = doc.getPage(i);
            PDEContent pagecontent = PDPageAcquirePDEContent(outPage, 0);

            PDEPath path = PDEPathCreate();
            PDEPath path2 = PDEPathCreate();
            PDEPath path3 = PDEPathCreate();
            PDEPathSetPaintOp(path, kPDEFill);
            PDEPathSetPaintOp(path2, kPDEFill);
            PDEPathSetPaintOp(path3, kPDEFill);

            //We will create a rect, just for simplicity's sake while I figure out transparency.
            PDEPathAddSegment(path, kPDERect, fixedZero, fixedZero, squareLength, squareLength, //x1,y1,width,height.
                0, 0);
            PDEPathAddSegment(path2, kPDERect, fixedZero, fixedZero, squareLength, squareLength, //x1,y1,width,height.
                0, 0);
            PDEPathAddSegment(path3, kPDERect, fixedZero, fixedZero, squareLength, squareLength, //x1,y1,width,height.
                0, 0);

            ASFixed delta = squareLength/3;

            ASFixedMatrix position{ fixedOne, 0, 0, fixedOne, 0, 0 };
            position.h = rightHalfCenter_X + delta + 0.25*delta;
            position.v =  rightHalfCenter_Y + 0.25*delta;

            ASFixedMatrix position2{ fixedOne, 0, 0, fixedOne, 0, 0 };
            position2.h = rightHalfCenter_X;
            position2.v = rightHalfCenter_Y - delta - 0.25*delta;

            ASFixedMatrix position3{ fixedOne, 0, 0, fixedOne, 0, 0 };
            position3.h = rightHalfCenter_X - delta - 0.25*delta;
            position3.v = rightHalfCenter_Y - 0.25*delta;

            PDEElementSetMatrix((PDEElement)path, &position);
            PDEElementSetMatrix((PDEElement)path2, &position2);
            PDEElementSetMatrix((PDEElement)path3, &position3);

            ///aaaaaaaaaaaaaaaaaaaa
            PDEExtGState egs = PDEExtGStateCreateNew(PDDocGetCosDoc(pdoc));
            PDEExtGStateSetOpacityFill(egs, fixedThreeQuarters);
            PDEExtGStateSetBlendMode(egs,ASAtomFromString(blendModes[i]));
            gState.extGState = egs;
            ///aaaaaaaaaaaaaaaaaaaa

            //This will iterate through Red, Green, and blue.
            gState.fillColorSpec.value.color[0] = fixedOne;  //r
            gState.fillColorSpec.value.color[1] = fixedZero; //g
            gState.fillColorSpec.value.color[2] = fixedZero; //b.
            PDEElementSetGState((PDEElement)path, &gState, sizeof(PDEGraphicState));

            gState.fillColorSpec.value.color[2] = fixedOne;
            gState.fillColorSpec.value.color[0] = fixedZero;
            gState.fillColorSpec.value.color[1] = fixedZero;
            PDEElementSetGState((PDEElement)path2, &gState, sizeof(PDEGraphicState));

            gState.fillColorSpec.value.color[1] = fixedOne;
            gState.fillColorSpec.value.color[2] = fixedZero;
            gState.fillColorSpec.value.color[0] = fixedZero;
            PDEElementSetGState((PDEElement)path3, &gState, sizeof(PDEGraphicState));

            PDEContentAddElem(pagecontent, kPDEAfterLast, (PDEElement)path);
            PDEContentAddElem(pagecontent, kPDEAfterLast, (PDEElement)path2);
            PDEContentAddElem(pagecontent, kPDEAfterLast, (PDEElement)path3);


            PDPageSetPDEContentCanRaise(outPage, 0);
            PDPageReleasePDEContent(outPage, 0);
            PDERelease((PDEObject)path);
            PDPageRelease(outPage);

        }



        gState.fillColorSpec.space = PDEColorSpaceCreateFromName(ASAtomFromString(("DeviceCMYK")));
        for (int i = 0; i < NUM_PAGES; ++i) {

            PDPage outPage = doc.getPage(i);
            PDEContent pagecontent = PDPageAcquirePDEContent(outPage, 0);

            PDEPath path = PDEPathCreate();
            PDEPath path2 = PDEPathCreate();
            PDEPath path3 = PDEPathCreate();
            PDEPathSetPaintOp(path, kPDEFill);
            PDEPathSetPaintOp(path2, kPDEFill);
            PDEPathSetPaintOp(path3, kPDEFill);

            //We will create a rect, just for simplicity's sake while I figure out transparency.
            PDEPathAddSegment(path, kPDERect, fixedZero, fixedZero, squareLength, squareLength, //x1,y1,width,height.
                0, 0);
            PDEPathAddSegment(path2, kPDERect, fixedZero, fixedZero, squareLength, squareLength, //x1,y1,width,height.
                0, 0);
            PDEPathAddSegment(path3, kPDERect, fixedZero, fixedZero, squareLength, squareLength, //x1,y1,width,height.
                0, 0);

            ASFixed delta = squareLength/3;

            ASFixedMatrix position{ fixedOne, 0, 0, fixedOne, 0, 0 };
            position.h = leftHalfCenter_X - delta - 0.25*delta;
            position.v = leftHalfCenter_Y + 0.25*delta;

            ASFixedMatrix position2{ fixedOne, 0, 0, fixedOne, 0, 0 };
            position2.h = leftHalfCenter_X;
            position2.v = leftHalfCenter_Y - delta - 0.25*delta;

            ASFixedMatrix position3{ fixedOne, 0, 0, fixedOne, 0, 0 };
            position3.h = leftHalfCenter_X + delta + 0.25*delta;
            position3.v = leftHalfCenter_Y - 0.25*delta;

            PDEElementSetMatrix((PDEElement)path, &position);
            PDEElementSetMatrix((PDEElement)path2, &position2);
            PDEElementSetMatrix((PDEElement)path3, &position3);
            ///aaaaaaaaaaaaaaaaaaaa
            PDEExtGState egs = PDEExtGStateCreateNew(PDDocGetCosDoc(pdoc));
            PDEExtGStateSetOpacityFill(egs, fixedThreeQuarters);
            PDEExtGStateSetBlendMode(egs,ASAtomFromString(blendModes[i]));
            gState.extGState = egs;
            ///aaaaaaaaaaaaaaaaaaa
            //This will iterate through Red, Green, and blue.
            gState.fillColorSpec.value.color[0] = fixedOne;    //c
            gState.fillColorSpec.value.color[1] = fixedZero;   //m
            gState.fillColorSpec.value.color[2] = fixedZero;   //y. we leave k to default to 0.
            PDEElementSetGState((PDEElement)path, &gState, sizeof(PDEGraphicState));

            gState.fillColorSpec.value.color[2] = fixedOne;
            gState.fillColorSpec.value.color[0] = fixedZero;
            gState.fillColorSpec.value.color[1] = fixedZero;
            PDEElementSetGState((PDEElement)path2, &gState, sizeof(PDEGraphicState));

            gState.fillColorSpec.value.color[1] = fixedOne;
            gState.fillColorSpec.value.color[2] = fixedZero;
            gState.fillColorSpec.value.color[0] = fixedZero;
            PDEElementSetGState((PDEElement)path3, &gState, sizeof(PDEGraphicState));

            PDEContentAddElem(pagecontent, kPDEAfterLast, (PDEElement)path);
            PDEContentAddElem(pagecontent, kPDEAfterLast, (PDEElement)path2);
            PDEContentAddElem(pagecontent, kPDEAfterLast, (PDEElement)path3);

            PDPageSetPDEContentCanRaise(outPage, 0);
            PDPageReleasePDEContent(outPage, 0);
            PDERelease((PDEObject)path);
            PDPageRelease(outPage);
            PDEExtGStateSetBlendMode(egs,ASAtomFromString(blendModes[i]));
        }

        PDERelease(reinterpret_cast<PDEObject>(gState.strokeColorSpec.space));
        PDERelease(reinterpret_cast<PDEObject>(gState.fillColorSpec.space));

        doc.saveDoc(L"Transparency.pdf");


    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);             //If there was an error, display it.

    END_HANDLER

    return errCode;
};
