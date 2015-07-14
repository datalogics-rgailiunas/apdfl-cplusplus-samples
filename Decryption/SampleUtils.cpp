// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// Sample addAttachment / Performs several utility functions for the sample.
//
// This agreement is between Datalogics, Inc. 101 N. Wacker Drive, Suite 1800,
// Chicago, IL 60606 ("Datalogics") and you, an end user who downloads
// source code examples for integrating to the Adobe PDF Library
// ("the Example Code"). By accepting this agreement you agree to be bound
// by the following terms of use for the Example Code.
//
// LICENSE
// -------
// Datalogics hereby grants you a royalty-free, non-exclusive license to
// download and use the Example Code for any lawful purpose. There is no charge
// for use of Example Code.
//
// OWNERSHIP
// ---------
// The Example Code and any related documentation and trademarks are and shall
// remain the sole and exclusive property of Datalogics and are protected by
// the laws of copyright in the U.S. and other countries.
//
// Datalogics is a trademark of Datalogics, Inc.
//
// TERM
// ----
// This license is effective until terminated. You may terminate it at any
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
// NON-INFRINGEMENT, ACCURACY OR COMPLETENESS OF RESPONSES, RESULTS, AND/OR
// LACK OF WORKMANLIKE EFFORT. THE PROVISIONS OF THIS SECTION SET FORTH
// SUBLICENSEE'S SOLE REMEDY AND DATALOGICS'S SOLE LIABILITY WITH RESPECT
// TO THE WARRANTY SET FORTH HEREIN. NO REPRESENTATION OR OTHER AFFIRMATION
// OF FACT, INCLUDING STATEMENTS REGARDING PERFORMANCE OF THE EXAMPLE CODE,
// WHICH IS NOT CONTAINED IN THIS AGREEMENT, SHALL BE BINDING ON DATALOGICS.
// NEITHER DATALOGICS WARRANT AGAINST ANY BUG, ERROR, OMISSION, DEFECT,
// DEFICIENCY, OR NONCONFORMITY IN ANY EXAMPLE CODE.

#include "SampleUtils.h"

Utilities::Utilities(){
    //Compute host's unicode format
    if (sizeof(wchar_t) == 2)
        hostUniFormat = kUTF16HostEndian;
    else
        hostUniFormat = kUTF32HostEndian;
}

ASPathName Utilities::makeASPathName(wchar_t* pathname){

    ASText pathText = NULL;         //Text of pathname
    ASPathName pathASPath = NULL;   //ASPathName for path

    DURING
        pathText = ASTextFromUnicode((ASUTF16Val *)pathname, hostUniFormat);
        pathASPath = ASFileSysCreatePathFromDIPathText(NULL, pathText, NULL);
    HANDLER
        RERAISE();
    END_HANDLER

    //Release resources
    ASTextDestroy(pathText);

    return pathASPath;
}

ASFile Utilities::openASFile(wchar_t* filepath){
    ASErrorCode errCode = 0;       //Tracks errors
    ASFile file = NULL;            //The file we want to open
    ASPathName aspfilepath = makeASPathName(filepath);    //Filepath of the document

    DURING
        errCode = ASFileSysOpenFile(
                    ASGetDefaultFileSys(), //ASFileSys
                    aspfilepath,           //ASPathName
                    ASFILE_READ,           //ASFileMode
                    &file);                //ASFile*, filled by ASFileSysOpenFile
    HANDLER
        RERAISE();
    END_HANDLER

    if (errCode)
        ASRaise(errCode);
    
    //Release resources
    ASFileSysReleasePath(ASGetDefaultFileSys(), aspfilepath);

    return file;
}

PDDoc Utilities::openPDFNoSecurity(wchar_t* filepath){

    ASPathName doc_path = makeASPathName(filepath);    //Filepath of the document
    PDDoc doc = NULL;                                  //Reference to the document

    DURING
        doc = PDDocOpen(doc_path, NULL, NULL, true);
    HANDLER
        RERAISE();
    END_HANDLER

    //Release resources
    ASFileSysReleasePath(ASGetDefaultFileSys(), doc_path);

    return doc;
}