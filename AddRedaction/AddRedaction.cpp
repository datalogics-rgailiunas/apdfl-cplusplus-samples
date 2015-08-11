// Copyright (c) 2015, Datalogics, Inc. All rights reserved.

//====================================================================================
// Sample: AddRedaction - This sample locates text to be redacted using the
// PDWordFinder and permanently removes it from the document, replacing the word with 
// a black redaction box.
//      
// Steps:
//  1) Use the PDWordFinder to locate words that will be redacted.
//  2) Create and apply the redactions.  
//  3) Verify that the words were permanently removed.
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

        APDFLDoc document(L"../_Input/AddRedaction.pdf", true);

//===================================================================================================================================================================================
// Step 1) Use the PDWordFinder to locate words that will be redacted. We will save their location in a vector of ASFixedQuads. 
//===================================================================================================================================================================================

        std::vector<ASFixedQuad> quadVector;                                                                          //This vector will hold quad points for located words.

        PDWordFinderConfigRec wfConfig;                                                                               //Set the default word finder settings.
        memset(&wfConfig, 0, sizeof(PDWordFinderConfigRec));
        wfConfig.recSize = sizeof(PDWordFinderConfigRec);

        PDWordFinder wordFinder = PDDocCreateWordFinderEx(document.getPDDoc(), WF_LATEST_VERSION, true, &wfConfig);   //Create the word finder object.    
        PDWord pdWordArray;                                                                                           //Create a PDWord object used to hold individual words.

        ASInt32 numberOfWords = 0;

        std::wcout << L"Acquiring word list..." << std::endl;

        PDWordFinderAcquireWordList(wordFinder, 0, &pdWordArray, nullptr, nullptr, &numberOfWords);                   //Acquire the word list from the page.

        //Determine the unicode format before converting the ASText to a wstring.        
        ASUnicodeFormat unicodeFormat;                                                          
        if (sizeof(wchar_t) == 2)                                                                   
            unicodeFormat = kUTF16HostEndian;
        else
            unicodeFormat = kUTF32HostEndian;

        std::wcout << L"Found " <<  numberOfWords << L" in document, searching for matches..." << std::endl;

        for (ASInt32 index = 0; index < numberOfWords; ++index)                                                       //Iterate through the word list.
        {
            PDWord pdWord = PDWordFinderGetNthWord(wordFinder, index);                                                //Get the PDWord at the given index.

            ASText asTextWord = ASTextNew();                                                                          //Convert word to an ASText object.

            PDWordGetASText(pdWord, 0, asTextWord);

            std::wstring testString = reinterpret_cast<wchar_t*>(ASTextGetUnicodeCopy(asTextWord, unicodeFormat));    //Convert ASText object to a wstring.

            std::transform(testString.begin(), testString.end(), testString.begin(), ::tolower);

            //If either string is matched, push the words quad points into a vector.
            if ((wcsstr(testString.c_str(), L"navigation") != nullptr) || (wcsstr(testString.c_str(), L"screen") != nullptr))
            {
                ASFixedQuad quad;
                PDWordGetNthQuad(pdWord, 0, &quad);
                quadVector.push_back(quad);
            }

            ASTextDestroy(asTextWord);                                                                                //Destroy the ASText object.
        }

        PDWordFinderReleaseWordList(wordFinder, 0);                                                                   //Release the word list.

        std::wcout << L"Found " << quadVector.size() << L" matches..." << std::endl;

//===================================================================================================================================================================================
// Step 2) Create and apply the redactions. The redaction configurations are set, the redaction is created and finally applied. If PDDocApplyRedactions is not called the words will
// be marked for redaction, but not removed.
//===================================================================================================================================================================================
        if (quadVector.size() > 0)                                                            //Perform check on size to prevent quadVector.front() from having undefined behavior.
        {
            PDRedactParams redactParams;                                                      //Parameters controlling settings for the redaction annotation.
            PDRedactParamsRec rpRec;
            redactParams = &rpRec;

            PDColorValueRec cvRec;

            redactParams->size = sizeof(PDRedactParamsRec);                                   //Size is always set to the size of the PDRedactParamsRec struct.
            redactParams->pageNum = 0;                                                        //The page number that the redaction will be applied to.
            redactParams->redactQuads = &quadVector.front();                                  //The vector or array holding the quads.
            redactParams->numQuads = quadVector.size();                                       //The number of entries in the vector or array.
            redactParams->colorVal = &cvRec;
            redactParams->colorVal->space = PDDeviceRGB;                                      //Set device color space to RGB
            redactParams->colorVal->value[0] = FloatToASFixed(0.0);                           //The redaction box will be set to black.
            redactParams->colorVal->value[1] = FloatToASFixed(0.0);
            redactParams->colorVal->value[2] = FloatToASFixed(0.0);
            redactParams->horizAlign = kPDHorizLeft;                                          //Horizontal alignment of the text when generating the redaction mark.
            redactParams->overlayText = nullptr;                                              //Overlay text may be used to replace the underlying content.

            PDAnnot redactAnnot = PDDocCreateRedaction(document.getPDDoc(), redactParams);    //Create the redaction annotation. At this point the text HAS NOT been redacted.

            std::wcout << L"Marked words for redaction." << std::endl;

            PDDocApplyRedactions(document.getPDDoc(), nullptr);                               //Apply the redactions, the text is now redacted.

            std::wcout << L"Words have been permanently removed." << std::endl;
        }
        else
            std::wcout << L"No words were matched, no redactions will be made." << std::endl;

//===================================================================================================================================================================================
// Step 3) Verify that the words were permanently removed. This is an optional step to demonstrate that our words have been completely removed.
//===================================================================================================================================================================================

        wordFinder = PDDocCreateWordFinderEx(document.getPDDoc(), WF_LATEST_VERSION, true, &wfConfig);                //Create the word finder object.

        numberOfWords = 0;                                                                               
        memset(&pdWordArray, 0, sizeof(pdWordArray));

        std::wcout << L"Acquiring word list after redactions..." << std::endl;

        PDWordFinderAcquireWordList(wordFinder, 0, &pdWordArray, nullptr, nullptr, &numberOfWords);                   //Acquire the word list from the page.

        std::wcout << L"Found " << numberOfWords << L" in document, searching for matches..." << std::endl;

        ASInt32 cnt = 0;                                                                                              //Count used to track any words remaining.

        for (ASInt32 index = 0; index < numberOfWords; ++index)                                                       //Iterate through the word list.
        {

            PDWord pdWord = PDWordFinderGetNthWord(wordFinder, index);                                                //Get the PDWord at the given index.

            ASText asTextWord = ASTextNew();                                                                          //Convert word to an ASText object.

            PDWordGetASText(pdWord, 0, asTextWord);

            std::wstring testString = reinterpret_cast<wchar_t*>(ASTextGetUnicodeCopy(asTextWord, unicodeFormat));    //Convert ASText object to a wstring.

            std::transform(testString.begin(), testString.end(), testString.begin(), ::tolower);

            //If either string is matched, increment the count so we can report them.
            if ((wcsstr(testString.c_str(), L"navigation") != nullptr) || (wcsstr(testString.c_str(), L"screen") != nullptr))
                ++cnt;

            ASTextDestroy(asTextWord);                                                                                //Destroy the ASText object.
        }

        PDWordFinderReleaseWordList(wordFinder, 0);                                                                   //Release the word list.

        std::wcout << L"Found " << cnt << L" matches..." << std::endl;

        document.saveDoc(L"RedactedDoc.pdf", true);                                                                   //Save the document with redacted text.
        
    HANDLER

        errCode = ERRORCODE;

        libInit.displayError(errCode);                                                                                //If there was an error, display the error that occured.

    END_HANDLER

    return errCode;                                                                                                   //APDFLib's destructor terminates the library.
}
