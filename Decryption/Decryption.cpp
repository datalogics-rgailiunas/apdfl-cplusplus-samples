// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// Sample <<Decryption/Opens a password-protected document and removes the password>>
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
#include "Decryptor.h"

//STD includes
#include <iostream>

int main(){
	//Initialize the APDF libary
	int initError = Decryptor::initPDFL();
	if (initError) return initError;

	//APDFL variables
	ASErrorCode errCode = 0;				             //Tracks errors

	//Sample variables
	Decryptor decryptor;					             //Handles most of the sample code
	PDDoc encrypted_document;						     //For processing the still-encrypted document
	wchar_t* encrypted_path = L"../Input/encrypted.pdf"; //Filepath of the encrypted document
	char* password = "mypassword";						 //Password for that document. NOTE: This does not work for wide chars. 

	DURING
		
		//Open the encrypted PDF document
		encrypted_document = decryptor.openEncrypted(encrypted_path, password);
		std::wcout << L"Opened the document." << std::endl;

		//Remove the encryption
		decryptor.removeEncryption(encrypted_document);
		std::wcout << L"The encryption was removed." << std::endl;

		//Overwrite the document
		PDDocSave(encrypted_document,					  //Document to save
			PDDocNeedsSave | PDDocIsOpen,				  //PDDocSaveFlags
			decryptor.makeASPathName(encrypted_path),     //ASPath to save
			ASGetDefaultFileSys(),						  //File system
			NULL, NULL);								  //Progress monitor, progreess monitor client data
		std::wcout << L"The document was saved..." << std::endl;

		//Close the document
		PDDocClose(encrypted_document);
		
		//Verify it is no longer encrypted
		int openError = decryptor.attemptOpen(encrypted_path);
		if (openError){
			if (ErrGetSystem(openError) == ErrSysPDDoc && ErrGetCode(openError) == pdErrNeedPassword){
				std::wcout << L"...But still requires a password [FAILURE]." << std::endl;
			}
			else{
				std::wcout << L"An unexpected error occured." << std::endl;
				ASRaise(openError);
			}
		}
		else{
			std::wcout << L"...And is no longer encrypted!" << std::endl;
		}

	HANDLER
		errCode = ERRORCODE;
	END_HANDLER

	//Release created objects.
	if (encrypted_document) PDDocClose(encrypted_document);

	//If there was an error, display it.
	if (errCode) DisplayError(errCode);
	
	//Terminate the APDFL library.
	MyPDFLTerm();

	//End.
	return errCode;
}