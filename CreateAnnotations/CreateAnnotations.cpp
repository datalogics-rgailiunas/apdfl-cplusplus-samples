// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//=====================================================================================
// Sample: Annotations - Demonstrates adding annotations and extracting their content.
//
// Note:
// This sample annotates all the PDEElements of the input document (it highlights the
// text and adds a text annotation to everything else), and then extracts the text of
// the annotations into a new PDF document.
//
// Steps:
// 1) Create an annotation for each PDEElement on the page.
// 2) Save the document and close it.
// 3) Extract the annotations' text contents into a new document.
//======================================================================================

#include <vector>
#include <sstream>

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#include "PSFCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"
#include "DLExtrasCalls.h"
#include "CosCalls.h"
#include "ASExtraVers.h"

//void function: Searches through the contents of a PDEContent object, and the contents of its PDEContainer, PDEgroup, and PDEForm objects, adding all PDEElements to a list.
void extractPDEElements(PDEContent* c, std::vector<PDEElement>* list);

//void function: Sets an annotation's quads.
void PDAnnotSetQuads(PDAnnot annot, ASFixedQuad *quads, ASArraySize numQuads);

int main(int argc, char** argv)
{
    APDFLib libInit;                                                  //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;                                          //Variable used to report any exceptions or errors, if they occured.

    if (libInit.isValid() == false)                                   //If there was a problem in initialization, return the error code.
        return libInit.getInitError();

    DURING

    APDFLDoc doc(L"../_Input/CreateAnnotations.pdf", true);           //Open the input document, repairing it if necessary.

    PDPage inPage = doc.getPage(0);
    PDEContent inPageContent = PDPageAcquirePDEContent(inPage, 0);

//===================================================================================================================================================================================
// 1) Create an annotation for each PDEElement on the page.
//
// Note: An annotation with content describing each PDEElement will be placed in the same location as that PDEElement.
// For text elements, we will create a red, blue, or green highlight annotation. For other elements, we will create a
// text annotation.
//===================================================================================================================================================================================

    //Retrieve all the PDEElements on the page.
    std::vector<PDEElement> pageElements;
    extractPDEElements(&inPageContent,&pageElements);

    std::wcout << L"This page has " << pageElements.size() << L" PDEElements." <<std::endl;
    std::wcout << L"Creating an annotation for each." <<std::endl;

    int highlightColorNum = 0;                                                                                 //This will cycle between 0, 1, and 2 to cycle highlight colors.
    for (PDEElement next : pageElements)
    {
        //The annotation's location. We'll place the annotation where the original page element was.
        ASFixedRect elementLoc;
        PDEElementGetBBox(next, &elementLoc);

        //The annotation's type. We'll highlight text, and make a text annotation for everything else.
        char* annotType;
        bool isHighlight = (PDEObjectGetType((PDEObject)next) == kPDEText);
        if (isHighlight)
            annotType = "Highlight";
        else
            annotType = "Text";

        //Create the new annotation. Now we just have to set its appearance and content.
        PDAnnot annot = PDPageAddNewAnnot(inPage, kPDEAfterLast, ASAtomFromString(annotType), &elementLoc);

        //Will be filld with the annotation's text content.
        std::wstringstream annotContent;

        annotContent << L"This is a ";

        //These must be initialized outside of the switch statement.
        int numContained = 0;                                                                                  //If the element is a container, form, or group, we will check how many elements it has.
        PDEContainer container;
        PDEForm form;
        PDEGroup group;
        switch (PDEObjectGetType((PDEObject)next))
        {
        case kPDEContainer:
            annotContent << L"container";
            container = (PDEContainer)next;
            numContained = PDEContentGetNumElems(PDEContainerGetContent(container));                           //PDEContentGetNumElems will not count how many elements contained containers have.
            break;
        case kPDEForm:
            annotContent << L"form";
            form = (PDEForm)next;
            numContained = PDEContentGetNumElems(PDEFormGetContent(form));
            break;
        case kPDEGroup:
            group = (PDEGroup)next;
            numContained = PDEContentGetNumElems(PDEGroupGetContent(group));
            annotContent << L"group";
            break;
        case kPDEImage:
            annotContent << L"image";
            break;
        case kPDEPath:
            annotContent << L"path";
            break;
        case kPDEPlace:
            annotContent << L"place";
            break;
        case kPDEText:
            annotContent << L"text object";
            break;
        case kPDEXObject:
            annotContent << L"XObject";
            break;
        }

        if (numContained > 0)
            annotContent << L", containing " << numContained << L" elements ";

        annotContent << L". It is situated at:\n";
        annotContent << L"top: "    << ASFixedToFloat(elementLoc.top)    << L"\n";
        annotContent << L"bottom: " << ASFixedToFloat(elementLoc.bottom) << L"\n";
        annotContent << L"left: "   << ASFixedToFloat(elementLoc.left)   << L"\n";
        annotContent << L"right: "  << ASFixedToFloat(elementLoc.right);

        //We're done preparing the content string. Now make its ASText to add it to the annotation.
        std::wstring annotContentStr;
        annotContentStr = annotContent.str();
        ASText annotContentAST = ASTextFromUnicode((ASUTF16Val*)annotContentStr.c_str(), kUTF16HostEndian);

        //The annotation must be cast to a TextAnnot to set its text content.
        PDTextAnnot textAnnot = CastToPDTextAnnot(annot);
        PDTextAnnotSetContentsASText(textAnnot, annotContentAST);
        ASTextDestroy(annotContentAST);

        //The annotation's title.
        const char* annotTitleStr = "Page Element";
        PDAnnotSetTitle(annot, annotTitleStr, strlen(annotTitleStr));

        //Set the annot's quads. This will properly position a highlight annotation, and have no effect on the text annotation.
        ASFixedQuad annotLocQuad;
        //                  horizontal        vertical
        annotLocQuad.bl = { elementLoc.left,  elementLoc.bottom };
        annotLocQuad.br = { elementLoc.right, elementLoc.bottom };
        annotLocQuad.tl = { elementLoc.left,  elementLoc.top };
        annotLocQuad.tr = { elementLoc.right, elementLoc.top };
        PDAnnotSetQuads(annot, &annotLocQuad, 1);

        //The annotation will not be able to be edited.
        PDAnnotSetFlags(annot, PDAnnotGetFlags(annot) | pdAnnotLock | pdAnnotLockContents);

        //Set the annotation's color.
        PDColorValueRec colorRec; PDColorValue color;
        color = &colorRec;
        color->space = PDDeviceRGB;                                                                            //Only RGB is acceptable for annotations.
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
            //Text annotations will just be yellow.
            color->value[0] = fixedOne;
            color->value[1] = fixedOne;
            color->value[2] = fixedZero;
        }
        PDAnnotSetColor(annot, color);
    }

//===================================================================================================================================================================================
// 2) Save the document and close it.
//===================================================================================================================================================================================

    std::wcout << L"Saving the annotated document..." << std::endl;

    PDPageRelease(inPage);
    doc.saveDoc(L"Annotated.pdf");                                                                 //Save the document. APDFLDoc defaults to using the "PDSaveFull" flag while saving.

    doc.~APDFLDoc();                                                                              //APDFLDoc's destructor takes care of closing the document and releasing the rest of the resources.

//===================================================================================================================================================================================
// 3) Extract the annotations' text contents into a new document.
//===================================================================================================================================================================================

    std::wcout << L"Extracting the annotations to a new document." << std::endl;

    APDFLDoc annotDoc(L"Annotated.pdf", true);                                                     //Re-open the output document of steps 1-2.
    PDPage annotPage = annotDoc.getPage(0);

    //Prepare the text object which will hold all the extracted text.
    PDEText annotationsText = PDETextCreate();                                                     //This will hold all the extracted text.
    ASFixedMatrix textLoc;                                                                         //We'll use this to position the texts.
    memset(&textLoc, 0, sizeof(textLoc));                                                          //Ensure there's no garbage in the matrix.
    ASFixed fontSize = FloatToASFixed(12.0f);                                                      //12-point font.
    textLoc.a = textLoc.d = fontSize;                                                              //Character width and height, respectively.
    textLoc.v = textLoc.h = 0;                                                                     //The vertical and horizontal position of the text, respectively. We will set v as we place texts.

    //Load the font we'll use to write the text content.
    PDEFontAttrs fontAttrs;                                                                        //Contains data to retrieve the font.
    memset(&fontAttrs, 0, sizeof(fontAttrs));                                                      //Ensure there's no garbage in the struct.
    fontAttrs.name = ASAtomFromString("CourierStd");                                               //The font name.
    fontAttrs.type = ASAtomFromString("Type1");                                                    //The font type.
    PDSysFont sysFont = PDFindSysFont(&fontAttrs, sizeof(fontAttrs), 0);                           //Locate the system font that corresponds to the font attributes.
    PDEFont font = PDEFontCreateFromSysFont(sysFont, kPDEFontDoNotEmbed);                          //Create the pdeFont with the sysFont. We won't embed the font.

    //A default graphics state with which to draw the text.
    PDEGraphicState graphics;
    PDEDefaultGState(&graphics, sizeof(PDEGraphicState));

    int maxDigits = (int)(log10((double)PDPageGetNumAnnots(annotPage)) + 1);                       //The maximum number of digits of n that the nth text annotation can have. Used to pad the text string.
    int numAnnots = PDPageGetNumAnnots(annotPage);                                                 //The number of annotations on the page.
    int numTextAnnots = 0;                                                                         //The number of annotations actually containing textual content. Updated as we go along.
    int numBlankAnnots = 0;                                                                        //We'll track the number of annotations that don't have text content.

    std::wcout << L"The input page has " << numAnnots << L" annotations." << std::endl;

    //Extract each annotation's text content (if any) into our text object.
    ASFixed neededWidth  = fixedZero;                                                              //The required width of a page that can hold all the text objects.
    ASFixed neededHeight = fixedZero;                                                              //The required height.
    for (int i = 0; i < numAnnots; ++i)
    {
        PDAnnot next = PDPageGetAnnot(annotPage, i);

        //A buffer to hold the annotation's text content.
        const size_t buffersize = 1000;
        char contentBuffer[buffersize];

        //Does the annotation actually have text conent?
        PDTextAnnot nextAsText = CastToPDTextAnnot(next);
        PDTextAnnotGetContents(nextAsText, contentBuffer, buffersize);                             //Put the content of the text annotation into buffer.
        if (contentBuffer[0] != '\0')
        {
            numTextAnnots++;

            //Prepare the string to output.
            std::wstringstream extractedString;

            extractedString << L"Annotation no. " << numTextAnnots;

            //Pad out the spaces so that the extracted strings all line up.
            int numDigits = (int)(log10((double)numTextAnnots) + 1);                               //The number of digits of i that this annotation, the ith one, has.
            for (int i = 0; i < (maxDigits - numDigits); ++i)
                extractedString << L" ";

            extractedString << L" '" << contentBuffer << L"'";                                     //The content is placed in the output string here.

            //Add the text!
            ASText extractedAST = ASTextFromUnicode((ASUTF16Val*)extractedString.str().c_str(), kUTF16HostEndian);
            PDETextAddASText(annotationsText, kPDETextRun, numTextAnnots - 1, extractedAST, font, &graphics, sizeof(PDEGraphicState), NULL, 0, &textLoc);
            ASTextDestroy(extractedAST);

            //Adjust our page dimension requirements.
            ASFixedRect thisBox;
            PDETextGetBBox(annotationsText, kPDETextRun, numTextAnnots-1, &thisBox);
            ASFixed thisWidth  = (thisBox.right - thisBox.left);                                       //The width of this text object.
            ASFixed thisHeight = (thisBox.top - thisBox.bottom);                                       //The height of this text object.

            if (thisWidth > neededWidth)
                neededWidth = thisWidth;

            neededHeight += thisHeight + fontSize;

            //Position the next text.
            textLoc.v -= fontSize*2;
        }
        else
        {
            ++numBlankAnnots;
        }
    }

    PDERelease((PDEObject)font);
    PDERelease((PDEObject)graphics.fillColorSpec.space);

    std::wcout << numBlankAnnots << L" annotations on the page did not have text content." << std::endl;

    //We have to adjust the location of each text object up so it fits on the page. Otherwise it'd be added to the bottom-left corner, and be invisible.
    for (int i = 0; i < numTextAnnots; ++i)
    {
        PDETextItem textItem = PDETextGetItem(annotationsText, i);

        //The new location will be the old location, adjusted up, and accounting for the font size.
        ASFixedMatrix newLoc;
        PDETextItemGetTextMatrix(textItem, 0, &newLoc);
        newLoc.v += neededHeight - fontSize*2;

        PDETextItemSetTextMatrix(textItem, &newLoc);
    }

    //Create our output document.
    APDFLDoc extractDoc;
    extractDoc.insertPage(neededWidth,neededHeight, PDBeforeFirstPage);
    PDPage extractPage = extractDoc.getPage(0);

    //Put the texts onto the output document.
    PDEContentAddElem(PDPageAcquirePDEContent(extractPage, 0), 0, (PDEElement)annotationsText);
    PDERelease((PDEObject)annotationsText);

    std::wcout << L"I extracted " << numTextAnnots << L" text-containing annotations." << std::endl;
    std::wcout << L"Saving the extracted text document." << std::endl;

    PDPageSetPDEContentCanRaise(extractPage, 0);
    PDPageReleasePDEContent(extractPage, 0);
    PDPageRelease(extractPage);
    extractDoc.saveDoc(L"AnnotationTexts.pdf",PDSaveFull);

    HANDLER

        errCode = ERRORCODE;
        libInit.displayError(errCode);                                                             //If there was an error, display it.

    END_HANDLER

    if (!errCode)
        std::wcout << L"Success!" << std::endl;

    return errCode;                                                                                //APDFLib's destructor terminates the library.
}

//===================================================================================================================================================================================
//void function: Searches through a PDEContent object, and the PDEContents of its PDEContainer, PDEgroup, and PDEForm objects, adding all PDEElements to a list.
//===================================================================================================================================================================================
void extractPDEElements(PDEContent* c, std::vector<PDEElement>* elements)
{
    //These will represent nonatomic elements inside the input PDEContent.
    //They must be initialized outside the switch statement.
    PDEContainer deepContainer; 
    PDEGroup deepGroup;
    PDEForm deepForm;
    PDEContent deepContent;
    
    for (int i = 0; i < PDEContentGetNumElems(*c); ++i)
    {
        PDEElement next = PDEContentGetElem(*c, i);
        elements->push_back(next);

        switch (PDEObjectGetType((PDEObject)next))
        {
        case kPDEContainer:
            deepContainer = (PDEContainer)next;
            deepContent = PDEContainerGetContent(deepContainer);
            extractPDEElements(&deepContent, elements);
            break;
        case kPDEForm:
            deepForm = (PDEForm)next;
            deepContent = PDEFormGetContent(deepForm);
            extractPDEElements(&deepContent, elements);
            break;
        case kPDEGroup:
            deepGroup = (PDEGroup)next;
            deepContent = PDEGroupGetContent(deepGroup);
            extractPDEElements(&deepContent, elements);
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
