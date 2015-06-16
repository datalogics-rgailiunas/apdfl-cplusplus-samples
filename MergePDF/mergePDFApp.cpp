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

#include "MergePDF.h"

int main (int argc, char **argv)
{
   //Initialize PDFL 
    int  err = MyPDFLInit();
    //Check for errors in initialization 
    if (err != 0)
    {
        std::cerr << "Initialization error. See \"AcroErr.h\" for more info.\n" << std::endl;
        std::cerr << "Error system: " << ErrGetSystem(err) << std::endl;
        std::cerr << "Error Severity: " << ErrGetSeverity(err) << std::endl;
        std::cerr << "Error Code: " << ErrGetCode(err) << std::endl;
        return 0;
    }

    //Paths passed to OpenAndMerge class constructor
    wchar_t * pathToFile1 = L"../_Data/mergepdf1.pdf\0";
    wchar_t * pathToFile2 = L"../_Data/mergepdf2.pdf\0";
    wchar_t * outputFile = L"out.pdf\0";

//==================================================================
//Step 1) Constructor opens OpenAndMerges and performs error handling
//==================================================================

    OpenAndMerge pdDoc1 (pathToFile1);
    OpenAndMerge pdDoc2 (pathToFile2);

//==================================================================
//Step 2) Insert pdDoc2 to the end of pdDoc1 and save PdDoc1 as out.pdf
//==================================================================

    bool didMerge = pdDoc1.merge(pdDoc2, outputFile);
    //Check to see if merge was successful and print message 
    if (didMerge)
        std::wcout << "Saved the output file " << outputFile << " in the working directory." << std::endl;
    else
        std::cerr << "Failed to merge files." << std::endl;
//==================================================================
//Step 3) Close and release resources
//==================================================================

    pdDoc1.closeAndRelease();
    pdDoc2.closeAndRelease();

    //Close PDFL
    MyPDFLTerm();
}


