// Copyright(c) 2015, Datalogics, Inc.All rights reserved.

//************************************************************************
// Sample: Bookmarks - Adds some bookmarks to the input pdf
//
// This sample adds a bookmark wherever a bolded section of text occurs
// in the input document. The sample also adds a few bookmarks which
// demonstrate different zoom factors for bookmarks.
//
//Steps:
// 1) Find each section of bolded text in the document and 
//    record their location and copy.
// 2) Create a bookmark for each bolded section with this information.
// 3) Demonstration: Different zoom levels.
// 4) Save and close the document.
//************************************************************************

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

#include "SampleUtilities.h"
#include "PERCalls.h"
#include "PagePDECntCalls.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char* argv)
{
    Utilities util;                 //Performs some common functions

    //Initialize library and sample variables.
    int  err = util.initPDFL();     //Will display errors.
    if (err) return err;

    ASErrorCode errCode = 0;        //Tracks runtime errors in the application
    PDDoc mydoc = NULL;             //Reference to input pdf document

    //[S] Bolded text search iteration variables. Defined here to release after DURING/HANDLER.
    //see matching comment below.
    PDPage nextPage;                //Iterates over each PDPage
    PDEContent nextContent;         //Iterates over each PDEContent per PDPage
    PDEText nextText;               //Iterates over each text object per PDEContent
    PDEElement nextElem;            //Iterates over the elements of each text object
    PDEFont nextFont;               //The font of the next run of text
    ASFixedRect* nextTextLoc;       //The location of the bolded text

    //[B] Bookmark creation iteration variables. Defined here to release after DURING/HANDLER.
    //see matching comment below.
    PDPage nextDPage = NULL;        //The associated page of the bookmark destination.

    //[Z] Zoom Factor demonstration variables. Defined here to release after DURING/HANDLER.
    //see matching comment below.
    PDPage parentPage = NULL;

    DURING

    std::wcout << L"Opening the input document." << std::endl;
    mydoc = util.openPDFNoSecurity(L"../Input/NoBookmark.pdf");

    //==================================================================
    //Step 1) Find each section of bolded text in the document and 
    //        record their location and text.
    //==================================================================

    //These vectors store the needed information about the bolded text we will search for
    std::vector<ASFixedRect> btextLocs;            //The location of each text
    std::vector<ASInt16> btextPages;               //The associated page of each text.
    std::vector<std::wstring> btextCopy;           //The copy of each text

    //[S] Bolded text search iteration variables. See matching comment above.
    ASInt16 numPages = PDDocGetNumPages(mydoc);
    const ASInt8 bufferSize = 100;                 //For a buffer which reads the bolded text
    PDEFontAttrs nextFontAttrs;                    //The font attributes of the next text run's font

    std::wcout << L"Searching for bolded text..." << std::endl;
    for (auto page = 0; page < numPages; ++page)   //For each page...
    {
        nextPage = PDDocAcquirePage(mydoc, page);
        nextContent = PDPageAcquirePDEContent(nextPage, 0);
        ASInt32 numElem = PDEContentGetNumElems(nextContent);

        for (auto elemCount = 0; elemCount < numElem; ++elemCount)       //For each element in that page...
        {
            nextElem = PDEContentGetElem(nextContent, elemCount);

            if (PDEObjectGetType((PDEObject)nextElem) == kPDEText)       //For each text element among those elements...
            {
                //Determine where the bolded text is by iterating through the text runs
                //The bolded text may be made of several text runs, making iteration necessary.
                nextText = reinterpret_cast<PDEText>(nextElem);
                int numRuns = PDETextGetNumRuns(nextText);

                bool currentlyBold = false;                              //Whether the previous text run was bold
                std::wstring nextBCopy = L"";                            //Stores the bolded text, when found

                for (auto runCount = 0; runCount < numRuns; ++runCount)  //For each text run in that text element...
                {
                    //Check if it's bold
                    nextFont = PDETextGetFont(nextText, kPDETextRun, runCount);
                    PDEFontGetAttrs(nextFont, &nextFontAttrs, sizeof(nextFontAttrs));
                    std::string fontstr = ASAtomGetString(nextFontAttrs.name);

                    if ((fontstr.find("Bold") != std::string::npos))
                    {
                        if (!currentlyBold)                              //This is the first text run of a bolded text section.
                        {
                            //Retrieve the location and store it
                            nextTextLoc = new ASFixedRect;
                            PDETextGetBBox(nextText, kPDETextRun, runCount, nextTextLoc);
                            btextLocs.push_back(*nextTextLoc);

                            //Store the page index this bookmark was found on
                            btextPages.push_back((ASInt16)page);

                            //Begin storing the next bold text
                            char buffer[bufferSize];
                            memset(buffer, '\0', bufferSize);
                            PDETextGetText(nextText, kPDETextRun, runCount, (ASUns8*)buffer);
                            wchar_t* nextFragment = util.toWide(buffer);
                            nextBCopy += nextFragment;
                            delete[] nextFragment;

                            currentlyBold = true;
                        }
                        else
                        {
                            //This text run is NOT the first of a bold piece of text,
                            //but some part of that bolded text
                            //So just continue storing the text
                            char buffer[bufferSize];
                            memset(buffer, L'\0', bufferSize);
                            PDETextGetText(nextText, kPDETextRun, runCount, (ASUns8*)buffer);
                            wchar_t* nextFragment = util.toWide(buffer);
                            nextBCopy += nextFragment;
                            delete[] nextFragment;
                        }
                    }
                    else
                    {
                        //We've found an unbolded text run...
                        if (currentlyBold)    //...Which ended the bolded text.
                        {
                            //Now we've captured the full bolded text,
                            //So we record it and prepare to capture the next one
                            btextCopy.push_back(nextBCopy.substr(0, nextBCopy.size() - 2));
                            nextBCopy = L"";

                            currentlyBold = false;
                        }
                        //An unbolded text run is irrelevant otherwise.
                    }
                }
            }
        }
        //Release the page and its content. We acquire new ones in the next iteration
        PDPageReleasePDEContent(nextPage, 0);
        PDPageRelease(nextPage);
        nextPage = NULL;
        nextContent = NULL;
    }

    std::wcout << L"I found " << btextLocs.size() << L" bolded texts:" << std::endl;
    for (auto x : btextCopy)
        std::wcout << x << std::endl;

    //========================================================================
    //Step 2) Create a bookmark for each bolded section with this information.
    //========================================================================
    std::wcout << L"Creating a bookmark for each..." << std::endl;

    //We'll create each bookmark by iterating over the text we found.

    auto numBold = btextCopy.size();                               //The size of each vector is the same
    PDBookmark bmRoot = PDDocGetBookmarkRoot(mydoc);               //The root of the document's bookmark tree
    
    //[B] Bookmark creation iteration variables. See matching comment above.
    PDBookmark nextbm;                                             //Next bookmark to create
    std::wstringstream nextbmTitle;                                //The title for the next bookmark
    PDAction nextDestAct;                                          //The destination for the next bookmark

    //Create a bookmark for each bold text
    for (int btext = 0; btext < numBold; ++btext)
    {
        nextDPage = PDDocAcquirePage(mydoc, btextPages[btext]);    //Get the associated page
        nextbmTitle << (btext + 1) << L" " << btextCopy[btext];    //Construct the title
        nextbm = PDBookmarkAddNewChildASText(                      //Create the bookmark before setting its action
            bmRoot, util.toASText(nextbmTitle.str().c_str()));

        nextDestAct = PDActionNewFromDest(                         //Create the view destination action for the bookmark
                    mydoc,                                         //The associated document
                    PDViewDestCreate(mydoc,nextDPage,
                        ASAtomFromString("XYZ"),                   //View Destination Fit Type
                        &(btextLocs[btext]),                       //Location rectangle
                        Int16ToFixed(0),                           //Zoom factor. 0 means to inherit the current zoom factor
                        0),                                        //Unused argument
                    mydoc);                                        //The destination document. must be the same as the first argument

        PDBookmarkSetAction(nextbm, nextDestAct);
        PDPageRelease(nextDPage);
        nextDPage = NULL;

        nextbmTitle.str(std::wstring());                           //Clear the stringstream for the next text
    }
    std::wcout << L"Done." << std::endl;

    //==================================================================
    //Step 3) Demonstration: Different zoom levels
    //==================================================================
    std::wcout << L"Adding zoom demonstration bookmarks." << std::endl;

    //This steps adds a few child bookmarks to the first bookmark of the
    //document, which all copy that bookmark at different zoom levels.

    //[Z] Zoom Factor demonstration variables. See matching comment above.
    PDBookmark parentBm = PDBookmarkGetFirstChild(PDDocGetBookmarkRoot(mydoc));
    PDBookmark zoom100  = PDBookmarkAddNewChild(parentBm, "100% Zoom");
    PDBookmark zoom200  = PDBookmarkAddNewChild(parentBm, "200% Zoom");
    PDBookmark zoom800  = PDBookmarkAddNewChild(parentBm, "800% Zoom");
    PDBookmark zoom40   = PDBookmarkAddNewChild(parentBm, "40% Zoom");

    ASInt8 num_bookmarks = 4;
    PDBookmark bookmarks[] { zoom100, zoom200, zoom800, zoom40 };
    ASFloat zoomfactors[]  { 1, 2, 8, (ASFloat)0.40 };

    ASInt32 pageNumber;
    ASAtom fitType;
    ASFixedRect locationRect;
    ASFixed zoomFactor;

    PDViewDestGetAttr(    //Copy the attributes of the parent bookmark
        PDActionGetDest(PDBookmarkGetAction(parentBm)), &pageNumber, &fitType, &locationRect, &zoomFactor);
    parentPage = PDDocAcquirePage(mydoc, pageNumber);

    //Set each bookmark's zoom factor per the above array
    for (auto i = 0; i < num_bookmarks; ++i)
    {
        PDBookmarkSetAction(bookmarks[i],
            PDActionNewFromDest(
                mydoc, PDViewDestCreate(mydoc, parentPage, fitType, &locationRect, ASFloatToFixed(zoomfactors[i]), 0), mydoc));
    }
    PDPageRelease(parentPage);
    parentPage = NULL;

    std::wcout << L"Done. Saving and closing the document." << std::endl;

    //==================================================================
    //Step 5) Save and close the document.
    //==================================================================

    PDDocSave(mydoc,PDDocNeedsSave | PDDocIsOpen | PDSaveCopy, 
        util.makeASPathName(L"../Input/Bookmark.pdf"), ASGetDefaultFileSys(), NULL, NULL);
        
    PDDocClose(mydoc);

    HANDLER

        errCode = ERRORCODE;

    END_HANDLER

    //Display errors, if any
    if (errCode)
        DisplayError(errCode);
    else
        std::wcout << L"Success." << std::endl;

    //Release resources
    if (nextPage) 
    {
        if (nextContent) PDPageReleasePDEContent(nextPage,0);
        PDPageRelease(nextPage);
    }
    if (nextDPage)   PDPageRelease(nextDPage);
    if (parentPage)  PDPageRelease(parentPage);
    if (mydoc)       PDDocRelease(mydoc);

    MyPDFLTerm();      //Terminate the library
    return errCode;    //End
};
