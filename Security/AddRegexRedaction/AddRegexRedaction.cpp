//
// Copyright (c) 2021, Datalogics, Inc. All rights reserved.
//
// For complete copyright information, refer to:
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
// The AddRegexRedaction sample program uses DocTextFinder to locate matches to be
// redacted in a PDF document when given a user-supplied regular expression. The text is
// permanently removed from the document.
//
// Command-line:  <input-file> <output-file> <unredacted-output-file> <regular-expression> (Optional)
//
// For more detail see the description of the AddRegexRedaction sample program on our Developer’s site,
// http://dev.datalogics.com/adobe-pdf-library/sample-program-descriptions/c1samples#addregexredaction

#include <map>
#include "APDFLDoc.h"
#include "InitializeLibrary.h"
#include "DLExtrasCalls.h"

#define INPUT_LOC "../../../../Resources/Sample_Input/"
#define DEF_INPUT "AddRedaction.pdf"
#define DEF_OUTPUT "AddRegexRedaction-out.pdf"
#define DEF_UNREDACTED_OUTPUT "AddRegexRedaction-NotApplied-out.pdf"

#ifdef UNIX_PLATFORM
#define DEF_SEARCH_REGEX "[Nn]avigation|screen"
#else
#define DEF_SEARCH_REGEX R"([Nn]avigation|screen)"
#endif

int main(int argc, char **argv) {
    ASErrorCode errCode = 0;
    APDFLib libInit;

    if (libInit.isValid() == false) {
        errCode = libInit.getInitError();
        std::cout << "Initialization failed with code " << errCode << std::endl;
        return errCode;
    }

    std::string csInputFileName(argc > 1 ? argv[1] : INPUT_LOC DEF_INPUT);
    std::string csOutputFileName(argc > 2 ? argv[2] : DEF_OUTPUT);
    std::string csOutputUnredactedFileName(argc > 3 ? argv[3] : DEF_UNREDACTED_OUTPUT);
    std::string csSearchRegex(argc > 4 ? argv[4] : DEF_SEARCH_REGEX);
    std::cout << "Redacting regular expression matches for \"";
    std::cout << DEF_SEARCH_REGEX;
    std::cout << "\" from " << csInputFileName.c_str() << ", saving to " << csOutputFileName.c_str()
              << std::endl;

    DURING

        APDFLDoc document(csInputFileName.c_str(), true);

        // Step 1) Use DocTextFinder to locate matches that will be redacted and save their locations.

        std::map<ASInt32, std::vector<ASFixedQuad> > pageQuadMap;

        // Set the default word finder settings.
        PDWordFinderConfigRec wfConfig;
        memset(&wfConfig, 0, sizeof(PDWordFinderConfigRec));
        wfConfig.recSize = sizeof(PDWordFinderConfigRec);

        // Create the DocTextFinder object and use it to find matches.
        ASInt32 numberOfMatches = 0;
        PDDocTextFinder matchFinder = PDDocTextFinderCreate(WF_LATEST_VERSION, false, &wfConfig);
        PDDocTextFinderAcquireMatchList(matchFinder, document.getPDDoc(), PDAllPages, NULL,
                                        csSearchRegex.c_str(), &numberOfMatches);

        char phraseBuf[256];
        ASInt32 numberOfWords = 0;
        PDDocTextFinderWordMatchRec wordRec;

        // Iterate over the matches that were found by DocTextFinder.
        for (ASInt32 matchInstance = 0; matchInstance < numberOfMatches; ++matchInstance) {

            // Get the match.
            memset(phraseBuf, 0, sizeof(phraseBuf));
            PDDocTextFinderGetNthMatch(matchFinder, matchInstance, phraseBuf, sizeof(phraseBuf), &numberOfWords);

            // Examine each word of the match and store the quad location.
            for (ASInt32 wordInstance = 0; wordInstance < numberOfWords; ++wordInstance) {
                wordRec = PDDocTextFinderGetMatchNthWord(matchFinder, matchInstance, wordInstance);

                ASFixedQuad tempQuad;
                PDWordGetNthQuad(wordRec.word, 0, &tempQuad);
                pageQuadMap[wordRec.pageNum].push_back(tempQuad);
            }
        }

        // Step 2) Create and apply the redactions.

        if (pageQuadMap.size() > 0) {
            PDRedactParams redactParams;
            PDRedactParamsRec rpRec;
            memset((char *)&rpRec, 0, sizeof(PDRedactParamsRec));
            redactParams = &rpRec;

            PDColorValueRec cvRec;
            PDColorValueRec borderCVRec;
            PDColorValueRec fillCVRec;
            PDColorValueRec textCVRec;

            redactParams->size = sizeof(PDRedactParamsRec);
            redactParams->colorVal = &cvRec;
            redactParams->colorVal->space = PDDeviceRGB; // Set device color space to RGB.
            redactParams->colorVal->value[0] = FloatToASFixed(0.0); // The redaction box will be set to black.
            redactParams->colorVal->value[1] = FloatToASFixed(0.0);
            redactParams->colorVal->value[2] = FloatToASFixed(0.0);
            redactParams->horizAlign = kPDHorizCenter; // Horizontal alignment of the text when
                                                       // generating the redaction mark.

            // Describe the appearance of the text that is drawn to overlay the quads, when they are
            // redacted, or when they are viewed as if redacted.
            redactParams->overlayText = ASTextFromPDText(" Redacted "); // Overlay text may be used to overprint
                                                                        // the redacted areas.
            redactParams->textColor = &textCVRec;
            redactParams->textColor->space = PDDeviceGray; // Draw the overlay text in white.
            redactParams->textColor->value[0] = fixedOne;
            redactParams->textFontName = "CourierStd"; // Draw the overlay text using CourierStd font.
            redactParams->textSize = FloatToASFixed(10.0); // Draw the text size as 10 point (nominally, may
                                                           // be reduced if ScaleToFit is true).
            redactParams->repeat = true; // Draw the text in each quad, repeating to fill the quad.
            redactParams->scaleToFit = true; // Reduce text size if needed, to fit the quad.

            // Describe the appearance of the quads when they are drawn prior to redaction.
            redactParams->fillColor = &fillCVRec; // In the "normal" or "unredacted" appearance, fill each
            redactParams->fillColor->space = PDDeviceRGB; // quad with this color, at the specified opacity.
            redactParams->fillColor->value[0] = FloatToASFixed(1.0);
            redactParams->fillColor->value[1] =
                FloatToASFixed(0.0); // In this case, 25% red. Note that if opacity is set
            redactParams->fillColor->value[2] =
                FloatToASFixed(0.0); // to 0, there will be no visible color, and if opacity
            redactParams->fillOpacity = FloatToASFixed(0.25); // is set to 1, the underlaying text cannot be seen.
            redactParams->borderColor = &borderCVRec; // Set the color to draw the border around each quad,
            redactParams->borderColor->space = PDDeviceGray; // in the unredacted appearance.

            // Count how many words will be removed.
            size_t numOfWordsRemoved = 0;

            std::map<ASInt32, std::vector<ASFixedQuad> >::iterator iter;

            // Pass the page numbers and quads of all the matches to redactParams and create the redactions.
            for (iter = pageQuadMap.begin(); iter != pageQuadMap.end(); ++iter) {
                redactParams->pageNum = iter->first; // The page that the redaction will be applied to.
                redactParams->redactQuads = &(iter->second).front(); // The vector or array holding the quads.
                redactParams->numQuads = (iter->second).size(); // The number of entries in the vector or array.

                numOfWordsRemoved += (iter->second).size();

                // Create the redaction annotation. At this point the text HAS NOT been redacted.
                PDAnnot redactAnnot = PDDocCreateRedaction(document.getPDDoc(), redactParams);
            }

            // Save the document with the redactions created, but not applied.
            document.saveDoc(csOutputUnredactedFileName.c_str(), true);

            // Apply the redactions. IMPORTANT: until PDDocApplyRedactions is called, the
            // words are merely _marked for redaction_, but not removed!
            PDDocApplyRedactions(document.getPDDoc(), NULL);

            std::cout << numOfWordsRemoved << " words have been permanently removed... " << std::endl;
        } else {
            std::cout << "No words were matched, no redactions needed." << std::endl;
        }

        PDDocTextFinderDestroy(matchFinder);
        document.saveDoc(csOutputFileName.c_str(), true);

    HANDLER
        errCode = ERRORCODE;
        libInit.displayError(errCode);
    END_HANDLER

    return errCode; // APDFLib's destructor terminates the library.
}
