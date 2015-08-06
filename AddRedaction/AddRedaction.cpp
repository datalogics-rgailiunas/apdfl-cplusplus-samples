// Copyright (c) 2015, Datalogics, Inc. All rights reserved.

//====================================================================================
// Sample: AddRedaction - 
//      
// Steps:
//  1) Open the input PDF document.
//  2) 
//  3)   
//====================================================================================

// This agreement is between Datalogics, Inc. 101 N. Wacker Drive, Suite 1800,
// Chicago, IL 60606 ("Datalogics") and you, an end user who downloads
// source code examples for integrating to the Adobe PDF Library
// ("the Example Code"). By accepting this agreement you agree to be bound
// by the following terms of use for the Example Code.
//
// LICENSE
// -------
// Datalogics hereby grants you a royalty-free, non-exclusive license to
// download and use the Example Code for any lawful purpose. There is no charge
// for use of Example Code.
//
// OWNERSHIP
// ---------
// The Example Code and any related documentation and trademarks are and shall
// remain the sole and exclusive property of Datalogics and are protected by
// the laws of copyright in the U.S. and other countries.
//
// Datalogics is a trademark of Datalogics, Inc.
//
// TERM
// ----
// This license is effective until terminated. You may terminate it at any
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
// NON-INFRINGEMENT, ACCURACY OR COMPLETENESS OF RESPONSES, RESULTS, AND/OR
// LACK OF WORKMANLIKE EFFORT. THE PROVISIONS OF THIS SECTION SET FORTH
// SUBLICENSEE'S SOLE REMEDY AND DATALOGICS'S SOLE LIABILITY WITH RESPECT
// TO THE WARRANTY SET FORTH HEREIN. NO REPRESENTATION OR OTHER AFFIRMATION
// OF FACT, INCLUDING STATEMENTS REGARDING PERFORMANCE OF THE EXAMPLE CODE,
// WHICH IS NOT CONTAINED IN THIS AGREEMENT, SHALL BE BINDING ON DATALOGICS.
// NEITHER DATALOGICS WARRANT AGAINST ANY BUG, ERROR, OMISSION, DEFECT,
// DEFICIENCY, OR NONCONFORMITY IN ANY EXAMPLE CODE.

#include <iostream>
#include <string>
#include <algorithm>
#include "CosCalls.h"
#include "APDFLDoc.h"
#include "InitializeLibrary.h"

int main(int argc, char** argv)
{
    APDFLib libInit;                   //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;           //Variable used to report any exceptions/errors if they occured.

    if (libInit.isValid() == false)    //If there was a problem in initialization, return the error code.
        return libInit.getInitError();

    DURING

//=============================================================================================================================================================
// Step 1) Open the input PDF document.
//=============================================================================================================================================================

        std::wstring pathName = L"../_Input/AddRedaction.pdf";    
        PDDoc pdDoc;                                              
        
        //Determine the unicode format for opening the input file.
        ASUnicodeFormat unicodeFormat;
        if (sizeof(wchar_t) == 2)
            unicodeFormat = kUTF16HostEndian;
        else
            unicodeFormat = kUTF32HostEndian;

        ASText asText = ASTextFromUnicode(reinterpret_cast<const ASUTF16Val*>(pathName.c_str()), unicodeFormat);    //ASText object will be used to create an ASPathName object.

        ASPathName asPathName = ASFileSysCreatePathFromDIPathText(nullptr, asText, nullptr);                        //ASPathName will be used to open the PDF document.



        pdDoc = PDDocOpen(asPathName, nullptr, nullptr, true);                                                      //Open the input document.

        ASTextDestroy(asText);                                                                                      //Release resources that are no longer in use.
        ASFileSysReleasePath(NULL, asPathName);

//=============================================================================================================================================================
// Step 2) Use the PDWordFinder to locate words that will be redacted. We will save their location in a vector of ASFixedQuads. 
//=============================================================================================================================================================
        std::vector<ASFixedQuad> quadVector;

        PDWordFinderConfigRec wfConfig;                                                                               //Set the default word finder settings.
        memset(&wfConfig, 0, sizeof(PDWordFinderConfigRec));

        PDWordFinder wordFinder = PDDocCreateWordFinderEx(pdDoc, WF_LATEST_VERSION, true, &wfConfig);                 //Create the word finder object.    
        PDWord pdWordArray;                                                                                           //Create a PDWord object used to hold individual words.

        ASInt32 numberOfWords = 0;

        PDWordFinderAcquireWordList(wordFinder, 0, &pdWordArray, nullptr, nullptr, &numberOfWords);                   //Acquire the word list from the page.
        std::cout << numberOfWords;

        for (ASInt32 index = 0; index < numberOfWords; ++index)                                                       //Iterate through the word list.
        {
            PDWord pdWord = PDWordFinderGetNthWord(wordFinder, index);                                                //Get the PDWord at the given index.

            ASText asTextWord = ASTextNew();                                                                          //Convert word to an ASText object.

            PDWordGetASText(pdWord, 0, asTextWord);

            std::wstring testString = reinterpret_cast<wchar_t*>(ASTextGetUnicodeCopy(asTextWord, unicodeFormat));    //Convert ASText object to a wstring.

            std::transform(testString.begin(), testString.end(), testString.begin(), ::tolower);

            if (wcsstr(testString.c_str(), L"navigation") != nullptr)                                                 
            {
                ASFixedQuad quad;
                PDWordGetNthQuad(pdWord, 0, &quad);
                quadVector.push_back(quad);
            }

            ASTextDestroy(asTextWord);                                                                                //Destroy the ASText object.
        }

        PDWordFinderReleaseWordList(wordFinder, 0);                                                                   //Release the word list.

//=============================================================================================================================================================
// Step 2) Create and apply the redactions.
//=============================================================================================================================================================
        
        //Set color for redactions to black.
        PDColorValue redactColor;
        PDColorValueRec redactColorRec;
        redactColor = &redactColorRec;

        redactColor->space = PDDeviceRGB;
        redactColor->value[0] = FloatToASFixed(1.0);
        redactColor->value[1] = FloatToASFixed(0.0);
        redactColor->value[2] = FloatToASFixed(0.0);

        PDRedactParams redactParams;
        PDRedactParamsRec rpRec;
        redactParams = &rpRec;

        redactParams->size = sizeof(PDRedactParamsRec);                     //Size is always set to the size of the PDRedactParamsRec struct.
        redactParams->pageNum = 0;                                          //The page number that the redaction will be applied to.
        redactParams->redactQuads = quadVector.data();                      //The vector or array holding the quads.
        redactParams->numQuads = quadVector.size();                         //The number of entries in the vector or array.
        redactParams->colorVal = redactColor;                               //The color of the redaction that will be applied.
        redactParams->horizAlign = kPDHorizLeft;                            //Horizontal alignment of the text when generating the redaction mark.
        redactParams->overlayText = nullptr;                                //Overlay text may be used to replace the underlying content.

        PDAnnot redactAnnot = PDDocCreateRedaction(pdDoc, redactParams);    //Create the redaction annotation.

        //PDApplyRedactionParams applyParams;
        //PDApplyRedactionParamsRec arpRec;
        //applyParams = &arpRec;

        //applyParams->size = sizeof(PDApplyRedactionParams);
        //applyParams->redactionAnnots = &redactAnnot;
        //applyParams->keepMarks = true;
        //applyParams->statusProcs = nullptr;

        PDDocApplyRedactions(pdDoc, nullptr);

//=============================================================================================================================================================
// Step 2) Use the PDWordFinder to locate words that will be redacted.
//=============================================================================================================================================================

        pathName = L"outMarked.pdf";

        asText = ASTextFromUnicode(reinterpret_cast<const ASUTF16Val*>(pathName.c_str()), unicodeFormat);

        asPathName = ASFileSysCreatePathFromDIPathText(nullptr, asText, nullptr);

        PDDocSave(pdDoc, PDSaveFull, asPathName, nullptr, nullptr, nullptr);

        //Release any remaining resources.
        ASTextDestroy(asText);
        ASFileSysReleasePath(nullptr, asPathName);
        PDDocClose(pdDoc);

    HANDLER

        errCode = ERRORCODE;

        libInit.displayError(errCode);                                //If there was an error, display the error that occured.

    END_HANDLER

    system("pause");
    return errCode;                                                   //APDFLib's destructor terminates the library.
}
