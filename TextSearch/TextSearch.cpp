// Copyright(c) 2015, Datalogics, Inc.All rights reserved.

//=============================================================================
// Sample: TextSearch - PUT A DESCRIPTION HERE <------------------------------------------------------------------DONT FORGET ME
//
//Steps:
// 1) 
// 2) 
// 3) 
//=============================================================================

// This agreement is between Datalogics, Inc. 101 N.Wacker Drive, Suite 1800,
// Chicago, IL 60606 ("Datalogics") and you, an end user who downloads
// source code examples for integrating to the Adobe PDF Library
// ("the Example Code"). By accepting this agreement you agree to be bound
// by the following terms of use for the Example Code.
//
// LICENSE
// -------
// Datalogics hereby grants you a royalty - free, non - exclusive license to
// download and use the Example Code for any lawful purpose. There is no charge
// for use of Example Code.
//
// OWNERSHIP
// ---------
// The Example Code and any related documentation and trademarks are and shall
// remain the sole and exclusive property of Datalogics and are protected by
// the laws of copyright in the U.S.and other countries.
//
// Datalogics is a trademark of Datalogics, Inc.
//
// TERM
// ----
// This license is effective until terminated.You may terminate it at any
// other time by destroying the Example Code.
//
// WARRANTY DISCLAIMER
// -------------------
// THE EXAMPLE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
// EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// DATALOGICS DISCLAIM ALL OTHER WARRANTIES, CONDITIONS, UNDERTAKINGS OR
// TERMS OF ANY KIND, EXPRESS OR IMPLIED, WRITTEN OR ORAL, BY OPERATION OF
// LAW, ARISING BY STATUTE, COURSE OF DEALING, USAGE OF TRADE OR OTHERWISE,
// INCLUDING, WARRANTIES OR CONDITIONS OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, SATISFACTORY QUALITY, LACK OF VIRUSES, TITLE,
// NON - INFRINGEMENT, ACCURACY OR COMPLETENESS OF RESPONSES, RESULTS, AND / OR
// LACK OF WORKMANLIKE EFFORT.THE PROVISIONS OF THIS SECTION SET FORTH
// SUBLICENSEE'S SOLE REMEDY AND DATALOGICS'S SOLE LIABILITY WITH RESPECT
// TO THE WARRANTY SET FORTH HEREIN.NO REPRESENTATION OR OTHER AFFIRMATION
// OF FACT, INCLUDING STATEMENTS REGARDING PERFORMANCE OF THE EXAMPLE CODE,
// WHICH IS NOT CONTAINED IN THIS AGREEMENT, SHALL BE BINDING ON DATALOGICS.
// NEITHER DATALOGICS WARRANT AGAINST ANY BUG, ERROR, OMISSION, DEFECT,
// DEFICIENCY, OR NONCONFORMITY IN ANY EXAMPLE CODE.

#include <iostream>
#include <algorithm>
#include <vector>

#include "CosCalls.h"

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

void PDAnnotSetQuads(PDAnnot, ASFixedQuad *, ASArraySize);

int main()
{
    APDFLib libInit;                      //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;              //Variable used to report any exceptions/errors if they occured. 

    if (libInit.isValid() == false)       //Check for errors upon initialization.
        return libInit.getInitError();    //If it failed, return the error code.
    
    DURING

        APDFLDoc document(L"../_Input/TextSearch.pdf", true);

//===================================================================================================================================================================================
// Step 1) Set the word finder configurations.
//===================================================================================================================================================================================

        PDWordFinderConfigRec wfConfig;
        memset(&wfConfig, 0, sizeof(wfConfig));

        wfConfig.recSize = sizeof(PDWordFinderConfigRec);    //Always set to sizeof PDWordFinderConfigRec.
        wfConfig.disableTaggedPDF = true;                    //Setting to true will treat this as a non-tagged PDF document.
        wfConfig.noXYSort = true;                            //Don't generate an XY-ordered word list.
        wfConfig.preserveSpaces = false;                     //Don't preserve spaces during word breaking. [HERE]
        wfConfig.noLigatureExp = false;                      //Enable expansion of ligatures using the default ligatures.
        wfConfig.noEncodingGuess = true;                     //Disable guessing encoding of fonts with unknown or custom encoding.
        wfConfig.unknownToStdEnc = true;                     //Don't assume all fonts are Standard Roman. Setting to true overrides noEncodingGuess. [HERE]
        wfConfig.ignoreCharGaps = false;                     //Disable converting large character gaps to spaces. [HERE]
        wfConfig.ignoreLineGaps = false;                     //Treat vertical movements as line breaks.
        wfConfig.noAnnots = true;                            //Don't extract from annotations.
        wfConfig.noHyphenDetection = false;                  //Don't differentiate between hard and soft hyphens.
        wfConfig.trustNBSpace = true;                        //Don't differentiate between breaking and non-breaking spaces. [HERE]
        wfConfig.noExtCharOffset = false;                    //If client doesn't have a need for detailed character offset information set to true for improvement in efficiency.
        wfConfig.noStyleInfo = false;                        //Set to true if client doesn't have a need for style information for improvement in efficiency.
        wfConfig.decomposeTbl = NULL;                        //Table may be used to expand unicode ligatures not in the default list.
        wfConfig.decomposeTblSize = 0;                       //Not using decomposeTbl so the size is 0.
        wfConfig.charTypeTbl = NULL;                         //Custom table to enhance word breaking quality.
        wfConfig.charTypeTblSize = 0;                        //Unused, so the size will be 0.
        wfConfig.preserveRedundantChars = false;             //May be used to preserve overlapping redundant characters in some PDF documents.
        wfConfig.disableCharReordering = false;              //Used in cases where the PDF page has heavily overlapped character bounding boxes.

//===================================================================================================================================================================================
// Step 2) Fill in color information. In this case our highlighting color will be set to orange.
//===================================================================================================================================================================================

        ASFixed red = ASFloatToFixed(1.0);          //Set the colors to be used for highlighting.
        ASFixed green = ASFloatToFixed(0.65);
        ASFixed blue = ASFloatToFixed(0.0);         

        PDColorValueRec colorValRec;
        PDColorValue pdColorValue;                  //Variable passed to PDAnnotSetColor, in order to set the annotations color.
        pdColorValue = &colorValRec;

        pdColorValue->value[0] = red;               //Assign the RGB color values to the structure.
        pdColorValue->value[1] = green;
        pdColorValue->value[2] = blue;
        pdColorValue->space = PDDeviceRGB;          //Colors are set using the RGB color space.
//===================================================================================================================================================================================
// Step 2) Fill in color information.
//===================================================================================================================================================================================

        //Create the PDWordFinder object used to extract and enumerate the words on pages in a PDF document.
        PDWordFinder wordFinder = PDDocCreateWordFinderEx(document.getPDDoc(), WF_LATEST_VERSION, true, &wfConfig);    

        PDWord pdfWordArray;          //This will point at an array of PDWord objects. Do not try to access this directly, acquire the list through PDWordFinderGetNthWord().
        PDWord * xySortedWordTable;   //Table containing PDWords sorted by their (x, y) coordinates in the document.
        ASInt32 numberOfWords = 0;    //Number of words on the page.

        for (ASInt32 pageNum = 0; pageNum < (PDDocGetNumPages(document.getPDDoc()) - 1); ++pageNum)                       //Iterate through each page in the PDDoc.
        {
            PDWordFinderAcquireWordList(wordFinder, pageNum, &pdfWordArray, &xySortedWordTable, NULL, &numberOfWords);    //Get all words in the PDF document.

            for (ASInt32 index = 0; index < numberOfWords; ++index)                                                       //Iterate through the words on the page.
            {
                PDWord pdWord = PDWordFinderGetNthWord(wordFinder, index);                                                //Acquire the PDWord from the word finder.

                ASText asTextWord = ASTextNew();                                                                          //Create a new empty ASText object.
                PDWordGetASText(pdWord, 0, asTextWord);                                                                   //Get the ASText object from the PDWord.

                std::wstring testString;                                                                                  //String used to match values.
                testString = (wchar_t *)ASTextGetUnicodeCopy(asTextWord, kUTF16HostEndian);                               //Set string equal to the word being examined.
                std::transform(testString.begin(), testString.end(), testString.begin(), ::tolower);                      //Convert the test string to all lowercase letters.

                if (wcsstr(testString.c_str(), L"pirate") != NULL)                                                        //Check for any occurences of the string "pirate".
                {
                    PDPage pdPage = document.getPage(pageNum);
                    ASFixedQuad tempQuad;

                    PDWordGetNthQuad(pdWord, 0, &tempQuad);
                    ASFixedRect annotationRect;
                    annotationRect.left = tempQuad.bl.h;
                    annotationRect.top = tempQuad.tr.v;
                    annotationRect.right = tempQuad.tr.h;
                    annotationRect.bottom = tempQuad.bl.v;

                    PDAnnot highlight = PDPageCreateAnnot(pdPage, ASAtomFromString("Highlight"), &annotationRect); //adding the annotation
                    PDAnnotSetQuads(highlight, &tempQuad, 1);
                    PDAnnotSetColor(highlight, pdColorValue);   
                    PDPageAddAnnot(pdPage, -2, highlight);
                    PDPageRelease(pdPage);
                }
 
                ASTextDestroy(asTextWord);                                                                                //Destroy the ASText object before creating a new one.
            }

            PDWordFinderReleaseWordList(wordFinder, pageNum);                                                             //Release the PDWordFinder object before acquiring the next one.
        }

//===================================================================================================================================================================================
// Step 3)
//===================================================================================================================================================================================

        //PDPageNotifyContentsDidChangeEx(pdPage, true);

        document.saveDoc(L"out.pdf");



    HANDLER

        errCode = ERRORCODE;

        libInit.displayError(errCode);    //If there was an error, display it.

    END_HANDLER
            system("pause");
    return errCode;                       //APDFLib's destructor terminates the APDFL.                            
}

//===================================================================================================================================================================================
// Function: PDAnnotSetQuads() - Function that needs to be called in order to 
//===================================================================================================================================================================================
void PDAnnotSetQuads(PDAnnot annot, ASFixedQuad *quads, ASArraySize numQuads) {

    CosObj coAnnot = PDAnnotGetCosObj(annot);
    CosDoc coDoc = CosObjGetDoc(coAnnot);
    CosObj coQuads = CosNewArray(coDoc, false, numQuads * 8);

    for (ASUns32 i = 0, n = 0; i < numQuads; ++i)
    {
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].bl.h));
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].bl.v));
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].br.h));
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].br.v));
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].tl.h)); 
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].tr.v));
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].tr.h)); 
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].tl.v));
    }

    CosDictPut(coAnnot, ASAtomFromString("QuadPoints"), coQuads);
}
