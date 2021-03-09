//
// Copyright (c) 2021, Datalogics, Inc. All rights reserved.
//
// For complete copyright information, refer to:
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
// This sample demonstrates using the DocTextFinder to find examples of a specific phrase in a PDF
// document that matches a user-supplied regular expression. When the sample finds the text it
// highlights each example and saves the file as an output document.
//
// Command-line:   <input-file> <output-file> <search-regular-expression>    (All optional)
//
// For more detail see the description of the RegexTextSearch sample program on our Developer’s site,
// http://dev.datalogics.com/adobe-pdf-library/sample-program-descriptions/c1samples#regextextsearch

#include <iostream>
#include "InitializeLibrary.h"
#include "APDFLDoc.h"
#include "CosCalls.h"
#include "DLExtrasCalls.h"

#define DIR_LOC "../../../../Resources/Sample_Input/"
#define DEF_INPUT "TextSearch.pdf"
#define DEF_OUTPUT "RegexTextSearch-out.pdf"
#define DEF_SEARCH_REGEX R"([Tt]he)"

static void ApplyQuadsToAnnot(PDAnnot, ASFixedQuad *, ASArraySize);
static void AnnotateWord(PDWord, PDPage, PDColorValue);

int main(int argc, char *argv[]) {
    APDFLib libInit;
    ASErrorCode errCode = 0;

    if (libInit.isValid() == false) {
        errCode = libInit.getInitError();
        std::cout << "Initialization failed with code " << errCode << std::endl;
        return libInit.getInitError();
    }

    std::string csInputFileName(argc > 1 ? argv[1] : DIR_LOC DEF_INPUT);
    std::string csOutputFileName(argc > 2 ? argv[2] : DEF_OUTPUT);
    std::string csSearchRegex(argc > 3 ? argv[3] : DEF_SEARCH_REGEX);
    std::cout << "Will search " << csInputFileName.c_str() << " for all occurrences of "
              << "the regular expression \"" << csSearchRegex.c_str() << "\"," << std::endl
              << "highlight them all, "
              << " and save to " << csOutputFileName.c_str() << std::endl;

    DURING

        APDFLDoc document(csInputFileName.c_str(), true);

        // Step 1) Set up the word finder configuration record.

        PDWordFinderConfigRec wfConfig;

        memset(&wfConfig, 0, sizeof(wfConfig));           // Always do this!
        wfConfig.recSize = sizeof(PDWordFinderConfigRec); //...and this!

        wfConfig.disableTaggedPDF = true; // Treat this as a non-tagged PDF document.
        wfConfig.noXYSort = true;         // Don't generate an XY-ordered word list.
        wfConfig.preserveSpaces = true;   // Preserve spaces during word breaking.
        wfConfig.noLigatureExp = false; // Enable expansion of ligatures using the default ligatures.
        wfConfig.noEncodingGuess = true; // Disable guessing encoding of fonts with unknown/custom encoding.
        wfConfig.unknownToStdEnc = false;   // Don't assume all fonts are Standard Roman.
                                            //   Setting to true overrides noEncodingGuess.
        wfConfig.ignoreCharGaps = false;    // Convert large character gaps to spaces.
        wfConfig.ignoreLineGaps = false;    // Treat vertical movements as line breaks.
        wfConfig.noAnnots = true;           // Don't extract from annotations.
        wfConfig.noHyphenDetection = false; // Don't differentiate between hard and soft hyphens.
        wfConfig.trustNBSpace = false; // Don't differentiate between breaking and non-breaking spaces.
        wfConfig.noExtCharOffset = false; // If client doesn't have a need for detailed character
                                          //   offset information set to true for improvement in efficiency.
        wfConfig.noStyleInfo = false;  // Set to true if client doesn't have a need for style
                                       //   information for improvement in efficiency.
        wfConfig.decomposeTbl = NULL;  // Table may be used to expand unicode ligatures
                                       //   not in the default list.
        wfConfig.decomposeTblSize = 0; // Not using decomposeTbl, so the size is 0.
        wfConfig.charTypeTbl = NULL;   // Custom table to enhance word breaking quality.
        wfConfig.charTypeTblSize = 0;  // Unused, so the size will be 0.
        wfConfig.preserveRedundantChars = false; // May be used to preserve overlapping redundant
                                                 //   characters in some PDF documents.
        wfConfig.disableCharReordering = false;  // Used in cases where the PDF page has heavily
                                                 //   overlapped character bounding boxes.

        // Step 2) Fill in color information for highlighting text. In this case, our color will be set to orange.

        PDColorValueRec colorValRec;
        PDColorValue pdColorValue(&colorValRec);
        pdColorValue->space = PDDeviceRGB;             // Colors are set using the RGB color space.
        pdColorValue->value[0] = ASFloatToFixed(1.0);  // red level
        pdColorValue->value[1] = ASFloatToFixed(0.65); // green level
        pdColorValue->value[2] = ASFloatToFixed(0.0);  // blue level

        // Step 3) Search for the text that matches the regular expression and highlight all occurrences.

        ASInt32 numberOfMatches = 0;
        PDDocTextFinder matchFinder = PDDocTextFinderCreate(WF_LATEST_VERSION, false, &wfConfig);
        PDDocTextFinderAcquireMatchList(matchFinder, document.getPDDoc(), PDAllPages, NULL,
                                        csSearchRegex.c_str(), &numberOfMatches);

        char phraseBuf[256];
        ASInt32 numberOfWords = 0;
        PDDocTextFinderWordMatchRec wordRec;

        // Iterate over the matches that were found by DocTextFinder
        for (ASInt32 matchInstance = 0; matchInstance < numberOfMatches; ++matchInstance) {

            // Get the match
            memset(phraseBuf, 0, sizeof(phraseBuf));
            PDDocTextFinderGetNthMatch(matchFinder, matchInstance, phraseBuf, sizeof(phraseBuf), &numberOfWords);

            // Examine each word of the match and highlight it
            for (ASInt32 wordInstance = 0; wordInstance < numberOfWords; ++wordInstance) {
                wordRec = PDDocTextFinderGetMatchNthWord(matchFinder, matchInstance, wordInstance);

                // The PDPage object will be needed for adding the highlight annotation
                PDPage pdPage = document.getPage(wordRec.pageNum);
                AnnotateWord(wordRec.word, pdPage, pdColorValue);

                PDPageRelease(pdPage);
            }
        }

        PDDocTextFinderDestroy(matchFinder);
        document.saveDoc(csOutputFileName.c_str());

    HANDLER
        errCode = ERRORCODE;
        libInit.displayError(errCode);
    END_HANDLER

    return errCode;
}

// Add quads to an annotation's CosObj.
// NOTE: There is Adobe documentation that specifies quads be added in the order - BL, BR, TR, TL.
// However, currently the order needs to be BL, BR, TL, TR to get correct output.
void ApplyQuadsToAnnot(PDAnnot annot, ASFixedQuad *quads, ASArraySize numQuads) {
    CosObj coAnnot = PDAnnotGetCosObj(annot);
    CosDoc coDoc = CosObjGetDoc(coAnnot);
    CosObj coQuads = CosNewArray(coDoc, false, numQuads * 8);
    static ASAtom atQP = ASAtomFromString("QuadPoints");

    for (ASUns32 i = 0, n = 0; i < numQuads; ++i) {
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].bl.h));
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].bl.v));
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].br.h));
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].br.v));
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].tl.h)); // See note above
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].tl.v));
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].tr.h)); // See note above
        CosArrayPut(coQuads, n++, CosNewFixed(coDoc, false, quads[i].tr.v));
    }

    CosDictPut(coAnnot, atQP, coQuads);
}

void AnnotateWord(PDWord w, PDPage p, PDColorValue c) {
    static ASAtom atH = ASAtomFromString("Highlight");

    // A value of -2 adds the annotation to the end of the page's annotation array
    ASInt32 addAfterCode = -2;

    // Coordinates of the annotation
    ASFixedQuad tempQuad;
    PDWordGetNthQuad(w, 0, &tempQuad);

    ASFixedRect annotationRect;
    annotationRect.left = tempQuad.bl.h;
    annotationRect.top = tempQuad.tr.v;
    annotationRect.right = tempQuad.tr.h;
    annotationRect.bottom = tempQuad.bl.v;

    // Create the annotation
    PDAnnot highlight = PDPageCreateAnnot(p, atH, &annotationRect);

    // Set its coordinates and color
    ApplyQuadsToAnnot(highlight, &tempQuad, 1);
    PDAnnotSetColor(highlight, c);

    PDPageAddAnnot(p, addAfterCode, highlight);
}
