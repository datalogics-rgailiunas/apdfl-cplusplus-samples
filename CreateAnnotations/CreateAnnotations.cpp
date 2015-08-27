// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//==============================================================================
// Sample: Annotations - Demonstrates adding annotations and extracting their
//             content.
//
// Note:
// This sample annotates all the PDEElements of the input document (it
// highlights the text and adds a text annotation to everything else),
// and then extracts the text of the annotations into a new PDF document.
//
// Steps:
// 1) Create an annotation for each PDEElement on the page.
// 2) Save the document and close it.
// 3) Extract the annotations' text content into a new document.
//==============================================================================

#include <vector>
#include <sstream>
#include <map>

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#include "PSFCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"
#include "DLExtrasCalls.h"
#include "CosCalls.h"
#include "ASExtraVers.h"

//void function: Searches through a PDEContent object, and the PDEContents of its PDEContainer, PDEgroup, and PDEForm objects, adding all PDEElements to a list.
void extractPDEElements(PDEContent* c, std::vector<PDEElement>* list);

//void function: Sets the annotation's quads.
void PDAnnotSetQuads(PDAnnot annot, ASFixedQuad *quads, ASArraySize numQuads);

int main(int argc, char** argv)
{
    APDFLib libInit;                                         //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;                                 //Variable used to report any exceptions/errors if they occured.

    if (libInit.isValid() == false)                          //If there was a problem in initialization, return the error code.
        return libInit.getInitError();

    DURING

        APDFLDoc doc(L"../_Input/CreateAnnotations.pdf", true);    //Open the input document, repairing it if necessary.

    //We will add text annotations.

    //We will add text annotations to every 10th word on the page.
    PDPage inPage = doc.getPage(0);

//===================================================================================================================================================================================
// 1) Create an annotation for each PDEElement on the page.
//===================================================================================================================================================================================

    //Retrieve all the PDEElements on the page.
    PDEContent inPageContent = PDPageAcquirePDEContent(inPage, 0);
    ASInt32 elems = PDEContentGetNumElems(inPageContent);

    std::vector<PDEElement> pageElements;
    extractPDEElements(&inPageContent,&pageElements);

    std::wcout << L"This page has " << pageElements.size() << L" PDEElements." <<std::endl;
    std::wcout << L"Creating an annotation for each." <<std::endl;

    int highlightColorNum = 0;                                                                 //This will cycle between 0, 1, and 2 to cycle highlight colors.
    for (PDEElement next : pageElements)
    {
        //Create the text content for the annotation.
        int numContained = 0;                                                                  //If the element is a container, form, or group, we will check how many elements it has.

        //These must be initialized outside of the switch statement.
        PDEContainer container;
        PDEForm form;
        PDEGroup group;

        std::stringstream annotContent;
        annotContent << "This is a ";
        switch (PDEObjectGetType((PDEObject)next))
        {
        case kPDEContainer:
            annotContent << "container";
            container = (PDEContainer)next;
            numContained = PDEContentGetNumElems(PDEContainerGetContent(container));           //PDEContentGetNumElems will not count how many elements contained containers have.
            break;
        case kPDEForm:
            annotContent << "form";
            form = (PDEForm)next;
            numContained = PDEContentGetNumElems(PDEFormGetContent(form));
            break;
        case kPDEGroup:
            group = (PDEGroup)next;
            numContained = PDEContentGetNumElems(PDEGroupGetContent(group));
            annotContent << "group";
            break;
        case kPDEImage:
            annotContent << "image";
            break;
        case kPDEPath:
            annotContent << "path";
            break;
        case kPDEPlace:
            annotContent << "place";
            break;
        case kPDEText:
            annotContent << "text object";
            break;
        case kPDEXObject:
            annotContent << "XObject";
            break;
        }

        if (numContained)
            annotContent << ", containing " << numContained << " elements";

        annotContent << ". It is situated at:\n";

        ASFixedRect elementLoc;
        PDEElementGetBBox(next, &elementLoc);

        annotContent << "top: " << ASFixedToFloat(elementLoc.top)    << "\n";
        annotContent << "bot: " << ASFixedToFloat(elementLoc.bottom) << "\n";
        annotContent << "lef: " << ASFixedToFloat(elementLoc.left)   << "\n";
        annotContent << "rgt: " << ASFixedToFloat(elementLoc.right);

        //The content string
        const char* annotContentStr = annotContent.str().c_str();

        //The title string (and associated ASText)
        const char* annotTitleStr = "Page Element";
        ASText annotTitleAST = ASTextFromUnicode((ASUTF16Val*)annotContentStr, kUTF8);

        ASFixedRect annotLoc = elementLoc;

        char* annotType;
        bool isHighlight = (PDEObjectGetType((PDEObject)next) == kPDEText);

        if (isHighlight)
            annotType = "Highlight";
        else
            annotType = "Text";

        PDAnnot annot = PDPageCreateAnnot(inPage, ASAtomFromString(annotType), &annotLoc);     //Create the annotation. The annotation is not actually added to the page until PDPageAddAnnot.

        //Set the annot's quads. This will only have an effect for the highlight annotation, not the text annotation.
        ASFixedQuad annotLocQuad;
        //                  horizontal      vertical
        annotLocQuad.bl = { annotLoc.left,  annotLoc.bottom };
        annotLocQuad.br = { annotLoc.right, annotLoc.bottom };
        annotLocQuad.tl = { annotLoc.left,  annotLoc.top };
        annotLocQuad.tr = { annotLoc.right, annotLoc.top };
        PDAnnotSetQuads(annot, &annotLocQuad, 1);

        //This annot will not be able to be edited.
        PDAnnotSetFlags(annot, PDAnnotGetFlags(annot) | pdAnnotLock | pdAnnotLockContents);

        //The annot's title.
        PDAnnotSetTitle(annot, annotTitleStr, strlen(annotTitleStr));

        //The annot's text. We have to cast it to a TextAnnot to access the SetContents method, though, which is perfectly acceptable.
        PDTextAnnot textAnnot = CastToPDTextAnnot(annot);
        //give the text annot its content
        PDTextAnnotSetContentsASText(textAnnot, annotTitleAST);
        ASTextDestroy(annotTitleAST);

        //Set the annot's color.
        PDColorValueRec cr; PDColorValue color;
        color = &cr;
        color->space = PDDeviceRGB;                                                            //Only RGB is acctebable for annotations.
        if (isHighlight)
        {
            //Cycle between R/G/B for highlight annotations.
            color->value[0] = highlightColorNum == 0 ? fixedOne : fixedHalf;
            color->value[1] = highlightColorNum == 1 ? fixedOne : fixedHalf;
            color->value[2] = highlightColorNum == 2 ? fixedOne : fixedHalf;
            ++highlightColorNum %= 3;    //Add one and mod 3.
        }
        else
        {
            //For the text annotations, yellow.
            color->value[0] = fixedOne;
            color->value[1] = fixedOne;
            color->value[2] = fixedZero;
        }
        PDAnnotSetColor(annot, color);

        //Add the annot to the page.
        PDPageAddAnnot(inPage, kPDEAfterLast, annot);

    }


//===================================================================================================================================================================================
// 2) Save the document and close it.
//
// Note: We want it closed so we can re-open it for annotation content extraction.
//===================================================================================================================================================================================

    std::wcout << L"Saving the annotated document..." << std::endl;

    PDPageRelease(inPage);
    doc.saveDoc(L"Annotated.pdf");                     //Save the document. APDFLDoc defaults to using the "PDSaveFull" flag while saving.

    //APDFLDoc's destructor takes care of closing the document and releasing the rest of the resources.

//===================================================================================================================================================================================
// 3) Extract the annotations' text content into a new document.
//===================================================================================================================================================================================

    std::wcout << L"Extracting the annotations to a new document." << std::endl;

    APDFLDoc annotDoc(L"Annotated.pdf", true);
    PDPage annotPage = annotDoc.getPage(0);

    //Prepare the text object which will hold all the extracted text.
    PDEText annotationsText = PDETextCreate();                               //This will hold all the extracted text.
    ASFixedMatrix textLoc;                                                   //We'll use this to place the texts inside annotationsText.
    memset(&textLoc, 0, sizeof(textLoc));
    ASFixed fontSize = FloatToASFixed(12.0f);                                //12-point font.
    textLoc.a = textLoc.d = fontSize;                                        //Character width and height, respectively.
    textLoc.v = textLoc.h = 0;                                               //The vertical and horizontal position of the text, respectively. We will set v as we place texts.

    //Load the font we'll use to write the text content.
    PDEFontAttrs fontAttrs;                                                  //Contains data to retrieve the font.
    memset(&fontAttrs, 0, sizeof(fontAttrs));                                //Ensure there's no garbage in the struct.
    fontAttrs.name = ASAtomFromString("CourierStd");                         //The font name.
    fontAttrs.type = ASAtomFromString("Type1");                              //The font type.
    PDSysFont sysFont = PDFindSysFont(&fontAttrs, sizeof(fontAttrs), 0);     //Locate the system font that corresponds to the font attributes.
    PDEFont font = PDEFontCreateFromSysFont(sysFont, kPDEFontDoNotEmbed);    //Create the pdeFont with the sysFont. We won't embed the font.

    //A default graphics state with which to draw the text.
    PDEGraphicState graphics;
    PDEDefaultGState(&graphics, sizeof(PDEGraphicState));

    int maxDigits = log10((double)PDPageGetNumAnnots(annotPage)) + 1;                            //The maximum number of digits of n that the nth text annotation can have. Used to pad the text string.
    int numTextAnnots = 0;                                                   //The number of annotations actually containing textual content, updated as we go along.

    //Extract each annotation's text content (if any) into our text object.
    for (int i = 0; i < PDPageGetNumAnnots(annotPage); ++i)
    {
        PDAnnot next = PDPageGetAnnot(annotPage, i);

        //A buffer to hold the annotation's text content.
        const size_t buffersize = 1000;
        char buffer[buffersize];

        //Does the annotation actually have text conent?
        PDTextAnnot nextAsText = CastToPDTextAnnot(next);
        PDTextAnnotGetContents(nextAsText, buffer, buffersize);              //Put the content of the text annotation into buffer.
        if (buffer[0] != '\0')
        {
            numTextAnnots++;

            //Prepare the string to output.
            std::stringstream extractedString;

            extractedString << "Annotation no. " << numTextAnnots;

            //Pad out the spaces so that the extracted strings line up.
            int numDigits = log10((double)numTextAnnots) + 1;                //The number of digits of i that this annotation, the ith one, has.
            for (int i = 0; i < (maxDigits - numDigits); ++i)
                extractedString << " ";

            extractedString << " '" << buffer << "'";                        //The content is placed in the output string here.

            //Add the text!
            ASText extractedAST = ASTextFromUnicode((ASUTF16Val*)extractedString.str().c_str(), kUTF8);
            PDETextAddASText(annotationsText, kPDETextRun, numTextAnnots - 1, extractedAST, font, &graphics, sizeof(PDEGraphicState), NULL, 0, &textLoc);
            ASTextDestroy(extractedAST);

            //Position the next text.
            textLoc.v -= fontSize;
        }
    }

    //Determine the size of the page required to place this text object.
    ASFixed neededWidth  = fixedZero;
    ASFixed neededHeight = fixedZero;

    for (int i = 0; i < numTextAnnots; ++i)
    {
        ASFixedRect thisBox;
        PDETextGetBBox(annotationsText, kPDETextRun, i, &thisBox);
        
        //The width of this text object.
        ASFixed thisWidth  = (thisBox.right - thisBox.left);
        //The height of this text object.
        ASFixed thisHeight = (thisBox.top - thisBox.bottom);

        if (thisWidth > neededWidth)
            neededWidth = thisWidth;

        neededHeight += thisHeight;
    }

    //We have to adjust the location of each text object up so it fits on the page. Otherwise it'd be added to the bottom-left corner, and be invisible.
    for (int i = 0; i < numTextAnnots; ++i)
    {
        PDETextItem textItem = PDETextGetItem(annotationsText, i);

        //The new location will be the old location, adjusted up.
        ASFixedMatrix newLoc;
        PDETextItemGetTextMatrix(textItem, 0, &newLoc);
        newLoc.v += neededHeight - fontSize;

        PDETextItemSetTextMatrix(textItem, &newLoc);
    }

    //Create our output document.
    APDFLDoc extractDoc;
    extractDoc.insertPage(neededWidth,neededHeight, PDBeforeFirstPage);
    PDPage extractPage = extractDoc.getPage(0);

    //Put the texts onto the output document.
    PDEContentAddElem(PDPageAcquirePDEContent(extractPage, 0), 0, (PDEElement)annotationsText);

    std::wcout << L"I extracted " << numTextAnnots << L" text annotations." << std::endl;
    std::wcout << L"Saving the extracted text document." << std::endl;

    PDPageSetPDEContentCanRaise(extractPage, 0);
    PDPageReleasePDEContent(extractPage, 0);
    PDPageRelease(extractPage);
    extractDoc.saveDoc(L"AnnotTexts.pdf",PDSaveFull);

    HANDLER

        errCode = ERRORCODE;
        libInit.displayError(errCode);                                       //If there was an error, display it.

    END_HANDLER

    if (!errCode)
        std::wcout << L"Success!" << std::endl;

    return errCode;                                                          //APDFLib's destructor terminates the library.
}
//===================================================================================================================================================================================
//void function: Searches through a PDEContent object, and the PDEContents of its PDEContainer, PDEgroup, and PDEForm objects, adding all PDEElements to a list.
//===================================================================================================================================================================================
void extractPDEElements(PDEContent* c, std::vector<PDEElement>* list)
{
    //These represent nonatomic PDEObjects inside the input PDEContent, *c.
    //These must be initialized outside of the switch statement.
    PDEContainer deepContainer; 
    PDEGroup deepGroup;
    PDEForm deepForm;
    PDEContent deepContent;
    
    for (int i = 0; i < PDEContentGetNumElems(*c); ++i)
    {
        PDEElement next = PDEContentGetElem(*c, i);
        list->push_back(next);

        switch (PDEObjectGetType((PDEObject)next))
        {
        case kPDEContainer:
            deepContainer = (PDEContainer)next;
            deepContent = PDEContainerGetContent(deepContainer);
            extractPDEElements(&deepContent, list);
            break;
        case kPDEForm:
            deepForm = (PDEForm)next;
            deepContent = PDEFormGetContent(deepForm);
            extractPDEElements(&deepContent, list);
            break;
        case kPDEGroup:
            deepGroup = (PDEGroup)next;
            deepContent = PDEGroupGetContent(deepGroup);
            extractPDEElements(&deepContent, list);
            break;
        }
    }
}

//===================================================================================================================================================================================
// void function: Sets the annotation's quads.
//
// Note: Adobe specifies quads be added in this order - Bottom Left, Bottom Right, Top Right, Top left. They currently need to be added in as BL, BR, TL, TR to get correct output. 
//===================================================================================================================================================================================
void PDAnnotSetQuads(PDAnnot annot, ASFixedQuad *quads, ASArraySize numQuads)
{
    CosObj coAnnot = PDAnnotGetCosObj(annot);                                   //Acquire the annotation's cos object.
    CosDoc coDoc = CosObjGetDoc(coAnnot);                                       //Get the CosDoc containing the annotation.
    CosObj coQuads = CosNewArray(coDoc, false, numQuads * 8);                   //Create a cos array to hold the quadpoints.

    for (ASUns32 i = 0, n = 0; i < numQuads; ++i)
    {
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].bl.h));    //Add the quad points to the cos array, this will grow and shrink as needed.
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].bl.v));
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].br.h));
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].br.v));
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].tl.h));    //These two points currently do not conform to the PDF specification.
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].tl.v));
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].tr.h));    //These two points currently do not conform to the PDF specification.
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].tr.v));
    }

    CosDictPut(coAnnot, ASAtomFromString("QuadPoints"), coQuads);
}