//# Copyright(c) 2015, Datalogics, Inc.All rights reserved.
//
// Sample: Initialize / Initializes the APDF Library.
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

#ifndef INTERNIT_H
#define INTERNIT_H

#include "PDFInit.h"
#include "PDFLCalls.h"

class APDFLib{

public:
    APDFLib();

    ~APDFLib();

    ASInt32 getInitError();
    static void displayError(ASErrorCode errCode);

private:
    static PDFLDataRec	pdflData;
    ASInt32 initError;

    //Memory Allocation Procedures
    static TKAllocatorProcs intMemAllocProcs;
    static void* intPDFLMemAllocProc(void* clientData, ASSize_t size);
    static void* intPDFLMemRealloc(void* clientData, void* ptr, ASSize_t size);
    static void intPDFLMemFree(void *clientData, void* ptr);
    static ASSize_t intPDFLMemAvail(void *clientData);
    

    //Directory Lists
    static void fillDirectories();
    static ASUTF16Val** fontDirList;      //TODO: platform divergences
    static ASInt32 fontDirListLen;
    static ASUTF16Val* cMapDir;          //TODO: platform divergences
    static ASUTF16Val* unicodeDir;       //TODO: platform divergences
    static ASUTF16Val** colorProfDirList;     //TODO: ibid
    static ASInt32 colorProfDirListLen;

    //Utilities
    static void CopyStrToASUTF16(ASUTF16Val *utf, const char *s);
};

ASUTF16Val** APDFLib::fontDirList = 0;
ASInt32 APDFLib::fontDirListLen = 0;
ASUTF16Val* APDFLib::cMapDir = 0;
ASUTF16Val* APDFLib::unicodeDir = 0;
ASUTF16Val** APDFLib::colorProfDirList = 0;
ASInt32 APDFLib::colorProfDirListLen = 0;

//*************************************
//Reports whether an error happened
//and returns that error.
//************************************
ASInt32 APDFLib::getInitError(){
    if (initError)
    {
        std::wcerr << L"Initialization error. See \"AcroErr.h\" for more info.\n" << std::endl;
        std::wcerr << L"Error system: " << ErrGetSystem(initError) << std::endl;
        std::wcerr << L"Error Severity: " << ErrGetSeverity(initError) << std::endl;
        std::wcerr << L"Error Code: " << ErrGetCode(initError) << std::endl;
    }

    return initError;
}

//*************************************
//Converts a char string to an
//ASUTF16 string
//************************************
void APDFLib::CopyStrToASUTF16(ASUTF16Val *utf, const char *s)
{
    while ((*utf) != (ASUTF16Val)'\0'){
        *utf = *s;
        ++utf;
        ++s;
    }
}

#define NUM_FONTS 2
#define NUM_COLOR_PROFS 1
//*************************************
//Sets directory information
//************************************
void APDFLib::fillDirectories(){
    const char font[] = "..\\..\\..\\APDFL\\Resource\\Font\0";
    const char cmap[] = "..\\..\\..\\APDFL\\Resource\\CMap\0";
    const char unidir[] = "..\\..\\..\\APDFL\\Resource\\Unicode\0";
    const char colpro[] = "..\\..\\..\\APDFL\\Resource\\Color\\Profiles\0";

    //****FONT DIR LIST****//
    fontDirList[NUM_FONTS];
    fontDirListLen = NUM_FONTS;

    CopyStrToASUTF16(fontDirList[0], font);
    CopyStrToASUTF16(fontDirList[1], cmap);

    //****CMAP DIR****//
    cMapDir[_MAX_PATH];

    CopyStrToASUTF16(cMapDir, cmap);


    //****UNICODE DIR****//
    unicodeDir[_MAX_PATH];

    CopyStrToASUTF16(unicodeDir, unidir);

    //****COLOR PROFILE DIR****//
    colorProfDirList[NUM_COLOR_PROFS];
    colorProfDirListLen = NUM_COLOR_PROFS;

    CopyStrToASUTF16(colorProfDirList[0], colpro);

}
#undef NUM_FONTS
#undef NUM_COLOR_PROFS

//*************************************
//Sets the PDFLDataRec and inits
//with that data.
//************************************
APDFLib::APDFLib(){
    PDFLDataRec	pdflData;
    memset(&pdflData, 0, sizeof(PDFLDataRec));

    pdflData.size = sizeof(PDFLDataRec);

    pdflData.allocator = &intMemAllocProcs;
    
    pdflData.dirList = fontDirList;
    pdflData.listLen = fontDirListLen; 

    pdflData.cMapDirectory = cMapDir;

    pdflData.unicodeDirectory = unicodeDir;

    pdflData.colorProfileDirList = colorProfDirList;
    pdflData.colorProfileDirListLen = colorProfDirListLen;
    
#ifdef LOAD_PLUGIN
    //pdflData.pluginDirListLen = PDFLGetPluginDirList(&pdflData.pluginDirList); /* specify plugin path */
    pdflData.pluginDirListLen = PDFLGetPluginDirList(&pdflData.pluginDirList); /* specify plugin path */
#endif
    
#ifdef PLUGIN
    initError = PDFLInitHFT(&pdflData);
#else
    return PDFLInit(&pdflData);
#endif
    
}

//*************************************
//Deconstructor terminates the
//APDFL library.
//************************************
APDFLib::~APDFLib(){
#ifdef PLUGIN
    PDFLTermHFT();
#else
    PDFLTerm();
#endif
}

TKAllocatorProcs APDFLib::intMemAllocProcs = {
    intPDFLMemAllocProc,
    intPDFLMemRealloc,
    intPDFLMemFree,
    intPDFLMemAvail,
    NULL					///clientData 
};

void* APDFLib::intPDFLMemAllocProc(void* clientData, ASSize_t size)
{
    return malloc(size);
}

void* APDFLib::intPDFLMemRealloc(void* clientData, void* ptr, ASSize_t size)
{
    return realloc(ptr, size);
}

 void APDFLib::intPDFLMemFree(void *clientData, void* ptr)
{
    if (ptr != NULL)
        free(ptr);
}

 ASSize_t APDFLib::intPDFLMemAvail(void *clientData){
     return INT_MAX;
 }

 void APDFLib::displayError(ASErrorCode errCode)
 {
     if (errCode == 0)	return;

     char errStr[250];
     fprintf(stderr, "[Error %ld] %s\n", errCode, ASGetErrorString(errCode, errStr, sizeof(errStr)));
 }

#endif //INTERNIT_H
