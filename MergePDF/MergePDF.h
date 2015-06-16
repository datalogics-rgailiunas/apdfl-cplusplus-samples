
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
//# DEFICIENCY, OR NONCONFORMITY IN ANY EXAMPLE CODE. */

#ifndef MERGEPDF_H
#define MERGEPDF_H

#include <iostream>
#include <string>
#include "MyPDFLibUtils.h"
#include "ASCalls.h"
#include "ASExtraCalls.h"

class OpenAndMerge {
    private:
        //PDF OpenAndMerge
        volatile PDDoc pdDoc;
        //Path name used to open the OpenAndMerge
        volatile ASPathName asPathName;
        //Path name that is passed to contructor
        wchar_t * pathName;
        //Attempt to repair damaged file in PDDOCOpen
        ASBool doRepairDamagedFile;
        //Path name used to save output file
        volatile ASPathName asOutPathName;
        //Helper method sets ASPathName
        void setASPathName(wchar_t *, volatile ASPathName&);
        
    public:
        //Default constructor
        OpenAndMerge();
        //Constructor used to open PDDoc
        OpenAndMerge(wchar_t *);
        //Method in charge of merging passed OpenAndMerge.
        bool merge(OpenAndMerge &, wchar_t *);
        //Releases resources
        void closeAndRelease();
};

#endif