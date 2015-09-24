// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//==============================================================================
// Sample: EncryptDocument - Encrypting and saving an existing document
//
// Steps: 
//  1) Create new security data, set with a user password for encryption using 
//     the RC4 algorithm.
//  2) Set the encrptyion method to the document, save and exit   
//==============================================================================

#include <iostream>
#include "ASExtraCalls.h"
#include "InitializeLibrary.h"
#include "APDFLDoc.h"

int main(int argv, char** argc)
{

    APDFLib libInit;                      //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;              //Variable used to report any exceptions/errors if they occured.

    if (libInit.isValid() == false)       //Check for errors upon initialization.
        return libInit.getInitError();    //If initialization failed, return the error code.

    DURING                        

        //Open input document with path and repair if damaged.
        APDFLDoc document(L"../_Input/toBeEncrypted.pdf", true);    

        std::wcout << L"Input document successfully opened. " << std::endl;

//=======================================================================================================================================
// Step 1) Create new security data, set with a user password for encryption using the RC4 algorithm.
//
// Note: Encrytpion can be set from among 4 different types.
//       2 = CF_METHOD_RC4_V2 - RC4 algorithm.
//       5 = CF_METHOD_AES_V1 - AES algorithm with a zero initialization vector.
//       6 = CF_METHOD_AES_V2 - AES algorithm with a 16 byte random initialization vector.
//       7 = CF_METHOD_AES_V3 - AES algorithm with a 4 byte random initialization vector.
//=======================================================================================================================================

        //Sets specified document’s new security handler.
        PDDocSetNewCryptHandler(document.pdDoc, ASAtomFromString("Standard"));
        
        //Declare a structure describing the data for the standard security handler.
        StdSecurityData securityData = (StdSecurityData)PDDocNewSecurityData(document.pdDoc);
              
        securityData->size = sizeof(StdSecurityDataRec);    //Set the size of the structure       
                                                            
        securityData->hasUserPW = true;                     //If there is a user password.
                                                            
        securityData->newUserPW = true;                     //If the user password should be changed.
                                                           
        strcpy(securityData->userPW, "myPass");             //Set the user password.
                                                            
        securityData->hasOwnerPW = false;                   //If there is a owner password.
                                                           
        securityData->newOwnerPW = false;                   //If the owner password should be changed.       
                                                            
        strcpy(securityData->ownerPW, "");                  //Set the password.
                                                           
        securityData->perms = pdPermUser;                   //Permissions flags to allow.
                                            
        securityData->keyLength = 16;                       //Password key's length.
        
        securityData->encryptMethod = 2;                    //Set the encryption method to the RC4 algorithm.

//=======================================================================================================================================
// Step 2) Set the encrptyion method to the document, save and exit                      
//=======================================================================================================================================
              
        PDDocSetNewSecurityData(document.pdDoc, securityData);                     //Set the security data to the document. 
        
        PDDocSetFlags(document.pdDoc, PDDocRequiresFullSave);                      //Changing the document security requires a full save.
      
        ASfree(securityData);                                                      //Release object no longer in use.
        
        document.saveDoc(L"encrypted.pdf", PDSaveFull | PDSaveLinearized);         //Save the document.

        //Check if the document has an encryption set.                                                                            
        if (PDDocGetCryptHandler(document.pdDoc))                                  
            std::wcout << L"encrypted.pdf saved with encryption." << std::endl;
    
    HANDLER

        errCode = ERRORCODE;
        
        libInit.displayError(errCode);                                             //If there was an error, display it.

    END_HANDLER

    return errCode;                                                                //Return program status.
}
