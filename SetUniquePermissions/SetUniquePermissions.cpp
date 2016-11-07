// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
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

    std::vector<std::pair<bool, PDPerms> > permList(15);          //There are 15 permissions listed here. C++03 require a space between two righ angle brackets.

    ////////////////////////////////
    //Most Permissions            //
    ////////////////////////////////
    //All permissions, except page extraction.
    permList.push_back(PERM_PAIR(false, pdPermAll));
    //All permissions, except page extraction.
    permList.push_back(PERM_PAIR(false, pdPermUser));

    ////////////////////////////////
    ////User Editing Permissions  //
    ////////////////////////////////
    //Sets these four permissions true: pdPermEdit, pdPermEditNotes, pdPermPrint, pdPermCopy.
    permList.push_back(PERM_PAIR(false, pdPermSettable));
    //Enables changing the document, document assembly, form field filling, signing, and template page spawning.
    permList.push_back(PERM_PAIR(false, pdPermEdit));
    //Enables commenting, form field filling, and signing.
    permList.push_back(PERM_PAIR(false, pdPermEditNotes));
    //Enables page insertion/deletion/rotation, as well as bookmark creation.
    permList.push_back(PERM_PAIR(false, pdPrivPermDocAssembly));
    //Enables form field filling, signing, and spawning template pages.
    permList.push_back(PERM_PAIR(false, pdPrivPermFillandSign));
    //Whether the form can be submitted outside of a browser. Be sure you enable form field filling and signing.
    permList.push_back(PERM_PAIR(false, pdPrivPermFormSubmit));

    ////////////////////////////////
    ////Copying                   //
    ////////////////////////////////
    //Enables content copying (i.e., to the clipboard) and content copying for accessibility.
    permList.push_back(PERM_PAIR(false, pdPermCopy));
    //Enables copying of content related to Acrobat's accessibility features for people with disabilities. 
    permList.push_back(PERM_PAIR(false, pdPrivPermAccessible));

    ////////////////////////////////
    ////Printing                  //
    ////////////////////////////////
    //Enables printing.
    permList.push_back(PERM_PAIR(false, pdPermPrint));
    //If pdPermPrint is true and this is false, only low quality printing (Print As Image) is allowed. On UNIX platforms where Print As Image doesn't exist, printing will be disabled.
    permList.push_back(PERM_PAIR(false, pdPrivPermHighPrint));

    ////////////////////////////////
    ////Open/Save                 //
    ////////////////////////////////
    //The user can open and decrypt the document. This will have no effect if a user password is not set.
    permList.push_back(PERM_PAIR(false, pdPermOpen));
    //Enables Save As..., with the followng caveats: if both pdPermEdit and pdPermEditNotes are disallowed, Save will be disabled but Save As... will be enabled. The Save As... menu item is not necessarily disabled even if this is set to false!
    permList.push_back(PERM_PAIR(false, pdPermSaveAs));

    ////////////////////////////////
    ////Security                  //
    ////////////////////////////////
    //The user can change the document's security settings. This will have no effect unless an owner password is set.
    permList.push_back(PERM_PAIR(false, pdPermSecure));

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

        //See the sample Encryption for a demonstration of user passwords.
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
        for (int i = 0; i < 15; i++)                                  //Then bitwise OR it with all the permissions we paired with true.
        {
            std::pair<bool, PDPerms> x = permList[i];
            if (x.first) securityData->perms |= x.second;
        }
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
