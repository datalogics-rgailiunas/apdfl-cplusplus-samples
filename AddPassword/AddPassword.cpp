// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//================================================================================
// Sample: AddPassword - Adds a password to open an otherwise unsecured document.
//
// Note: This sample takes the unsecured input document and creates an output
//     document that can only be opened with the password "Datalogics".
//
//Steps: 
//1) Open the input document and create a password for it.
//2) Set the new security data into the document.
//3) Save and close the document.
//================================================================================

#include <iostream>
#include "InitializeLibrary.h"
#include "APDFLDoc.h"

int main(int argc, char** argv)
{
    APDFLib lib(argv[1]);                                                                           //Initialize the Adobe PDF Library.

    if (lib.isValid() == false)                                                            //If it failed to initialize, return the error code.
        return lib.getInitError();

    wchar_t* inPath  = L"../_Input/AddPassword.pdf";                                       //Input document path.
    wchar_t* outPath = L"AddPassword_Out.pdf";                                             //Output document path.
    char*   password = "Datalogics";                                                       //Password we'll give to the output document (cannot be a wide char).

    ASErrorCode errCode = 0;                                                               //This will catch error codes thrown during library usage.

    DURING

//==========================================================================================================================================================================
//Step 1) Open the input document and create a password for it.
//==========================================================================================================================================================================

        std::cout << "Opening the input document." << std::endl;

        APDFLDoc APDoc(inPath, true);
        PDDoc document = APDoc.getPDDoc();

        std::cout << "Creating the new security data." << std::endl;

        //This structure will hold the new security data.
        PDDocSetNewCryptHandler(document, ASAtomFromString("Standard"));                   //Prepare to create new security data...
        StdSecurityData securityData = (StdSecurityData)PDDocNewSecurityData(document);    //...and create it.
        securityData->size = sizeof(StdSecurityDataRec);

        //Set the password needed to open the output file.
        securityData->hasUserPW = true;
        securityData->newUserPW = true;
        strcpy(securityData->userPW, password);

        //Don't add a password to change restrictions once the file is open. (See SetUniquePermissions for this behavior.)
        securityData->hasOwnerPW = false;

        //Set the encryption method:
        //2 = CF_METHOD_RC4_V2 - RC4 algorithm.
        //5 = CF_METHOD_AES_V1 - AES algorithm with a zero initialization vector.
        //6 = CF_METHOD_AES_V2 - AES algorithm with a 16 byte random initialization vector.
        //7 = CF_METHOD_AES_V3 - AES algorithm with a 4 byte random initialization vector.
        securityData->encryptMethod = 2;
        securityData->keyLength = 16;                                                      //encryption key length, in bytes.
        securityData->encryptMetadata = true;                                              //Either of these two booleans can be set to true, but not both. It's up to you.
        securityData->encryptAttachmentsOnly = false;

        //The user will be able to perform all operations when the file is opened with the password.
        securityData->perms = pdPermOwner;

        std::cout << "New security permissions have been created...." << std::endl;

//==========================================================================================================================================================================
//Step 2) Set the new security data into the document.
//==========================================================================================================================================================================

        PDDocSetNewSecurityData(document, (void*)securityData);
        ASfree((void*)securityData);

        std::cout << "...and added to the document." << std::endl;

//==========================================================================================================================================================================
//Step 3) Save and close the document.
//==========================================================================================================================================================================

        std::cout << "Saving the new file." << std::endl;

        APDoc.saveDoc(outPath, PDSaveFull);                                                 //Changing security permissions requires a full save. APDFLDoc saves with
        PDDocClose(document);                                                               //    PDSaveFull by default, but it's important to make this explicit.

        std::cout << "Success." << std::endl;

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);                                                         //If there was an error, display it.

    END_HANDLER

    return errCode;                                                                        //lib's destructor terminates the library.
};
