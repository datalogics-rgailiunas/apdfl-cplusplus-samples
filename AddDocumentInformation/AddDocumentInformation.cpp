//# Copyright(c) 2015, Datalogics, Inc.All rights reserved.

//************************************************************************
// Sample: AddDocumentInformation opens a document, inserts metadata and saves
//
// Note: This program only inserts standard metadata contained in the 
// "Document Information Dictionary" see the PDF Reference section 10.2 for 
// more information
//
//Steps:
// 1) Open the Document that the document information (metadata) will be inserted into
// 2) Insert the document information (meta data)
// 3) Save the document and release resources
//************************************************************************

//# This agreement is between Datalogics, Inc. 101 N.Wacker Drive, Suite 1800,
//# Chicago, IL 60606 ("Datalogics") and you, an end user who downloads
//# source code examples for integrating to the Adobe PDF Library
//# ("the Example Code"). By accepting this agreement you agree to be bound
//# by the following terms of use for the Example Code.
//#
//# LICENSE
//# -------
//# Datalogics hereby grants you a royalty - free, non - exclusive license to
//# download and use the Example Code for any lawful purpose.There is no charge
//# for use of Example Code.
//#
//# OWNERSHIP
//# ---------
//# The Example Code and any related documentation and trademarks are and shall
//# remain the sole and exclusive property of Datalogics and are protected by
//# the laws of copyright in the U.S.and other countries.
//#
//# Datalogics is a trademark of Datalogics, Inc.
//#
//# TERM
//# ----
//# This license is effective until terminated.You may terminate it at any
//# other time by destroying the Example Code.
//#
//# WARRANTY DISCLAIMER
//# -------------------
//# THE EXAMPLE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
//# EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES
//# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//#
//# DATALOGICS DISCLAIM ALL OTHER WARRANTIES, CONDITIONS, UNDERTAKINGS OR
//# TERMS OF ANY KIND, EXPRESS OR IMPLIED, WRITTEN OR ORAL, BY OPERATION OF
//# LAW, ARISING BY STATUTE, COURSE OF DEALING, USAGE OF TRADE OR OTHERWISE,
//# INCLUDING, WARRANTIES OR CONDITIONS OF MERCHANTABILITY, FITNESS FOR A
//# PARTICULAR PURPOSE, SATISFACTORY QUALITY, LACK OF VIRUSES, TITLE,
//# NON - INFRINGEMENT, ACCURACY OR COMPLETENESS OF RESPONSES, RESULTS, AND / OR
//# LACK OF WORKMANLIKE EFFORT.THE PROVISIONS OF THIS SECTION SET FORTH
//# SUBLICENSEE'S SOLE REMEDY AND DATALOGICS'S SOLE LIABILITY WITH RESPECT
//# TO THE WARRANTY SET FORTH HEREIN.NO REPRESENTATION OR OTHER AFFIRMATION
//# OF FACT, INCLUDING STATEMENTS REGARDING PERFORMANCE OF THE EXAMPLE CODE,
//# WHICH IS NOT CONTAINED IN THIS AGREEMENT, SHALL BE BINDING ON DATALOGICS.
//# NEITHER DATALOGICS WARRANT AGAINST ANY BUG, ERROR, OMISSION, DEFECT,
//# DEFICIENCY, OR NONCONFORMITY IN ANY EXAMPLE CODE. 

#include <iostream>
#include "MyPDFLibUtils.h"
#include "ASExtraCalls.h"

int main()
{
	//Initialize the APDFL prior to making any APDFL calls
	ASErrorCode errorCode = MyPDFLInit();

	//Check for errors upon initialization of APDFL
	if (errorCode != 0)
	{
		std::cerr << "Initialization error. See \"AcroErr.h\" for more info.\n" << std::endl;
		std::cerr << "Error system: " << ErrGetSystem(errorCode) << std::endl;
		std::cerr << "Error Severity: " << ErrGetSeverity(errorCode) << std::endl;
		std::cerr << "Error Code: " << ErrGetCode(errorCode) << std::endl;

		return errorCode;
	}

	DURING
//*************************************************************************
//Step 1) Open the PDF Document
//*************************************************************************

		//File path used to open the PDF Document
		wchar_t * inputFilePath = L"../Input/AddDocumentInformation.pdf\0";

		//Argument passed to function ASTextFromUnicode
		ASUnicodeFormat unicodeFormat;

		//Determine host computers unicode format	
		if (sizeof(wchar_t) == 2)
			unicodeFormat = kUTF16HostEndian;
		else
			unicodeFormat = kUTF32HostEndian;

		//ASText is the unicode string used to create ASPathName for opening the PDDoc
		ASText asText = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(inputFilePath), unicodeFormat);

		//Create the asPathName to open the input file
		ASPathName asPathName = ASFileSysCreatePathFromDIPathText(NULL, asText, NULL);

		//Free up resources
		ASTextDestroy(asText);
		
		//Open the Document
		PDDoc pdDoc = PDDocOpen(asPathName, NULL, NULL, true);

		std::wcout << L"Document was sucessfully opened." << std::endl;

		//Free up resources
		ASFileSysReleasePath(NULL, asPathName);

//*************************************************************************
//Step 2) Insert document information (metadata) into Document
//        Title and Author are demonstrated here.
//
//		  Standard Key values contained in Document Information Dictionary: 
//        {"Title, Author, Subject, Keywords, Creator, Producer, Trapped"}  
//
//		  "key" refers to the field the metadata will be inserted into
//        "value" is the metadata being inserted
//*************************************************************************

		//Document Title:
		//Create unicode strings for inserting metadata into the document.
		ASText key = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"Title\0"), unicodeFormat);
		ASText value = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"Sample Title\0"), unicodeFormat);

		//Insert the metadeta
		PDDocSetInfoAsASText(pdDoc, key, value);

		//Free up the resources
		ASTextDestroy(key);
		ASTextDestroy(value);

		std::wcout << L"The documents title was inserted." << std::endl;

		//Document's Author:
		//Create unicode strings for inserting metadata into the document.
		key = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"Author\0"), unicodeFormat);
		value = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"Sample Author\0"), unicodeFormat);

		//Insert the metadeta
		PDDocSetInfoAsASText(pdDoc, key, value);

		//Free up the resources
		ASTextDestroy(key);
		ASTextDestroy(value);

		std::wcout << L"The documents author was inserted." << std::endl;

//*************************************************************************
//Step 3) Save the Document and release Resources
//*************************************************************************

		//Create unicode string for output file
		asText = ASTextFromUnicode(reinterpret_cast<ASUTF16Val*>(L"out.pdf"), unicodeFormat);

		//Create the asPathName to open the input file
		asPathName = ASFileSysCreatePathFromDIPathText(NULL, asText, NULL);

		//Save the new document containing metadata
		PDDocSave(pdDoc, PDSaveFull, asPathName, NULL, NULL, NULL);

		std::wcout << L"The PDF Document has been saved and now contains metadata." << std::endl;

		//Release remaining resources
		ASTextDestroy(asText);
		ASFileSysReleasePath(NULL, asPathName);
		PDDocClose(pdDoc);

		std::wcout << L"All remaining resources have been released" << std::endl;

	HANDLER
		//If there was an exception generate an error code 
		errorCode = ERRORCODE;

		char buf[256];

		ASGetErrorString(ERRORCODE, buf, sizeof(buf));

		//Print out error code
		std::cerr << "Error Code: " << errorCode << "Error Message: " << buf << std::endl;
	END_HANDLER

	return errorCode;
}