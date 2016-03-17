// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
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
//=====================================================================

#include <iostream>
#include <fstream>
#include <cstring>

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#include "DLExtrasCalls.h"
#include "PSFCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"

int main(int argc, char** argv)
{
    APDFLib lib;                                                                                //Initialize the Adobe PDF Library.

    if (lib.isValid() == false)                                                                 //If it failed to initialize, return the error code.
        return lib.getInitError();

    wchar_t* inPath  = L"../_Input/ExtractText.pdf";                                            //Path to the input document.
    wchar_t* outPath = L"ExtractedText.pdf";                                                    //Path to the output document we'll create.

    ASErrorCode errCode = 0;                                                                    //This will catch error codes thrown during library usage.

    DURING

    std::wcout << L"Opening the input PDF." << std::endl;

    APDFLDoc inAPDoc(inPath, true);                                                             //Open the input document (and repair it if it's damaged).
    APDFLDoc outAPDoc;                                                                          //Create a new, blank document.

    outAPDoc.insertPage(FloatToASFixed(8.5 * 72), Int16ToFixed(11 * 72), PDBeforeFirstPage);    //Insert a new page into the document.                  

//=====================================================================================================================================================================================================================
//Step 1) Initialize the PDWordFinder class and the information we need to draw the text to the output.
//Note: This is not the only way to search through text (See the sample AddBookmarks for another way), but it is a well-supported way.
//=====================================================================================================================================================================================================================

    //Prepare the font we'll draw the text with.
    PDEFontAttrs fontAttrs;
    memset(&fontAttrs, 0, sizeof(fontAttrs));
    fontAttrs.name = ASAtomFromString("CourierStd");
    fontAttrs.type = ASAtomFromString("Type0");
    PDEFont font = PDEFontCreateFromSysFont(PDFindSysFont(&fontAttrs, sizeof(fontAttrs), 0), kPDEFontDoNotEmbed);
 
    //A default graphics state with which to draw the text.
    PDEGraphicState graphics;
    PDEDefaultGState(&graphics, sizeof(PDEGraphicState));

    //This FixedMatrix will point to where each next word will be drawn.
    ASFixedMatrix nextWordLocation;
    memset(&nextWordLocation, 0, sizeof(nextWordLocation));
    ASInt16 fontSize = 9;                                                                                            //We'll use a 9-point font size.
    nextWordLocation.a = Int16ToFixed(fontSize);                                                                     //Set the character width.
    nextWordLocation.d = Int16ToFixed(fontSize);                                                                     //Set the character height.
    ASFixed leftMargin = ASFloatToFixed(0.5 * 72);                                                                   //(We save this value to reset nextWordLocation for each new line.)
    nextWordLocation.h = leftMargin;                                                                                 //We'll start drawing text half an inch from the left margin. (72 pixels per inch.)
    nextWordLocation.v = ASInt32ToFixed(10 * 72);                                                                    //We'll start drawing text 10 inches from the bottom.

    //Use default settings for the PDWordFinder. See the sample TextSearch, step 1, for an example of PDWordFinder settings.
    PDWordFinderConfigRec wfConfig;
    memset (&wfConfig, 0, sizeof(PDWordFinderConfigRec));
    wfConfig.recSize = sizeof(PDWordFinderConfigRec);

    //We'll use the PDWordfinder class to iterate through all the words in our input document.
    PDWordFinder wordFinder = PDDocCreateWordFinderEx(inAPDoc.getPDDoc(), WF_LATEST_VERSION, false, &wfConfig);      //If boolean value is set to true, the word finder extracts text in unicode.

    PDWord wordList;
    ASInt32 numWordsFound;
    PDWordFinderAcquireWordList(wordFinder, 0, &wordList, NULL, NULL, &numWordsFound);                              //This acquires the list of words from the first page (0).           

//=====================================================================================================================================================================================================================
//Step 2) Iterate through each word of the input document and draw each new line of text to the output document.
//=====================================================================================================================================================================================================================

    std::wcout << L"There are " << numWordsFound << L" words on the first page:" << std::endl << std::endl;

    PDPage outPage = outAPDoc.getPage(0);                                                            //We must acquire the page and it's content to add text to it.
    PDEContent outPageContent = PDPageAcquirePDEContent(outPage, 0);

    for (int i = 0; i < numWordsFound; ++i)
    {
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

            //This will print the text in UTF-16BE or in PDFDocEncoding, this may not output correctly in the console. It will be correct in the output document.
            ASInt32 wordLen = 0;                                                                     
            char * consoleMessage = ASTextGetPDTextCopy(nextWordASText, &wordLen);
            std::cout << consoleMessage;                                                             //Print out the text being extracted.
            ASfree(consoleMessage);

            //Append the word to the line.
            PDETextAddASText(nextLine, kPDETextRun, nextLineIndex, nextWordASText,                   //Add the word to the next index in the text object. It's a text run because it's (usually) multiple characters.
                             font,                                                                   //The font we'll use.
                             &graphics, sizeof(PDEGraphicState),                                     //Let the graphics state default.
                             NULL, 0,                                                                //Let the text state default.
                             &nextWordLocation);                                                     //The starting location of the word on the page.

            //Now that the word's been added, get the new location of the end of the line to prepare to add text there.
            ASFixedRect newLocation;
            PDETextGetBBox(nextLine, kPDETextRun, PDETextGetNumRuns(nextLine) - 1, &newLocation);
            nextWordLocation.h = newLocation.right;                                                  //The h coordinate of our next word will be (at least) the very end of the last word.

            //If the last word we printed is followed by a space, add a little to the starting h coordinate of the next word to account for that.
            nextWordAttrs = PDWordGetAttr(nextWord);
            if (WXE_ADJACENT_TO_SPACE & nextWordAttrs)
            {
                std::wcout << L" ";
                nextWordLocation.h += ASFloatToFixed(0.5*fontSize);
            }

            ASTextDestroy(nextWordASText);
            
            ++nextLineIndex;
            ++i;

        } while ((!(WXE_LAST_WORD_ON_LINE & nextWordAttrs)));                                        //Note that this procedure works only if the last word in the page is the last word in its line (which is true).
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
    PDWordFinderDestroy(wordFinder);
    
    std::wcout << std::endl << L"(Unicode text will not have displayed correctly.)";
    std::wcout << std::endl << L"(But unicode text is printed in the output correctly.)" << std::endl;
    std::wcout << std::endl << L"The text has been added to the output document." << std::endl;

    PDPageReleasePDEContent(outPage, NULL);                                                          //The content must be released before we can release the page.
    PDPageRelease(outPage);                                                                          //The page must be released before we can save the document.
    outAPDoc.saveDoc(outPath);                                                                       //Save the new document. APDFLDoc's saveDoc method defaults to use the PDSaveFull flag.

//=====================================================================================================================================================================================================================
//Step 3) Extract unicode from a second PDF document. Open the document and extract the unicode chracters to a text file in the working directory.
//=====================================================================================================================================================================================================================

    APDFLDoc document(L"../_Input/ExtractUnicodeText.pdf", true);                                                       //Open the input document.

    std::ofstream outputFile("ExtractedUnicodeText.txt");                                                               //Create a .txt output file for text extraction.

    if (outputFile.is_open())
    {
        PDWordFinder pdWordFinder = PDDocCreateWordFinderEx(document.getPDDoc(), WF_LATEST_VERSION, true, &wfConfig);    //If boolean value is set to true, the word finder extracts text in unicode.

        ASInt32 numWords;
        PDWord wordArray;
        PDWordFinderAcquireWordList(pdWordFinder, 0, &wordArray, NULL, NULL, &numWords);                                //This acquires the list of words from the first page (0).

        std::wcout << L"There are " << numWords << L" words on the first page:" << std::endl << std::endl;

        PDPage pdPage = document.getPage(0);                                                                            //We must acquire the page and it's content to add text to it.
        PDEContent pdeContent = PDPageAcquirePDEContent(pdPage, 0);                                                     

        for (ASInt32 index = 0; index < numWords; ++index)                                                              //Iterate through the acquired word list and extract the words to a text file.
        {
            ASUTF8Val* utf8String;
            PDWord pdWord = PDWordFinderGetNthWord(pdWordFinder, index);

            ASText asText = ASTextNew();
            PDWordGetASText(pdWord, 0, asText);

            utf8String = reinterpret_cast<ASUTF8Val*>(ASTextGetUnicodeCopy(asText, kUTF8));                             //Get the endian neutral utf8 string.
        
            ASUns16 wordAttrs = PDWordGetAttr(pdWord);

            if ((WXE_LAST_WORD_ON_LINE & wordAttrs) == WXE_LAST_WORD_ON_LINE)                                           //Some formatting, more checks can be used for more complex documents.
                outputFile << utf8String << std::endl;                                                                  //Insert the text into the output file.
            else
                outputFile << utf8String << " ";

            ASTextDestroy(asText);
        }

        //Close any remaining resources. APDFLDoc's destructor will take care of closing the documents.
        outputFile.close();
        PDPageReleasePDEContent(pdPage, 0);
        PDPageRelease(pdPage);
        PDWordFinderReleaseWordList(pdWordFinder, 0);                                                      
        PDWordFinderDestroy(pdWordFinder);

        std::wcout << L"Success." << std::endl;
    }
    else
    {
        std::wcout << L"Failed to create or open the output file." << std::endl;
    }
    

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);                    //If there was an error, display it.

    END_HANDLER

    return errCode;                                   //APDFLib's destructor terminates the library.
};
