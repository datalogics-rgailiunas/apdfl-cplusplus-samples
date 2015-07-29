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

//Convert a wide string to an ASText object.
ASText toASText(const wchar_t* string);

int main(int argc, char** argv)
{
    APDFLib lib;                                                  //Initialize the Adobe PDF Library.

    if (lib.isValid() == false)                                   //If it failed to initialize, return the error code.
        return lib.getInitError();

    wchar_t* inputPath  = L"../_Input/Ulysses.pdf";               //Input PDF path.
    wchar_t* outputPath = L"Bookmarked.pdf";                      //Output path we'll save to.

    ASErrorCode errCode = 0;                                      //Tracks runtime errors in the application

    DURING

    std::wcout << L"Opening the input document." << std::endl;
    APDFLDoc APDoc(inputPath, true);
    PDDoc mydoc = APDoc.getPDDoc();

//=====================================================================================================================================================================================
//Step 1) Find each bolded subheading in the document and record their
//        location and text.
//=====================================================================================================================================================================================

    //These vectors store the needed information about the bolded subheadings we will search for.
    //These will all be the same size, and for each, the index n will indicate information for the
    //nth bookmark, in order of appearance.
    std::vector<ASFixedRect> bsLocts;            //bsLocts[n] = The location of the nth subheading on its page.
    std::vector<ASInt16> bsPages;                //bsPages[n] = The page number the nth subheading appeared on.
    std::vector<std::wstring> bsTexts;           //bsTexts[n] = The text of the nth subheading.

    //Search iteration variables.
    const ASInt8 bufferSize = 100;               //For a buffer which will copy the bolded text.

    std::wcout << L"Searching for bolded text..." << std::endl;

    for (int page = 0; page < PDDocGetNumPages(mydoc); ++page)                                   //For each page...
    {
        PDPage nextPage = APDoc.getPage(page);
        PDEContent nextContent = PDPageAcquirePDEContent(APDoc.getPage(page), 0);          //We need the content of the page. That's where the text is!

        for (int elemCount = 0; elemCount < PDEContentGetNumElems(nextContent); ++elemCount)     //For each element in that page...
        {
            PDEElement nextElem = PDEContentGetElem(nextContent, elemCount);

            if (PDEObjectGetType((PDEObject)nextElem) == kPDEText)                               //For each text element among those elements...
            {
                //Determine where the bolded text is by iterating
                //through the text runs of our text object. The
                //bolded text may be made of several text runs and
                //bounded by non-bolded text runs, making iteration
                //necessary.
                PDEText nextText = reinterpret_cast<PDEText>(nextElem);

                bool previousBold = false;                                                       //Whether the previous text run was bold.
                std::wstring nextBCopy = L"";                                                    //Will store the full text of the subheading, once we've located it.

                for (int runCount = 0; runCount < PDETextGetNumRuns(nextText); ++runCount)       //For each text run in that text element...
                {
                    //Is this text run bold? If so, "Bold" is appended to the font's name.
                    PDEFont nextFont = PDETextGetFont(nextText, kPDETextRun, runCount);          //The font of the next text run.
                    PDEFontAttrs nextFontAttrs;                                                  //Will store the font's attributes.
                    PDEFontGetAttrs(nextFont, &nextFontAttrs, sizeof(nextFontAttrs));            //Get the font attributes!
                    std::string fontstr = ASAtomGetString(nextFontAttrs.name);                   //This is the name of the font.

                    if ((fontstr.find("Bold") != std::string::npos))                             //If the font's bold...
                    {
                        if (!previousBold)                                                       //This is the first text run of a bolded subheading.
                        {                                                                        //All the next bolded text runs comprise the text of the subheading.
                            //Store the location of the beginning of
                            //the subheading.
                            ASFixedRect* nextBSLoc = new ASFixedRect;
                            PDETextGetBBox(nextText, kPDETextRun, runCount, nextBSLoc);
                            bsLocts.push_back(*nextBSLoc);

                            bsPages.push_back((ASInt16)page);                                    //Store the page index this subheading was found on.

                            //Store the first part of the text of the
                            //subheading.
                            char buffer[bufferSize];                                             //This buffer will be set to the first part of the text of the subheading.
                            memset(buffer, '\0', bufferSize);
                            PDETextGetText(nextText, kPDETextRun, runCount, (ASUns8*)buffer);
                            wchar_t* nextTextFragment = toWide(buffer);
                            nextBCopy += nextTextFragment;                                       //nextBCopy is now one step closer to storing the full text of the subheading.
                            delete[] nextTextFragment;

                            previousBold = true;                                                 //For the next iteration, the previous text run will have been bold.
                        }
                        else
                        {
                            //This text run is NOT the first of a
                            //subheading. So just continue storing
                            //the text.
                            char buffer[bufferSize];
                            memset(buffer, L'\0', bufferSize);
                            PDETextGetText(nextText, kPDETextRun, runCount, (ASUns8*)buffer);
                            wchar_t* nextFragment = toWide(buffer);
                            nextBCopy += nextFragment;                                           //nextBCopy is now one step closer to storing the full text of the subheading.
                            delete[] nextFragment;
                        }
                    }
                    else
                    {
                        //We've found an unbolded text run...
                        if (previousBold)
                        {
                            //...Which ended the subheading. Now 
                            //we've captured the full bolded text,
                            //So we record it and prepare to 
                            //capture the next one.
                            bsTexts.push_back(nextBCopy.substr(0, nextBCopy.size() - 2));

                            nextBCopy = L"";                                                     //We don't know anything about the text of the next subheading!
                            previousBold = false;                                                //For the next iteration, the previous text run will not have been bold.
                        }
                        //An unbolded text run is irrelevant otherwise.
                    }
                }
            }
        }
        //Release the page and its contents. We acquire new ones in the next iteration.
        PDPageReleasePDEContent(nextPage, 0);
        PDPageRelease(nextPage);
    }

    std::wcout << L"I found " << bsLocts.size() << L" subheading:" << std::endl;                 //We could have used any of the vectors, not just bsLocts.

    for (std::wstring subheadingText : bsTexts)
        std::wcout << subheadingText << std::endl;

//=====================================================================================================================================================================================
//Step 2) Create a bookmark for each bolded section with this information.
//=====================================================================================================================================================================================

    std::wcout << L"Creating a bookmark for each subheading..." << std::endl;

    //We'll create each bookmark by iterating over the subheadings.
    //The bookmark's text will be the subheading's text,
    //And the bookmark's action will be to bring the reader to
    //the location of that subheading.

    PDBookmark bookMarkRoot = PDDocGetBookmarkRoot(mydoc);                               //Bookmarks are added to a document's bookmark root.

    for (int nextBM = 0; nextBM < bsTexts.size(); ++nextBM)
    {
        //Get the necessary information.
        PDPage nextPage = APDoc.getPage(bsPages[nextBM]);                          //Get the associated page.

        std::wstringstream nextTitleWSTR;                                                //Get the associated title.
        nextTitleWSTR << (nextBM + 1) << L" " << bsTexts[nextBM];
        ASText nextTitle = toASText(nextTitleWSTR.str().c_str());

        ASFixedRect* nextLocation = bsLocts.begin()._Ptr + nextBM;                       //Get the associated page location (the method we use requires a pointer).

        //Make the bookmark and set its action.
        PDBookmark nextbm = PDBookmarkAddNewChildASText(bookMarkRoot, nextTitle);        //Bookmarks must be created before their action is set.
        ASTextDestroy(nextTitle);                                                        //We don't need this anymore, the bookmark has its title.

        //We create a View Destination pointing to the location of the subheading, and then create an action
        //which will take the reader to that destination.
        PDViewDestination nextDestination = PDViewDestCreate(mydoc, nextPage,
                                                             ASAtomFromString("XYZ"),    //View Destination Fit Type
                                                             nextLocation,               //Pointer to the location rectangle we want.
                                                             Int16ToFixed(0),            //Zoom factor. 0 means to inherit the current zoom factor
                                                             0);                         //Unused argument

        PDAction nextDestAct = PDActionNewFromDest(mydoc,nextDestination,mydoc);         //The first and third arguments are the source PDDoc and the destination PDDoc, respectively.
                                                                                         //They must the the same.
        PDBookmarkSetAction(nextbm, nextDestAct);                                        //Give the bookmark its action!
        PDPageRelease(nextPage);                                                         //Prepare to get the next bookmark's page. (It may end up being the same page.)
    }
    std::wcout << L"Done." << std::endl;

//=====================================================================================================================================================================================
//Step 3) Demonstrate different zoom levels.
//=====================================================================================================================================================================================

    std::wcout << L"Adding zoom demonstration bookmarks." << std::endl;

    //This steps adds a few children bookmarks to the first bookmark of the
    //document, which all copy that bookmark at different zoom levels.

    PDBookmark parentBm = PDBookmarkGetFirstChild(PDDocGetBookmarkRoot(mydoc));                     //The bookmark we'll add children to. (The first bookmark.)
    PDBookmark zoom100  = PDBookmarkAddNewChild(parentBm, "100% Zoom");
    PDBookmark zoom200  = PDBookmarkAddNewChild(parentBm, "200% Zoom");
    PDBookmark zoom800  = PDBookmarkAddNewChild(parentBm, "800% Zoom");
    PDBookmark zoom40   = PDBookmarkAddNewChild(parentBm, "40% Zoom");

    ASInt8 numBookmarks = 4;
    PDBookmark bookmarks[] {zoom100,      zoom200,      zoom800,      zoom40};
    ASFloat zoomfactors[]  {(ASFloat)1.0, (ASFloat)2.0, (ASFloat)8.0, (ASFloat)0.40};

    //Copy the attributes of the parent bookmark.
    ASInt32 pageNumber;                                                                             //The page index of the first bookmark.
    ASAtom fitType;                                                                                 //The first bookmark's view destination fit type.
    ASFixedRect locationRect;                                                                       //The location rectangle of the first bookmark.
    ASFixed zoomFactor;                                                                             //The first bookmark's zoom factor (we won't be using this).
    PDViewDestination parentViewDestination = PDActionGetDest(PDBookmarkGetAction(parentBm));

    PDViewDestGetAttr(parentViewDestination, &pageNumber, &fitType, &locationRect, &zoomFactor);

    //Set each bookmark's view destination per the above array.
    PDPage parentPage = APDoc.getPage(pageNumber);                                            //The page of the parent bookmark.
    for (int i = 0; i < numBookmarks; ++i)
    {
        PDViewDestination nextView = PDViewDestCreate(mydoc, parentPage, fitType,&locationRect, ASFloatToFixed(zoomfactors[i]), 0);
        PDAction nextAction = PDActionNewFromDest(mydoc, nextView, mydoc);
        PDBookmarkSetAction(bookmarks[i],nextAction);
    }

    PDPageRelease(parentPage);

    std::wcout << L"Done. Saving and closing the document." << std::endl;

//=====================================================================================================================================================================================
//Step 5) Save and close the document.
//=====================================================================================================================================================================================

    APDoc.saveDoc(outputPath);

    std::wcout << L"Success!" << std::endl;

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);             //If there was an error, display it.

    END_HANDLER

    return errCode;
};

//=====================================================================================================================================================================================
//wchar_t* function: Converts a char string to a wchar_t string.
//=====================================================================================================================================================================================
wchar_t* toWide(const char* str)
{
    const size_t strlen = (std::strlen(str)) + 1;
    wchar_t* wstr = new wchar_t[strlen];
    mbstowcs(wstr, str, strlen);
    return wstr;
};

//=====================================================================================================================================================================================
//ASText function: Convert a wide string to an ASText object.
//=====================================================================================================================================================================================
ASText toASText(const wchar_t* string)
{
    ASUnicodeFormat hostUniFormat;

    DURING

    if (sizeof(wchar_t) == 2)
        hostUniFormat = kUTF16HostEndian;
    else
        hostUniFormat = kUTF32HostEndian;

    return ASTextFromUnicode((ASUTF16Val *)string, hostUniFormat);

    HANDLER

        ASRaise(ERRORCODE);  //If there was an exception, let the caller handle it.

    END_HANDLER

    return NULL;
};
