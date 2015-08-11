// Copyright (c) 2015, Datalogics, Inc. All rights reserved.

//=====================================================================
// Sample: ExtractText - Extracts the text of the input PDF document
//             and draws it to a new PDF document.
//
// Note:
// This sample will extract text from two seperate PDF documents. It
// will save output in the working directory as a .pdf and .txt file. 
// This program demonstrates the APDFL's ability to handle ASCII and
// unicode text extraction.
//
//Steps:
// 1) Initialize the PDWordFinder class and the information we 
//    need to draw the text to the output.
// 2) Iterate through each word of the input document and draw
//    each new line of text to the output document.
// 3) Extract Unicode from a second PDF document.
// 4) Save and close the input and output documents.
//=====================================================================

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
#include <fstream>

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#include "DLExtrasCalls.h"
#include "PSFCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"

int main(int argc, char** argv)
{
    APDFLib lib;                                             //Initialize the Adobe PDF Library.

    if (lib.isValid() == false)                              //If it failed to initialize, return the error code.
        return lib.getInitError();

    wchar_t* inPath  = L"../_Input/ExtractText.pdf";         //Path to the input document.
    wchar_t* outPath = L"ExtractedText.pdf";                 //Path to the output document we'll create.

    ASErrorCode errCode = 0;                                 //This will catch error codes thrown during library usage.

    DURING

    std::wcout << L"Opening the input PDF." << std::endl;

    APDFLDoc inAPDoc(inPath, true);                          //Open the input document (and repair it if it's damaged).
    APDFLDoc outAPDoc;                                       //Create a new, blank document.

    //give us a new page to insert the text onto. 
    outAPDoc.insertPage(Int16ToFixed(8.5 * 72),              //The width.
                        Int16ToFixed(11 * 72),               //The height.
                        PDBeforeFirstPage);                  //Where we want the page inserted. This will make it the first page in the document.

//============================================================================================================================================================================================================================================================
//Step 1) Initialize the PDWordFinder class and the information we need to draw the text to the output.
//============================================================================================================================================================================================================================================================

    //Prepare the font we'll draw the text with.
    PDEFontAttrs fontAttrs;
    memset(&fontAttrs, 0, sizeof(fontAttrs));
    fontAttrs.name = ASAtomFromString("CourierStd");
    fontAttrs.type = ASAtomFromString("Type0");
    PDEFont font = PDEFontCreateFromSysFont(PDFindSysFont(&fontAttrs, sizeof(fontAttrs), 0), kPDEFontDoNotEmbed);

    //This FixedMatrix will point to where each next word will be drawn.
    ASFixedMatrix nextWordLocation;
    memset(&nextWordLocation, 0, sizeof(nextWordLocation));
    ASInt16 fontSize = 9;                                                                                           //We'll use an 11-point font size.
    nextWordLocation.a = Int16ToFixed(fontSize);                                                                     //Set the character width.
    nextWordLocation.d = Int16ToFixed(fontSize);                                                                     //Set the character height.
    ASFixed leftMargin = ASFloatToFixed(0.5 * 72);                                                                   //(We save this value to reset nextWordLocation for each new line.)
    nextWordLocation.h = leftMargin;                                                                                 //We'll start drawing text half an inch from the left margin. (72 pixels per inch.)
    nextWordLocation.v = ASInt32ToFixed(10 * 72);                                                                    //We'll start drawing text 10 inches from the bottom.

    //We must acquire the page and it's content to add text to it.
    PDPage outPage = outAPDoc.getPage(0);
    PDEContent outPageContent = PDPageAcquirePDEContent(outPage, 0);

    //We'll use the PDWordfinder class to iterate through all the words in our input document.
    //This is not the only way to search through text (See the sample AddBookmarks for another way), but it is a well-supported way.
    //I've chosen to use default settings. See the sample TextSearch, step 1, for an example of PDWordFinder settings.
    PDWordFinder wordFinder = PDDocCreateWordFinderUCS(inAPDoc.getPDDoc(), WF_LATEST_VERSION, 0, NULL);              //The UCS word finder extracts text in unicode format. (But with no Unicode header.)
    PDWord* wordList = new PDWord();
    ASInt32 numWordsFound;
    PDWordFinderAcquireWordList(wordFinder, 0, wordList, NULL, NULL, &numWordsFound);                                //Must be called before we call PDWordFinderGetNthWord. This sets up how we want to traverse the words on page 0. (I'm using the default settings.)

//============================================================================================================================================================================================================================================================
//Step 2) Iterate through each word of the input document and draw each new line of text to the output document.
//============================================================================================================================================================================================================================================================

    std::wcout << L"There are " << numWordsFound << L" words on the first page:" << std::endl << std::endl;

    for (int i = 0; i < numWordsFound; ++i){

        PDEText nextLine = PDETextCreate();                                                          //Prepare to capture the next line.
        int nextLineIndex = 0;                                                                       //Tracks the index in nextLine we'll add successive text objects (here, words) to.
        ASUns16 nextWordAttrs = 0;                                                                   //Tracks the attribute flags of each next word.

        //This do-while loop copies the next line of text into nextLine.
        do
        {
            PDWord nextWord = PDWordFinderGetNthWord(wordFinder, i);

            //We'll copy over the next word of the line using its associated ASText.
            ASText nextWordASText = ASTextNew();
            PDWordGetASText(nextWord, 0, nextWordASText);

            ASInt32 wordLen = 0;                                                                     //Unused. Only needed to call ASTextGetPDTextCopy.
            //std::wcout << ASTextGetPDTextCopy(nextWordASText, &wordLen);                           //This will output the text in UTF-16BE or in PDFDocEncoding, neither of which may display correctly in the terminal. Rest assured it will look fine in the output document.
            
            //Append the word to the line.
            PDETextAddASText(nextLine, kPDETextRun, nextLineIndex, nextWordASText,                   //Add the word to the next index in the text object. It's a text run because it's (usually) multiple characters.
                             font,                                                                   //The font we'll use.
                             NULL, 0,                                                                //Let the graphics state default.
                             NULL, 0,                                                                //Let the text state default.
                             &nextWordLocation);                                                     //The starting location of the word on the page.

            //Now that the word's been added, get the new location of the end of the line to prepare to add text there.
            ASFixedRect* newLocation = new ASFixedRect();
            PDETextGetBBox(nextLine, kPDETextRun, PDETextGetNumRuns(nextLine) - 1, newLocation);
            nextWordLocation.h = newLocation->right;                                                 //The h coordinate of our next word will be (at least) the very end of the last word.
            delete(newLocation);

            //If the last word we printed is followed by a space, add a little to the starting h coordinate of the next word to account for that.
            nextWordAttrs = PDWordGetAttr(nextWord);
            if (WXE_ADJACENT_TO_SPACE&nextWordAttrs)
            {
                std::wcout << L" ";
                nextWordLocation.h += ASFloatToFixed(0.5*fontSize);
            }

            ASTextDestroy(nextWordASText);
            
            ++nextLineIndex;
            ++i;

        } while ((!(WXE_LAST_WORD_ON_LINE&nextWordAttrs)));                                          //Note that this procedure works only if the last word in the page is the last word in its line (which is true).
        --i;                                                                                         //i is overstepped at the end of the do-while loop.

        PDEContentAddElem(outPageContent, kPDEAfterLast, reinterpret_cast<PDEElement>(nextLine));    //We've captured the line, so add it to our output page's content.

        std::wcout << std::endl;

        //Prepare to draw the next line.
        nextWordLocation.h  = leftMargin;                                                            //Reset our h coordinate to the left hand side we started on.
        nextWordLocation.v -= Int16ToFixed(fontSize);                                                //Drop down the v coordinate just enough to draw new, non-overlapping text.
        PDERelease(reinterpret_cast<PDEObject>(nextLine));                                           //We'll make a new PDEText object for the next line come next iteration.
    }
    PDPageSetPDEContentCanRaise(outPage, NULL);                                                      //We've now captured every line of text. This sets all the content we've just added into the page.

    //We're done with these, so we release them.
    PDERelease(reinterpret_cast<PDEObject>(font));
    PDWordFinderReleaseWordList(wordFinder, 0);                                                      //0 is the page index we used.
    
    std::wcout << std::endl << L"(Unicode text will not have displayed correctly.)";
    std::wcout << std::endl << L"(But unicode text is printed in the output correctly.)" << std::endl;
    std::wcout << std::endl << L"The text has been added to the output document." << std::endl;

//============================================================================================================================================================================================================================================================
//Step 3) Extract unicode from a second PDF document. Open the document and extract the unicode chracters to a text file in the working directory.
//============================================================================================================================================================================================================================================================

    APDFLDoc document(L"../_Input/ExtractUnicodeText.pdf", true);                              //Open the input document.

    PDPage pdPage = document.getPage(0);                                                       //Get the first page.

    PDEContent pdeContent = PDPageAcquirePDEContent(pdPage, 0);                                //Acquire the PDEContent from the page.

    PDEText pdeText = reinterpret_cast<PDEText>(PDEContentGetElem(pdeContent, 0));             //Get the PDEText element from the PDEContent.

    ASInt32 numberOfRuns = PDETextGetNumRuns(pdeText);                                         //Get the number of text runs in the PDEText object.

    std::ofstream outputFile(L"ExtractedUnicodeText.txt");                                     //Create a .txt output file for text extraction.                  

    ASUTF8Val * utf8String = nullptr;                                                          //Unicode string used to write to output file.

    if (outputFile.is_open())                                                                  //Ensure the .txt file opened correctly.
    {
        for (ASInt32 i = 0; i < numberOfRuns; ++i)                                             //For each text run.
        {
            ASText asText = ASTextNew();                                                       //Create a new empty ASText object.              

            PDETextGetASText(pdeText, kPDETextRun, i, asText);                                 //Get the ASText from the PDEText object.              

            utf8String = reinterpret_cast<ASUTF8Val*>(ASTextGetUnicodeCopy(asText, kUTF8));    //Acquire the UTF8 string from the ASText object.

            outputFile << utf8String << std::endl;                                             //Write the string and a newline to the output file.

            ASfree(utf8String);                                                                //Free up resources.

            ASTextDestroy(asText);
        }
    }
    else
    {
        std::wcerr << L"Failed to create or open ExtractedUnicodeText.txt" << std::endl;
    }

    outputFile.close();                                                                        

    PDPageReleasePDEContent(pdPage, nullptr);                                                  

    PDPageRelease(pdPage);                                                                     

//============================================================================================================================================================================================================================================================
// Step 3) Save and close the input and output documents.
//============================================================================================================================================================================================================================================================

    PDPageReleasePDEContent(outPage, NULL);           //The content must be released before we can release the page.
    PDPageRelease(outPage);                           //The page must be released before we can save the document.
    outAPDoc.saveDoc(outPath);                        //Save the new document. APDFLDoc's saveDoc method defaults to use the PDSaveFull flag.

    //APDFLDoc's destructor takes care of closing the input and output documents.

    std::cout << "Success." << std::endl;

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);                    //If there was an error, display it.

    END_HANDLER

    return errCode;                                   //lib's destructor terminates the library.
};
