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
#include "ASExtraCalls.h"

#include <string>
#include <vector>
#include <map>

// Regular expression patterns that follow the ECMAScript syntax.
// https://cplusplus.com/reference/regex/ECMAScript/

// These are samples of what can be done.
enum regexEnum { PHONE_PATTERN, EMAIL_PATTERN, URL_PATTERN };

#if __cplusplus >= 201103L
static std::map< regexEnum, const char * > regexPattern = {
   {PHONE_PATTERN, R"((1-)?(\()?\d{3}(\))?(\s)?(-)?\d{3}-\d{4})"},
   {EMAIL_PATTERN, R"(\b[\w.!#$%&'*+\/=?^`{|}~-]+@[\w-]+(?:\.[\w-]+)*\b)"},
   {URL_PATTERN, R"((https?:\/\/(?:www\.|(?!www))[a-zA-Z0-9][a-zA-Z0-9-]+[a-zA-Z0-9]\.[^\s]{2,}|www\.[a-zA-Z0-9][a-zA-Z0-9-]+[a-zA-Z0-9]\.[^\s]{2,}|https?:\/\/(?:www\.|(?!www))[a-zA-Z0-9]+\.[^\s]{2,}|www\.[a-zA-Z0-9]+\.[^\s]{2,}))"}
};
#else
static std::map< regexEnum, const char * > regexPattern = {
   {PHONE_PATTERN, "(1-)?(\\()?\\d{3}(\\))?(\\s)?(-)?\\d{3}-\\d{4}"},
   {EMAIL_PATTERN, "\\b[\\w.!#$%&'*+\\/=?^`{|}~-]+@[\\w-]+(?:\\.[\\w-]+)*\\b"},
   {URL_PATTERN, "(https?:\\/\\/(?:www\\.|(?!www))[a-zA-Z0-9][a-zA-Z0-9-]+[a-zA-Z0-9]\\.[^\\s]{2,}|www\\.[a-zA-Z0-9][a-zA-Z0-9-]+[a-zA-Z0-9]\\.[^\\s]{2,}|https?:\\/\\/(?:www\\.|(?!www))[a-zA-Z0-9]+\\.[^\\s]{2,}|www\\.[a-zA-Z0-9]+\\.[^\\s]{2,})"}
};
#endif

/** Structure representing only the text  */
typedef struct {
    std::string text;
} PDTextExtractRec;

/** Structure representing the text and the quads */
typedef struct {
    std::string text;
    std::vector<ASFixedQuad> boundingQuads;
} PDTextAndQuadsExtractRec;

class TextExtract {

private:
    PDDoc pDoc;
    PDWordFinderConfigRec wfConfig;
    PDWordFinder wordFinder;
    ASInt32 numWords;
    PDWord wordArray;

public:
    TextExtract(PDDoc inPDoc);
    ~TextExtract();
    std::vector<PDTextExtractRec> TextExtract::GetText();
    std::vector<PDTextExtractRec> TextExtract::GetText(ASInt32 pageNum);
    std::vector<PDTextAndQuadsExtractRec> TextExtract::GetTextAndQuads();
    std::vector<PDTextAndQuadsExtractRec> TextExtract::GetTextAndQuads(ASInt32 pageNum);
    void TextExtract::SetupWordFinderParams();
};

#endif
