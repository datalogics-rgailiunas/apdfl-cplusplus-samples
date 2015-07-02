//# Copyright(c) 2015, Datalogics, Inc.All rights reserved.
//
// Sample: Bookmarks / Adds a number of bookmarks to the input sample.
//                     It adds a bookmark wherever a bolded string of text occurs.
//                     The sample also demonstrates different zoom factors for
//                     a bookmark.
//
//# This agreement is between Datalogics, Inc. 101 N.Wacker Drive, Suite 1800,
//# Chicago, IL 60606 ("Datalogics") and you, an end user who downloads
//# source code examples for integrating to the Adobe PDF Library
//# ("the Example Code"). By accepting this agreement you agree to be bound
//# by the following terms of use for the Example Code.
//#
//# LICENSE
//# -------
//# Datalogics hereby grants you a royalty - free, non - exclusive license to
//# download and use the Example Code for any lawful purpose.There is no charge
//# for use of Example Code.
//#
//# OWNERSHIP
//# ---------
//# The Example Code and any related documentation and trademarks are and shall
//# remain the sole and exclusive property of Datalogics and are protected by
//# the laws of copyright in the U.S.and other countries.
//#
//# Datalogics is a trademark of Datalogics, Inc.
//#
//# TERM
//# ----
//# This license is effective until terminated.You may terminate it at any
//# other time by destroying the Example Code.
//#
//# WARRANTY DISCLAIMER
//# -------------------
//# THE EXAMPLE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
//# EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES
//# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//#
//# DATALOGICS DISCLAIM ALL OTHER WARRANTIES, CONDITIONS, UNDERTAKINGS OR
//# TERMS OF ANY KIND, EXPRESS OR IMPLIED, WRITTEN OR ORAL, BY OPERATION OF
//# LAW, ARISING BY STATUTE, COURSE OF DEALING, USAGE OF TRADE OR OTHERWISE,
//# INCLUDING, WARRANTIES OR CONDITIONS OF MERCHANTABILITY, FITNESS FOR A
//# PARTICULAR PURPOSE, SATISFACTORY QUALITY, LACK OF VIRUSES, TITLE,
//# NON - INFRINGEMENT, ACCURACY OR COMPLETENESS OF RESPONSES, RESULTS, AND / OR
//# LACK OF WORKMANLIKE EFFORT.THE PROVISIONS OF THIS SECTION SET FORTH
//# SUBLICENSEE'S SOLE REMEDY AND DATALOGICS'S SOLE LIABILITY WITH RESPECT
//# TO THE WARRANTY SET FORTH HEREIN.NO REPRESENTATION OR OTHER AFFIRMATION
//# OF FACT, INCLUDING STATEMENTS REGARDING PERFORMANCE OF THE EXAMPLE CODE,
//# WHICH IS NOT CONTAINED IN THIS AGREEMENT, SHALL BE BINDING ON DATALOGICS.
//# NEITHER DATALOGICS WARRANT AGAINST ANY BUG, ERROR, OMISSION, DEFECT,
//# DEFICIENCY, OR NONCONFORMITY IN ANY EXAMPLE CODE.

//Sample includes
#include "SampleUtilities.h"

//PDFL includes
#include "PERCalls.h"
#include "PagePDECntCalls.h"

//STD includes
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char* argv){

    Utilities util;            //Performs some common functions

    //Initialize library and sample variables.
    int  err = util.initPDFL(); //Will display errors.
    if (err) return err;

    ASErrorCode errCode = 0;  //Tracks runtime errors in the application
    PDDoc mydoc = NULL;       //Reference to input pdf document

    //[S] Bolded heading search iteration variables (defined here to release after DURING/HEADER block.
    //see matching comment below.)
    PDPage nextPage;               //Iterates over each PDPage
    PDEContent nextContent;        //Iterates over each PDEContent per PDPage
    PDEText nextText;              //Iterates over each text object per PDEContent
    PDEElement nextElem;           //Iterates over the elements of each text object
    PDEFont nextFont;              //The font of the next run of text
    ASFixedRect* nextTextLoc;      //The location of the bolded text

    //[B] Bookmark creation iteration variables. (defined here to release after DURING/HEADER block.
    //see matching comment below.)
    PDPage nextDPage = NULL;        //The associated page of the bookmark destination.


    DURING

    std::wcout << L"Opening the input document." << std::endl;
    mydoc = util.openPDFNoSecurity(L"../Input/NoBookmark.pdf");

    //==================================================================
    //Step 1) Find each section of bolded text in the document and 
    //        record their location and text.
    //==================================================================

    //These vectors store all needed information about the bolded headings.
    //The job of the search is to fill them.
    //The same index indicates the same section of text.
    //So each will have the same size after the search.
    std::vector<ASFixedRect> headingLocs;     //The location of each heading
    std::vector<ASInt16> headingPages;            //The associated page of each heading.
    std::vector<std::wstring> headingTitles;   //The text of each heaading

    //[S] Bolded heading search iteration variables (see matching comment before DURING)
    ASInt16 numPages = PDDocGetNumPages(mydoc); //Number of pages in document
    #define BUFFERSIZE 100                      //For a buffer which reads the bolded text
    PDEFontAttrs nextFontAttrs;                //The font attributes of the next text run's font

    std::wcout << L"Searching for bolded headings..." << std::endl;
    for (auto i = 0; i < numPages; ++i){ //For each page...

        //Acquire its elements
        nextPage = PDDocAcquirePage(mydoc, i);
        nextContent = PDPageAcquirePDEContent(nextPage, 0);
        ASInt32 numElem = PDEContentGetNumElems(nextContent); //num elements on a page

        for (auto elemCount = 0; elemCount < numElem; ++elemCount){ //For each element in that page...

            //Acquire the text
            nextElem = PDEContentGetElem(nextContent, elemCount);
            
            if (PDEObjectGetType((PDEObject)nextElem) == kPDEText){ //For each text element among those elements...
                
                //Determine where the bolded parts are by iterating through the text runs
                //The bolded header may be made of several text runs, making iteration necessary.
                nextText = reinterpret_cast<PDEText>(nextElem);
                int numRuns = PDETextGetNumRuns(nextText);

                bool currentlyBold = false; //Whether the previous text run was bold
                std::wstring nextHeader = L""; //Stores the bolded header, when found.

                for (auto runCount = 0; runCount < numRuns; ++runCount){ //For each text run in that text element...

                    //Get its font attributes. If bold, we've found a header, as per our assumptions
                    //about the input.
                    nextFont = PDETextGetFont(nextText, kPDETextRun, runCount);
                    PDEFontGetAttrs(nextFont, &nextFontAttrs, sizeof(nextFontAttrs));

                    std::string fontstr = ASAtomGetString(nextFontAttrs.name); //The font name

                    if ((fontstr.find("Bold") != std::string::npos)){ //If the font is bold
                        //We've found a bolded text run.

                        if (!currentlyBold){
                            //This is the first text run of the bolded header.
                            //We'll want the location of this run to be the destination of our bookmark.

                            //Retrieve the location and store it
                            nextTextLoc = new ASFixedRect;
                            PDETextGetBBox(nextText, kPDETextRun, runCount, nextTextLoc); //This gets the location of the text on the page.
                            headingLocs.push_back(*nextTextLoc);

                            //Store the page index this bookmark was found on.
                            headingPages.push_back((ASInt16)i);

                            //Begin storing the text of the header.
                            char buffer[BUFFERSIZE];
                            memset(buffer, '\0', BUFFERSIZE);
                            PDETextGetText(nextText, kPDETextRun, runCount, (ASUns8*)buffer);

                            //convert to wchar_t

                            nextHeader += util.toWide(buffer);

                            //Update the iteration state
                            currentlyBold = true;
                        }
                        else{
                            //This text run is NOT the first of a bolded header,
                            //but some part of that bolded header.

                            //Just continue storing the text of the header.
                            char buffer[BUFFERSIZE];
                            memset(buffer, L'\0', BUFFERSIZE);
                            PDETextGetText(nextText, kPDETextRun, runCount, (ASUns8*)buffer);
                            nextHeader += util.toWide(buffer);
                        }
                    }
                    else{
                        //We've found an unbolded text run...

                        if (currentlyBold){
                            //...Which ended the bolded header.
                            //Now we know we've captured the full text of the header,
                            //So we record it and prepare to capture the next one.
                            headingTitles.push_back(nextHeader.substr(0,nextHeader.size()-2)); //Remove the single trailing space
                            nextHeader = L"";
                            currentlyBold = false;
                        }
                        //An unbolded text run is irrelevant otherwise.
                    }
                }
            }
        }
        //Release the page and its content. We acquire new ones in the next iteration.
        PDPageRelease(nextPage);
        PDPageReleasePDEContent(nextPage, 0);
    }

    std::wcout << L"I found " << headingLocs.size() << L" bolded headings:" << std::endl;
    for (auto x : headingTitles){
        std::wcout << x << std::endl;
    }
#undef BUFFERSIZE

    //==================================================================
    //Step 2) Create a bookmark for each bolded section, based off
    //        the location/text we just recorded
    //==================================================================
    std::wcout << L"Creating a bookmark for each..." << std::endl;

    //We'll create each bookmark by iterating over the headings we found.

    auto numHeadings = headingTitles.size(); //We could have also used headingLocations.size() or headingTitles.size()
    PDBookmark bmRoot = PDDocGetBookmarkRoot(mydoc);    //The root of the document's bookmark tree.
    
    //[B] Bookmark creation iteration variables. (see matching comment before DURING.)
    PDBookmark nextbm;              //Next bookmark to create
    std::wstringstream nextbmTitle;  //The title for the next bookmark
    PDAction nextDest;              //The destination for the next bookmark

    //Create a bookmark for each heading
    for (int heading = 0; heading < numHeadings; ++heading){
        //Make an associated bookmark for each heading.

        //Get the associated page
        nextDPage = PDDocAcquirePage(mydoc, headingPages[heading]);
        //Construct the title
        nextbmTitle << (heading+1) << L" " << headingTitles[heading];
        //Create the bookmark before setting its action
        nextbm = PDBookmarkAddNewChildASText(
            bmRoot, util.toASText(nextbmTitle.str().c_str()));

        //Create the view destination action for the bookmark
        nextDest = PDActionNewFromDest(
                    mydoc, //The associated document
                    PDViewDestCreate(
                        mydoc,                      //Associated document
                        nextDPage,                  //Destination page
                        ASAtomFromString("XYZ"),    //View Destination Fit Type
                        &(headingLocs[heading]),    //Location rectangle
                        Int16ToFixed(0),            //Zoom factor. 0 means to inherit the current zoom factor.
                        0),                         //Unused argument
                    mydoc); //The destination document. must be the same as the first argument.

        PDBookmarkSetAction(nextbm, nextDest); //Set the action

        PDPageRelease(nextDPage);
        nextbmTitle.str(std::wstring()); //Clear the stringstream for the next title
    }
    std::wcout << L"Done." << std::endl;

    //==================================================================
    //Step 3) Demonstration: Different zoom levels
    //==================================================================
    std::wcout << L"Adding zoom demonstration bookmarks." << std::endl;

    //This steps adds a few child bookmarks to the first bookmark of the
    //document, which all copy that bookmark at different zoom levels.

    PDBookmark parentBm = PDBookmarkGetFirstChild(PDDocGetBookmarkRoot(mydoc));
    PDBookmark zoom100 = PDBookmarkAddNewChild(parentBm, "100% Zoom");
    PDBookmark zoom200 = PDBookmarkAddNewChild(parentBm, "200% Zoom");
    PDBookmark zoom800 = PDBookmarkAddNewChild(parentBm, "800% Zoom");
    PDBookmark zoom40 = PDBookmarkAddNewChild(parentBm, "40% Zoom");

    ASInt8 num_bookmarks = 4;
    PDBookmark bookmarks[] { zoom100, zoom200, zoom800, zoom40 };
    ASFloat zoomfactors[]  { 1, 2, 8, (ASFloat)0.40 };

    //Copy the attributes of the parent bookmark
    ASInt32 pageNumber;
    ASAtom fitType;
    ASFixedRect locationRect;
    ASFixed zoomFactor;
    PDViewDestGetAttr(
        PDActionGetDest(PDBookmarkGetAction(parentBm)),
        &pageNumber, &fitType, &locationRect, &zoomFactor);
    PDPage parentPage = PDDocAcquirePage(mydoc, pageNumber);

    //Set each bookmark's zoom factor per the above array.
    for (auto i = 0; i < num_bookmarks; ++i){
        PDBookmarkSetAction(bookmarks[i],
            PDActionNewFromDest(
                mydoc, PDViewDestCreate(mydoc, parentPage, fitType, &locationRect, ASFloatToFixed(zoomfactors[i]), 0), mydoc));
    }
    PDPageRelease(parentPage);

    std::wcout << L"Done. Saving and closing the document." << std::endl;

    //==================================================================
    //Step 5) Save and close the document.
    //==================================================================

    PDDocSave(
        mydoc,                                           //Document to save
        PDDocNeedsSave | PDDocIsOpen                     //PDDocSaveFlags
         | PDSaveCopy,
         util.makeASPathName(L"../Input/Bookmark.pdf"),    //ASPath to save to
        ASGetDefaultFileSys(),                           //The file system
        NULL, NULL);                                     //No ASProgressMonitor
        
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
    if (nextText) PDERelease(reinterpret_cast<PDEObject>(nextText));
    if (nextElem) PDERelease(reinterpret_cast<PDEObject>(nextElem));
    if (nextTextLoc) PDERelease(reinterpret_cast<PDEObject>(nextTextLoc));
    if (nextDPage) PDPageRelease(nextDPage);
    if (nextFont) PDERelease(reinterpret_cast<PDEObject>(nextFont));
    if (mydoc) PDDocRelease(mydoc);

    //Terminate the library
    MyPDFLTerm();

    //End
    return errCode;
};
