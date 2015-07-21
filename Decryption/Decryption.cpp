// Copyright (c) 2015, Datalogics, Inc. All rights reserved.

//===============================================================
// Sample: Decryption - Removes security from a document
//
// This sample completely removes the security from a password-
// protected document.
//
//Steps:
// 1) Open the document with the password
// 2) Remove the encryption
// 3) Save and close the document
// 4) Try opening it again to ensure the encryption is gone
//===============================================================

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

#include <iostream>
#include "InitializeLibrary.h"
#include "ASExtraCalls.h"
#include <iostream>
#include "ASCalls.h"
#include "PDCalls.h"
#include "PEWCalls.h"
#include "PDFLExpT.h"
#include "PEExpT.h"
#include "PagePDECntCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "ASExtraCalls.h"
#include "PDCalls.h"
#include "ASCalls.h"

static char* password = "";     //The document's password is stored statically
                                //because openAuthorizationProcedure is static.
                                //Note that the password cannot be a wchar_t*.

//Called by PDDocOpenEx to obtain permission to open the document by supplying the password.
static ACCB1 ASBool ACCB2 openAuthorizationProcedure(PDDoc encrypted, void *clientData);

//=====================================================
//Creates an ASPathName from a wchar string.
//=====================================================
ASPathName makeASPathName(wchar_t* pathname){

    ASText pathText = NULL;         //Text of pathname
    ASPathName pathASPath = NULL;   //ASPathName for path

    ASUnicodeFormat hostUniFormat;
    if (sizeof(wchar_t) == 2)
        hostUniFormat = kUTF16HostEndian;
    else
        hostUniFormat = kUTF32HostEndian;

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
int main()
{
    //Initialize the APDF Library.
    APDFLib lib;
    if (!lib.isValid())
        return lib.getInitError();    //Will display the error, if any.

    ASErrorCode errCode = 0;       //Tracks errors
    password = "mypassword";       //Password to open the document


    DURING

//========================================================================================
//Step 1) Open the document with the password
//========================================================================================

    ASPathName in_path = makeASPathName(L"../Input/encrypted.pdf");
    ASPathName out_path = makeASPathName(L"unencrypted.pdf");
    PDDoc document = PDDocOpenEx(in_path, ASGetDefaultFileSys(),       //Calls openAuthorizationProcedure to supply the password
                           ASCallbackCreateProto(PDAuthProcEx, 
                           &openAuthorizationProcedure), 0, true);
    ASFileSysReleasePath(ASGetDefaultFileSys(), in_path);

    std::wcout << L"Opened the document." << std::endl;

//========================================================================================
//Step 2) Remove the encryption
//========================================================================================

    PDDocSetNewCryptHandler(document, ASAtomNull);                //Completely removes security from the document

    std::wcout << L"The encryption was removed." << std::endl;

//========================================================================================
//Step 2) Save and close the document
//========================================================================================


    PDDocSave(document, PDDocNeedsSave | PDDocIsOpen,
        out_path, ASGetDefaultFileSys(), NULL, NULL);
    ASFileSysReleasePath(ASGetDefaultFileSys(), out_path);

    std::wcout << L"The document was saved..." << std::endl;

    PDDocClose(document);

//========================================================================================
//Step 2) Try opening it again to ensure the encryption is gone
//========================================================================================

    ASErrorCode openError = 0;

    DURING
        document = PDDocOpen(out_path, NULL, NULL, true);
        PDDocClose(document);
        document = NULL;
    HANDLER
        openError = ERRORCODE;
    END_HANDLER
        if (openError)
        {
            if (ErrGetSystem(openError) == ErrSysPDDoc 
               && ErrGetCode(openError) == pdErrNeedPassword)
            {
                std::wcout << L"...But still requires a password [FAILURE]." << std::endl;
            }
            else
            {
                std::wcout << L"An unexpected error occured." << std::endl;
                ASRaise(openError);
            }
        }
        else
        {
            std::wcout << L"...And is no longer encrypted!" << std::endl;
            PDDocClose(document);
            document = NULL;
        }

        if (document) PDDocClose(document);
        ASFileSysReleasePath(ASGetDefaultFileSys(), out_path);

    HANDLER

        errCode = ERRORCODE;

    END_HANDLER


    if (errCode) lib.displayError(errCode);    //If there was an error, display it
    return errCode;                            //End. lib's destructor terminates the library.
};

//========================================================================================
//Called by PDDocOpenEx to obtain permission to open the document
//by supplying the password.
//========================================================================================
static ACCB1 ASBool ACCB2 openAuthorizationProcedure(PDDoc encrypted, void *clientData){

    PDPermReqStatus permReqStatus;               //Stores the result of the permission request

    DURING

        permReqStatus = PDDocPermRequest(        //Request open permission by supplying the password
                            encrypted,
                            PDPermReqObjDoc,     //Object of the request: a document
                            PDPermReqOprOpen,    //Target operation of the request: to open
                            (void*)password);
    HANDLER

        RERAISE();

    END_HANDLER

    switch (permReqStatus)
    {
        case PDPermReqGranted:
            std::wcout << L"openAuthorizationProcedure: Password verified.\nPermission to open the document has been granted." << std::endl;
            break;
        case PDPermReqDenied:
            std::wcout << L"openAuthorizationProcedure: Invalid password.\nThe request to open the document has been denied." << std::endl;
            break;
        case PDPermReqUnknownObject:
            std::wcout << L"openAuthorizationProcedure: Target object unknown for the permisson request." << std::endl;
            break;
        case PDPermReqUnknownOperation:
            std::wcout << L"openAuthorizationProcedure: Target operation unknown for the permission request." << std::endl;
            break;
        default:
            std::wcout << L"openAuthorizationProcedure: An unexpected error occured while trying to open the document." << std::endl;
            break;
    }

    return (permReqStatus == PDPermReqGranted);
};
