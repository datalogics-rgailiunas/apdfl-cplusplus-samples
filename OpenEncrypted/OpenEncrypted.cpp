// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//=================================================================
// Sample: OpenEncrypted - Removes security from a document.
//
// Note: This sample completely removes the security from a 
// password-protected document.
//
//Steps:
// 1) Open the document with the password.
// 2) Remove the encryption.
// 3) Save and close the document.
// 4) Open it without a password to ensure the encryption is gone.
//=================================================================

#include <iostream>

#include "InitializeLibrary.h"
#include "ASExtraCalls.h"
#include "PDFInit.h"
#include "PDFLCalls.h"
#include "PDExpT.h"
#include "PDCalls.h"


//The password we will try is stored statically because openAuthorizationProcedure is static. Note that the password cannot be a wchar_t*.
static char* password = "";

//Callback function called by PDDocOpenEx to obtain permission to open the document by supplying the password.
static ACCB1 ASBool ACCB2 openAuthorizationProcedure(PDDoc encrypted, void* password);

//Creates an ASPathName from a wchar_t string.
ASPathName makeASPathName(wchar_t* pathname);

int main(int argc, char** argv)
{
    APDFLib lib;                                                         //Initialize the Adobe PDF Library.

    if (!lib.isValid())                                                  //If it failed to initialize, return the error code.
        return lib.getInitError();

    password = "mypassword";                                             //The password needed to open the document.

    ASErrorCode errCode = 0;                                             //This will catch error codes thrown during library usage.

    DURING

//===================================================================================================================================================
//Step 1) Open the document with the password.
//===================================================================================================================================================

    std::wcout << L"Attempting to open the document." << std::endl;

    ASPathName inputPathName  = makeASPathName(L"../_Input/OpenEncrypted.pdf");

    //PDDocOpenEx openAuthorizationProcedure to supply the password.
    PDDoc document = PDDocOpenEx(inputPathName, ASGetDefaultFileSys(), &openAuthorizationProcedure, 0, true);

    ASFileSysReleasePath(ASGetDefaultFileSys(), inputPathName);          //We only needed this to open the document. It will be saved to a new path.

//===================================================================================================================================================
//Step 2) Remove the encryption.
//===================================================================================================================================================

    PDDocSetNewCryptHandler(document, ASAtomNull);                       //Setting it to ASAtomNull completely removes security from the document.

    std::wcout << L"The encryption was removed." << std::endl;

//===================================================================================================================================================
//Step 3) Save and close the document.
//===================================================================================================================================================

    ASPathName pathOutput = makeASPathName(L"unencrypted.pdf");
    PDDocSave(document, PDDocNeedsSave | PDDocIsOpen, pathOutput, ASGetDefaultFileSys(), NULL, NULL);

    std::wcout << L"The document was saved..." << std::endl;

    //Release resources.
    PDDocClose(document);

//===================================================================================================================================================
//Step 4) Open it without a password to ensure the encryption is gone.
//===================================================================================================================================================

    ASErrorCode openError = 0;

    DURING

        document = PDDocOpen(pathOutput, NULL, NULL, true);              //Open the document as if it was unencrypted (which it should be).

        PDDocClose(document);

        document = NULL;

    HANDLER

        openError = ERRORCODE;

    END_HANDLER

    if (openError)
    {
        if (ErrGetSystem(openError) == ErrSysPDDoc && ErrGetCode(openError) == pdErrNeedPassword)
            std::wcout << L"...But still requires a password [FAILURE]." << std::endl;
        else
            std::wcout << L"An unexpected error occured." << std::endl;

        ASRaise(openError);
    }

    std::wcout << L"...And is no longer encrypted!" << std::endl;

    //Release resources.
    if (document) PDDocClose(document);
    ASFileSysReleasePath(ASGetDefaultFileSys(), pathOutput);

    std::wcout << L"Success." << std::endl;

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);                                       //If there was an error, display it.

    END_HANDLER

    return errCode;                                                      //lib's destructor terminates the library.
};

//===================================================================================================================================================
//ASBool function: Callback function called by PDDocOpenEx to obtain permission to open 
//the document by supplying the password.
//===================================================================================================================================================
static ACCB1 ASBool ACCB2 openAuthorizationProcedure(PDDoc encrypted, void *clientData){

    PDPermReqStatus permReqStatus;                                       //Stores the result of the permission request.

    DURING

        //Request open permission by supplying the password.
        permReqStatus = PDDocPermRequest(encrypted,                      //The document we want to open.
                                         PDPermReqObjDoc,                //Object of the request: a document.
                                         PDPermReqOprOpen,               //Target operation of the request: to open it.
                                         (void*)password);               //And, of course, the password.

    HANDLER

        ASRaise(ERRORCODE);                                              //If there was an error, let the caller handle it.

    END_HANDLER

    switch (permReqStatus)
    {
        std::wcout << "openAuthorizationProcedure: ";
        case PDPermReqGranted:
            std::wcout << L"Password verified.\nPermission to open the document has been granted." << std::endl;
            break;
        case PDPermReqDenied:
            std::wcout << L"Invalid password.\nThe request to open the document has been denied." << std::endl;
            break;
        case PDPermReqUnknownObject:
            std::wcout << L"Target object unknown for the permisson request." << std::endl;
            break;
        case PDPermReqUnknownOperation:
            std::wcout << L"Target operation unknown for the permission request." << std::endl;
            break;
        default:
            std::wcout << L"An unexpected error occured while trying to open the document." << std::endl;
            break;
    }

    return (permReqStatus == PDPermReqGranted);
};


//===================================================================================================================================================
//ASPathName function: Creates an ASPathName from a wchar_t string.
//===================================================================================================================================================
ASPathName makeASPathName(wchar_t* pathname){

    ASText pathText = NULL;                                              //Text object of the path.
    ASPathName pathASPath = NULL;                                        //Pathname of the path.

    ASUnicodeFormat hostUniFormat;
    if (sizeof(wchar_t) == 2)
        hostUniFormat = kUTF16HostEndian;
    else
        hostUniFormat = kUTF32HostEndian;

    DURING

        pathText = ASTextFromUnicode((ASUTF16Val*)pathname, hostUniFormat);
        pathASPath = ASFileSysCreatePathFromDIPathText(NULL, pathText, NULL);

    HANDLER

        ASRaise(ERRORCODE);                                              //If there was an error, let the caller handle it.

    END_HANDLER

    //Release resources.
    ASTextDestroy(pathText);

    return pathASPath;
};
