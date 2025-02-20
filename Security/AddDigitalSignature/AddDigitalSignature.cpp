//
// Copyright (c) 2025, Datalogics, Inc. All rights reserved.
//
//
// This sample adds a digital signature to a PDF document.
//
// Command-line:  <input-file> <output-file>    (All optional)
//

#include <iostream>
#include "InitializeLibrary.h"
#include "APDFLDoc.h"

// Header that includes Digital Signature methods
#include "DLExtrasCalls.h"

#define INPUT_LOC "../../../../Resources/Sample_Input/"
#define DEF_INPUT "SixPages.pdf"
#define DEF_OUTPUT "AddDigitalSignature-out.pdf"
#define DEF_CERT_FILE "Credentials/DER/RSA_certificate.der"
#define DEF_KEY_FILE "Credentials/DER/RSA_privKey.der"

int main(int argc, char **argv) {
    APDFLib lib;

    ASErrorCode errCode = 0;
    if (lib.isValid() == false) {
        errCode = lib.getInitError();
        std::cout << "Initialization failed with code " << errCode << std::endl;
        return errCode;
    }

    std::string csInputFileName(argc > 1 ? argv[1] : INPUT_LOC DEF_INPUT);
    std::string csOutputFileName(argc > 2 ? argv[2] : DEF_OUTPUT);
    std::cout << "Will apply a digital signature to " << csInputFileName.c_str() << " and save as "
              << csOutputFileName.c_str() << std::endl;

    DURING

        // Open the input document.
        APDFLDoc APDoc(csInputFileName.c_str(), true);
        PDDoc inDoc = APDoc.getPDDoc();

        // Populate digital signature data structure
        PDSignDocSignParamsRec signParams;
        memset(&signParams, 0x0, sizeof(PDSignDocSignParamsRec));

        signParams.fieldID = CreateFieldWithQualifiedName;
        ASText fieldName = ASTextNew();
        ASTextSetEncoded(fieldName, "Signature_es_:signatureblock", (ASHostEncoding)PDGetHostEncoding());
        signParams.sigFieldInfo.sigFieldIdAttr.name = fieldName;

        signParams.digestCat = sha256;

        signParams.dataFmt = DERFmt;
        signParams.storageFmt = OnDisk;

        ASPathName certPath = APDFLDoc::makePath(INPUT_LOC DEF_CERT_FILE);
        ASPathName keyPath = APDFLDoc::makePath(INPUT_LOC DEF_KEY_FILE);

        ASFile asCertFileDER{nullptr};
        ASErrorCode err = ASFileSysOpenFile64(nullptr, certPath, (ASFILE_READ | ASFILE_SERIAL), &asCertFileDER);

        ASFile asKeyFileDER{nullptr};
        err = ASFileSysOpenFile64(nullptr, keyPath, (ASFILE_READ | ASFILE_SERIAL), &asKeyFileDER);

        // No need to set credential size, since we're passing them as ASFile
        signParams.sign.nonPfxParamsRec = {asCertFileDER, 0, asKeyFileDER, 0, nullptr, 0};

        ASText name = ASTextNew();
        ASTextSetEncoded(name, "John Doe", (ASHostEncoding)PDGetHostEncoding()); // AVAppGetLanguageEncoding()

        ASText location = ASTextNew();
        ASTextSetEncoded(location, "Chicago, IL", (ASHostEncoding)PDGetHostEncoding());

        ASText reason = ASTextNew();
        ASTextSetEncoded(reason, "Approval", (ASHostEncoding)PDGetHostEncoding());

        ASText contact = ASTextNew();
        ASTextSetEncoded(contact, "Datalogics, Inc.", (ASHostEncoding)PDGetHostEncoding());

        signParams.signerInfo = {
            nullptr, fixedZero, name, location, reason, contact, DisplayTraits::kDisplayAll};

        // Finally, sign and save the document
        PDSignDocSaveParamsRec saveParams;
        memset(&saveParams, 0x0, sizeof(PDSignDocSaveParamsRec));

        ASPathName pathOutput = APDFLDoc::makePath(csOutputFileName.c_str());
        saveParams.newPath = pathOutput;
        PDSignDocWithParams(inDoc, &saveParams, &signParams);

        // cleanup
        ASTextDestroy(name);
        ASTextDestroy(location);
        ASTextDestroy(reason);
        ASTextDestroy(contact);

        ASFileClose(asCertFileDER);
        ASFileClose(asKeyFileDER);

        PDDocClose(inDoc);
        ASFileSysReleasePath(nullptr, pathOutput);

    HANDLER
        errCode = ERRORCODE;
        lib.displayError(errCode);
    END_HANDLER

    return errCode;
};
