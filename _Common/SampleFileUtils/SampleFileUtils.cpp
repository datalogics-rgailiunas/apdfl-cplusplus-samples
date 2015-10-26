//
// Copyright 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
// This is a Windows specific sample class...
//

#include <cstdio>

#include "ScopeGuard.h"         // for ON_BLOCK_EXIT

#include "PDFLCalls.h"
#include "PDCalls.h"
#include "ASCalls.h"
#include "ASExtraCalls.h"       // for ASTextFromUnicode

#include "SampleCustomExceptions.h"
#include "SampleFileUtils.h"

//
// File Utility Functions
//

PDDoc SelectPDFDocument()
{
    ASErrorCode err = 0;

    std::wstring pdfFilePath;
    SelectPDFPath(pdfFilePath);
    if (pdfFilePath.empty())
    {   // user canceled or similar
        return NULL;
    }

    ASFileSys fileSys = ASGetDefaultFileSys();
    ASAtom pathSpecType = ASAtomFromString("DIPathWithASText");
    ASText pathSpec = ASTextFromUnicode(((ASUTF16Val*)pdfFilePath.c_str()), kUTF16HostEndian);
    ASPathName path = ASFileSysCreatePathName(NULL, pathSpecType, pathSpec, NULL);
    ON_BLOCK_EXIT(ASFileSysReleasePath, fileSys, path);

    PDDoc pDoc = NULL;
    DURING
    {
        pDoc = PDDocOpen(path, fileSys, NULL, TRUE);
    }
    HANDLER
    {
        err = ERRORCODE;
    }
    END_HANDLER

    if (err)
    {   // something wrong happened while trying to open the document (maybe the selected file *is not* a PDF?)
        if (pDoc)
        {
            DURING
                PDDocClose(pDoc);
            HANDLER
                // ignore it
            END_HANDLER
        }
        
        char msg[64] = {0};
        sprintf_s(msg, sizeof(msg), "Error: %d\r\n", err);
        throw new InvalidFileException(msg);
    }

    return pDoc;
}

void SelectPDFPath(std::wstring &pdfFilePath /* OUT */)
{
    OPENFILENAMEW ofn;
    memset(&ofn, 0, sizeof(OPENFILENAMEW));
    ofn.lStructSize = sizeof(OPENFILENAMEW);

    ofn.lpstrTitle = L"Select a PDF";
    ofn.lpstrDefExt = L"pdf";
    ofn.lpstrFilter = L"Portable Document File\0*.pdf;\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_EXPLORER | OFN_LONGNAMES | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    WCHAR pathBuf[0x1000] = {0};

    ofn.lpstrFile = pathBuf;
    ofn.nMaxFile = sizeof(pathBuf) / sizeof(WCHAR);

    SetLastError(0);
    BOOL result = GetOpenFileNameW(&ofn);
    if (!result)
    {
        DWORD err = CommDlgExtendedError();
        if (err == 0)
        {   // dialog canceled
            return;
        }

        char msg[64] = {0};
        sprintf_s(msg, sizeof(msg), "Error: %d\r\n", err);
        throw new GeneralException(msg);
    }

    if (ofn.lpstrFile && ofn.lpstrFile[0] != 0 /* not empty */)
    {
        pdfFilePath = ofn.lpstrFile;
    }
}

void GetSaveAsFilePath(std::wstring &pdfFilePath /* OUT */)
{
    OPENFILENAMEW ofn;
    memset(&ofn, 0, sizeof(OPENFILENAMEW));
    ofn.lStructSize = sizeof(OPENFILENAMEW);

    ofn.lpstrTitle = L"Save as PDF...";

    ofn.lpstrDefExt = L"pdf";
    ofn.lpstrFilter = L"Portable Document File\0*.pdf;\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_EXPLORER | OFN_LONGNAMES | OFN_NONETWORKBUTTON | OFN_OVERWRITEPROMPT;

    WCHAR pathBuf[0x1000] = { 0 };

    ofn.lpstrFile = pathBuf;
    ofn.nMaxFile = sizeof(pathBuf) / sizeof(WCHAR);

    SetLastError(0);
    BOOL result = GetSaveFileNameW(&ofn);
    if (!result)
    {
        DWORD err = CommDlgExtendedError();
        {   // dialog canceled
            return;
        }

        char msg[64] = { 0 };
        sprintf_s(msg, sizeof(msg), "Error: %d\r\n", err);
        throw new GeneralException(msg);
    }

    if (ofn.lpstrFile && ofn.lpstrFile[0] != 0 /* not empty */)
    {
        size_t len = wcslen(ofn.lpstrFile);
        LPWSTR newFilePath = new WCHAR[len + 1];

        memcpy(newFilePath, ofn.lpstrFile, len * sizeof(WCHAR));
        newFilePath[len] = 0;

        if (ofn.nFileExtension > 0 && ofn.nFileExtension < len)
        {   // remove the extension (we'll add one later when we know what type of file to make)
            newFilePath[ofn.nFileExtension - 1 /* account for the '.' */] = 0;
        }

        pdfFilePath = newFilePath;
    }
}

void SelectICCProfile(std::wstring &iccFilePath /* OUT */)
{
    OPENFILENAMEW ofn;
    memset(&ofn, 0, sizeof(OPENFILENAMEW));
    ofn.lStructSize = sizeof(OPENFILENAMEW);

    ofn.lpstrTitle = L"Select a ICC Profile";
    ofn.lpstrDefExt = L"icc";
    ofn.lpstrFilter = L"ICC Color Profile\0*.icc;\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_EXPLORER | OFN_LONGNAMES | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    WCHAR pathBuf[0x1000] = { 0 };

    ofn.lpstrFile = pathBuf;
    ofn.nMaxFile = sizeof(pathBuf) / sizeof(WCHAR);

    SetLastError(0);
    BOOL result = GetOpenFileNameW(&ofn);
    if (!result)
    {
        DWORD err = CommDlgExtendedError();
        if (err == 0)
        {   // dialog canceled
            return;
        }

        char msg[64] = { 0 };
        sprintf_s(msg, sizeof(msg), "Error: %d\r\n", err);
        throw new GeneralException(msg);
    }

    if (ofn.lpstrFile && ofn.lpstrFile[0] != 0 /* not empty */)
    {
        iccFilePath = ofn.lpstrFile;
    }
}
