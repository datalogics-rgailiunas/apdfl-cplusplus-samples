// Copyright(c) 2015, Datalogics, Inc.All rights reserved.

//===========================================================================
// Sample: AddBookmarks - Adds some bookmarks to the input PDF.
//
// Note: The input for this sample is a text-heavy PDF document with bolded 
// subheadings and non-bolded body text. This sample searchs for each bolded 
// subheading and adds a bookmark which will take the reader to that
// subheading.
// This sample also adds a few children bookmarks to the first
// bookmark, which copy that bookmark at different zoom levels.
//
//Steps:
// 1) Find each bolded subheading in the document and record their
//    location and text.
// 2) Create a bookmark for each bolded section with this information.
// 3) Demonstrate different zoom levels.
// 4) Save and close the document.
//===========================================================================

// This agreement is between Datalogics, Inc. 101 N.Wacker Drive, Suite 1800,
// Chicago, IL 60606 ("Datalogics") and you, an end user who downloads
// source code examples for integrating to the Adobe PDF Library
// ("the Example Code"). By accepting this agreement you agree to be bound
// by the following terms of use for the Example Code.
//
// LICENSE
// -------
// Datalogics hereby grants you a royalty - free, non - exclusive license to
// download and use the Example Code for any lawful purpose.There is no charge
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

#include <sstream>
#include <string>
#include <vector>

#include "APDFLDoc.h"
#include "InitializeLibrary.h"

#include "PERCalls.h"
#include "PagePDECntCalls.h"
#include "ASCalls.h"

//Converts a char string to a wchar_t string.
wchar_t* toWide(const char* str);

//Converts a wide string to an ASText object.
ASText toASText(const wchar_t* string);

//Checks to see if a PDWord is bolded.
bool isBold(PDWord word, PDWordFinder finder);

int main(int argc, char** argv)
{
    APDFLib lib;                                                  //Initialize the Adobe PDF Library.
    if (lib.isValid() == false)                                   //If it failed to initialize, return the error code.
        return lib.getInitError();

    wchar_t* inputPath  = L"../_Input/Ulysses.pdf";               //Input PDF path.
    wchar_t* outputPath = L"Bookmarked.pdf";                      //Output path we'll save to.

    ASErrorCode errCode = 0;                                      //Tracks runtime errors in the application.

    DURING

    std::wcout << L"Opening the input document." << std::endl;

    APDFLDoc APDoc(inputPath, true);
    PDDoc mydoc = APDoc.getPDDoc();

//======================================================================================================================================================================================================================================================
//Step 1) Find each bolded subheading in the document and record their location and text.
//======================================================================================================================================================================================================================================================

    ASInt32 numBookmarks = 0;
    std::vector<ASFixedRect> bsLocts;                                                               //bsLocts[n] is the bounding rectangle (relative to its page) of the nth subheading.
    std::vector<ASInt32> bsPages;                                                                   //bsPages[n] is the page number the nth subheading appeared on.
    std::vector<ASText> bsTexts;                                                                    //bsTexts[n] is the text of the nth subheading.

    //We'll use the PDWordFinder class to iterate through our document's words, looking for sequences of bold words.
    PDWordFinder wordFinder = PDDocCreateWordFinderUCS(mydoc, WF_LATEST_VERSION, 0, NULL);

    PDWord* wordList = new PDWord();                                                                //Our PDWordFinder will iterate through the words with this. We will not directly access it.
    ASInt32 numWordsFound;

    //This algorithm finds sequences of words which satisfy a boolean function. Here, we use a boolean function
    //which checks to see if the word is bold.
    //We assume that no sequence will span more than one page. Or, if a sequence does span more than one page,
    //it is treated as more than one sequence.
    for (int nextPage = 0; nextPage < PDDocGetNumPages(mydoc); ++nextPage)
    {
        PDWordFinderAcquireWordList(wordFinder, nextPage, wordList, NULL, NULL, &numWordsFound);    //Must be called before we call PDWordFinderGetNthWord. This sets up how we want to traverse the words on page 0. (I'm using the default settings.)

        bool foundSubheading = false;                                                               //False when we haven't yet found a new bolded text, true when we're currently traversing through bold text.

        for (int nextWordIndex = 0; nextWordIndex < numWordsFound; ++nextWordIndex)
        {
            PDWord nextWord = PDWordFinderGetNthWord(wordFinder, nextWordIndex);

            bool wordCondition = isBold(nextWord,wordFinder);                                       //I've abstracted this out to demonstrate the generality of this algorithm. You could use any boolean function of a PDWord here.

            if (foundSubheading)
            {
                if (!wordCondition)
                {
                    //We've found the end of the subheading: the new word is not bold.
                    //We'll record only the right-hand quads of the previous word.
                    PDWord previousWord = PDWordFinderGetNthWord(wordFinder, nextWordIndex - 1);
                    ASInt16 nQuads = PDWordGetNumQuads(previousWord);                               //The word might be split up into several quads (e.g., if it's hyphenated), and we want the last set.
                    ASFixedQuad quad;
                    PDWordGetNthQuad(previousWord, nQuads - 1, &quad);
                    (*(bsLocts.end() - 1)).right = quad.tr.h;                                       //The right end of the location box lines up with the horizontal coordinate of the top-right point.
                    (*(bsLocts.end() - 1)).bottom = quad.tr.v;                                      //The bottom end of the location box lines up with the vertical of the top-right point.


                    foundSubheading = false;
                }
                else if (wordCondition && (nextWordIndex == numWordsFound - 1))
                {
                    //We've found the end of the subheading: The new word is bold, and we're at the end of the page.
                    //We'll record this word's right-hand quads, and the word's text.
                    
                    //The quads.
                    ASInt16 nQuads = PDWordGetNumQuads(nextWord);                                   //The word might be split up into several quads (e.g., if it's hyphenated), and we want the last set.
                    ASFixedQuad quad;
                    PDWordGetNthQuad(nextWord, nQuads - 1, &quad);
                    (*(bsLocts.end() - 1)).right = quad.tr.h;                                       //The right end of the location box lines up with the horizontal coordinate of the top-right point.
                    (*(bsLocts.end() - 1)).bottom = quad.tr.v;                                      //The bottom end of the location box lines up with the vertical of the top-right point.

                    //The text.
                    ASText nextWordASText = ASTextNew();
                    PDWordGetASText(nextWord, 0, nextWordASText);

                    if (PDWordGetAttr(nextWord)&WXE_ADJACENT_TO_SPACE)
                    {
                        ASText space = toASText(L" ");
                        ASTextCatMany((*(bsTexts.end() - 1)), space, nextWordASText);
                        ASTextDestroy(space);
                    }
                    else
                    {
                        ASTextCat((*(bsTexts.end() - 1)), nextWordASText);
                    }
                    ASTextDestroy(nextWordASText);

                    foundSubheading = false;
                }
                else
                {
                    //We've found the next word of our subheading.
                    //We must record the text of this word.
                    ASText nextWordASText = ASTextNew();
                    PDWordGetASText(nextWord, 0, nextWordASText);

                    if (PDWordGetAttr(nextWord)&WXE_ADJACENT_TO_SPACE)
                    {
                        ASText space = toASText(L" ");
                        ASTextCat((*(bsTexts.end() - 1)), space);
                        ASTextCat((*(bsTexts.end() - 1)), nextWordASText);
                        ASTextDestroy(space);
                    }
                    else
                    {
                        ASTextCat((*(bsTexts.end() - 1)), nextWordASText);
                    }
                    ASTextDestroy(nextWordASText);
                }
            }
            else
            {
                if (wordCondition)
                {
                    //We haven't found a subheading yet, but this word is bold. So it's the start of a new subheading.
                    //We must record the page number, the left-hand quads of this word, and its text.

                    //The page number.
                    bsPages.push_back(nextPage);

                    //The text of the word.
                    ASText nextSubhText = ASTextNew();
                    PDWordGetASText(nextWord, 0, nextSubhText);
                    bsTexts.push_back(nextSubhText);
                    
                    //Its quads.
                    ASInt16 nQuads = PDWordGetNumQuads(nextWord);                                   //The word might be split up into several quads (e.g., if it's hyphenated), and we want the last set.
                    ASFixedQuad quad;
                    PDWordGetNthQuad(nextWord, nQuads - 1, &quad);
                    ASFixedRect nextSubhRect;
                    nextSubhRect.left = quad.tl.h;                                                  //The left location of the box lines up with the horizontal coordinate of the top-left point.
                    nextSubhRect.top = quad.tl.v;                                                   //The top location of the box lines up with the vertical coordinate of the top-left pont.
                    bsLocts.push_back(nextSubhRect);
                    foundSubheading = true;

                    ++numBookmarks;                                                                 //Update the number of bookmarks.
                }
                else
                {
                    //We haven't found a subheading, nor is this word bold. So we'll just continue traversing.
                }
            }
            //One condition this does NOT account for is if the subheading spans more than one line. Then the matter of constructing the quads
            //is somewhat more complicated. You need the top-left quad of the first word, the top-right quad of the last word on the first line,
            //the bottom-left quad of the first word on the last line, and the bottom-right quad of the last word. Otherwise it would be the same algorithm.
        }
        PDWordFinderReleaseWordList(wordFinder, nextPage);                                          //Prepare to iterate over the word list for the next page.
    }


    std::wcout << L"I found " << numBookmarks << L" subheadings:" << std::endl;                     //We could have used any of the vectors, not just bsLocts.

    for (ASText subheadingText : bsTexts)
    {
        ASInt32 wordLen = 0;                                                                        //Unused. Only for calling ASTextGetPDTextCopy.
        std::wcout << ASTextGetPDTextCopy(subheadingText,&wordLen) << std::endl;                    //Unicode text will probably not display correctly. Rest assured it will look fine in the document.
    }

//======================================================================================================================================================================================================================================================
//Step 2) Create a bookmark for each bolded section with this information.
//======================================================================================================================================================================================================================================================

    std::wcout << L"Creating a bookmark for each subheading..." << std::endl;

    //We'll create each bookmark by iterating over the subheadings.
    //The bookmark's text will be the subheading's text,
    //And the bookmark's action will be to bring the reader to
    //the location of that subheading.

    PDBookmark bookMarkRoot = PDDocGetBookmarkRoot(mydoc);                                 //Bookmarks are added to a document's bookmark root.

    for (int nextBM = 0; nextBM < numBookmarks; ++nextBM)
    {
        //Get the necessary information.
        PDPage nextPage = APDoc.getPage(bsPages[nextBM]);                                  //Get the associated page.

        ASFixedRect* nextLocation = bsLocts.begin()._Ptr + nextBM;                         //Get the associated page location (the method we use requires a pointer).

        //Make the bookmark and set its action.
        PDBookmark nextbm = PDBookmarkAddNewChildASText(bookMarkRoot, bsTexts[nextBM]);    //Bookmarks must be created before their action is set.

        //We create a View Destination pointing to the location of the subheading, and then create an action
        //which will take the reader to that destination.
        PDViewDestination nextDestination = PDViewDestCreate(mydoc, nextPage,
                                                             ASAtomFromString("XYZ"),      //View Destination Fit Type
                                                             nextLocation,                 //Pointer to the location rectangle we want.
                                                             Int16ToFixed(0),              //Zoom factor. 0 means to inherit the current zoom factor
                                                             0);                           //Unused argument

        PDAction nextDestAct = PDActionNewFromDest(mydoc,nextDestination,mydoc);           //The first and third arguments are the source PDDoc and the destination PDDoc, respectively.
                                                                                           //They must the the same.
        PDBookmarkSetAction(nextbm, nextDestAct);                                          //Give the bookmark its action!
        PDPageRelease(nextPage);                                                           //Prepare to get the next bookmark's page. (It may end up being the same page.)
    }
    std::wcout << L"Done." << std::endl;

//======================================================================================================================================================================================================================================================
//Step 3) Demonstrate different zoom levels.
//======================================================================================================================================================================================================================================================

    std::wcout << L"Adding zoom demonstration bookmarks." << std::endl;

    //This steps adds a few children bookmarks to the first bookmark of the
    //document, which all copy that bookmark at different zoom levels.

    PDBookmark parentBm = PDBookmarkGetFirstChild(PDDocGetBookmarkRoot(mydoc));                                                        //The bookmark we'll add children to. (The first bookmark.)
    PDBookmark zoom100  = PDBookmarkAddNewChild(parentBm, "100% Zoom");
    PDBookmark zoom200  = PDBookmarkAddNewChild(parentBm, "200% Zoom");
    PDBookmark zoom800  = PDBookmarkAddNewChild(parentBm, "800% Zoom");
    PDBookmark zoom40   = PDBookmarkAddNewChild(parentBm, "40% Zoom");

    ASInt8 numZoomBookmarks = 4;
    PDBookmark zoomBookmarks[] = {zoom100,      zoom200,      zoom800,      zoom40};
    ASFloat zoomFactors[] = {1.0, 2.0, 8.0, 0.40};

    //Copy the attributes of the parent bookmark.
    ASInt32 pageNumber;                                                                                                                //The page index of the first bookmark.
    ASAtom fitType;                                                                                                                    //The first bookmark's view destination fit type.
    ASFixedRect locationRect;                                                                                                          //The location rectangle of the first bookmark.
    ASFixed zoomFactor;                                                                                                                //The first bookmark's zoom factor (we won't be using this).
    PDViewDestination parentViewDestination = PDActionGetDest(PDBookmarkGetAction(parentBm));

    PDViewDestGetAttr(parentViewDestination, &pageNumber, &fitType, &locationRect, &zoomFactor);

    //Set each bookmark's view destination per the above array.
    PDPage parentPage = APDoc.getPage(pageNumber);                                                                                     //The page of the parent bookmark.
    for (int i = 0; i < numZoomBookmarks; ++i)
    {
        PDViewDestination nextView = PDViewDestCreate(mydoc, parentPage, fitType,&locationRect, ASFloatToFixed(zoomFactors[i]), 0);
        PDAction nextAction = PDActionNewFromDest(mydoc, nextView, mydoc);
        PDBookmarkSetAction(zoomBookmarks[i],nextAction);
    }

    PDPageRelease(parentPage);

    std::wcout << L"Done. Saving and closing the document." << std::endl;

//======================================================================================================================================================================================================================================================
//Step 5) Save and close the document.
//======================================================================================================================================================================================================================================================

    for (auto x : bsTexts)
        ASTextDestroy(x);

    APDoc.saveDoc(outputPath);

    std::wcout << L"Success!" << std::endl;

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);             //If there was an error, display it.

    END_HANDLER

    return errCode;
};

//======================================================================================================================================================================================================================================================
//wchar_t* function: Converts a char string to a wchar_t string.
//======================================================================================================================================================================================================================================================
wchar_t* toWide(const char* str)
{
    const size_t strlen = (std::strlen(str)) + 1;
    wchar_t* wstr = new wchar_t[strlen];
    mbstowcs(wstr, str, strlen);
    return wstr;
};

//======================================================================================================================================================================================================================================================
//ASText function: Convert a wide string to an ASText object.
//======================================================================================================================================================================================================================================================
ASText toASText(const wchar_t* string)
{
    ASUnicodeFormat hostUniFormat;

    DURING

    if (sizeof(wchar_t) == 2)
        hostUniFormat = kUTF16HostEndian;
    else
        hostUniFormat = kUTF32HostEndian;

    E_RETURN( ASTextFromUnicode((ASUTF16Val *)string, hostUniFormat));

    HANDLER

        RERAISE();                                                        //If there was an exception, let the caller handle it.

    END_HANDLER

    return NULL;
};

//======================================================================================================================================================================================================================================================
//bool function: Checks to see if a PDWord is bolded. (Assumes that if the first letter is bolded, the whole word is.)
//======================================================================================================================================================================================================================================================
bool isBold(PDWord word, PDWordFinder finder)
{
    PDStyle firstCharStyle = PDWordGetNthCharStyle(finder,word, 0);
    PDFont firstCharFont = PDStyleGetFont(firstCharStyle);
 
    //We know we have a bolded font if "Bold" is in the font's name. This is a crude way of determining of the font is bold.
    char* fontName = new char[100];
    PDFontGetName(firstCharFont, fontName,100);
    std::string fontNs(fontName);
    delete[](fontName);
    return fontNs.find("Bold") != fontNs.npos;
}
