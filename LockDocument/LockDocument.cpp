// Copyright(c) 2015, Datalogics, Inc.All rights reserved.

//=================================================================
// Sample: LockDocument - Makes the input document read-only.
//
//Steps: 
//1) Open the document and create new security data
//2) Set the security data into the document
//3) Save and close the document
//=================================================================

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
    //Initialize the Adobe PDF Library.
    APDFLib lib;
    if (!lib.isValid())
        return lib.getInitError();    //Will display the error, if any.

    //Sample variables
    ASErrorCode errCode = 0;                                //Tracks runtime errors in the application
    wchar_t* inPath  = L"../_Input/LockDocument.pdf";        //Input document path
    wchar_t* outPath = L"LockDocument_Out.pdf";             //Output document path
    char*   password = "Datalogics";                        //Password to change permissions

    DURING

//=============================================================================
//Step 1) Open the document and create new security data
//=============================================================================

        std::cout << "Opening the input document." << std::endl;

        APDFLDoc APDoc(inPath,false);
        PDDoc document = APDoc.getPDDoc();

        std::cout << "Creating the new security data." << std::endl;

        //This structure will hold the new security data
        PDDocSetNewCryptHandler(document, ASAtomFromString("Standard"));   //Prepare to create new security data
        StdSecurityData securityData = (StdSecurityData)PDDocNewSecurityData(document);    //...and create it
        securityData->size = sizeof(StdSecurityDataRec);

        //No password to open the document
        securityData->hasUserPW = false;

        //The permissions of the document cannot be changed back (unlocked) unless the user supplies this password.
        securityData->hasOwnerPW = true;
        securityData->newOwnerPW = true;
        strcpy(securityData->ownerPW, password);

        //Set the encryption method
        //2 = CF_METHOD_RC4_V2 - RC4 algorithm
        //5 = CF_METHOD_AES_V1 - AES algorithm with a zero initialization vector
        //6 = CF_METHOD_AES_V2 - AES algorithm with a 16 byte random initialization vector
        //7 = CF_METHOD_AES_V3 - AES algorithm with a 4 byte random initialization vector
        securityData->encryptMethod = 2;
        securityData->keyLength = 16;    //encryption key length in bytes
        securityData->encryptMetadata = true;
        securityData->encryptAttachmentsOnly = false;

        //No permissions at all will be enabled.
        securityData->perms = 0x00000000;

        std::cout << "New security permissions have been set...." << std::endl;

//=============================================================================
//Step 2) Set the security data into the document
//=============================================================================

        PDDocSetNewSecurityData(document, (void*)securityData);
        ASfree((void*)securityData);
        PDDocSetFlags(document, PDDocRequiresFullSave );           //Changing the security data requires a full save

        std::cout << "...and added to the document." << std::endl;

//=============================================================================
//Step 3) Save and close the document.
//=============================================================================

        std::cout << "Saving the new file." << std::endl;
        APDoc.saveDoc(outPath, PDSaveFull | PDSaveLinearized);

    HANDLER

        errCode = ERRORCODE;

    END_HANDLER

    if (errCode) lib.displayError(errCode);    //If there was an error, display it
    return errCode;                            //End. lib's destructor terminates the library.
};
