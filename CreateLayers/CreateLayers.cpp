// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//===========================================================================
// Sample: CreateLayers - Creates two layers in a pdf that will each contain  
//                        and display text and annotations.
//					  
// Steps:
//  1) Create a pdf document and extract its content.
//  2) Add text to the page and set what layer they belong to.
//  3) Add annotations to the page and set what layer they belong to.
//  4) Save the output document and exit.
//===========================================================================

#include <iostream>
#include "PSFCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"
#include "CosCalls.h"
#include "APDFLDoc.h"
#include "InitializeLibrary.h"

//A function that places text onto a given position in a PDF.
PDEText textMaker(std::string displayText, double xPos, double yPos);

int main(int argc, char** argv)
{

    APDFLib libInit;                      //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;              //Variable used to report any exceptions/errors if they occurred.

    if (libInit.isValid() == false)       //If there was a problem in initialization, return the error code.
        return libInit.getInitError();

    DURING

//==============================================================================================================================================================
// Step 1) Create a pdf document and extract its content.
//==============================================================================================================================================================

        APDFLDoc doc;													 //Create a new empty document.

        //Insert a standard 8.5 inch x 11 inch page into the document.
        doc.insertPage(Int16ToFixed((8.5 * 72)), Int16ToFixed((11 * 72)), PDBeforeFirstPage);

        PDPage page = doc.getPage(0);									 //Get the first page from the document. 

        PDEContent pageContent = PDPageAcquirePDEContent(page, NULL);    //Acquire the content of the page.

//==============================================================================================================================================================
// Step 2) Set up the optional content groups, commonly referred to as layers.
//==============================================================================================================================================================

        //Create optional content groups (Layers) for texts and annotations.
        PDOCG optionalGroupText = PDOCGCreate(doc.pdDoc, ASTextFromPDText("TextLayer"));
        PDOCG optionalGroupAnnot = PDOCGCreate(doc.pdDoc, ASTextFromPDText("AnnotationLayer"));

        //Set the layers initial state to visible.
        PDOCConfig  ocConfig = PDDocGetOCConfig(doc.pdDoc);
        PDOCGSetInitialState(optionalGroupText, ocConfig, true);
        PDOCGSetInitialState(optionalGroupAnnot, ocConfig, true);

        CosObj order;                               //The order of the optional content.
        ASInt32 cosObjectTotal;                     //Keeps track of the total cosObjects.

        PDOCConfigGetOCGOrder(ocConfig, &order);    //Find the order.
        cosObjectTotal = CosArrayLength(order);     //Find the total cosObjects.

        //Insert the layers as a cosObject in the pdf. 
        CosArrayInsert(order, cosObjectTotal, PDOCGGetCosObj(optionalGroupText));
        CosArrayInsert(order, cosObjectTotal + 1, PDOCGGetCosObj(optionalGroupAnnot));

        //Put the new order back as a part of the pdf's configuration.
        PDOCConfigSetOCGOrder(ocConfig, order);

        //Create a layer array in order to properly pass the layer into the membership dictionary creation function.
        PDOCG pdDocArrayText[2];
        pdDocArrayText[0] = optionalGroupText;
        pdDocArrayText[1] = NULL;

        //Obtain the membership dictionary of the text layer.
        PDOCMD optionalGroupMDText = PDOCMDCreate(doc.pdDoc, pdDocArrayText, kOCMDVisibility_AllOn);

        //Create a layer array in order to properly pass the layer into the membership dictionary creation function.
        PDOCG  pdDocArrayAnnot[2];
        pdDocArrayAnnot[0] = optionalGroupAnnot;
        pdDocArrayAnnot[1] = NULL;

        //Obtain the membership dictionary of the annotation layer.
        PDOCMD optionalGroupMDAnnot = PDOCMDCreate(doc.pdDoc, pdDocArrayAnnot, kOCMDVisibility_AllOn);

//==============================================================================================================================================================
// Step 3) Add text to the page and set what layer they belong to.
//==============================================================================================================================================================

        //By calling the textMaker function, place the following text at the given location.
        PDEText displayText1 = textMaker("All the text on this page will be placed in it's own layer", 72 * 1, 72 * 10);
        PDEText displayText2 = textMaker("Whereas the attachments will appear in a separate layer", 72 * 1, 72 * 9.75);
        PDEText displayText3 = textMaker("There will be a .xlsx file attachment to the right", 72 * 1, 72 * 8);
        PDEText displayText4 = textMaker("There will be a .docx file attachment to the right", 72 * 1, 72 * 7);

        //Add the created text objects to the page's content.
        PDEContentAddElem(pageContent, kPDEAfterLast, (PDEElement)displayText1);
        PDEContentAddElem(pageContent, kPDEAfterLast, (PDEElement)displayText2);
        PDEContentAddElem(pageContent, kPDEAfterLast, (PDEElement)displayText3);
        PDEContentAddElem(pageContent, kPDEAfterLast, (PDEElement)displayText4);

        PDEContainer textContainer = PDEContainerCreate(ASAtomFromString("Texts"), NULL, false);    //Create an empty container for the text.

        PDEContainerSetContent(textContainer, pageContent);					                        //Create a container for all the text objects.

        PDEElementSetOCMD((PDEElement)textContainer, optionalGroupMDText);                          //Set the container's membership dictionary to the text layer.

        PDPageSetPDEContentCanRaise(page, NULL);							                        //Set the content back into the page.

        //Release the text, no longer in use.
        PDERelease(reinterpret_cast<PDEObject>(displayText1));
        PDERelease(reinterpret_cast<PDEObject>(displayText2));
        PDERelease(reinterpret_cast<PDEObject>(displayText3));
        PDERelease(reinterpret_cast<PDEObject>(displayText4));

//==============================================================================================================================================================
// Step 4) Add annotations to the page and set the layer they belong to. 
//==============================================================================================================================================================

        //Set up the bounds for the first annotation, where 72 represents an inch.
        ASFixedRect annotLocation;
        annotLocation.left = ASFloatToFixed(5.50 * 72);
        annotLocation.right = ASFloatToFixed(6.00 * 72);
        annotLocation.top = ASFloatToFixed(8.20 * 72);
        annotLocation.bottom = ASFloatToFixed(7.70 * 72);

        PDAnnot newAnnot = PDPageCreateAnnot(page, ASAtomFromString("FileAttachment"), &annotLocation);    //Create the annotation at the location.

        //Add the annotation to the page, where -2 means to add to the end of the array.
        PDPageAddAnnot(page, -2, newAnnot);

        PDAnnotSetOCMD(newAnnot, optionalGroupMDAnnot);		                                               //Set the annotation to the annotation layer.

        //Move the bounds for the second annotation.
        annotLocation.left = ASFloatToFixed(5.50 * 72);
        annotLocation.right = ASFloatToFixed(6.00 * 72);
        annotLocation.top = ASFloatToFixed(7.20 * 72);
        annotLocation.bottom = ASFloatToFixed(6.70 * 72);

        PDAnnot newAnnot2 = PDPageCreateAnnot(page, ASAtomFromString("FileAttachment"), &annotLocation);   //Create the second annotation.

        PDAnnotSetOCMD(newAnnot2, optionalGroupMDAnnot);                                                   //Set the annotation to the annotation layer.

        PDPageAddAnnot(page, -2, newAnnot2);			                                                   //Add the annotation to the page.

//==============================================================================================================================================================
// Step 5) Save the output document and exit.
//==============================================================================================================================================================

        doc.saveDoc(L"LayersCreated.pdf", PDSaveFull | PDSaveLinearized);    //Save the PDF document with the given name.

        std::wcout << L"LayersCreated.pdf saved with text to be placed." << std::endl;

    HANDLER

        errCode = ERRORCODE;

        libInit.displayError(errCode);								  	     //If there was an error, display the error that occurred.

    END_HANDLER

    return errCode;															 //Returns the program status.

}

//==============================================================================================================================================================
// PDEText Function: Creates and displays the text given string onto a pdf page given the x and y position.
//==============================================================================================================================================================
PDEText textMaker(std::string displayText, double xPos, double yPos)
{
    PDEFontAttrs fontAttrs;                                                   //Struct that will contain font name and type.

    memset(&fontAttrs, 0, sizeof(fontAttrs));                                 //Ensure any "garbage" data is cleared out.

    fontAttrs.name = ASAtomFromString("CourierStd");                          //Set the font name and type. 
    fontAttrs.type = ASAtomFromString("Type1");

    //Locate the system font that corresponds to the PDEFontAttrs struct we just set.
    PDSysFont sysFont = PDFindSysFont(&fontAttrs, sizeof(fontAttrs), 0);

    //Create the CourierStd Type1 font with embed flag set.       
    PDEFont courierFont = PDEFontCreateFromSysFont(sysFont, kPDEFontCreateEmbedded);

    PDETextState tState;                                                      //Structure holding the attributes of a PDEText.

    ASDoubleMatrix textMatrix;                                                //Transformation matrix for text which determines location of the text on page.

    memset(&textMatrix, 0, sizeof(textMatrix));                               //Clear structure. 
    textMatrix.a = 10;                                                        //Set font width and height. 
    textMatrix.d = 10;                                                        //Set font point size.     
    textMatrix.h = xPos;                                                      //x coordinate on page (72 pixels = 1 inch).
    textMatrix.v = yPos;                                                      //y coordinate on page.   

    PDEGraphicState gState;                                                   //Struct that will hold display attributes.
    PDEDefaultGState(&gState, sizeof(PDEGraphicState));                       //Set graphics state to default values.

    PDEText textObj = PDETextCreate();                                        //Create a new text run.

    //Adding the text run to the PDE text object.
    PDETextAddEx(textObj,                                                     //Text container to add to. 
        kPDETextRun,                                                          //kPDETextRun or kPDETextChar for text runs or text characters. 
        0,                                                                    //The index after which to add the text run.
        (Uns8 *)displayText.c_str(),                                          //Text to add.    
        displayText.length(),                                                 //Length of text. 
        courierFont,                                                          //Font to apply to text. 
        &gState, sizeof(gState),                                              //PDEGraphicState and its size. Contains graphical attributes of the text object.
        &tState, 0,                                                           //Text state and its size. Contains textual attributes of the text object.
        &textMatrix,                                                          //Matrix containing size and location for the text.
        NULL);                                                                //Stroke matrix for the line width when stroking text.  

    //Release used objects.
    PDERelease(reinterpret_cast<PDEObject>(courierFont));
    PDERelease(reinterpret_cast<PDEObject>(gState.strokeColorSpec.space));
    PDERelease(reinterpret_cast<PDEObject>(gState.fillColorSpec.space));

    return textObj;                                                           //Return the text object.

}
