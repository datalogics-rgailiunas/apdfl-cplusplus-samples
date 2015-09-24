// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//=============================================================================
// Sample: TextSearch - This sample demonstrates searching a document for text
// using the PDWordFinder. When text is found it marks it with a highlight 
// annotation and saves the output document in the working directory.
//
//Steps:
// 1) Set the word finder configurations.
// 2) Fill in color information for highlighting text.
// 3) Check pages for words matching the search string.
// 4) Add a highlight annotation when the word is found.
//=============================================================================

#include <iostream>
#include <algorithm>
#include <vector>
#include "CosCalls.h"
#include "InitializeLibrary.h"
#include "APDFLDoc.h"

void PDAnnotSetQuads(PDAnnot, ASFixedQuad *, ASArraySize);

int main(int argc, char** argv)
{
    APDFLib libInit(argv[1]);                      //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;              //Variable used to report any exceptions/errors if they occured. 

    if (libInit.isValid() == false)       //Check for errors upon initialization.
        return libInit.getInitError();    //If it failed, return the error code.
    
    DURING

        APDFLDoc document(L"../_Input/TextSearch.pdf", true);

//===================================================================================================================================================================================
// Step 1) Set the word finder configurations. In this case memset() will take care of initializing most of the variables, but they are all display here with a short
// description in order to show what types of settings are available. Ensure that wfConfig.recSize is ALWAYS set.
//===================================================================================================================================================================================

        PDWordFinderConfigRec wfConfig;                      //This structure determines how the PDWordFinder will behave.
        memset(&wfConfig, 0, sizeof(wfConfig));

        wfConfig.recSize = sizeof(PDWordFinderConfigRec);    //Always set to sizeof PDWordFinderConfigRec.
        wfConfig.disableTaggedPDF = true;                    //Setting to true will treat this as a non-tagged PDF document.
        wfConfig.noXYSort = true;                            //Don't generate an XY-ordered word list.
        wfConfig.preserveSpaces = false;                     //Don't preserve spaces during word breaking.
        wfConfig.noLigatureExp = false;                      //Enable expansion of ligatures using the default ligatures.
        wfConfig.noEncodingGuess = true;                     //Disable guessing encoding of fonts with unknown or custom encoding.
        wfConfig.unknownToStdEnc = false;                    //Don't assume all fonts are Standard Roman. Setting to true overrides noEncodingGuess.
        wfConfig.ignoreCharGaps = true;                      //Disable converting large character gaps to spaces.
        wfConfig.ignoreLineGaps = false;                     //Treat vertical movements as line breaks.
        wfConfig.noAnnots = true;                            //Don't extract from annotations.
        wfConfig.noHyphenDetection = false;                  //Don't differentiate between hard and soft hyphens.
        wfConfig.trustNBSpace = false;                       //Don't differentiate between breaking and non-breaking spaces.
        wfConfig.noExtCharOffset = false;                    //If client doesn't have a need for detailed character offset information set to true for improvement in efficiency.
        wfConfig.noStyleInfo = false;                        //Set to true if client doesn't have a need for style information for improvement in efficiency.
        wfConfig.decomposeTbl = nullptr;                     //Table may be used to expand unicode ligatures not in the default list.
        wfConfig.decomposeTblSize = 0;                       //Not using decomposeTbl, so the size is 0.
        wfConfig.charTypeTbl = nullptr;                      //Custom table to enhance word breaking quality.
        wfConfig.charTypeTblSize = 0;                        //Unused, so the size will be 0.
        wfConfig.preserveRedundantChars = false;             //May be used to preserve overlapping redundant characters in some PDF documents.
        wfConfig.disableCharReordering = false;              //Used in cases where the PDF page has heavily overlapped character bounding boxes.

//===================================================================================================================================================================================
// Step 2) Fill in color information for highlighting text. In this case our color will be set to orange.
//===================================================================================================================================================================================

        ASFixed red = ASFloatToFixed(1.0);       //Set the colors to be used for highlighting.
        ASFixed green = ASFloatToFixed(0.65);    //These values may range from 0.0 to 1.0.
        ASFixed blue = ASFloatToFixed(0.0);         

        PDColorValueRec colorValRec;
        PDColorValue pdColorValue;               //This variable is passed to PDAnnotSetColor, in order to set the annotations color.
        pdColorValue = &colorValRec;             //PDColorValueRec must be assigned to PDColorValue before attempting to access the values.

        pdColorValue->value[0] = red;            //Assign the RGB color values to the PDColorValue structure.
        pdColorValue->value[1] = green;
        pdColorValue->value[2] = blue;
        pdColorValue->space = PDDeviceRGB;       //Colors are set using the RGB color space.

//===================================================================================================================================================================================
// Step 3) Check pages for words matching the search string. In this case it is done by acquiring an ASText object from the PDWord being examined and then converting it to a
// std::wstring for making the comparisons.
// Note: This will match exact values and any strings containing the text. For example searching for the word "as" will match the entire word "Thomas." 
//===================================================================================================================================================================================

        //Create the PDWordFinder object used to extract and enumerate the words on pages in a PDF document.
        PDWordFinder wordFinder = PDDocCreateWordFinderEx(document.getPDDoc(), WF_LATEST_VERSION, true, &wfConfig);    

        PDWord pdfWordArray;          //This will point at an array of PDWord objects. Do not try to access this directly, acquire the list through PDWordFinderGetNthWord().
        ASInt32 numberOfWords = 0;    //Number of words on the page.

        for (ASInt32 pageNum = 0; pageNum < (PDDocGetNumPages(document.getPDDoc())); ++pageNum)                               //Iterate through each page in the PDDoc.
        {
            PDWordFinderAcquireWordList(wordFinder, pageNum, &pdfWordArray, nullptr, nullptr, &numberOfWords);                //Get all words in the PDPage specified.

            PDPage pdPage = document.getPage(pageNum);                                                                        //Get the PDPage object for adding the highlight annotation.

            for (ASInt32 index = 0; index < numberOfWords; ++index)                                                           //Iterate through the words in the wordlist.
            {
                PDWord pdWord = PDWordFinderGetNthWord(wordFinder, index);                                                    //Acquire the PDWord from the word finder.

                ASText asTextWord = ASTextNew();                                                                              //Create a new empty ASText object.
                PDWordGetASText(pdWord, 0, asTextWord);                                                                       //Get the ASText object from the PDWord.
                                                                               
                std::wstring testString = reinterpret_cast<wchar_t *>(ASTextGetUnicodeCopy(asTextWord, kUTF16HostEndian));    //Set string equal to the word being examined.

                std::transform(testString.begin(), testString.end(), testString.begin(), ::tolower);                          //Convert the test string to all lowercase letters.

//===================================================================================================================================================================================
// Step 4) Add a highlight annotation when the word is found. The Annotation's Subtype, QuadPoints and rectangle must be set in order to render the highlight annotation to the page.
// tempQuad contains the (x,y) coordinates of the annotation where each point represents one of the corners of the quadrilateral. 
// Note:bl stands for bottom left and tr for top right
//===================================================================================================================================================================================

                if (wcsstr(testString.c_str(), L"pirate") != nullptr)                                                     //Check for any occurences of the string "pirate".
                {
                    ASFixedQuad tempQuad;
                    PDWordGetNthQuad(pdWord, 0, &tempQuad);                                                               //Obtain the PDWords quad. 

                    ASFixedRect annotationRect;                                                                           //The rectangle that must be set to render the annotation.
                    annotationRect.left = tempQuad.bl.h;                                                                  //Assigning these works because both shapes are rectangles.
                    annotationRect.top = tempQuad.tr.v;                                                                   
                    annotationRect.right = tempQuad.tr.h;                                                                 
                    annotationRect.bottom = tempQuad.bl.v;                                          
 
                    PDAnnot highlight = PDPageCreateAnnot(pdPage, ASAtomFromString("Highlight"), &annotationRect);        //Create the annotation.

                    PDAnnotSetQuads(highlight, &tempQuad, 1);                                                             //Set the newly created annotation's coordinates.
                    PDAnnotSetColor(highlight, pdColorValue);                                                             //Set the annotation's color (orange.)

                    PDPageAddAnnot(pdPage, -2, highlight);                                                                //Render the annotation to the page.
                }

                ASTextDestroy(asTextWord);                                                                                //Destroy the ASText object before creating a new one.
            }

            PDPageRelease(pdPage);                                                                                        //Release the acquired page.

            PDWordFinderReleaseWordList(wordFinder, pageNum);                                                             //Release the word list before acquiring the next one.
        }

        PDWordFinderDestroy(wordFinder);                                                                                  //Destroy the word finder object.

        document.saveDoc(L"out.pdf");                                                                                     //Save the output document in the working directory.

    HANDLER

        errCode = ERRORCODE;

        libInit.displayError(errCode);                                                                                    //If there was an error, display it.

    END_HANDLER

    return errCode;                                                                                                       //APDFLib's destructor terminates the APDFL.                            
}

//===================================================================================================================================================================================
// Function: PDAnnotSetQuads() - Function that needs to be called in order to add quads to the annotation's CosObj.
// Note: Adobe specifies quads be added in this order - Bottom Left, Bottom Right, Top Right, Top left. They currently need to be added in as BL, BR, TL, TR to get correct output. 
//===================================================================================================================================================================================

void PDAnnotSetQuads(PDAnnot annot, ASFixedQuad *quads, ASArraySize numQuads) {

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
