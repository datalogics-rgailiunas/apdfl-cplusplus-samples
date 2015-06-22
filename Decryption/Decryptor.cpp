// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// Sample Decryption / Performs important auxiliary functions for the encryption sample.
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
#include "Decryptor.h"

char* Decryptor::password = "";     //Default the password to nothing.

Decryptor::Decryptor(){

	//Compute host's unicode format.
	if (sizeof(wchar_t) == 2)
		hostUniFormat = kUTF16HostEndian;
	else
		hostUniFormat = kUTF32HostEndian;
}

void Decryptor::setPassword(char* pass){
	Decryptor::password = pass;
}

ACCB1 ASBool ACCB2 Decryptor::openAuthorizationProcedure(PDDoc encrypted, void *clientData){

	PDPermReqStatus permReqStatus; //Stores the result of the permission request

	DURING
		//Request open permission by supplying the password
		permReqStatus = PDDocPermRequest(encrypted,
			PDPermReqObjDoc,  //Permission request Object
			PDPermReqOprOpen, //Permission request operation
			(void*)Decryptor::password);
	HANDLER
		RERAISE();
	END_HANDLER
		switch (permReqStatus)
	{
		case PDPermReqGranted:
			std::wcout << L"openAuthorizationProcedure: Password verified.\nPermission to open the document has been granted." << std::endl;
			break;
		case PDPermReqDenied:
			std::wcout << L"openAuthorizationProcedure: Invalid password.\nThe request to open the document has been denied." << std::endl;
			break;
		case PDPermReqUnknownObject:
			std::wcout << L"openAuthorizationProcedure: Target object unknown for the permisson request." << std::endl;
			break;
		case PDPermReqUnknownOperation:
			std::wcout << L"openAuthorizationProcedure: Target operation unknown for the permission request." << std::endl;
			break;
		default:
			std::wcout << L"openAuthorizationProcedure: An unexpected error occured while trying to open the document." << std::endl;
			break;
	}
	return (permReqStatus == PDPermReqGranted);
}

ASErrorCode Decryptor::attemptOpen(wchar_t* file_path){

	ASPathName doc_path = makeASPathName(file_path);    //Filepath of the document
	PDDoc doc = NULL;                                   //Stores the document
	ASErrorCode errCode = 0;                            //Tracks errors

	DURING
		doc = PDDocOpen(doc_path, NULL, NULL, true);
	HANDLER
		errCode = ERRORCODE;
	END_HANDLER

	//Release instantiated objects
	ASFileSysReleasePath(ASGetDefaultFileSys(), doc_path);
	if (doc) PDDocClose(doc);

	return errCode;
}

ASPathName Decryptor::makeASPathName(wchar_t* pathname){

	ASText pathText = NULL;         //Text of pathname
	ASPathName pathASPath = NULL;   //ASPathName for path

	DURING
		pathText = ASTextFromUnicode((ASUTF16Val *)pathname, hostUniFormat);
		pathASPath = ASFileSysCreatePathFromDIPathText(NULL, pathText, NULL);
	HANDLER
		RERAISE();
	END_HANDLER
	
	//Release resources
	ASTextDestroy(pathText);

	return pathASPath;
}

int Decryptor::initPDFL(){

	int initError = MyPDFLInit();

	if (initError) {
		std::wcerr << L"Initialization error. See \"AcroErr.h\" for more info.\n" << std::endl;
		std::wcerr << L"Error system: " << ErrGetSystem(initError) << std::endl;
		std::wcerr << L"Error Severity: " << ErrGetSeverity(initError) << std::endl;
		std::wcerr << L"Error Code: " << ErrGetCode(initError) << std::endl;
	}

	return initError;
}
