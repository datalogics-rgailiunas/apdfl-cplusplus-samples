

#include <iostream>

#include "PSFCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"
#include "ASExtraCalls.h"
#include "CosCalls.h"
#include "APDFLDoc.h"
#include "InitializeLibrary.h"

PDEText textMaker(std::string displayText, double xPos, double yPos);
PDAnnot annotMaker(wchar_t* filepath, PDDoc inDoc, CosDoc inputCosDoc, int leftPlace, int rightPlace, int topPlace, int botPlace);

int main(int argc, char** argv)
{

    APDFLib libInit;                      //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;              //Variable used to report any exceptions/errors if they occured.

    if (libInit.isValid() == false)       //If there was a problem in initialization, return the error code.
        return libInit.getInitError();    
    
    DURING

//============================================================================================================

//============================================================================================================

        APDFLDoc doc;    //Create a new empty document.

        //Insert a standard 8.5 inch x 11 inch page into the document;
        doc.insertPage(Int16ToFixed((8.5 * 72)), Int16ToFixed((11 * 72)), PDBeforeFirstPage);
        
        //Get the first page from the document. Note: caller is responsible for releasing the PDPage acquired.
        PDPage page = doc.getPage(0);

        //Acquire PDEContent, PDE objects can be added to the acquired content.
		PDEContent pageContent = PDPageAcquirePDEContent(page, NULL);

//================================================================================================================================================

//================================================================================================================================================
		
        PDOCG optionalGroupText = PDOCGCreate(doc.pdDoc, ASTextFromPDText("TextLayer"));

        PDOCG optionalGroupAnnot = PDOCGCreate(doc.pdDoc, ASTextFromPDText("AnnotationLayer"));

		//PDOCMD optionalGroupMDText = PDOCMDCreate(doc.pdDoc, &optionalGroupText, kOCMDVisibility_AnyOn);

        PDOCConfig  docConfig = PDDocGetOCConfig(doc.pdDoc);
        PDOCGSetInitialState(optionalGroupText, docConfig, true);
        PDOCGSetInitialState(optionalGroupAnnot, docConfig, true);

        CosObj  existingOCOrder;
        ASInt32  numberOfObjs;

        PDOCConfigGetOCGOrder(docConfig, &existingOCOrder);
        numberOfObjs = CosArrayLength(existingOCOrder);
        CosArrayInsert(existingOCOrder, numberOfObjs, PDOCGGetCosObj(optionalGroupText));

        PDOCConfigGetOCGOrder(docConfig, &existingOCOrder);
        numberOfObjs = CosArrayLength(existingOCOrder);
        CosArrayInsert(existingOCOrder, numberOfObjs, PDOCGGetCosObj(optionalGroupAnnot));

        //// save back to the OCProperties for the document 
        //PDOCConfigSetOCGOrder(docConfig, existingOCOrder);

        // need to add the PDOCG to an array 
        PDOCG  pdDocArrayText[2];
        pdDocArrayText[0] = optionalGroupText;
        pdDocArrayText[1] = NULL;
        PDOCMD  optionalGroupMDText = PDOCMDCreate(doc.pdDoc, pdDocArrayText, kOCMDVisibility_AllOn);

        PDOCG  pdDocArrayAnnot[2];
        pdDocArrayAnnot[0] = optionalGroupAnnot;
        pdDocArrayAnnot[1] = NULL;
        PDOCMD  optionalGroupMDAnnot = PDOCMDCreate(doc.pdDoc, pdDocArrayAnnot, kOCMDVisibility_AllOn);

//================================================================================================================================================

//================================================================================================================================================
        
		PDEText displayText1 = textMaker("All the text on this page will be placed in it's own layer", 72 * 1, 72 * 10);

		PDEText displayText2 = textMaker("Whereas the attachments will appear in a seperate layer", 72 * 1, 72 * 9.75);

        PDEText displayText3 = textMaker("There will be a .xlsx file attachment to the right", 72 * 1, 72 * 8);

        PDEText displayText4 = textMaker("There will be a .docx file attachment to the right", 72 * 1, 72 * 7);
    
		PDEContentAddElem(pageContent, kPDEAfterLast, (PDEElement)displayText1);                            //Add the text element to the page's content.

		PDEContentAddElem(pageContent, kPDEAfterLast, (PDEElement)displayText2);                            //Add the text element to the page's content.

        PDEContentAddElem(pageContent, kPDEAfterLast, (PDEElement)displayText3);                            //Add the text element to the page's content.

        PDEContentAddElem(pageContent, kPDEAfterLast, (PDEElement)displayText4);                            //Add the text element to the page's content.

		PDEContainer textContainer = PDEContainerCreate(ASAtomFromString("Texts"), NULL, false);

		PDEContainerSetContent(textContainer, pageContent);

		PDEElementSetOCMD((PDEElement)textContainer, optionalGroupMDText);

		PDPageSetPDEContentCanRaise(page, NULL);                     //Set the content back into the page.

		PDERelease(reinterpret_cast<PDEObject>(displayText1));

		PDERelease(reinterpret_cast<PDEObject>(displayText2));

        PDERelease(reinterpret_cast<PDEObject>(displayText3));

        PDERelease(reinterpret_cast<PDEObject>(displayText4));

//================================================================================================================================================

//================================================================================================================================================

        ASFixedRect annotLocation;
        annotLocation.left = ASFloatToFixed(5.50 * 72);                           //There are 72 pixels per inch.
        annotLocation.right = ASFloatToFixed(6.00 * 72);
        annotLocation.top = ASFloatToFixed(8.20 * 72);
        annotLocation.bottom = ASFloatToFixed(7.70 * 72);

        PDPage page1 = PDDocAcquirePage(doc.pdDoc, 0);

        PDAnnot newAnnot = PDPageCreateAnnot(page1, ASAtomFromString("FileAttachment"), &annotLocation);

        PDPageAddAnnot(page1, -2, newAnnot);
        PDAnnotSetOCMD(newAnnot, optionalGroupMDAnnot);

        annotLocation.left = ASFloatToFixed(5.50 * 72);                           //There are 72 pixels per inch.
        annotLocation.right = ASFloatToFixed(6.00 * 72);
        annotLocation.top = ASFloatToFixed(7.20 * 72);
        annotLocation.bottom = ASFloatToFixed(6.70 * 72);

        PDAnnot newAnnot2 = PDPageCreateAnnot(page1, ASAtomFromString("FileAttachment"), &annotLocation);

        PDAnnotSetOCMD(newAnnot2, optionalGroupMDAnnot);
        PDPageAddAnnot(page1, -2, newAnnot2);

//================================================================================================================================================

//================================================================================================================================================

        doc.saveDoc(L"LayersCreated.pdf", PDSaveFull | PDSaveLinearized);    //Save the PDF document in the working directory.

        std::wcout << L"LayersCreated.pdf saved with text to be placed." << std::endl << std::endl;

    HANDLER

            errCode = ERRORCODE;           

            libInit.displayError(errCode);    //If there was an error, display the error that occured.

    END_HANDLER

    return errCode;                           //APDFLib's destructor terminates the library.

}

//====================================================================================================================================================================================
// PDEText Function: Creates and displays the text given string onto a pdf page given the x and y position
//====================================================================================================================================================================================
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
		&tState, 0,                                                           //Text state and its size .Contains textual attributes of the text object.
		&textMatrix,                                                          //Matrix containing size and location for the text.
		NULL);                                                                //Stroke matrix for the line width when stroking text.  

	//Release used objects
	PDERelease(reinterpret_cast<PDEObject>(courierFont));
	PDERelease(reinterpret_cast<PDEObject>(gState.strokeColorSpec.space));
	PDERelease(reinterpret_cast<PDEObject>(gState.fillColorSpec.space));

	return textObj;                                                           //Return the text object.

}
