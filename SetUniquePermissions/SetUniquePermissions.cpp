// Copyright(c) 2015, Datalogics, Inc.All rights reserved.

//=======================================================================
// Sample: SetUniquePermissions - Changes the user security permissions
//              of the input document to allow or deny any permission
//              you'd like.
//
//Steps: 
//1) Select which permissions you want to allow/deny.
//2) Create new security data with the specified permissions.
//3) Set the new security data into the document.
//4) Save and close the document.
//=======================================================================

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

#include "APDFLDoc.h"
#include "InitializeLibrary.h"

//To reduce verbosity.
#define PERM_PAIR std::make_pair<bool,PDPerms>

int main(int argc, char** argv)
{
    APDFLib lib;                                                 //Initialize the Adobe PDF Library.

    if (lib.isValid() == false)                                  //If it failed to initialize, return the error code.
        return lib.getInitError();

    wchar_t* inPath  = L"../_Input/SetUniquePermissions.pdf";    //Input document path.
    wchar_t* outPath = L"SetUniquePermissions_Out.pdf";          //Output document path to save the new document to.
    char*   password = "Datalogics";                             //Password we'll add for changing permissions in the output document.

    ASErrorCode errCode = 0;                                     //This will catch error codes thrown during library usage.

//====================================================================================================================================================================================================================================================================
//Step 1) Select which permissions you want to allow/deny.
//====================================================================================================================================================================================================================================================================

    std::vector<std::pair<bool, PDPerms>> permList(17);          //There are 17 permissions listed here.

    ////////////////////////////////
    ////All permissions           //
    ////////////////////////////////
    //All permissions.
    permList.push_back(PERM_PAIR(false, pdPermUser));
    //The user is permitted to perform all operations, regardless of the permissions specified by the document. Unless this permission is set, the document's permissions will be reset to those in the document after a full save.
    permList.push_back(PERM_PAIR(false, pdPermOwner));

    ////////////////////////////////
    ////User Editing Permissions  //
    ////////////////////////////////
    //The OR of all operations that can be set by the user in the security restrictions (pdPermEdit + pdPermEditNotes + pdPermPrint + pdPermCopy).
    permList.push_back(PERM_PAIR(false, pdPermSettable));
    //The user can edit the document more than adding or modifying text notes (see also pdPermEditNotes). In the Document Security dialog, this corresponds to the Changing the Document entry.
    permList.push_back(PERM_PAIR(true, pdPermEdit));
    //The user can add, modify, and delete text notes (see also pdPermEdit). In the document restrictions, this corresponds to the Authoring Comments and Form Fields entry.
    permList.push_back(PERM_PAIR(false, pdPermEditNotes));
    //Overrides various pdPermEdit bits and allows the following operations: page insert/delete/rotate and create bookmark and thumbnail.
    permList.push_back(PERM_PAIR(false, pdPrivPermDocAssembly));
    //Overrides other PDPerm bits. It allows the user to fill in or sign existing form or signature fields.
    permList.push_back(PERM_PAIR(false, pdPrivPermFillandSign));
    //This should be set if the user can submit forms outside of the browser. This bit is a supplement to pdPrivPermFillandSign.
    permList.push_back(PERM_PAIR(false, pdPrivPermFormSubmit));

    ////////////////////////////////
    ////Copying                   //
    ////////////////////////////////
    //The user can copy information from the document to the clipboard. In the document restrictions, this corresponds to the Content Copying or Extraction entry.
    permList.push_back(PERM_PAIR(false, pdPermCopy));
    //Overrides pdPermCopy to enable the Accessibility API. If a document is saved in Rev2 format (Acrobat 4.0 compatible), only the pdPermCopy bit is checked to determine the Accessibility API state.
    permList.push_back(PERM_PAIR(false, pdPrivPermAccessible));

    ////////////////////////////////
    ////Printing                  //
    ////////////////////////////////
    //The user can print the document. Page Setup access is unaffected by this permission, since that affects Acrobat's preferences - not the document's. In the Document Security dialog, this corresponds to the Printing entry.
    permList.push_back(PERM_PAIR(true, pdPermPrint));
    //This bit is a supplement to pdPermPrint. If it is clear (disabled) only low quality printing (Print As Image) is allowed. On UNIX platforms where Print As Image doesn't exist, printing is disabled.
    permList.push_back(PERM_PAIR(true, pdPrivPermHighPrint));

    ////////////////////////////////
    ////Open/Save                 //
    ////////////////////////////////
    //The user can open and decrypt the document.
    permList.push_back(PERM_PAIR(false, pdPermOpen));
    //The user can perform a Save As.... If both pdPermEdit and pdPermEditNotes are disallowed, Save will be disabled but Save As... will be enabled. The Save As... menu item is not necessarily disabled even if the user is not permitted to perform a Save As....
    permList.push_back(PERM_PAIR(false, pdPermSaveAs));

    ////////////////////////////////
    ////Security                  //
    ////////////////////////////////
    //The user can change the document's security settings.
    permList.push_back(PERM_PAIR(false, pdPermSecure));
    //The OR of all operations that can be set by the user in the security restrictions (pdPermPrint + pdPermEdit + pdPermCopy + pdPermEditNotes).
    permList.push_back(PERM_PAIR(false, pdPermSettable));

    ////////////////////////////////
    ////Templates                 //
    ////////////////////////////////
    //This should be set if the user can spawn template pages. This bit will allow page template spawning even if pdPermEdit and pdPermEditNotes are clear.
    permList.push_back(PERM_PAIR(false, pdPrivPermFormSpawnTempl));

    DURING

        std::cout << "Opening the input document." << std::endl;

        APDFLDoc APDoc(inPath, true);
        PDDoc document = APDoc.getPDDoc();

//====================================================================================================================================================================================================================================================================
//Step 2) Create new security data with the specified permissions.
//====================================================================================================================================================================================================================================================================

        std::cout << "Creating the new security data." << std::endl;

        //This structure will hold the new security data.
        PDDocSetNewCryptHandler(document, ASAtomFromString("Standard"));                   //Prepare to create new security data...
        StdSecurityData securityData = (StdSecurityData)PDDocNewSecurityData(document);    //...and create it.
        securityData->size = sizeof(StdSecurityDataRec);

        //Our output will have no password to open it. See Samples "AddPassword" and "LockDocument" for password demonstration.
        securityData->hasUserPW = false;

        //The permissions of the document will not be able to be changed back unless the user supplies this password.
        securityData->hasOwnerPW = true;
        securityData->newOwnerPW = true;
        strcpy(securityData->ownerPW, password);

        //Set the encryption method:
        //2 = CF_METHOD_RC4_V2 - RC4 algorithm.
        //5 = CF_METHOD_AES_V1 - AES algorithm with a zero initialization vector.
        //6 = CF_METHOD_AES_V2 - AES algorithm with a 16 byte random initialization vector.
        //7 = CF_METHOD_AES_V3 - AES algorithm with a 4 byte random initialization vector.
        securityData->encryptMethod = 2;
        securityData->keyLength = 16;                                                      //encryption key length in bytes.
        securityData->encryptMetadata = true;
        securityData->encryptAttachmentsOnly = false;

        //Set the supplied user permissions flags.
        securityData->perms = 0x00000000;                                                  //First, default to no permissions...
        for (std::pair<const bool, PDPerms> x : permList)                                  //Then bitwise OR it with all the permissions we paired with true.
            if (x.first) securityData->perms |= x.second;

        std::cout << "New security permissions have been created...." << std::endl;

//====================================================================================================================================================================================================================================================================
//Step 3) Set the new security data into the document.
//====================================================================================================================================================================================================================================================================

        PDDocSetNewSecurityData(document, (void*)securityData);
        ASfree((void*)securityData);

        std::cout << "...and added to the document." << std::endl;

//====================================================================================================================================================================================================================================================================
//Step 4) Save and close the document.
//====================================================================================================================================================================================================================================================================

        std::cout << "Saving the new file." << std::endl;
        APDoc.saveDoc(outPath, PDSaveFull);                                                //Changing the security handler requires a full save. APDFLDoc defaults
                                                                                           //   to PDSaveFull, but it's important to make this explicit.
        std::cout << "Success." << std::endl;

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);                                                         //If there was an error, display it.

    END_HANDLER

    return errCode;                                                                        //lib's destructor terminates the library.
};
