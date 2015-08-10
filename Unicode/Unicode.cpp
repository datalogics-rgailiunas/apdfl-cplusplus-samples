// Copyright(c) 2015, Datalogics, Inc.All rights reserved.

//======================================================================================================================
// Sample: ?
//
//Steps:
// Step 1) ?
//======================================================================================================================

// This agreement is between Datalogics, Inc. 101 N.Wacker Drive, Suite 1800,
// Chicago, IL 60606 ("Datalogics") and you, an end user who downloads
// source code examples for ingrating to the Adobe PDF Library
// ("the Example Code"). By accepting this agreement you agree to be bound
// by the following terms of use for the Example Code.
//
// LICENSE
// -------
// Datalogics hereby grants you a royalty - free, non - exclusive license to
// download and use the Example Code for any lawful purpose.There is no charge
// for use of Example Code.
//
// OWNERSHIP
// ---------
// The Example Code and any related documentation and trademarks are and shall
// remain the sole and exclusive property of Datalogics and are protected by
// the laws of copyright in the U.S.and other countries.
//
// Datalogics is a trademark of Datalogics, Inc.
//
// TERM
// ----
// This license is effective until terminated.You may terminate it at any
// other time by destroying the Example Code.
//
// WARRANTY DISCLAIMER
// -------------------
// THE EXAMPLE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
// EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// DATALOGICS DISCLAIM ALL OTHER WARRANTIES, CONDITIONS, UNDERTAKINGS OR
// TERMS OF ANY KIND, EXPRESS OR IMPLIED, WRITTEN OR ORAL, BY OPERATION OF
// LAW, ARISING BY STATUTE, COURSE OF DEALING, USAGE OF TRADE OR OTHERWISE,
// INCLUDING, WARRANTIES OR CONDITIONS OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, SATISFACTORY QUALITY, LACK OF VIRUSES, TITLE,
// NON - INFRINGEMENT, ACCURACY OR COMPLETENESS OF RESPONSES, RESULTS, AND / OR
// LACK OF WORKMANLIKE EFFORT.THE PROVISIONS OF THIS SECTION SET FORTH
// SUBLICENSEE'S SOLE REMEDY AND DATALOGICS'S SOLE LIABILITY WITH RESPECT
// TO THE WARRANTY SET FORTH HEREIN.NO REPRESENTATION OR OTHER AFFIRMATION
// OF FACT, INCLUDING STATEMENTS REGARDING PERFORMANCE OF THE EXAMPLE CODE,
// WHICH IS NOT CONTAINED IN THIS AGREEMENT, SHALL BE BINDING ON DATALOGICS.
// NEITHER DATALOGICS WARRANT AGAINST ANY BUG, ERROR, OMISSION, DEFECT,
// DEFICIENCY, OR NONCONFORMITY IN ANY EXAMPLE CODE.


#include "APDFLDoc.h"
#include "InitializeLibrary.h"

#include "PSFCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"
#include "DLExtrasCalls.h"


int main(int argc, char** argv)
{
    APDFLib lib;                                         //Initialize the Adobe PDF Library.

    if (lib.isValid() == false)                          //If it failed to initialize, return the error code.
        return lib.getInitError();

    ASErrorCode errCode = 0;                             //This will catch error codes thrown during library usage.

    //Note the lack of BOM and any other identifiers.
    //Note also the null-termination. These are all required characteristics for
    //creating an ASText object from a unicode string like these.

    //UTF8. This could have been a regular, double-quoted string.
    ASUTF8Val englishStr_U8[] = {
        0x55, 0x6E, 0x69, 0x76, 0x65, 0x72, 0x73, 0x61, 0x6C, 0x20,
        0x44, 0x65, 0x63, 0x6C, 0x61, 0x72, 0x61, 0x74, 0x69, 0x6F,
        0x6E, 0x20, 0x6F, 0x66, 0x20, 0x48, 0x75, 0x6D, 0x61, 0x6E,
        0x20, 0x52, 0x69, 0x67, 0x68, 0x74, 0x73,
        0x00
    };

    //UTF16BE
    ASUTF8Val frenchStr_U16B[] = {
            0x00, 0x44, 0x00, 0xE9, 0x00, 0x63, 0x00, 0x6C,
            0x00, 0x61, 0x00, 0x72, 0x00, 0x61, 0x00, 0x74,
            0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x20,
            0x00, 0x75, 0x00, 0x6E, 0x00, 0x69, 0x00, 0x76,
            0x00, 0x65, 0x00, 0x72, 0x00, 0x73, 0x00, 0x65,
            0x00, 0x6C, 0x00, 0x6C, 0x00, 0x65, 0x00, 0x20,
            0x00, 0x64, 0x00, 0x65, 0x00, 0x73, 0x00, 0x20,
            0x00, 0x64, 0x00, 0x72, 0x00, 0x6F, 0x00, 0x69,
            0x00, 0x74, 0x00, 0x73, 0x00, 0x20, 0x00, 0x64,
            0x00, 0x65, 0x00, 0x20, 0x00, 0x6C, 0x00, 0x27,
            0x00, 0x68, 0x00, 0x6F, 0x00, 0x6D, 0x00, 0x6D,
            0x00, 0x65,
            0x00, 0x00
    };
    //UTF16LE MUST BECOME BE!
    //ASUTF8Val cyrillicStr_U16L[] = {
    //        0x1f, 0x04, 0x3e, 0x04, 0x36, 0x04, 0x30, 0x04,
    //        0x3b, 0x04, 0x43, 0x04, 0x39, 0x04, 0x41, 0x04,
    //        0x42, 0x04, 0x30, 0x04, 0x2c, 0x00, 0x20, 0x00,
    //        0x34, 0x04, 0x30, 0x04, 0x39, 0x04, 0x42, 0x04,
    //        0x35, 0x04, 0x20, 0x00, 0x3d, 0x04, 0x30, 0x04,
    //        0x3c, 0x04, 0x20, 0x00, 0x3f, 0x04, 0x3e, 0x04,
    //        0x3b, 0x04, 0x43, 0x04, 0x40, 0x04, 0x30, 0x04,
    //        0x41, 0x04, 0x3f, 0x04, 0x30, 0x04, 0x34, 0x04,
    //        0x30, 0x04, 0x20, 0x00, 0x42, 0x04, 0x40, 0x04,
    //        0x00, 0x38, 0x04, 0x2c, 0x00, 0x20, 0x00, 0x30,
    //        0x04, 0x20, 0x00, 0x42, 0x04, 0x3e, 0x04, 0x20,
    //        0x00, 0x4f, 0x04, 0x20, 0x00, 0x31, 0x04, 0x43,
    //        0x04, 0x34, 0x04, 0x43, 0x04,
    //        0x00, 0x00,
    //};
    //UTF32BE
    ASUTF8Val japaneseStr_U32B[] = {
            0x00, 0x00, 0x30, 0x0E, 0x00, 0x00, 0x4E, 0x16,
            0x00, 0x00, 0x75, 0x4C, 0x00, 0x00, 0x4E, 0xBA,
            0x00, 0x00, 0x6A, 0x29, 0x00, 0x00, 0x5B, 0xA3,
            0x00, 0x00, 0x8A, 0x00, 0x00, 0x00, 0x30, 0x0F,
            0x00, 0x00, 0x00, 0x00,
    };
    ASUTF8Val koreanStr_U32B[] = {
        0x00, 0x00, 0xC7, 0x7C, 0x00, 0x00, 0xB1, 0x44,
        0x00, 0x00, 0xC5, 0xD0, 0x00, 0x00, 0xC5, 0x44,
        0x00, 0x00, 0xBE, 0x0C, 0x00, 0x00, 0xB7, 0x7C,
        0x00, 0x00, 0xD5, 0x68, 0x00, 0x00, 0xC7, 0x58,
        0x00, 0x00, 0xBB, 0x3C, 0x00, 0x00, 0xC7, 0x40,
        0x00, 0x00, 0xB2, 0xC8, 0x00, 0x00, 0xC8, 0x1C,
        0x00, 0x00, 0xB9, 0x74, 0x00, 0x00, 0xAC, 0x15,
        0x00, 0x00, 0xD6, 0x4D, 0x00, 0x00, 0xC2, 0x18,
        0x00, 0x00, 0xAC, 0x83,
        0x00, 0x00, 0x00, 0x00
    };

    DURING

///////////////
//// RETRIEVE FONTS 
//////////////

        char* vertEnc  = "Identity-V";           //The encoding we'll use for vertical text.
        char* horzEnc= "Identity-H";           //The encoding we'll use for vertical text.

        PDEFontAttrs euFontAttrs;
        char* euFontName = "CourierStd";             //this font will be used for English, French, and Cyrillic text.
        memset(&euFontAttrs, 0, sizeof(euFontAttrs));
        PDEFont euVFont;
        PDEFont euHFont;

        PDEFontAttrs jnFontAttrs;
        char* jnFontName = "KozGoPr6N-Medium";       //This font will be used for Japanese text.
        memset(&jnFontAttrs, 0, sizeof(jnFontAttrs));
        PDEFont jnVFont;
        PDEFont jnHFont;

        PDEFontAttrs krFontAttrs;
        char* krFontName = "AdobeMyungjoStd-Medium"; //This font will be used for Korean text.
        memset(&krFontAttrs, 0, sizeof(krFontAttrs));
        PDEFont krVFont;
        PDEFont krHFont;

        euFontAttrs.type = jnFontAttrs.type = krFontAttrs.type = ASAtomFromString("Type0");

        euFontAttrs.name = ASAtomFromString(euFontName);
        jnFontAttrs.name = ASAtomFromString(jnFontName);
        krFontAttrs.name = ASAtomFromString(krFontName);

        PDSysEncoding identityHEnc = PDSysEncodingCreateFromCMapName(ASAtomFromString(horzEnc));
        PDSysEncoding identityVEnc = PDSysEncodingCreateFromCMapName(ASAtomFromString(vertEnc));

        PDEFontCreateFlags fontFlags = (PDEFontCreateFlags)
            (kPDEFontCreateEmbedded | kPDEFontWillSubset | kPDEFontCreateToUnicode | kPDEFontEncodeByGID);

        PDSysFont nextSysFont;

        //these arrays are for looping that later, maybe.
        PDEFontAttrs *reqFontAt[] { &euFontAttrs, &euFontAttrs, &jnFontAttrs, &krFontAttrs};
        PDEFont *requiredFontsV[] { &euVFont,     &euVFont,     &jnVFont,     &krVFont};
        PDEFont *requiredFontsH[] { &euHFont,     &euHFont,     &jnHFont,     &krHFont};


        //Todo: loop this?
        nextSysFont = PDFindSysFont(&euFontAttrs, sizeof(PDEFontAttrs), 0);
        euVFont = PDEFontCreateFromSysFontAndEncoding(nextSysFont, identityVEnc, euFontAttrs.name, fontFlags);
        euHFont = PDEFontCreateFromSysFontAndEncoding(nextSysFont, identityHEnc, euFontAttrs.name, fontFlags);
      
        nextSysFont = PDFindSysFont(&jnFontAttrs, sizeof(PDEFontAttrs), 0);
        jnVFont = PDEFontCreateFromSysFontAndEncoding(nextSysFont, identityVEnc, jnFontAttrs.name, fontFlags);
        jnHFont = PDEFontCreateFromSysFontAndEncoding(nextSysFont, identityHEnc, jnFontAttrs.name, fontFlags);

        nextSysFont = PDFindSysFont(&krFontAttrs, sizeof(PDEFontAttrs), 0);
        krVFont = PDEFontCreateFromSysFontAndEncoding(nextSysFont, identityVEnc, krFontAttrs.name, fontFlags);
        krHFont = PDEFontCreateFromSysFontAndEncoding(nextSysFont, identityHEnc, krFontAttrs.name, fontFlags);

///////////////
//// PREPARE THE TEXT OBJETS!
//////////////

        //Looping this would be overkill.
        ASText enAST = ASTextFromUnicode((ASUTF16Val*)englishStr_U8, kUTF8);
        ASText frAST  = ASTextFromUnicode((ASUTF16Val*)frenchStr_U16B, kUTF16BigEndian);
        ASText jnAST = ASTextFromUnicode((ASUTF16Val*)japaneseStr_U32B, kUTF32BigEndian);
        ASText krAST = ASTextFromUnicode((ASUTF16Val*)koreanStr_U32B, kUTF32BigEndian);

#define NUM_TEXTS 4

        ASText *texts[] { &enAST,   &frAST,   &jnAST,   &krAST }; //todo: make it more obvious this corresponds to the arrays above.

        ASUns32 firstBadGlyph = 0;

        for (int i = 0; i < NUM_TEXTS; ++i)
        {
            if (!PDEFontCheckASTextIsRepresentable(*requiredFontsV[i], *texts[i], &firstBadGlyph)) //RequiredFontsV[i] and requiredFontsH[i] are just encoded differently, so should have the same glyphs.
            {
                PDEFontAttrs fontAtt;
                PDEFontGetAttrs(*requiredFontsV[i], &fontAtt, sizeof(PDEFontAttrs));
                std::wcout << L"Error: could not place " << i << "th text: " << ASAtomGetString(fontAtt.name)
                           << " is missing the glyph at position " << firstBadGlyph << "." << std::endl;
                return -1;
            }
        }


///////////////
//// PREPARE TO DRAW!
//////////////


        //Create the output document
        APDFLDoc outDoc;
        outDoc.insertPage(FloatToASFixed(8.5 * 72),FloatToASFixed(11 * 72),kPDEBeforeFirst); //Insert an 8.5" by 7.5" page.
        PDPage outPage = outDoc.getPage(0);
        PDEContent outPageCont = PDPageAcquirePDEContent(outPage, 0);
        PDEText pageText = PDETextCreate(); //We'll fill this with the texts.

        //PREPARE FOR GENERAL TEXT PLACMENT
        //Prepare matrices
        ASFixedRect pageRect;
        PDPageGetCropBox(outPage, &pageRect);
        ASFixedMatrix strPlaceMatrix;
        memset(&strPlaceMatrix, 0, sizeof(strPlaceMatrix));
        strPlaceMatrix.a = Int16ToFixed(20); //Font width, in points.
        strPlaceMatrix.d = Int16ToFixed(14); //Font height, in points.
        strPlaceMatrix.h = Int16ToFixed(2 * 72); //X coord on page. Two inches from left side.
        strPlaceMatrix.v = pageRect.top - Int16ToFixed(1 * 72); //Y coord on page.One inch from the top.

        //Prepare graphics state
        PDEColorSpace colorSpace = PDEColorSpaceCreateFromName(ASAtomFromString("DeviceGray"));
        PDEGraphicState gS;
        memset(&gS, 0, sizeof(PDEGraphicState));
        gS.strokeColorSpec.space = gS.fillColorSpec.space = colorSpace;
        bool isV = true;

        for (int i = 0; i < NUM_TEXTS; ++i)
        {
            PDEFont** nextFont = (isV ? requiredFontsV : requiredFontsH);
            PDETextAddASText(pageText, kPDETextRun, i, *texts[i], *nextFont[i],&gS, sizeof(gS), NULL, 0, &strPlaceMatrix);

            if (isV)
                strPlaceMatrix.h += strPlaceMatrix.a + Int16ToFixed(10); //Advance the x value by the width of the font, plus 10 points padding for ease of readability.
            else
                strPlaceMatrix.v += strPlaceMatrix.d + Int16ToFixed(10); //Advance the y value by the height of the font, plus padding.

        }
        PDEContentAddElem(outPageCont, kPDEBeforeFirst, (PDEElement) pageText);
        PDPageSetPDEContentCanRaise(outPage, 0);

        //Simply calling "SubsetNow" subsets the font, creates a ToUnicode table, and creates widths.
        CosDoc cosDoc = PDDocGetCosDoc(outDoc.getPDDoc());
        for (int i = 0; i < NUM_TEXTS; ++i)
        {
            PDEFontSubsetNow(*requiredFontsV[i], cosDoc);
            //TODO: add to subset the H fonts whenn you use them.
        }
        
        PDERelease((PDEObject)pageText);
        PDPageReleasePDEContent(outPage, 0);
        PDPageRelease(outPage);
        ASTextDestroy(enAST);
        ASTextDestroy(frAST);
        ASTextDestroy(jnAST);
        PDERelease((PDEObject)euVFont);
        PDERelease((PDEObject)jnVFont);
        PDERelease((PDEObject)krVFont);

        //N: this actually won't work, since you have the eu font twice to start. array the fonts each text uses? is that overkill?
        //for (int i = 0; i < NUM_FONTS; ++i)
        //{
        //    PDERelease((PDEObject)*requiredFontsV[i]);
        //    //todo: add H, when you use them.
        //}
        PDERelease((PDEObject)colorSpace);

        outDoc.saveDoc(L"Unicode.pdf");

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);                             //If there was an error, display it.

    END_HANDLER

    return errCode;                                            //lib's destructor terminates the library.
}
