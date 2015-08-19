// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//============================================================================================
// Sample: CreateTransparency - This sample demonstrates transparency and blending modes
//             in APDFL for CMYK and RGB color spaces. It creates 2 color blending
//             triads, one for APDFL and one for CMYK, once for each of the 12
//             blending modes, for a total of 24 color blending triads.
//
//
//Steps:
// 1) Create and title a page for each color blending mode.
// 2) Define and create a PDEForm prototype for the basic shape of each triad.
// 3) Create a PDEForm prototype for the CMYK and RGB triads by triplicating the basic shape.
// 4) Copy the triads to each page, with different blending modes each time.
//============================================================================================

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#include "PagePDECntCalls.h"
#include "PERCalls.h"
#include "PSFCalls.h"
#include "PEWCalls.h"

#define NUM_BLENDING_SHAPES 3    //The number of shapes in each color blending group. We've gone with a triad. This cannot be changed without altering its usage.

// PDEForm function: Transforms the supplied PDEContent into a PDEForm. The caller is responsible for releasing the input content.
PDEForm contentToForm(PDEContent content, ASInt32 formType, PDDoc document);

int main(int argc, char** argv)
{
    APDFLib lib;                      //Initialize the Adobe PDF Library.
    if (lib.isValid() == false)       //If it failed to initialize, return the error code.
        return lib.getInitError();

    ASErrorCode errCode = 0;          //Tracks runtime errors in the application.

    DURING

//=================================================================================================================================================================================================
//Step 1) Create and title a page for each color blending mode.
//=================================================================================================================================================================================================

        std::wcout << L"Creating a new document with a page for each blending mode." << std::endl;

        APDFLDoc doc;                                                                 //APDFLDoc's constructor creates a new, pageless document.

        ASFixed pageLength   = ASFloatToFixed(10.0 * 72.0);                           //Each page will be 10 inches wide.
        ASFixed pageHeight   = ASFloatToFixed(4.25  * 72.0);                          //Each page will be 4.25 inches high.

        //As long as we're on the page dimensions, we're going to need these calculations to place the triads later on.
        ASFixed pageCenter_X = (pageLength / 2);
        ASFixed pageCenter_Y = (pageHeight / 2);

        //The center point of the left half of the page.
        ASFixed leftHalfCenter_X = (pageCenter_X / 2);
        ASFixed leftHalfCenter_Y = pageCenter_Y;

        //The center point of the right half of the page.
        ASFixed rightHalfCenter_X = pageCenter_X + leftHalfCenter_X;
        ASFixed rightHalfCenter_Y = pageCenter_Y;

        //All twelve blend modes available in APDFL. See the Adobe PDF Reference, sixth edition, section 7.2.4, table 7.2 for a description of each.
        //The reference is available here: http://www.adobe.com/content/dam/Adobe/en/devnet/acrobat/pdfs/pdf_reference_1-7.pdf
        std::vector<char*> blendModes {
            "ColorBurn",  "ColorDodge", "Darken",
            "Difference", "Exclusion",  "HardLight",
            "Lighten",    "Multiply",   "Normal",
            "Overlay",    "Screen",     "SoftLight",
        };
        int NUM_PAGES = blendModes.size();                                            //A page to demonstrate each blend mode!

        //Make a page for each blend mode.
        for (int i = 0; i < NUM_PAGES; ++i){
            doc.insertPage(pageLength, pageHeight, PDBeforeFirstPage);
        }

        std::wcout << L"Titling each page." << std::endl;

        //Now we will title each page with the name of its associated blending mode. We'll use the Courier font for its wide availability.
        PDEFontAttrs fontAttrs;
        memset(&fontAttrs, 0, sizeof(fontAttrs));
        fontAttrs.name = ASAtomFromString("CourierStd");
        fontAttrs.type = ASAtomFromString("Type1");

        PDSysFont sysFont = PDFindSysFont(&fontAttrs, sizeof(fontAttrs), 0);
        PDEFont pdeFont   = PDEFontCreateFromSysFont(sysFont, kPDEFontDoNotEmbed);

        //This matrix will determine how the text is displayed.
        ASDoubleMatrix textMatrix;
        memset(&textMatrix, 0, sizeof(textMatrix));
        ASDouble fontSize = 24.0;                                                     //What our font size will be, in points.
        textMatrix.a = textMatrix.d =  fontSize;                                      //Character width and height, respectively.
        textMatrix.h = (0.25 * 72.0);                                                 //This will place the text 1/4 of an inch from the left margin.
        textMatrix.v = (ASFixedToFloat(pageHeight) - (0.20 *  72.00) - fontSize);     //This will place the text 1/5 of an inch from the top margin, adjusting for font size.

        //Now place each page title.
        for (int i = 0; i < NUM_PAGES; i++)
        {
            PDEText textObj = PDETextCreate();                                        //Title text will be added to this object, which will be set into the page's content.

            std::stringstream title;
            title << "Blend Mode: " << blendModes[i] << ".";

            PDETextAddEx(textObj, kPDETextRun, 0,                                     //Add a text run to the first index of our text object.
                (Uns8*)title.str().c_str(),                                           //The text,
                strlen(title.str().c_str()),                                          //and its length.
                pdeFont,                                                              //The font we want used.
                NULL,0,                                                               //Default the graphics state.
                NULL,0,                                                               //Default the text state.
                &textMatrix,                                                          //The size and location of the text.
                NULL);                                                                //Default the stroke matrix.

            PDPage outPage = doc.getPage(i);
            PDEContent pagecontent = PDPageAcquirePDEContent(outPage, 0);

            PDEContentAddElem(pagecontent,kPDEBeforeFirst, (PDEElement)(textObj));    //Add the text element to the page's content.
            PDERelease((PDEObject)(textObj));                                         //We're done with the text element now that we've added it.
            PDPageSetPDEContentCanRaise(outPage, NULL);                               //Set the content back into the page.

            //We're now done with this page and its contents.
            PDPageReleasePDEContent(outPage, 0);
            PDPageRelease(outPage);
        }
        PDERelease((PDEObject)pdeFont);                                               //We're done with the font too!

//=================================================================================================================================================================================================
// 2) Define and create a prototype for the basic shape of each triad.
//
// Note: PDEForm objects will be used as prototypes because they can group different PDEElements together, and for their cloning ability.
//=================================================================================================================================================================================================

        std::wcout << L"Creating the basic shape prototype." << std::endl;

        //First we define the shape we'll use as a base. We'll make squares.

        ASFixed squareLength = FloatToASFixed(2.0  * 72.0);                                         //The length of each square, in inches. Here, two inches.

        //Now we want to define how the shapes will be positioned relative to each other, by making position deltas (or vectors, if you prefer) for each.
        ASFixed delta = (squareLength / 4);                                                         //We'll scale the deltas/vectors to the size of the actual shape.
        //                                     shape 1       shape 2        shape 3
        ASFixed delta_x[NUM_BLENDING_SHAPES] { 1.25 * delta,  0.00 * delta, -1.25 * delta };
        ASFixed delta_y[NUM_BLENDING_SHAPES] { 0.25 * delta, -1.25 * delta, -0.25 * delta };

        //Now we can create the basic shape's prototype.

        PDEContent singleShapeContent = PDEContentCreate();                                         //We'll put our square into this content object, which will be converted into a PDEForm object.

        //Make a rectangle shape.
        PDEPath shapePath = PDEPathCreate();
        PDEPathSetPaintOp(shapePath, kPDEFill);
        PDEPathAddSegment(shapePath, kPDERect, fixedZero, fixedZero, squareLength, squareLength,    //The rectangle's x1, y1, width, and height.
            0, 0);                                                                                  //unused for kPDERect.

        //Give the rectangle its graphics state. This graphics state will be re-used for the triads.
        PDEGraphicState shapeGState;
        PDEDefaultGState(&shapeGState, sizeof(PDEGraphicState));
        shapeGState.wasSetFlags = 0;                                                                //We haven't added anything special to this graphics state yet.
        PDEElementSetGState((PDEElement)shapePath, &shapeGState, sizeof(shapeGState));

        //The square's complete. Add it to our content object.
        PDEContentAddElem(singleShapeContent, kPDEBeforeFirst, (PDEElement)shapePath);
        PDERelease((PDEObject)shapePath);

        //Convert the content containing our square into a PDEForm.
        PDDoc pdoc = doc.getPDDoc();
        PDEForm singleShape = contentToForm(singleShapeContent, 1, pdoc);                           //This is the basic shape prototype.
        PDERelease((PDEObject)singleShapeContent);

//=================================================================================================================================================================================================
// 3) Create a prototype for the CMYK and RGB triads by triplicating the basic shape.
//=================================================================================================================================================================================================

        std::wcout << L"Creating CMYK and RGB triad prototypes." << std::endl;

        //These containers will hold the shapes the CMYK triad and the RGB triad. They'll also be converted to PDEForms to create our prototypes.
        PDEContent cmykContent = PDEContentCreate();
        PDEContent rgbContent  = PDEContentCreate();

        for (PDEContent* nextContent : { &cmykContent, &rgbContent })
        {
            //Set the color space of the next triad.
            char* colorSpace = (nextContent == &cmykContent ? "DeviceCMYK" : "DeviceRGB");
            shapeGState.fillColorSpec.space = PDEColorSpaceCreateFromName(ASAtomFromString((colorSpace)));
            shapeGState.wasSetFlags = kPDEFillCSpaceWasSet;

            //Give each triad three shapes of different colors, positioned as specified in delta_x and delta_y.
            for (int i = 0; i < NUM_BLENDING_SHAPES; ++i)
            {
                PDEForm nextShape = PDEFormCreateClone(singleShape);                              //Clone the basic shape prototype.

                //Set the position for this shape.
                ASFixedMatrix shapePosition = { fixedOne, 0, 0, fixedOne, 0, 0 };
                int symmetry    = ((nextContent == &cmykContent) ? -1 : 1);                       //This is used to achieve symmetry between the two triads on the page.
                shapePosition.h = symmetry * delta_x[i];
                shapePosition.v = delta_y[i];

                //Set the graphics state of this shape. The conditional values are used to easily create three shapes of different colors.
                shapeGState.fillColorSpec.value.color[0] = (i == 0 ? fixedOne : fixedZero);       //Red/Cyan value.
                shapeGState.fillColorSpec.value.color[1] = (i == 1 ? fixedOne : fixedZero);       //Green/Magenta value.
                shapeGState.fillColorSpec.value.color[2] = (i == 2 ? fixedOne : fixedZero);       //Blue/Yellow value.
                shapeGState.wasSetFlags |= kPDEFillCValueWasSet;                                  //CMYK also has a fourth component, Key (Black). But we will not use it.

                //Set the position and graphics state of this triad.
                PDEElementSetMatrix((PDEElement)nextShape, &shapePosition);
                PDEElementSetGState((PDEElement)nextShape, &shapeGState, sizeof(shapeGState));

                //Add the triad to its content.
                PDEContentAddElem(*nextContent, kPDEBeforeFirst, (PDEElement)nextShape);
                PDERelease((PDEObject)nextShape);
            }
        }

        PDERelease((PDEObject)singleShape);                                                       //We're now done with the basic shape prototype.

        PDEForm rgbTriad = contentToForm(rgbContent,1,pdoc);                                      //The RGB triad prototype.
        PDERelease((PDEObject)rgbContent);

        PDEForm cmykTriad = contentToForm(cmykContent,1,pdoc);                                    //The CMYK triad prototype.
        PDERelease((PDEObject)cmykContent);

//=================================================================================================================================================================================================
// 4) Copy the triads to each page, with a different blending modes each time.
//=================================================================================================================================================================================================

        std::wcout << L"Placing the triads on each page." << std::endl;

        for (int i = 0; i < NUM_PAGES; ++i) {
            PDPage outPage = doc.getPage(i);
            PDEContent pagecontent = PDPageAcquirePDEContent(outPage, 0);

            //The PDEExtGState determines the transparency and blending mode of whatever PDEGraphicsState object it is set to.
            PDEExtGState shapeExtGState = PDEExtGStateCreateNew(PDDocGetCosDoc(pdoc));
            PDEExtGStateSetOpacityFill(shapeExtGState, fixedThreeQuarters);                             //Each shape will have 3/4 transparency.
            PDEExtGStateSetBlendMode(shapeExtGState, ASAtomFromString(blendModes[i]));                  //This will correspond to the title on the page.

            shapeGState.extGState = shapeExtGState;
            shapeGState.wasSetFlags |= kPDEExtGStateWasSet;

            PDEElementSetGState((PDEElement)rgbTriad,  &shapeGState, sizeof(PDEGraphicState));
            PDEElementSetGState((PDEElement)cmykTriad, &shapeGState, sizeof(PDEGraphicState));

            //Position and set the RGB triad.
            ASFixedMatrix rgbPosition  = { fixedOne, 0, 0, fixedOne, 0, 0 };
            rgbPosition.h = rightHalfCenter_X - squareLength / 2;
            rgbPosition.v = rightHalfCenter_Y - squareLength / 2;

            PDEElementSetMatrix((PDEElement)rgbTriad, &rgbPosition);
            PDEContentAddElem(pagecontent, kPDEBeforeFirst, (PDEElement)rgbTriad);

            //Position and set the CMYK triad.
            ASFixedMatrix cmykPosition = { fixedOne, 0, 0, fixedOne, 0, 0 };
            cmykPosition.h = leftHalfCenter_X - squareLength / 2;
            cmykPosition.v = leftHalfCenter_Y - squareLength / 2;

            PDEElementSetMatrix((PDEElement)cmykTriad, &cmykPosition);
            PDEContentAddElem(pagecontent, kPDEBeforeFirst, (PDEElement)cmykTriad);

            //Set all the new content into the page and release resources.
            PDPageSetPDEContentCanRaise(outPage, 0);
            PDPageReleasePDEContent(outPage, 0);
            PDPageRelease(outPage);
        }

        //Now we're done with everything except for the document itself.
        PDERelease((PDEObject)rgbTriad);
        PDERelease((PDEObject)cmykTriad);
        PDERelease((PDEObject)shapeGState.strokeColorSpec.space);
        PDERelease((PDEObject)shapeGState.fillColorSpec.space);

        std::wcout << L"Saving the document." << std::endl;

        doc.saveDoc(L"CreateTransparency.pdf");                                                         //APDFLDoc's destructor takes care of properly closing and releasing the document.

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);                                                                      //If there was an error, display it.

    END_HANDLER

    if(!errCode)
        std::wcout << L"Success!" << std::endl;

    return errCode;
};

//=================================================================================================================================================================================================
// PDEForm function: Transforms the supplied PDEContent into a PDEForm. The caller is responsible for releasing the input content.
//
// Note: The argument formType is used only if the PDEContent contains a FormXObject.
//       The argument document is the document in which this creation is taking place.
//=================================================================================================================================================================================================
PDEForm contentToForm(PDEContent content, ASInt32 formType, PDDoc document)
{
    //Determine the attributes of our input content.
    PDEContentAttrs contentAttrs;
    memset((char *)&contentAttrs, 0, sizeof (PDEContentAttrs));
    contentAttrs.formType = formType;                              //This must be set for XObject-carrying PDEForms.
    PDEElementGetBBox((PDEElement)content, &contentAttrs.bbox);    //The bounding box of our output PDEForm will match the bounding box of our input PDEContent.
    contentAttrs.matrix.a = contentAttrs.matrix.d = fixedOne;      //b,c,h, and v were set to 0 by memset.

    //Convert the PDEContent into CosObjects.
    CosObj cosContent, cosResources;
    PDEContentToCosObj(content, kPDEContentToForm, &contentAttrs, sizeof(PDEContentAttrs), PDDocGetCosDoc(document), NULL, &cosContent, &cosResources);

    //Convert the CosObjects into a PDEForm.
    ASFixedMatrix unity = { fixedOne, 0, 0, fixedOne, 0, 0 };
    return PDEFormCreateFromCosObj(&cosContent, &cosResources, &unity);
}
