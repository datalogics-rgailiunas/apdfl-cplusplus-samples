// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//==============================================================================
// Sample: ? - ?
//
// Steps:
// 1) ?
//==============================================================================

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#include "PDCalls.h"
#include "ASExtraCalls.h"

#include "XPS2PDFCalls.h"

ExtensionID gExtensionID = 0; 

int main(int argc, char** argv)
{
    APDFLib libInit;                      //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;              //Variable used to report any exceptions/errors if they occured.

    if (libInit.isValid() == false)       //If there was a problem in initialization, return the error code.
        return libInit.getInitError();

    DURING

    gXPS2PDFHFT = InitXPS2PDFHFT;         //Sets the function called during XPS2PDFInitialize().

//Load the plugin.

    //Load the XPS2PDF plugin.
    if (!XPS2PDFInitialize()) {
        std::wcout << L"XPS2PDF Could not initialize." << std::endl;
        ASRaise(ERRORCODE);    //The handler will display the error code.
    }

//Prepare the settings for the converter..
    ASCab settings = ASCabNew();

    ASText language = ASTextFromUnicode((ASUTF16Val*)"ENU", kUTF8);
    ASCabPutText(settings, "PDFSettingsLang", language);
    ASText jobNameText = ASTextFromUnicode((ASUTF16Val*)"../../Resource/joboptions/Standard.joboptions", kUTF8);
    ASCabPutText(settings, "PDFSettings", jobNameText);

    ASPathName asInPathName = ASFileSysCreatePathName(NULL, ASAtomFromString("Cstring"), "../_Input/XPStoPDF_New.xps", 0);

    PDDoc odoc = NULL;

    int ret_val = XPS2PDFConvert(settings,0,asInPathName,NULL,&odoc,NULL);
    if (ret_val)
    {
        /* DLADD: kshahn 13Aug2009 - Fix printf wording. */
        APDFLDoc doc;
        doc.pdDoc = odoc;
        doc.saveDoc(L"converted.pdf");
        //APDFLDoc's destructor takes care of closing the document and releasing the rest of the resources.
    }
    else
        ASRaise(ret_val);

    //clean up args when done
    ASCabDestroy(settings);
    ASFileSysReleasePath( NULL, asInPathName);

    //Close the XPS2PDF plugin.
    XPS2PDFTerminate();

    HANDLER

        errCode = ERRORCODE;
        libInit.displayError(errCode);                                //If there was an error, display it.

    END_HANDLER

    if (!errCode)
        std::wcout << L"Success!" << std::endl;

    return errCode;                                                   //APDFLib's destructor terminates the library.
}
