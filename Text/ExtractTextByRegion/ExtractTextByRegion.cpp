//
// Copyright (c) 2022, Datalogics, Inc. All rights reserved.
//
// For complete copyright information, refer to:
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
// This sample extracts text from a specific target area of a page in a PDF
// document and saves the text to a file.
//
// There are different options for determining whether a Word is in the target area:
//   * if the complete Word fits within the target area
//   * if any part of the Word fits within the target area
//   * if the midpoint of the Word fits within the target area
// For this sample, we will consider a Word to be in the target area if the
// midpoint of any of it's quads intersects the target area
//
// Command-line:    <input-pdf>  <output-name>  <target_left>  <target_bottom>  <target_right>
// <target_top> (All optional)
//
// For more detail see the description of the ExtractText sample program on our Developer’s site,
// http://dev.datalogics.com/adobe-pdf-library/sample-program-descriptions/c1samples#extracttext

#include <fstream>
#include <string>

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

const char *DEF_INPUT = "../../../../Resources/Sample_Input/ExtractTextByRegion.pdf";
const char *DEF_OUTPUT = "ExtractTextByRegion-out.txt";

// Default specified region to extract text in points (origin of the page is bottom left)
// (0,0,200,792) is a rectangle encompassing the left side of the page
const float DEF_LEFT = 0;    // left
const float DEF_BOTTOM = 0;  // bottom
const float DEF_RIGHT = 200; // right
const float DEF_TOP = 792;   // top

int main(int argc, char **argv) {

    // Initialize the library
    APDFLib libInit;
    ASErrorCode errCode = 0;
    if (libInit.isValid() == false) {
        errCode = libInit.getInitError();
        std::cout << "Initialization failed with code " << errCode << std::endl;
        return libInit.getInitError();
    }

    // Capture command line arguments if given.
    // Otherwise, use defaults.
    std::string csInputFileName(argc > 1 ? argv[1] : DEF_INPUT);
    std::string csOutputFileName(argc > 2 ? argv[2] : DEF_OUTPUT);

    float userTargetRegionL(argc > 6 ? atoi(argv[3]) : DEF_LEFT);
    float userTargetRegionB(argc > 6 ? atoi(argv[4]) : DEF_BOTTOM);
    float userTargetRegionR(argc > 6 ? atoi(argv[5]) : DEF_RIGHT);
    float userTargetRegionT(argc > 6 ? atoi(argv[6]) : DEF_TOP);

    // Need to convert the region to ASFixed for APDFL
    ASFixed userTargetRegionFixedL = FloatToASFixed(userTargetRegionL);
    ASFixed userTargetRegionFixedB = FloatToASFixed(userTargetRegionB);
    ASFixed userTargetRegionFixedR = FloatToASFixed(userTargetRegionR);
    ASFixed userTargetRegionFixedT = FloatToASFixed(userTargetRegionT);

    DURING

        APDFLDoc inAPDoc(csInputFileName.c_str(), true);
        std::ofstream outputFile(csOutputFileName.c_str());

        // Step 1) Use PDWordfinder to find all the Words in our input document
        PDWordFinderConfigRec wfConfig;
        memset(&wfConfig, 0, sizeof(PDWordFinderConfigRec));
        wfConfig.recSize = sizeof(PDWordFinderConfigRec);
        PDWordFinder wordFinder =
            PDDocCreateWordFinderEx(inAPDoc.getPDDoc(), WF_LATEST_VERSION, true, &wfConfig);

        // Step 2) Acquire the Words on each page
        ASInt32 numWords;
        PDWord wordArray;

        ASInt32 numPages = PDDocGetNumPages(inAPDoc.getPDDoc());
        for (ASInt32 pageNum = 0; pageNum < numPages; ++pageNum) {

            PDWordFinderAcquireWordList(wordFinder, pageNum, &wordArray, NULL, NULL, &numWords);

            std::cout << "Extracting a region of words on page " << pageNum << " of "
                      << csInputFileName.c_str() << " and saving to " << csOutputFileName.c_str()
                      << std::endl;

            // Step 3) Select the Words in the specified region
            for (ASInt32 wordNum = 0; wordNum < numWords; ++wordNum) {

                ASFixedQuad wordQuad;

                PDWord pdWord = PDWordFinderGetNthWord(wordFinder, wordNum);
                ASInt32 numQuads = PDWordGetNumQuads(pdWord);

                // A Word typically has only 1 quad, but can have more than one for hyphenated words, words on a curve, etc.
                for (ASInt32 quadNum = 0; quadNum < numQuads; ++quadNum) {

                    PDWordGetNthQuad(pdWord, quadNum, &wordQuad);

                    // Criteria: If the midpoint of any of it's quads intersects the target area
                    ASFixed centerH = (wordQuad.bl.h + wordQuad.br.h + wordQuad.tr.h + wordQuad.tl.h) / 4;
                    ASFixed centerV = (wordQuad.bl.v + wordQuad.br.v + wordQuad.tr.v + wordQuad.tl.v) / 4;

                    if ((centerH >= userTargetRegionFixedL && centerH <= userTargetRegionFixedR) &&
                        (centerV >= userTargetRegionFixedB && centerV <= userTargetRegionFixedT)) {
                        ASText asTextWord = ASTextNew();
                        PDWordGetASText(pdWord, 0, asTextWord);

                        // Get the endian neutral UTF-8 string.
                        ASUTF8Val *utf8String =
                            reinterpret_cast<ASUTF8Val *>(ASTextGetUnicodeCopy(asTextWord, kUTF8));

                        // Put this Word that is within our region in the output document
                        outputFile << utf8String << std::endl;
                        ASTextDestroy(asTextWord);
                        ASfree(utf8String);
                    }
                }
            }
            // Free the Word list for this page
            PDWordFinderReleaseWordList(wordFinder, pageNum);
        }

        // Close any remaining resources.
        // APDFLDoc's destructor will take care of closing the documents.
        outputFile.close();
        PDWordFinderDestroy(wordFinder);

    HANDLER
        errCode = ERRORCODE;
        libInit.displayError(errCode);
    END_HANDLER

    return errCode;
};
