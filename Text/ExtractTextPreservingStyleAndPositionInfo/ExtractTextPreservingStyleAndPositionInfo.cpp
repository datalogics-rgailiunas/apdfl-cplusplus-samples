//
// Copyright (c) 2022, Datalogics, Inc. All rights reserved.
//
// For complete copyright information, refer to:
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
// This sample extracts text, quads, and styles from a PDF
// document and saves the text to a file.
//
// For more detail see the description of the ExtractTextPreservingStyleAndPositionInfo
// sample program on our Developer’s site,
// http://dev.datalogics.com/adobe-pdf-library/sample-program-descriptions/c1samples

#include <fstream>
#include <string>

#include "InitializeLibrary.h"
#include "APDFLDoc.h"
#include "TextExtract.h"

#ifdef MAC_PLATFORM
#include "../../_Common/ThirdParty/json.hpp"
#else
#include "ThirdParty/json.hpp"
#endif

// Set the namespace for the JSON library
using json = nlohmann::json;

const char *DEF_INPUT = "../../../../Resources/Sample_Input/sample.pdf";
const char *DEF_OUTPUT = "ExtractTextPreservingStyleAndPositionInfo-out.json";

void SaveJson(std::vector<PDTextAndDetailsExtractRec> extractedText);

int main(int argc, char **argv) {

    // Initialize the library
    APDFLib libInit;
    ASErrorCode errCode = 0;
    if (libInit.isValid() == false) {
        errCode = libInit.getInitError();
        std::cout << "Initialization failed with code " << errCode << std::endl;
        return libInit.getInitError();
    }

    DURING

        APDFLDoc inAPDoc(DEF_INPUT, true);

        TextExtract textExtract(inAPDoc.getPDDoc());

        std::vector<PDTextAndDetailsExtractRec> extractedText = textExtract.GetTextAndDetails();

        SaveJson(extractedText);

    HANDLER
        errCode = ERRORCODE;
        libInit.displayError(errCode);
    END_HANDLER

    return errCode;
}

void SaveJson(std::vector<PDTextAndDetailsExtractRec> extractedText)
{
    // This array will hold the JSON stream that we will print to the output JSON file.
    json result = json::array();

    for (ASInt32 textIndex = 0; textIndex < extractedText.size(); ++textIndex) {

        json textObject = json::object();
        textObject["text"] = extractedText[textIndex].text;
        for (int quadIndex = 0; quadIndex < extractedText[textIndex].boundingQuads.size(); ++quadIndex) {
            json quadPointValues = json::array();
            quadPointValues.push_back(std::to_string(extractedText[textIndex].boundingQuads[quadIndex].tl.h));
            quadPointValues.push_back(std::to_string(extractedText[textIndex].boundingQuads[quadIndex].tl.v));
            textObject["quad"]["top-left"] = quadPointValues;
            quadPointValues.clear();

            quadPointValues.push_back(std::to_string(extractedText[textIndex].boundingQuads[quadIndex].tr.h));
            quadPointValues.push_back(std::to_string(extractedText[textIndex].boundingQuads[quadIndex].tr.v));
            textObject["quad"]["top-right"] = quadPointValues;
            quadPointValues.clear();

            quadPointValues.push_back(std::to_string(extractedText[textIndex].boundingQuads[quadIndex].bl.h));
            quadPointValues.push_back(std::to_string(extractedText[textIndex].boundingQuads[quadIndex].bl.v));
            textObject["quad"]["bottom-left"] = quadPointValues;
            quadPointValues.clear();

            quadPointValues.push_back(std::to_string(extractedText[textIndex].boundingQuads[quadIndex].br.h));
            quadPointValues.push_back(std::to_string(extractedText[textIndex].boundingQuads[quadIndex].br.v));
            textObject["quad"]["bottom-right"] = quadPointValues;
            quadPointValues.clear();
        }
        json styles = json::array();
        for (int styleIndex = 0; styleIndex < extractedText[textIndex].styles.size(); ++styleIndex) {
            json styleObject = json::object();
            styleObject["char-index"] = extractedText[textIndex].styles[styleIndex].charIndex;
            styleObject["font-name"] = extractedText[textIndex].styles[styleIndex].fontname;
            styleObject["font-size"] = std::to_string(extractedText[textIndex].styles[styleIndex].fontsize);
            styleObject["color-space"] = extractedText[textIndex].styles[styleIndex].colorValues.DLSpace;
            json colorValues = json::array();
            for (int colorIndex = 0; colorIndex < 4; ++colorIndex) {
                colorValues.push_back(extractedText[textIndex].styles[styleIndex].colorValues.DLColor[colorIndex]);
            }
            styleObject["color-values"] = colorValues;
            styles.push_back(styleObject);
        }
        textObject["styles"] = styles;
        result.push_back(textObject);
    }
    std::ofstream jsonOutputFileStream(DEF_OUTPUT, std::ios_base::trunc | std::ios_base::out);
    jsonOutputFileStream << std::setw(4) << result;
    jsonOutputFileStream.close();
}
