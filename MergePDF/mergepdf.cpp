//# Copyright(c) 2015, Datalogics, Inc.All rights reserved.

//************************************************************************
// Sample: MergePDF - Open and merge two PDF documents
//
// Note: This program opens two files called mergepdf1.pdf
// 	  mergepdf2.pdf in the _Data directory. It merges these two files and outputs
// 	  the resulting file to "out.pdf" in the working directory.
//	
//MergePDF.h contains OpenAndMerge class definition
//mergePDFAPP.cpp contains the driver
//MergePDF.cpp contains method implementations
//Steps:
// 1) Open two PDF OpenAndMerges
// 2) Insert pdDoc2 to pdDoc1
// 3) Save pdDoc1 as out.pdf and release resources
//************************************************************************

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
#ifndef MERGEPDF_APP
#define MERGEPDF_APP

#include "mergePDF.h"
//==================================================================
//Default constructor
//==================================================================
OpenAndMerge::OpenAndMerge()
{
    //Initialize data members to NULL values
    pdDoc = NULL;
    pathName = L"";
    doRepairDamagedFile = true;
    asPathName = NULL;
    asOutPathName = NULL; 
}
//==================================================================
//Constructor for OpenAndMerge Class:
//Opens a PDF OpenAndMerge and checks to see that the open was sucessful
//==================================================================
OpenAndMerge::OpenAndMerge(wchar_t * newPathName)
{
    //Bool used by PDDocOpen
    doRepairDamagedFile = true;
    pdDoc = NULL;
    //Copy passed name into data member
    pathName = newPathName;

    DURING
        //Set the path name
        setASPathName(pathName, asPathName);

        //Open pdDoc from asPathName.
        pdDoc = PDDocOpen(asPathName, NULL, NULL, doRepairDamagedFile);

        //Check to see if OpenAndMerge opened successfully and print message
        if (pdDoc != NULL)
            std::wcout << "Successfully opened " << pathName << "!" << std::endl;
		else
		{
			std::wcerr << "Failed to open " << pathName << "." << std::endl;
			exit(EXIT_FAILURE);
		}

    HANDLER
        //If there was an exception generate an error code
        ASErrorCode errCode = ERRORCODE;

        char buf[256];

        ASGetErrorString(ERRORCODE, buf, sizeof(buf));

        //Print out error code
        std::cerr << "Error Code: " << errCode << "Error Message: " << buf;
    END_HANDLER

    return;
}

//==================================================================
//This method merges the argument passed in and returns true on success
//returns false if the documents were not merged.
//==================================================================
bool OpenAndMerge::merge(OpenAndMerge &docToMerge, wchar_t * outputFileName)
{
    if (this->pdDoc != NULL && docToMerge.pdDoc != NULL)
    {
        DURING
            //Merge the OpenAndMerges
            PDDocInsertPages(this->pdDoc, PDLastPage, docToMerge.pdDoc, 0, PDAllPages, PDInsertAll, NULL, NULL, NULL, NULL);
            
            //Set the output path
            setASPathName(outputFileName, asOutPathName);

            //Save the newly merged PDF OpenAndMerge
            PDDocSave(this->pdDoc, PDSaveFull | PDSaveLinearized, asOutPathName, ASGetDefaultFileSys(), NULL, NULL);

        HANDLER
            //If there was an exception generate an error code
            ASErrorCode errCode = ERRORCODE;

            char buf[256];

            ASGetErrorString(ERRORCODE, buf, sizeof(buf));

            //Print out error code
            std::cerr << "Error Code: " << errCode << "Error Message: " << buf;
        END_HANDLER

        //Return true on success
        return true;
    }
    else
    {
        //Failed to merge, return false
        return false;
    }
}
//==================================================================
//Private helper method: sets the ASPathName 
//==================================================================
void OpenAndMerge::setASPathName(wchar_t * pathToCreate, volatile ASPathName & asPathToModify)
{
    //Text object to create ASPathName
    ASText textToCreatePath = NULL;

    DURING
        //Determine size of wchar_t on system and get the ASText
        if (sizeof(wchar_t) == 2)
            textToCreatePath = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*> (pathToCreate), kUTF16HostEndian);
        else
            textToCreatePath = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(pathToCreate), kUTF32HostEndian);

        //Create the path for output file
        asPathToModify = ASFileSysCreatePathFromDIPathText(NULL, textToCreatePath, NULL);

    HANDLER
        //If there was an exception generate an error code
        ASErrorCode errCode = ERRORCODE;

        char buf[256];

        ASGetErrorString(ERRORCODE, buf, sizeof(buf));

        //Print out error code
        std::cerr << "Error Code: " << errCode << "Error Message: " << buf;
    END_HANDLER

    //Release text object
    ASTextDestroy(textToCreatePath);
    
    return;
}
//==================================================================
//Closes the PDDoc and releases path
//==================================================================
void OpenAndMerge::closeAndRelease()
{
    //Release PDDoc object 
    PDDocClose(pdDoc);

    //Release ASPathName objects 
    ASFileSysReleasePath(NULL, asPathName);
    ASFileSysReleasePath(NULL, asOutPathName);

    //Print Message upon release 
    std::cout << "PDDoc and ASPathNames have been released. " << std::endl;

    return;
}

#endif