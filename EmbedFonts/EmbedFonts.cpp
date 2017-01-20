//
// Copyright (c) 2010-2017, Datalogics, Inc. All rights reserved.
//
// For complete copyright information, see:
// http://dev.datalogics.com/adobe-pdf-library/adobe-pdf-library-c-language-interface/license-for-downloaded-pdf-samples/
//
// Project:  FontEmbedder 
//
//  This sample for the PDF Library demonstrates the following:
//      - Scanning a PDF document and obtaining information on the embedding status of
//          each font in the PDF document
//      - Subsetting each font in the PDF document that is not embedded in the PDF
//
//  As an overview, this sample carries out the following high-level steps:
//
//  1) Prints each font used in the PDF file and its embedding status
//  2) Determine which fonts in the PDF file are not embedded
//  3) Determine if there is a suitable font available to use to subset each unembedded font,
//      and notifies the PDF Library to use the system font for the unembedded font
//  4) For each unembedded font, this sample
//      a) Scans each page in the PDF file to see if there is text set in the font
//      b) If there is, the sample gathers the text in the unembedded font and signals to
//          the PDF Library that the text is used in that font
//  5) After (4) is performed, for each unembedded font this sample
//      a) Requests that the PDF Library generate a subset font stream for the characters
//          that we've signaled are used from the font & embeds this font subset datastream
//      b) Generates a new name for the font to signal that it was embedded and subset
//
//  One may see significant speed improvements over large documents with many unembedded fonts
//  if steps 4 and 5 are collapsed into one scan over each page for all unembedded fonts --
//  it's clearer for example purposes to keep these two steps separate.
//
//  Note that the default input file for this program has been custom crafted with some embedded and some
//  unembedded fonts.  If you re-run the program a second time, using the output of the first run as the
//  input file, you will see that the fonts got embedded.

#include "PSFCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#include "EmbedFonts.h"

#define INPUT_DIR "../../Samples/_Input/"
#define INPUT_NAME  "EmbedFonts-in.pdf"
#define OUTPUT_NAME  "EmbedFonts-out.pdf"
    
int main ( int argc, char **argv)
{
    APDFLib libInit;       // Initialize the library
    if (libInit.isValid() == false)
    {
        ASErrorCode errCode = libInit.getInitError();
        APDFLib::displayError(errCode);
        return errCode;
    }

    std::string csInputFileName(argc>1 ? argv[1] : INPUT_DIR INPUT_NAME);
    std::string csOutputFileName(argc>2 ? argv[2] : OUTPUT_NAME);
    std::cout << "Will embed fonts into " << csInputFileName.c_str() << " and rewrite file as "
              << csOutputFileName.c_str() << std::endl;

    ContainerOfFonts vFontsInUse;

DURING

    // Open the input document
    APDFLDoc apdflDoc ( csInputFileName.c_str(), true );
    PDDoc pdDoc = apdflDoc.getPDDoc();

    // Enumerate the fonts used in the document, and save those currently unembedded
    PDDocEnumFonts(pdDoc, 0, PDDocGetNumPages(pdDoc) - 1, GetFontInfoProc, &vFontsInUse, 0, 0);

    // Embed a suitable system font for each font in the to-embed list
    ContainerOfFonts::iterator it, itE = vFontsInUse.end();
    for ( it = vFontsInUse.begin(); it != itE; ++it )
    {
        EmbedSysFontForFontEntry ( *it, pdDoc);
    
        // Perform clean up while we're here
        delete *it;
    }
   
    // Save the PDF to a new file
    apdflDoc.saveDoc ( csOutputFileName.c_str(), PDSaveFull | PDSaveCollectGarbage );

HANDLER
    APDFLib::displayError(ERRORCODE);
    return ERRORCODE;
END_HANDLER
 
    return 0;
}

void EmbedSysFontForFontEntry(struct _t_pdfUsedFont *fontEntry, PDDoc pdDoc)
{
DURING
    fontEntry->pdeFont = PDEFontCreateFromCosObj(&(fontEntry->fontCosObj) );
    fontEntry->pdSysFont = PDFindSysFontForPDEFont(fontEntry->pdeFont, kPDSysFontMatchNameAndCharSet);

    // If the font is not found on the system, sysFont will be 0.
    if ( 0 == fontEntry->pdSysFont )
    {
        std::cout << "Could not find a pdSysFont " << std::endl;
        return;
    }
     
    PDEFontSetSysFont(fontEntry->pdeFont, fontEntry->pdSysFont);

    PDEFontAttrs attrs;
    memset(&attrs, 0, sizeof(attrs));
    PDSysFontGetAttrs(fontEntry->pdSysFont, &attrs, sizeof(PDEFontAttrs));
    if (attrs.cantEmbed != 0)
    {
        std::cout << "Font " <<  ASAtomGetString(attrs.name) << " cannot be embedded" << std::endl;
    }
    else
    {
        if (PDEFontIsMultiByte(fontEntry->pdeFont))
        {
            // Subset embed font
            PDEFontCreateFromSysFont(fontEntry->pdSysFont, kPDEFontCreateEmbedded | kPDEFontCreateSubset);
            PDEFontSubsetNow(fontEntry->pdeFont, PDDocGetCosDoc(pdDoc));
        }
        else
        {
            // Fully embed font
            PDEFontCreateFromSysFont(fontEntry->pdSysFont, kPDEFontCreateEmbedded);
            PDEFontEmbedNow(fontEntry->pdeFont, PDDocGetCosDoc(pdDoc));
        }
    }
    // TODO: We chould check system font for at least high-level compatibility, to be certain
    
HANDLER
    APDFLib::displayError(ERRORCODE);
END_HANDLER
}

ACCB1 ASBool ACCB2 GetFontInfoProc(PDFont pdFont, PDFontFlags *pdFontFlagsPtr, void *clientData)
{
    CosObj cosFont;
    PDEFontAttrs attrs;
    PDSysFont sysFont;
    char fontNameBuf[PSNAMESIZE];
    const char  *fontSubtypeP;
    char        *fontNameStart = 0;
    ASBool      fontEmbedded = false,
                fontSubset = false,
                fontIsSysFont = false;
DURING
    memset(&attrs, 0, sizeof(attrs));

    PDFontGetName(pdFont, fontNameBuf, PSNAMESIZE);
    attrs.name = ASAtomFromString(fontNameBuf);
    attrs.type = PDFontGetSubtype(pdFont);
    fontSubtypeP = ASAtomGetString(attrs.type);

    fontEmbedded = PDFontIsEmbedded(pdFont);
    // Subset test: a font was subset if the 7th character is '+' (a plus-sign), according
    // to Acrobat/Reader and industry norms
    if (fontEmbedded)
    {
        if ((strlen(fontNameBuf)) > 7 && (fontNameBuf[6] == '+') )
            fontSubset = true;
    }
    if (fontSubset)
        fontNameStart = fontNameBuf + 7;    // skip the "ABCDEF+"
    else
        fontNameStart = fontNameBuf;

    // If the font is not found on the system, sysFont will be 0.
    sysFont = PDFindSysFont(&attrs, sizeof(PDEFontAttrs), 0);
    if (sysFont)
        fontIsSysFont = true;

    // Print font information
    std::cout << "Font " << fontNameStart << ", Subtype " << fontSubtypeP << " (" <<
                 ( fontIsSysFont ? "" : "Not a " ) << "System Font, ";
    if ( fontEmbedded )
    {
        std::cout << "embedded" << ( fontSubset ? " subset" : "" );
    }
    else
    {
        std::cout << "unembedded";
    }
    std::cout << ")" << std::endl;

    // Add font to the list of fonts to be subset -- this example only subsets System
    // fonts that are not embedded in this PDF document; it doesn't subset fonts
    // that are fully embedded in the PDF file.
    if (fontIsSysFont && !fontEmbedded)
    {
        pContainerOfFonts pcf = (pContainerOfFonts)clientData;
        cosFont = PDFontGetCosObj(pdFont);
        pcf->push_back ( new _t_pdfUsedFont ( cosFont ) );
    }
HANDLER
    std::cout << "Exception raised in GetFontInfoProc(): "; 
    APDFLib::displayError(ERRORCODE);
END_HANDLER

    return true;
}


