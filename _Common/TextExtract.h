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

#ifndef TEXTEXTRACT_H
#define TEXTEXTRACT_H

#include "PDCalls.h"
#include "ASCalls.h"
#include "CosCalls.h"
#include "ASExtraCalls.h"

#include <string>
#include <vector>
#include <map>

// Regular expression patterns that follow the ECMAScript syntax.
// https://cplusplus.com/reference/regex/ECMAScript/

// These are samples of what can be done.
enum regexEnum { PHONE_PATTERN, EMAIL_PATTERN, URL_PATTERN };

static std::map<regexEnum, const char *> regexPattern = {
    {PHONE_PATTERN, R"((1-)?(\()?\d{3}(\))?(\s)?(-)?\d{3}-\d{4})"},
    {EMAIL_PATTERN, R"(\b[\w.!#$%&'*+\/=?^`{|}~-]+@[\w-]+(?:\.[\w-]+)*\b)"},
    {URL_PATTERN, R"((https?:\/\/(?:www\.|(?!www))[a-zA-Z0-9][a-zA-Z0-9-]+[a-zA-Z0-9]\.[^\s]{2,}|www\.[a-zA-Z0-9][a-zA-Z0-9-]+[a-zA-Z0-9]\.[^\s]{2,}|https?:\/\/(?:www\.|(?!www))[a-zA-Z0-9]+\.[^\s]{2,}|www\.[a-zA-Z0-9]+\.[^\s]{2,}))"}};

/** Structure representing only the text  */
typedef struct {
    std::string text;
} PDTextExtractRec;

/** Structure representing the text and the quads */
typedef struct {
    std::string text;
    std::vector<ASFixedQuad> boundingQuads;
} PDTextAndQuadsExtractRec;

/** Structure representing the AcroForm text and the field names */
typedef struct {
    std::string text;
    std::string fieldName;
} PDAcroFormExtractRec;

class TextExtract {

  private:
    PDDoc pDoc = nullptr;
    PDWordFinderConfigRec wfConfig;
    PDWordFinder wordFinder;
    ASInt32 numWords = 0;
    PDWord wordArray;

  public:
    TextExtract(PDDoc inPDoc, bool useWordFinder = true);
    ~TextExtract();
    std::vector<PDTextExtractRec> GetText();
    std::vector<PDTextExtractRec> GetText(ASInt32 pageNum);
    std::vector<PDTextAndQuadsExtractRec> GetTextAndQuads();
    std::vector<PDTextAndQuadsExtractRec> GetTextAndQuads(ASInt32 pageNum);
    std::vector<PDAcroFormExtractRec> GetAcroFormFieldData();
    void SetupWordFinderParams();
};

#endif
