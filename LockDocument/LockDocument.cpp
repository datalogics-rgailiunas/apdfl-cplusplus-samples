// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//=================================================================
// Sample: LockDocument - Makes the input document read-only.
//
//Steps: 
//1) Open the document, and create new security data for 
//   it which will disallow all editing permissions.
//2) Set the new security data into the document.
//3) Save and close the document.
//=================================================================

#include <iostream>
#include "InitializeLibrary.h"
#include "APDFLDoc.h"

int main(int argc, char** argv)
{
    APDFLib lib(argv[1]);                                                                           //Initialize the Adobe PDF Library.

    if (lib.isValid() == false)                                                            //If it failed to initialize, return the error code.
        return lib.getInitError();

    wchar_t* inPath  = L"../_Input/LockDocument.pdf";                                      //Input document path.
    wchar_t* outPath = L"LockDocument_Out.pdf";                                            //Output document path.
    char*   password = "Datalogics";                                                       //Password to change permissions we will add. Note: cannot be a wide character string.

    ASErrorCode errCode = 0;                                                               //This will catch error codes thrown during library usage.

    DURING

//================================================================================================================================================================================
//Step 1) Open the document, and create new security data for 
//        it which will disallow all editing permissions.
//================================================================================================================================================================================

        std::cout << "Opening the input document." << std::endl;

        APDFLDoc APDoc(inPath,true);                                                       //Open the input document, repairing it if it is damaged.
        PDDoc document = APDoc.getPDDoc();

        std::cout << "Creating the new security data." << std::endl;

        //This structure will hold the new security data.
        PDDocSetNewCryptHandler(document, ASAtomFromString("Standard"));                   //Prepare to create new security data...
        StdSecurityData securityData = (StdSecurityData)PDDocNewSecurityData(document);    //...and create it
        securityData->size = sizeof(StdSecurityDataRec);

        //Do not require a password to open the document.
        securityData->hasUserPW = false;

        //The new permissions of the document will not be able to be changed unless the user supplies this password.
        securityData->hasOwnerPW = true;
        securityData->newOwnerPW = true;
        strcpy(securityData->ownerPW, password);

        //Set the encryption method:
        //2 = CF_METHOD_RC4_V2 - RC4 algorithm.
        //5 = CF_METHOD_AES_V1 - AES algorithm with a zero initialization vector.
        //6 = CF_METHOD_AES_V2 - AES algorithm with a 16 byte random initialization vector.
        //7 = CF_METHOD_AES_V3 - AES algorithm with a 4 byte random initialization vector.
        securityData->encryptMethod = 2;
        securityData->keyLength = 16;                                                      //Encryption key length, in bytes.
        securityData->encryptMetadata = true;
        securityData->encryptAttachmentsOnly = false;

        //No permissions at all will be enabled.
        securityData->perms = 0x00000000;

        std::cout << "New security permissions have been created...." << std::endl;

//================================================================================================================================================================================
//Step 2) Set the new security data into the document.
//================================================================================================================================================================================

        PDDocSetNewSecurityData(document, (void*)securityData);
        ASfree((void*)securityData);

        std::cout << "...and added to the document." << std::endl;

//================================================================================================================================================================================
//Step 3) Save and close the document.
//================================================================================================================================================================================

        std::cout << "Saving the new file." << std::endl;
        APDoc.saveDoc(outPath, PDSaveFull);                                                //Changing the security of a document requires a full save. APDFLDoc
                                                                                           //   saves with PDSaveFull by default, but this is just to be explicit.
        std::cout << "Success." << std::endl;

    HANDLER

        errCode = ERRORCODE;
        if (errCode) lib.displayError(errCode);                                            //If there was an error, display it.

    END_HANDLER

    return errCode;                                                                        //End. lib's destructor terminates the library.
};
