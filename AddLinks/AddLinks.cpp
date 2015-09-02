// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//=======================================================================
// Sample: AddLinks - Adds the different links into a pdf. The first 
//                    being a link the opens a file, the second goes
//					  to a different page, and the third opens a website. 
//					  
// Steps:
//  1) Create, set up, and place a link that will open a file
//  2) Create, set up, and place a link that will move to a new location
//  3) Create, set up, and place a link the will open a webpage
//  4) Save and close.
//========================================================================

#include <iostream>
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"
#include "PSFCalls.h"
#include "ASCalls.h"
#include "ASExtraCalls.h"
#include "InitializeLibrary.h"
#include "APDFLDoc.h"
#include "CosCalls.h"

//Function that creates and displays the text "Click Me" onto a pdf page given the x and y position.
PDEText clickMeTextMaker(double xPos, double yPos);

int main(int argc, char** argv)
{
    APDFLib lib(argv[1]);                                                 //Initialize the Adobe PDF Library.

    if (lib.isValid() == false)                                  //If it failed to initialize, return the error code.
        return lib.getInitError();

    ASErrorCode errCode = 0;                                     //This will catch error codes thrown during library usage.

    DURING

        std::wcout << L"Opening the input PDF." << std::endl;

        APDFLDoc inDoc(L"../_Input/AddLinks.pdf", true);                           //Opens the input PDF document.

        CosDoc inputDocCosDoc = PDDocGetCosDoc(inDoc.pdDoc);                       //The PDDoc's COS representation.

        PDPage page1 = PDDocAcquirePage(inDoc.pdDoc, 0);                           //The annotation will go on page 1.

        //Acquire PDEContent, PDE objects can be added to the acquired content.
        PDEContent pageContent = PDPageAcquirePDEContent(page1, NULL);

//====================================================================================================================================================================================
// Step 1) Create, set up, and place a link that will open a file.
//====================================================================================================================================================================================

        //Create a text object which displays "Click Me" at the given location with the clickMeTextMaker() function call
        PDEText clickMeText1 = clickMeTextMaker(72 * 6.48, 72 * 7.1);

        //Add this text object to the page's content
        PDEContentAddElem(pageContent, kPDEAfterLast, (PDEElement)clickMeText1);                             //Add the text element to the page's content.

        //Set up the annotation's bounds
        //Note: 72 represent an inch
        ASFixedRect annotLocation;
        PDETextGetBBox(clickMeText1, kPDETextRun, 0, &annotLocation);

        PDERelease(reinterpret_cast<PDEObject>(clickMeText1));

        //Create a file attachment annotation with the input page and bounds
        PDAnnot fileAnnot = PDPageCreateAnnot(page1, ASAtomFromString("FileAttachment"), &annotLocation);

        PDPageAddAnnot(page1, 0, fileAnnot);                                                                 //Add the annotation as the first annotation on the page.

        PDLinkAnnot fileLink = CastToPDLinkAnnot(fileAnnot);                                                 //Cast the file attachment annotation as a link.

        //Make the link's border unable to see
        PDLinkAnnotBorder linkBorder;
        linkBorder.width = 0;
        linkBorder.dashArrayLen = 0;
        PDLinkAnnotSetBorder(fileLink, &linkBorder);

        CosObj fileLinkDict = CosNewDict(inputDocCosDoc, false, 2);                                          //Create a cos dictionary object to hold attributes of the link.

        //Using the file specification key "F" set up the file input path
        CosDictPut(fileLinkDict, ASAtomFromString("F"), CosNewString(inputDocCosDoc, false, "../_Input/DOCXLink.docx", strlen("../_Input/DOCXLink.docx")));

        //Using the name key "S" set the type to a Launch for opening the file
        CosDictPut(fileLinkDict, ASAtomFromString("S"), CosNewName(inputDocCosDoc, false, ASAtomFromString("Launch")));

        PDAction fileLinkAction = PDActionFromCosObj(fileLinkDict);                                          //Set up an action object with the given attributes from the dictionary.

        PDLinkAnnotSetAction(fileLink, fileLinkAction);                                                      //Set the action to the link.

        CosObj fileLinkObj = PDAnnotGetCosObj(fileLink);                                                     //Get the cos object from the link.                       

        CosDictPutKeyString(fileLinkObj, "Subtype", CosNewNameFromString(inputDocCosDoc, false, "Link"));    //Set the cos object type to the subtype, link.

        std::wcout << L"Added file link." << std::endl;

//====================================================================================================================================================================================
// Step 2) Create, set up, and place a link that will move to a new location.
//====================================================================================================================================================================================

        //Create a text object which displays "Click Me" at the given location with the clickMeTextMaker() function call.
        PDEText clickMeText2 = clickMeTextMaker(72 * 6.48, 72 * 6.3);

        PDEContentAddElem(pageContent, kPDEAfterLast, (PDEElement)clickMeText2);                                //Add the text element to the page's content.

        PDERelease(reinterpret_cast<PDEObject>(clickMeText2));

        //Set up the annotation's bounds
        PDETextGetBBox(clickMeText2, kPDETextRun, 0, &annotLocation);

        //Create a new destination annotation with the input page and bounds
        PDAnnot newDestAnnot = PDPageCreateAnnot(page1, ASAtomFromString("Text"), &annotLocation);

        PDPageAddAnnot(page1, 1, newDestAnnot);                                                                 //Add the annotation as the second annotation on the page.

        PDLinkAnnot newDestLink = CastToPDLinkAnnot(newDestAnnot);                                              //Cast the file attachment annotation as a link.

        PDLinkAnnotSetBorder(newDestLink, &linkBorder);                                                         //Set the link's border to the previously made unseeable border.

        PDPage destPage = PDDocAcquirePage(inDoc.pdDoc, 2);                                                     //Create an instance of the page to jump to.

        //Get the bounds of that page
        ASFixedRect bounds;
        PDPageGetBBox(destPage, &bounds);

        //Create a View Destination pointing to the location desired along with different settings
        PDViewDestination nextDestination = PDViewDestCreate(inDoc.pdDoc, destPage,                             //The source document and destination page.
            ASAtomFromString("XYZ"),                                                                            //Fit Type, set to the upper-left corner.
            &bounds,                                                                                             //Pointer to the location rectangle we want.
            PDViewDestNULL,                                                                                     //Zoom factor. 0 means to inherit the current zoom factor.
            0);                                                                                                 //Unused argument.

        PDPageRelease(destPage);                                                                                //Release reference to page.

        PDAction nextDestAct = PDActionNewFromDest(inDoc.pdDoc, nextDestination, inDoc.pdDoc);                  //Create an action representing the destination.   

        PDLinkAnnotSetAction(newDestLink, nextDestAct);                                                         //Set the action to the link.

        CosObj newDestLinkObj = PDAnnotGetCosObj(newDestLink);                                                  //Get the cos object from the link.                         

        CosDictPutKeyString(newDestLinkObj, "Subtype", CosNewNameFromString(inputDocCosDoc, false, "Link"));    //Set the cos object type to the subtype, link.

        std::wcout << L"Added destination link." << std::endl;

//====================================================================================================================================================================================
// Step 3) Create, set up, and place a link the will open a webpage.
//====================================================================================================================================================================================

        //Create a text object which displays "Click Me" at the given location with the clickMeTextMaker() function call.
        PDEText clickMeText3 = clickMeTextMaker(72 * 6.48, 72 * 5.5);

        PDEContentAddElem(pageContent, kPDEAfterLast, (PDEElement)clickMeText3);                            //Add the text element to the page's content.

        PDERelease(reinterpret_cast<PDEObject>(clickMeText3));

        //Set up the annotation's bounds
        PDETextGetBBox(clickMeText3, kPDETextRun, 0, &annotLocation);

        PDAnnot URIAnnot = PDPageCreateAnnot(page1, ASAtomFromString("Text"), &annotLocation);              //Set up a new annotation.

        PDPageAddAnnot(page1, 1, URIAnnot);                                                                 //Add the annotation as the second annotation on the page.

        PDLinkAnnot URILink = CastToPDLinkAnnot(URIAnnot);                                                  //Cast the file attachment annotation as a link.

        PDLinkAnnotSetBorder(URILink, &linkBorder);                                                         //Set the link's border to the previously made unseeable border.

        CosObj URIDict = CosNewDict(inputDocCosDoc, false, 2);                                              //Create a cos dictionary object to hold attributes of the link.
        
        //Using the name key "S" set up the URI type
        CosDictPut(URIDict, ASAtomFromString("S"), CosNewName(inputDocCosDoc, false, ASAtomFromString("URI")));

        //Using the URI key "URI" set up the desired path
        CosDictPut(URIDict, ASAtomFromString("URI"), CosNewString(inputDocCosDoc, false, "http://www.datalogics.com", strlen("http://www.datalogics.com")));

        PDAction newPdAction = PDActionFromCosObj(URIDict);                                                 //Set up an action object with the given attributes from the dictionary.

        PDLinkAnnotSetAction(URILink, newPdAction);                                                         //Set the action to the link.

        CosObj URILinkObj = PDAnnotGetCosObj(URILink);                                                      //Get the cos object from the link.                    

        CosDictPutKeyString(URILinkObj, "Subtype", CosNewNameFromString(inputDocCosDoc, false, "Link"));    //Set the cos object type to the subtype, link.

        std::wcout << L"Added webpage link." << std::endl;

//====================================================================================================================================================================================
// Step 4) Save and close.
//====================================================================================================================================================================================

        PDPageSetPDEContentCanRaise(page1, NULL);                     //Set the content back into the page.

        std::wcout << L"Saving the output document." << std::endl;

        //Page and content released before closing the document.
        PDPageReleasePDEContent(page1, NULL);
        PDPageRelease(page1);

        inDoc.saveDoc(L"AddedLinks.pdf");                             //Save the pdf to the output save path.

        std::wcout << L"AddedLinks.pdf saved." << std::endl;

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);                                    //If there was an error, display it.

    END_HANDLER

    return errCode;                                                   //Returns program status.
}

//====================================================================================================================================================================================
// PDEText Function: Creates and displays the text "Click Me" onto a pdf page given the x and y position
//====================================================================================================================================================================================
PDEText clickMeTextMaker(double xPos, double yPos)
{
    PDEFontAttrs fontAttrs;                                                   //Struct that will contain font name and type.

    memset(&fontAttrs, 0, sizeof(fontAttrs));                                 //Ensure any "garbage" data is cleared out.

    fontAttrs.name = ASAtomFromString("CourierStd");                          //Set the font name and type. 
    fontAttrs.type = ASAtomFromString("Type1");

    //Locate the system font that corresponds to the PDEFontAttrs struct we just set.
    PDSysFont sysFont = PDFindSysFont(&fontAttrs, sizeof(fontAttrs), 0);

    //Create the CourierStd Type1 font with embed flag set.       
    PDEFont courierFont = PDEFontCreateFromSysFont(sysFont, kPDEFontCreateEmbedded);

    std::string textToDisplay = " Click Me ";                                 //Text that will be displayed on page.

    PDETextState tState;                                                      //Structure holding the attributes of a PDEText.

    ASDoubleMatrix textMatrix;                                                //Transformation matrix for text which determines location of the text on page.

    memset(&textMatrix, 0, sizeof(textMatrix));                               //Clear structure. 
    textMatrix.a = 10;                                                        //Set font width and height. 
    textMatrix.d = 10;                                                        //Set font point size.     
    textMatrix.h = xPos;                                                      //x coordinate on page (72 pixels = 1 inch).
    textMatrix.v = yPos;                                                      //y coordinate on page.   

    PDEGraphicState gState;                                                   //Struct that will hold display attributes.
    PDEDefaultGState(&gState, sizeof(PDEGraphicState));                       //Set graphics state to default values.

    PDERelease(reinterpret_cast<PDEObject>(gState.fillColorSpec.space));      //Release the stroke color space before modifying it.

    //Set color to blue.
    ASFixed red = ASFloatToFixed(0);
    ASFixed green = ASFloatToFixed(0);
    ASFixed blue = ASFloatToFixed(1);

    //Set the RGB color space.
    gState.fillColorSpec.space = PDEColorSpaceCreateFromName(ASAtomFromString("DeviceRGB"));

    //Set up the color space values to form the wanted color.
    gState.fillColorSpec.value.color[0] = red;
    gState.fillColorSpec.value.color[1] = green;
    gState.fillColorSpec.value.color[2] = blue;

    PDEText textObj = PDETextCreate();                                        //Create a new text run.

    //Adding the text run to the PDE text object.
    PDETextAddEx(textObj,                                                     //Text container to add to. 
        kPDETextRun,                                                          //kPDETextRun or kPDETextChar for text runs or text characters. 
        0,                                                                    //The index after which to add the text run.
        (Uns8 *)textToDisplay.c_str(),                                        //Text to add.    
        textToDisplay.length(),                                               //Length of text. 
        courierFont,                                                          //Font to apply to text. 
        &gState, sizeof(gState),                                              //PDEGraphicState and its size. Contains graphical attributes of the text object.
        &tState, 0,                                                           //Text state and its size .Contains textual attributes of the text object.
        &textMatrix,                                                          //Matrix containing size and location for the text.
        NULL);                                                                //Stroke matrix for the line width when stroking text.  

    //Release used objects
    PDERelease(reinterpret_cast<PDEObject>(courierFont));
    PDERelease(reinterpret_cast<PDEObject>(gState.strokeColorSpec.space));
    PDERelease(reinterpret_cast<PDEObject>(gState.fillColorSpec.space));

    return textObj;                                                           //Return the text object.

}
