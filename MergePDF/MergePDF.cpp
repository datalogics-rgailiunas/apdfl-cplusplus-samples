//# Copyright(c) 2015, Datalogics, Inc.All rights reserved.

//************************************************************************
// Sample: MergePDF: Opens two documents, merges them and saves the
// resulting document in the working directory.
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

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

int main(int argc, char** argv)
{
    APDFLib libInit;                                               //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;                                       //Variable used to report any exceptions/errors if they occured.

    if (libInit.isValid() == false)                                //If there was a problem in initialization, return the error code.
        return libInit.getInitError(); 

    DURING  

        std::wcout << L"Opening input files..." << std::endl;

        APDFLDoc doc1(L"../_Input/merge1.pdf", true);              //Open both of the documents that will be merged together.
        APDFLDoc doc2(L"../_Input/merge2.pdf", true);

        std::wcout << L"Inserting doc2's pages into doc1..." << std::endl;

        //Insert doc2's pages into doc1. If PDBeforeFirstPage is used doc2's pages will be inserted into doc1's.
        PDDocInsertPages(doc1.getPDDoc(), PDLastPage, doc2.getPDDoc(), 0, PDAllPages, PDInsertAll, NULL, NULL, NULL, NULL);

        std::wcout << L"Saving the output file in the working directory..." << std::endl;

        doc1.saveDoc(L"out.pdf", PDSaveFull | PDSaveLinearized);    //Save the output file as out.pdf in the working directory.

    HANDLER                             

        errCode = ERRORCODE;

        libInit.displayError(errCode);                              //If there was an error, display the error that occured.

    END_HANDLER

    return errCode;                                                 //APDFLib's destructor terminates the APDFL.                         
}
