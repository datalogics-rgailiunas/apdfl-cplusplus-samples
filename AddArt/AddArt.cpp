//******************************************************************************
#include <iostream>
#include "MyPDFLibUtils.h"
#include "PagePDECntCalls.h"
#include "PEWCalls.h"
#include "PERCalls.h"
#include "ASCalls.h"
#include "ASExtraCalls.h"

int main()
{
   
    int errorCode = MyPDFLInit();  //Initialize the APDFL

    //Check for errors upon initialization
    if (errorCode != 0)
    {
        std::cerr << "Initialization error. See \"AcroErr.h\" for more info.\n" << std::endl;
        std::cerr << "Error system: " << ErrGetSystem(errorCode) << std::endl;
        std::cerr << "Error Severity: " << ErrGetSeverity(errorCode) << std::endl;
        std::cerr << "Error Code: " << ErrGetCode(errorCode) << std::endl;
        return errorCode;
    }

//******************************************************************************
//Step 1) Create a new PDF Document, Insert Page, Acquire Content
//******************************************************************************

    wchar_t * pathName = L"AddArtOut.pdf"; //Output file path

    PDDoc pdDoc = PDDocCreate();           //Create the document
    
    //Set the page dimensions for the new document (10x10 inches)
    ASFixedRect mediaBox; 
    mediaBox.left = fixedZero;
    mediaBox.right = Int16ToFixed(72 * 10);
    mediaBox.bottom = fixedZero;
    mediaBox.top = Int16ToFixed(72 * 10);
    
    PDPage pdPage = PDDocCreatePage(pdDoc, PDBeforeFirstPage, mediaBox);    //Create and insert the page into the PDDoc

    PDEContent pdeContent = PDPageAcquirePDEContent(pdPage, 0);             //Acquire list of contents from page

//******************************************************************************
//Step 2) Initialize the PDEGraphicsState and color information
//******************************************************************************

    PDEPath pdePath = PDEPathCreate();                      //Create path which will be used for drawing shapes
    
    PDEPathSetPaintOp(pdePath, kPDEStroke);                 //PDEPath will be stroked, but not filled

    PDEColorValue strokeColorValue, fillColorValue;         //Stroke and fill colors
    
    memset(&strokeColorValue, 0, sizeof(PDEColorValue));    //Initially values = 0
    memset(&fillColorValue, 0, sizeof(PDEColorValue));

    ASFixed red = ASInt32ToFixed(.1);                                        //Values red + blue = purple                              
    ASFixed green = ASInt32ToFixed(0);
    ASFixed blue = ASInt32ToFixed(.1);

    strokeColorValue.color[0] = red;                        //Assign values to the struct
    strokeColorValue.color[1] = green;
    strokeColorValue.color[2] = blue;

    fillColorValue.color[0] = red;
    fillColorValue.color[1] = green;
    fillColorValue.color[2] = blue;

    PDEColorSpace colorSpace = PDEColorSpaceCreateFromName(ASAtomFromString("DeviceRGB")); //RGB color space

    PDEColorSpec strokeColorSpec, fillColorSpec;              

    strokeColorSpec.space = fillColorSpec.space = colorSpace; //Set the color space to RGB

    strokeColorSpec.value = strokeColorValue;                 //Set color value to purple
    fillColorSpec.value = fillColorValue;

    //Initialize the graphic state
    PDEGraphicState gState;                                  
    memset(&gState, 0, sizeof(gState));
    gState.fillColorSpec = fillColorSpec;
    gState.strokeColorSpec = strokeColorSpec;
    gState.lineWidth = fixedOne;
    gState.miterLimit = fixedTen;
    gState.flatness = fixedZero;

    PDEElementSetGState(reinterpret_cast<PDEElement>(pdePath), &gState, sizeof(PDEGraphicState));

//******************************************************************************
//Step 3) Profit.
//******************************************************************************

    ASFixed dataForShape[5];
    dataForShape[0] = kPDERect;
    dataForShape[1] = ASInt32ToFixed(72 * 3);
    dataForShape[2] = ASInt32ToFixed(72 * 5);
    dataForShape[3] = ASInt32ToFixed(72 * 1);
    dataForShape[4] = ASInt32ToFixed(72 * 4);

    PDEPathSetData(pdePath, dataForShape, sizeof(dataForShape));

    PDEContentAddElem(pdeContent, kPDEAfterLast, reinterpret_cast<PDEElement>(pdePath));

    PDPageNotifyContentsDidChange(pdPage);

    PDPageSetPDEContentCanRaise(pdPage, 0);

//******************************************************************************
//Step 4) Save and release objects
//******************************************************************************

    //Get the systems unicode format
    ASUnicodeFormat unicodeFormat = NULL;
    if (sizeof(wchar_t) == 2)
        unicodeFormat = kUTF16HostEndian;
    else
        unicodeFormat = kUTF32HostEndian;

    //Create unicode string asText used to create the ASPathName object
    ASText asText = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(pathName), unicodeFormat);

    //Path name will be used to save the document
    ASPathName asPathName = ASFileSysCreatePathFromDIPathText(NULL, asText, NULL);

    //Save the document
    PDDocSave(pdDoc, PDSaveFull, asPathName, NULL, NULL, NULL);

    //Release objects
    PDERelease(reinterpret_cast<PDEObject>(pdePath));
    PDPageReleasePDEContent(pdPage, 0);
    PDPageRelease(pdPage);
    ASTextDestroy(asText);
    ASFileSysReleasePath(NULL, asPathName);
    PDDocClose(pdDoc);

    MyPDFLTerm();      //Terminate the APDFL

    return errorCode;  //Return 0 if no errors
}
