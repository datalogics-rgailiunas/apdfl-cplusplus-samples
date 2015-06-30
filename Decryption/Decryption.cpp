// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// Sample Decryption / Opens a password-protected document, removes the password, saves, 
//					   and ensures the document is no longer password-protected.
//
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

//Sample includes
#include "SampleUtils.h"
#include "OpenAuthorizationProcedure.cpp"

//STD includes
#include <iostream>

int main(){
	//Initialize the APDF libary
    int initError = MyPDFLInit();
    if (initError) {
        std::wcerr << L"Initialization error. See \"AcroErr.h\" for more info.\n" << std::endl;
        std::wcerr << L"Error system: " << ErrGetSystem(initError) << std::endl;
        std::wcerr << L"Error Severity: " << ErrGetSeverity(initError) << std::endl;
        std::wcerr << L"Error Code: " << ErrGetCode(initError) << std::endl;
        return initError;
    }

	//APDFL variables
	ASErrorCode errCode = 0; //Tracks errors

	//Sample variables
	//Decryptor decryptor;                               //Handles most of the sample code
    Utilities util;                                      //Performs some common actions
	PDDoc encrypted_document;                            //For processing the still-encrypted document
    PDDoc unencrypted_document;                          //For checking that the document has been unencrypted.
    ASPathName encrypted_path = util.makeASPathName(L"../Input/encrypted.pdf"); //Filepath of the encrypted document
	char* myPassword = "mypassword";                     //Password for that document. NOTE: This does not work for wide chars. 

	DURING

		//Open the encrypted PDF document
        setPassword(myPassword);
        encrypted_document = PDDocOpenEx(encrypted_path, ASGetDefaultFileSys(),
			ASCallbackCreateProto(PDAuthProcEx, &openAuthorizationProcedure), 0, true);
		std::wcout << L"Opened the document." << std::endl;

		//Remove the encryption
		PDDocSetNewCryptHandler(encrypted_document, ASAtomNull); //Completely removes security from the document
		std::wcout << L"The encryption was removed." << std::endl;

		//Overwrite the document
		PDDocSave(encrypted_document,        //Document to save
			PDDocNeedsSave | PDDocIsOpen,    //PDDocSaveFlags
            encrypted_path,                  //ASPath to save
			ASGetDefaultFileSys(),           //File system
			NULL, NULL);                     //Progress monitor, progreess monitor client data
		std::wcout << L"The document was saved..." << std::endl;

		//Close the document
		PDDocClose(encrypted_document);
		
		//Verify it is no longer encrypted by attempting to open it
        ASErrorCode openError = 0;
	    DURING
            unencrypted_document = PDDocOpen(encrypted_path, NULL, NULL, true);
	    HANDLER
            openError = ERRORCODE;
	    END_HANDLER
		if (openError){
		    if (ErrGetSystem(openError) == ErrSysPDDoc && ErrGetCode(openError) == pdErrNeedPassword){
		    	std::wcout << L"...But still requires a password [FAILURE]." << std::endl;
		    } else{
		    	std::wcout << L"An unexpected error occured." << std::endl;
		    	ASRaise(openError);
		    }
		} else{
			std::wcout << L"...And is no longer encrypted!" << std::endl;
            PDDocClose(unencrypted_document);
		}

	HANDLER
		errCode = ERRORCODE;
	END_HANDLER

	//Release created objects
    if (encrypted_document) PDDocClose(encrypted_document);
    if (unencrypted_document) PDDocClose(unencrypted_document);
    ASFileSysReleasePath(ASGetDefaultFileSys(), encrypted_path);

	//If there was an error, display it
	if (errCode) DisplayError(errCode);
	
	//Terminate the APDFL library
	MyPDFLTerm();

	//End
	return errCode;
}