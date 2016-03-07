// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//===========================================================================
// Sample: AddBookmarks - Adds some bookmarks to the input PDF.
//
// Note: The input for this sample is a text-heavy PDF document.
// This sample searchs for target word and adds a bookmark which will
// take the reader to that word.
// This sample also adds a few children bookmarks to the first
// bookmark, which copy that bookmark at different zoom levels.
//
//Steps:
// 1) Find each target word in the document and record their
//    location and text.
// 2) Create a bookmark for each occurrence with this information.
// 3) Demonstrate different zoom levels.
// 4) Save and close the document.
//===========================================================================

#include <sstream>
#include <string>
#include <vector>

#include "APDFLDoc.h"
#include "InitializeLibrary.h"

#include "PERCalls.h"
#include "PagePDECntCalls.h"
#include "ASCalls.h"

bool searchWord(PDWord nextWord, const char* target);
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
//Step 1) Find each occurrence of target word in the document and record their location and text.
//======================================================================================================================================================================================================================================================

	//We are using three vectors to keep track of what we are doing, they have to be updated synchronizely.
    std::vector<ASFixedRect> bsLocts;                                                               //bsLocts[n] is the bounding rectangle (relative to its page) of the nth occurrence.
    std::vector<ASInt32> bsPages;                                                                   //bsPages[n] is the page number the nth occurrence appeared on.
    std::vector<ASText> bsTexts;                                                                    //bsTexts[n] is the text of the nth occurrence.

    //We'll use the PDWordFinder class to iterate through our document's words, looking for target word.
    PDWordFinder wordFinder = PDDocCreateWordFinderUCS(mydoc, WF_LATEST_VERSION, 0, NULL);

    PDWord* wordList = new PDWord();                                                                //Our PDWordFinder will iterate through the words with this. We will not directly access it.
    ASInt32 numWordsFound;

    //This algorithm finds target word which satisfy a boolean function. Here, we use a boolean function
    //which checks to see if the word is match.
    //We assume that no word will span more than one page.
    for (int nextPage = 0; nextPage < PDDocGetNumPages(mydoc); ++nextPage)
    {
        PDWordFinderAcquireWordList(wordFinder, nextPage, wordList, NULL, NULL, &numWordsFound);    //Must be called before we call PDWordFinderGetNthWord. This sets up how we want to traverse the words on page 0. (I'm using the default settings.)

        for (int nextWordIndex = 0; nextWordIndex < numWordsFound; ++nextWordIndex)
        {
            PDWord nextWord = PDWordFinderGetNthWord(wordFinder, nextWordIndex);
			// target word is "before" in this sample code
			char	myWord[20] = "before";
			if (searchWord(nextWord, myWord)){
				//After we found the word
				//The page number.
				bsPages.push_back(nextPage);
				//The text of the word.
				ASText nextWordText = ASTextNew();
				PDWordGetASText(nextWord, 0, nextWordText);
				bsTexts.push_back(nextWordText);

				//Its quads.
				ASInt16 nQuads = PDWordGetNumQuads(nextWord);                                   //The word might be split up into several quads (e.g., if it's hyphenated), and we want the last set.
				ASFixedQuad quad;
				PDWordGetNthQuad(nextWord, nQuads - 1, &quad);
				ASFixedRect partialWord;
				partialWord.left = quad.tl.h;                                                  //The left location of the box lines up with the horizontal coordinate of the top-left point.
				partialWord.top = quad.tl.v;                                                   //The top location of the box lines up with the vertical coordinate of the top-left pont.
				bsLocts.push_back(partialWord);
			}
		}
        PDWordFinderReleaseWordList(wordFinder, nextPage);                                          //Prepare to iterate over the word list for the next page.
    }


    std::wcout << L"I found " << bsPages.size() << L" search occurrence:" << std::endl;                     //We could have used any of the vectors, not just bsLocts.

    for (ASText partialText : bsTexts)
    {
        ASInt32 wordLen = 0;                                                                        //Unused. Only for calling ASTextGetPDTextCopy.
		std::wcout << ASTextGetPDTextCopy(partialText, &wordLen) << std::endl;                      //Unicode text will probably not display correctly. Rest assured it will look fine in the document.
    }

//======================================================================================================================================================================================================================================================
//Step 2) Create a bookmark for each occurrence of target word with this information.
//======================================================================================================================================================================================================================================================

    std::wcout << L"Creating a bookmark for each search occurrence..." << std::endl;

    //We'll create each bookmark by iterating over the subheadings.
    //The bookmark's text will be the subheading's text,
    //And the bookmark's action will be to bring the reader to
    //the location of that subheading.

    PDBookmark bookMarkRoot = PDDocGetBookmarkRoot(mydoc);                                 //Bookmarks are added to a document's bookmark root.

    for (int nextBM = 0; nextBM < bsPages.size(); ++nextBM)
    {
        //Get the necessary information.
        PDPage nextPage = APDoc.getPage(bsPages[nextBM]);                                  //Get the associated page.

        ASFixedRect* nextLocation = &bsLocts[nextBM];						  //Get the associated page location (the method we use requires a pointer).

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
//bool function: Checks to see if a PDWord contains target word. (Assumes target occurance count only once in each word.)
//======================================================================================================================================================================================================================================================
bool searchWord(PDWord nextWord, const char* target)
{

	ASText nextWordASText = ASTextNew();
	PDWordGetASText(nextWord, 0, nextWordASText);
	ASInt32 wordLen = 0;
	char * nextWordChar = ASTextGetPDTextCopy(nextWordASText, &wordLen);
	if (strlen(nextWordChar) < strlen(target))
		return false;
	for (unsigned int index = 0; index < strlen(target); index++)
		if (nextWordChar[index] != target[index])
			return false;

	ASfree(nextWordChar);
	return true;
}