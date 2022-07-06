// Copyright (c) 2022, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//===============================================================================
// Sample: TextExtract -This class is intended to assist with operations common to
// text extraction samples. The class contains methods to control types of words
// found and what information is returned to the user.
//
// TextExtract.cpp: Contains implementations of methods.
// TextExtract.h: Contains class definition.
//===============================================================================

#include "TextExtract.h"
#include <vector>
#include <sstream>

static void enumerateField(CosObj fieldObj, std::string prefix, std::vector<PDAcroFormExtractRec> &returnText);

//==============================================================================================================================
// Default Constructor - This creates a new TextExtract object.
//==============================================================================================================================

TextExtract::TextExtract(PDDoc inPDoc,  bool useWordFinder) {
    pDoc = inPDoc;
    if (useWordFinder)
    {
        void SetupWordFinderParams();
        wordFinder = PDDocCreateWordFinderEx(inPDoc, WF_LATEST_VERSION, true, &wfConfig);
    }
    numWords = 0;
}

//==============================================================================================================================
// ~TextExtract() - Releases resources if they haven't already been freed.
//==============================================================================================================================

TextExtract::~TextExtract() { PDWordFinderDestroy(wordFinder); }

//==============================================================================================================================
// SetupWordFinderParams() - Setup params for WordFinder.  User can modify based on needs.
//==============================================================================================================================

void TextExtract::SetupWordFinderParams() {
    memset(&wfConfig, 0, sizeof(PDWordFinderConfigRec));
    wfConfig.recSize = sizeof(PDWordFinderConfigRec);

    wfConfig.disableTaggedPDF = false;
    wfConfig.noXYSort = false;
    wfConfig.preserveSpaces = false;
    wfConfig.noLigatureExp = false;
    wfConfig.noEncodingGuess = false;
    wfConfig.unknownToStdEnc = false;
    wfConfig.ignoreCharGaps = false;
    wfConfig.ignoreLineGaps = false;
    wfConfig.noAnnots = false;
    wfConfig.noHyphenDetection = false;
    wfConfig.trustNBSpace = false;
    wfConfig.noExtCharOffset = false;
    wfConfig.noStyleInfo = false;
    wfConfig.decomposeTbl = NULL;
    wfConfig.decomposeTblSize = 0;
    wfConfig.charTypeTbl = NULL;
    wfConfig.charTypeTblSize = 0;
    wfConfig.preserveRedundantChars = false;
    wfConfig.disableCharReordering = false;
    wfConfig.noSkewedQuads = false;
    wfConfig.noTextRenderMode3 = false;
    wfConfig.preciseQuad = false;
}

//==============================================================================================================================
// GetText() - Gets the text for the entire document.
//==============================================================================================================================

std::vector<PDTextExtractRec> TextExtract::GetText() {
    std::vector<PDTextExtractRec> returnText;

    ASInt32 numPages = PDDocGetNumPages(pDoc);

    for (ASInt32 pageNum = 0; pageNum < numPages; ++pageNum) {
        PDWordFinderAcquireWordList(wordFinder, pageNum, &wordArray, NULL, NULL, &numWords);
        for (ASInt32 wordNum = 0; wordNum < numWords; ++wordNum) {

            PDWord pdWord = PDWordFinderGetNthWord(wordFinder, wordNum);
            ASText asTextWord = ASTextNew();
            PDWordGetASText(pdWord, 0, asTextWord);

            // Get the endian neutral UTF-8 string.
            ASUTF8Val *utf8String = reinterpret_cast<ASUTF8Val *>(ASTextGetUnicodeCopy(asTextWord, kUTF8));

            PDTextExtractRec record;
            record.text = reinterpret_cast<char *>(utf8String);
            ASTextDestroy(asTextWord);
            ASfree(utf8String);
            returnText.emplace_back(record);
        }
        PDWordFinderReleaseWordList(wordFinder, pageNum);
    }
    return returnText;
}

//==============================================================================================================================
// GetText() - Gets the text on a specified page.
//==============================================================================================================================

std::vector<PDTextExtractRec> TextExtract::GetText(ASInt32 pageNum) {
    std::vector<PDTextExtractRec> returnText;
    PDWordFinderAcquireWordList(wordFinder, pageNum, &wordArray, NULL, NULL, &numWords);
    for (ASInt32 wordNum = 0; wordNum < numWords; ++wordNum) {

        PDWord pdWord = PDWordFinderGetNthWord(wordFinder, wordNum);
        ASText asTextWord = ASTextNew();
        PDWordGetASText(pdWord, 0, asTextWord);

        // Get the endian neutral UTF-8 string.
        ASUTF8Val *utf8String = reinterpret_cast<ASUTF8Val *>(ASTextGetUnicodeCopy(asTextWord, kUTF8));

        PDTextExtractRec record;
        record.text = reinterpret_cast<char *>(utf8String);
        ASTextDestroy(asTextWord);
        ASfree(utf8String);
        returnText.emplace_back(record);
    }
    PDWordFinderReleaseWordList(wordFinder, pageNum);
    return returnText;
}

//==============================================================================================================================
// GetTextAndQuads() - Gets the text and quad info for the entire document.
//==============================================================================================================================

std::vector<PDTextAndQuadsExtractRec> TextExtract::GetTextAndQuads() {
    std::vector<PDTextAndQuadsExtractRec> returnText;

    ASInt32 numPages = PDDocGetNumPages(pDoc);

    for (ASInt32 pageNum = 0; pageNum < numPages; ++pageNum) {
        PDWordFinderAcquireWordList(wordFinder, pageNum, &wordArray, NULL, NULL, &numWords);
        for (ASInt32 wordNum = 0; wordNum < numWords; ++wordNum) {

            PDWord pdWord = PDWordFinderGetNthWord(wordFinder, wordNum);
            ASText asTextWord = ASTextNew();
            PDWordGetASText(pdWord, 0, asTextWord);

            // Get the endian neutral UTF-8 string.
            ASUTF8Val *utf8String = reinterpret_cast<ASUTF8Val *>(ASTextGetUnicodeCopy(asTextWord, kUTF8));

            PDTextAndQuadsExtractRec record;
            record.text = reinterpret_cast<char *>(utf8String);
            ASTextDestroy(asTextWord);
            ASfree(utf8String);

            ASInt32 numQuads = PDWordGetNumQuads(pdWord);

            // A Word typically has only 1 quad, but can have more than one for hyphenated words, words on a curve, etc.
            for (ASInt32 quadNum = 0; quadNum < numQuads; ++quadNum) {
                ASFixedQuad wordQuad;
                PDWordGetNthQuad(pdWord, quadNum, &wordQuad);
                record.boundingQuads.emplace_back(wordQuad);
            }
            returnText.emplace_back(record);
        }
        PDWordFinderReleaseWordList(wordFinder, pageNum);
    }
    return returnText;
}

//==============================================================================================================================
// GetTextAndQuads() - Gets the text and quad info for a specific page.
//==============================================================================================================================

std::vector<PDTextAndQuadsExtractRec> TextExtract::GetTextAndQuads(ASInt32 pageNum) {
    std::vector<PDTextAndQuadsExtractRec> returnText;
    PDWordFinderAcquireWordList(wordFinder, pageNum, &wordArray, NULL, NULL, &numWords);
    for (ASInt32 wordNum = 0; wordNum < numWords; ++wordNum) {

        ASFixedQuad wordQuad;

        PDWord pdWord = PDWordFinderGetNthWord(wordFinder, wordNum);
        ASText asTextWord = ASTextNew();
        PDWordGetASText(pdWord, 0, asTextWord);

        // Get the endian neutral UTF-8 string.
        ASUTF8Val *utf8String = reinterpret_cast<ASUTF8Val *>(ASTextGetUnicodeCopy(asTextWord, kUTF8));

        PDTextAndQuadsExtractRec record;
        record.text = reinterpret_cast<char *>(utf8String);
        ASTextDestroy(asTextWord);
        ASfree(utf8String);

        ASInt32 numQuads = PDWordGetNumQuads(pdWord);

        // A Word typically has only 1 quad, but can have more than one for hyphenated words, words on a curve, etc.
        for (ASInt32 quadNum = 0; quadNum < numQuads; ++quadNum) {

            PDWordGetNthQuad(pdWord, quadNum, &wordQuad);
            record.boundingQuads.emplace_back(wordQuad);
        }
        returnText.emplace_back(record);
    }
    PDWordFinderReleaseWordList(wordFinder, pageNum);
    return returnText;
}

//==============================================================================================================================
// GetAcroFormFieldData() - Gets the AcroForm field data.
//==============================================================================================================================

std::vector<PDAcroFormExtractRec> TextExtract::GetAcroFormFieldData() {
    std::vector<PDAcroFormExtractRec> returnText;
    CosObj rootObj = CosDocGetRoot(PDDocGetCosDoc(pDoc));
    CosObj acroFormObj = CosDictGet(rootObj, ASAtomFromString("AcroForm"));
    if (CosObjGetType(acroFormObj) == CosNull) {
        return returnText;
    } else {
        CosObj fieldsObj = CosDictGet(acroFormObj, ASAtomFromString("Fields"));
        if ((CosObjGetType(fieldsObj) != CosArray) || (CosArrayLength(fieldsObj) == 0)) {
            return returnText;
        } else {
            for (ASInt32 fieldIndex = 0; fieldIndex < CosArrayLength(fieldsObj); ++fieldIndex) {
                CosObj fieldObj = CosArrayGet(fieldsObj, fieldIndex);
                enumerateField(fieldObj, "", returnText);
            }
        }
    }
    return returnText;
}

static void enumerateField(CosObj fieldObj, std::string prefix, std::vector<PDAcroFormExtractRec> &returnText) {

    std::string field_name;
    ASTCount textLength;

    if (CosObjGetType(fieldObj) == CosDict) {
        if (CosDictKnown(fieldObj, ASAtomFromString("T"))) {
            CosObj entryObj = CosDictGet(fieldObj, ASAtomFromString("T"));
            if (CosObjGetType(entryObj) == CosString) {
                std::string name_part(CosStringValue(entryObj, &textLength));
                if (prefix == "") {
                    field_name = name_part;
                }
                else {
                    std::ostringstream stringStream;
                    stringStream << prefix << "." << name_part;
                    field_name = stringStream.str();
                }
                // Process the Kids
                CosObj kidsObj = CosDictGet(fieldObj, ASAtomFromString("Kids"));
                if (CosObjGetType(kidsObj) == CosArray) {
                    for (ASInt32 kidIndex = 0; kidIndex < CosArrayLength(kidsObj); ++kidIndex) {
                        CosObj fieldObj = CosArrayGet(kidsObj, kidIndex);
                        enumerateField(fieldObj, field_name, returnText);
                    }
                }

                // Process this node
                CosObj nameObj = CosDictGet(fieldObj, ASAtomFromString("FT"));
                if (CosObjGetType(nameObj) == CosName) {
                    if (CosNameValue(nameObj) == ASAtomFromString("Tx")) {
                        PDAcroFormExtractRec record;
                        record.fieldName = field_name;
                        if (CosDictKnown(fieldObj, ASAtomFromString("V"))) {
                            CosObj entryValueObj = CosDictGet(fieldObj, ASAtomFromString("V"));
                            std::string textString(CosStringValue(entryValueObj, &textLength));
                            ASText asText = ASTextFromSizedPDText(textString.c_str(), textLength);
                            char *textStr = reinterpret_cast<char *>(ASTextGetUnicodeCopy(asText, kUTF8));
                            record.text = textStr;
                        }
                        returnText.emplace_back(record);
                    }
                }
            }
        }
    }
}
