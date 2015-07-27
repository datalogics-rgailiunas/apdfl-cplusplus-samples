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
#include <vector>

#include "CorCalls.h"
#include "PDCalls.h"
#include "PDExpT.h"

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

static ACCB1 ASBool ACCB2 wordCounter(PDWordFinder, PDWord, ASInt32, void *);

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
        wfConfig.noXYSort = false;                           //Generate an XY-ordered word list.
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
        wfConfig.decomposeTbl = NULL;                        //Table may be used to expand unicode ligatures not in the default list.
        wfConfig.decomposeTblSize = 0;                       //Not using decomposeTbl so the size is 0.
        wfConfig.charTypeTbl = NULL;                         //Custom table to enhance word breaking quality.
        wfConfig.charTypeTblSize = 0;                        //Unused, so the size will be 0.
        wfConfig.preserveRedundantChars = false;             //May be used to preserve overlapping redundant characters in some PDF documents.
        wfConfig.disableCharReordering = false;              //Used in cases where the PDF page has heavily overlapped character bounding boxes.

//===================================================================================================================================================================================
// Step 2) DO SOME STUFF HERE OKAY
//===================================================================================================================================================================================
   
        ASBool useHostEncoding = true;

        //Create the PDWordFinder object used to extract and enumerate the words on pages in a PDF document.
        PDWordFinder wordFinder = PDDocCreateWordFinderEx(document.getPDDoc(), WF_LATEST_VERSION, useHostEncoding, &wfConfig);    

        PDWord pdWord;
        PDWord * wordList;
        ASInt32 numberOfWords = 0;
        ASInt32 pageNum = 0;

        //Acquire the sorted list of words on the page.
        PDWordFinderAcquireWordList(wordFinder, pageNum, &pdWord, &wordList, 0, &numberOfWords);

        for (int i = 0; i < numberOfWords; ++i)
        {
            std::vector<ASFixedQuad> quadTable;
            std::vector<ASFixedQuad> charQuadTable;

            PDWord tempWord = PDWordFinderGetNthWord(wordFinder, i);

            while (PDWordGetNthQuad(tempWord, quadCount))

        }

    HANDLER

        errCode = ERRORCODE;

        libInit.displayError(errCode);    //If there was an error, display it.

        END_HANDLER
            system("pause");
    return errCode;                       //APDFLib's destructor terminates the APDFL.                            
}

//===================================================================================================================================================================================
// Function wordCounter() - Callback function used by PDWordFinderEnumWords to count the number of times a particular word is found.
// This callback is called once for each word.
//===================================================================================================================================================================================

static ACCB1 ASBool ACCB2 wordCounter(PDWordFinder wordFinder, PDWord pdWord, ASInt32 pageNum, void * fileNum)
{
    return true;
}