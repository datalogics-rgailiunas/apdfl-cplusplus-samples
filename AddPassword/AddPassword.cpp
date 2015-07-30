// Copyright(c) 2015, Datalogics, Inc.All rights reserved.

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

#include <iostream>
#include "InitializeLibrary.h"
#include "APDFLDoc.h"

int main(int argc, char** argv)
{
    APDFLib lib;                                                                           //Initialize the Adobe PDF Library.

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
