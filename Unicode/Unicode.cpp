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


int main(int argc, char** argv)
{
    APDFLib lib;                                         //Initialize the Adobe PDF Library.

    if (lib.isValid() == false)                          //If it failed to initialize, return the error code.
        return lib.getInitError();

    ASErrorCode errCode = 0;                             //This will catch error codes thrown during library usage.

    //Note the lack of BOM and any other identifiers.

    //UTF8. This could have been a regular, double-quoted string.
    ASUTF8Val englishStr_U8[] = {
        0x55, 0x6E, 0x69, 0x76, 0x65,
        0x72, 0x73, 0x61, 0x6C, 0x20,
        0x44, 0x65, 0x63, 0x6C, 0x61,
        0x72, 0x61, 0x74, 0x69, 0x6F,
        0x6E, 0x20, 0x6F, 0x66, 0x20,
        0x48, 0x75, 0x6D, 0x61, 0x6E,
        0x20, 0x52, 0x69, 0x67, 0x68,
        0x74, 0x73,
        0x00};

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

    //UTF16LE
    ASUTF8Val cyrillicStr_U16L[] = {
            0x1f, 0x04, 0x3e, 0x04, 0x36, 0x04, 0x30, 0x04,
            0x3b, 0x04, 0x43, 0x04, 0x39, 0x04, 0x41, 0x04,
            0x42, 0x04, 0x30, 0x04, 0x2c, 0x00, 0x20, 0x00,
            0x34, 0x04, 0x30, 0x04, 0x39, 0x04, 0x42, 0x04,
            0x35, 0x04, 0x20, 0x00, 0x3d, 0x04, 0x30, 0x04,
            0x3c, 0x04, 0x20, 0x00, 0x3f, 0x04, 0x3e, 0x04,
            0x3b, 0x04, 0x43, 0x04, 0x40, 0x04, 0x30, 0x04,
            0x41, 0x04, 0x3f, 0x04, 0x30, 0x04, 0x34, 0x04,
            0x30, 0x04, 0x20, 0x00, 0x42, 0x04, 0x40, 0x04,
            0x00, 0x38, 0x04, 0x2c, 0x00, 0x20, 0x00, 0x30,
            0x04, 0x20, 0x00, 0x42, 0x04, 0x3e, 0x04, 0x20,
            0x00, 0x4f, 0x04, 0x20, 0x00, 0x31, 0x04, 0x43,
            0x04, 0x34, 0x04, 0x43, 0x04,
            0x00, 0x00,
    };

    //UTF32BE
    ASUTF8Val japaneseStr_U32B[] = {
            0x00, 0x00, 0x30, 0x0E, 0x00, 0x00, 0x4E, 0x16,
            0x00, 0x00, 0x75, 0x4C, 0x00, 0x00, 0x4E, 0xBA,
            0x00, 0x00, 0x6A, 0x29, 0x00, 0x00, 0x5B, 0xA3,
            0x00, 0x00, 0x8A, 0x00, 0x00, 0x00, 0x30, 0x0F,
            0x00, 0x00, 0x00, 0x00,
    };
    //UTF32LE
    ASUTF8Val koreanStr_U32L[] = {
        0x7C, 0xC7, 0x00, 0x00, 0x44, 0xB1, 0x00, 0x00,
        0xD0, 0xC5, 0x00, 0x00, 0x44, 0xC5, 0x00, 0x00,
        0x0C, 0xBE, 0x00, 0x00, 0x7C, 0xB7, 0x00, 0x00,
        0x68, 0xD5, 0x00, 0x00, 0x58, 0xC7, 0x00, 0x00,
        0x3C, 0xBB, 0x00, 0x00, 0x40, 0xC7, 0x00, 0x00,
        0xC8, 0xB2, 0x00, 0x00, 0x1C, 0xC8, 0x00, 0x00,
        0x74, 0xB9, 0x00, 0x00, 0x15, 0xAC, 0x00, 0x00,
        0x4D, 0xD6, 0x00, 0x00, 0x18, 0xC2, 0x00, 0x00,
        0x83, 0xAC, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    DURING



///////////////
//// RETRIEVE FONTS 
//////////////

        char* euFontName = "CourierStd";             //this font will be used for English, French, and Cyrillic text.
        char* jnFontName = "KozGoPr6N-Medium";       //This font will be used for Japanese text.
        char* krFontName = "AdobeMyungjoStd-Medium"; //This font will be used for Korean text.

        char* fontType = "Type0";                //All fonts used are Type0.

        char* horizEnc = "Identity-H";           //The encoding we'll use for horizontal text.

        char* vertEnc  = "Identity-V";           //The encoding we'll use for vertical text.

        PDEFont euVFont;
        PDEFont euHFont;
        PDEFontAttrs euFontAttrs;

        PDEFont jnVFont;
        PDEFont jnHFont;
        PDEFontAttrs jnFontAttrs;

        PDEFont krVFont;
        PDEFont krHFont;
        PDEFontAttrs krFontAttrs;

        memset(&euFontAttrs, 0, sizeof(euFontAttrs));
        memset(&jnFontAttrs, 0, sizeof(jnFontAttrs));
        memset(&krFontAttrs, 0, sizeof(krFontAttrs));

        euFontAttrs.name = ASAtomFromString(euFontName);
        jnFontAttrs.name = ASAtomFromString(jnFontName);
        krFontAttrs.name = ASAtomFromString(krFontName);

        euFontAttrs.type = jnFontAttrs.type = krFontAttrs.type = ASAtomFromString(fontType);

        PDSysEncoding identityHEnc = PDSysEncodingCreateFromCMapName(ASAtomFromString(vertEnc));
        PDSysEncoding identityVEnc = PDSysEncodingCreateFromCMapName(ASAtomFromString(vertEnc));

        PDEFontCreateFlags fontFlags = (PDEFontCreateFlags)
            (kPDEFontCreateEmbedded | kPDEFontWillSubset | kPDEFontCreateToUnicode | kPDEFontEncodeByGID);

        PDSysFont nextSysFont;

        nextSysFont = PDFindSysFont(&euFontAttrs, sizeof(PDEFontAttrs), 0);
        euVFont = PDEFontCreateFromSysFontAndEncoding(nextSysFont, identityVEnc, euFontAttrs.name, fontFlags);
        //euHFont = PDEFontCreateFromSysFontAndEncoding(nextSysFont, identityHEnc, euFontAttrs.name, fontFlags);

        nextSysFont = PDFindSysFont(&jnFontAttrs, sizeof(PDEFontAttrs), 0);
        jnVFont = PDEFontCreateFromSysFontAndEncoding(nextSysFont, identityVEnc, jnFontAttrs.name, fontFlags);
        //jnHFont = PDEFontCreateFromSysFontAndEncoding(nextSysFont, identityHEnc, jnFontAttrs.name, fontFlags);

        nextSysFont = PDFindSysFont(&krFontAttrs, sizeof(PDEFontAttrs), 0);
        krVFont = PDEFontCreateFromSysFontAndEncoding(nextSysFont, identityVEnc, krFontAttrs.name, fontFlags);
        //krHFont = PDEFontCreateFromSysFontAndEncoding(nextSysFont, identityHEnc, krFontAttrs.name, fontFlags);

///////////////
//// NEXT?
//////////////



        //APDFLDoc outDoc;
        //outDoc.insertPage(8 * 72, 11 * 72,kPDEBeforeFirst); //Insert an 8.5" by 7.5" page.
        //PDPage outPage = outDoc.getPage(0);
        //PDEContent outPageCont = PDPageAcquirePDEContent(outPage, 0);





        //see orig smp
        PDEColorSpace ColorSpace = PDEColorSpaceCreateFromName(ASAtomFromString("DeviceGray"));
        memset(ColorSpace, 0, sizeof(PDEGraphicState));
        PDEGraphicState gState;
        gState.strokeColorSpec.space = gState.fillColorSpec.space = ColorSpace;
        gState.miterLimit = fixedTen;
        gState.flatness = fixedOne;
        gState.lineWidth = fixedOne;

        //PREPARE FOR VERTICAL PLACEMENT
        ASFixedRect pageRect;
        //PDPageGetCropBox(outPage, &pageRect);

        ASFixedMatrix strPlaceMatrix;
        strPlaceMatrix.a = Int16ToFixed(14); //Font width/height
        strPlaceMatrix.d = Int16ToFixed(18); //18-pt size...
        strPlaceMatrix.h = Int16ToFixed(1 * 72); //x,y coord on page.
        //strPlaceMatrix.v = pageRect.top - Int16ToFixed(1 * 72); //Begin 1 inch from top of page.

        //ASText engAST = ASTextNew();
        //engAST = ASTextFromUnicode((ASUTF16Val*)englishStr_U8, kUTF8);
        //std::cout << "yeay" << std::endl;

        /////I think it is likely that non utf-8 needs to be in two-byte strings.

        //ASText frAST  = ASTextFromUnicode((ASUTF16Val*)frenchStr_U16B, kUTF16BigEndian);
        //std::cout << "yeay" << std::endl;
        //ASText jnAST = ASTextFromUnicode((ASUTF16Val*)japaneseStr_U32B, kUTF32BigEndian);
        //std::cout << "yeay" << std::endl;
        //
        //ASUns32 firstBadGlyph;
        //if (PDEFontCheckASTextIsRepresentable(euVFont, engAST, &firstBadGlyph))
        //{
        //}
        //else
        //{
        //    std::cout << "uh oh, brother! E" << std::endl;
        //}
        //ASUns32 ThirstBadGlyph;
        //if (PDEFontCheckASTextIsRepresentable(euVFont, frAST, &ThirstBadGlyph))
        //{
        //}
        //else
        //{
        //    std::cout << "uh oh, brother! f" << std::endl;
        //}
        //ASUns32 thirstBadGlyph;
        //if (PDEFontCheckASTextIsRepresentable(jnVFont, jnAST, &thirstBadGlyph))
        //{
        //}
        //else
        //{
        //    std::cout << "uh oh, brother! J" << std::endl;
        //}


        PDERelease((PDEObject)euVFont);
        PDERelease((PDEObject)jnVFont);
        PDERelease((PDEObject)krVFont);
       // PDERelease((PDEObject)euHFont);
       // PDERelease((PDEObject)jnHFont);
       // PDERelease((PDEObject)krHFont);
        PDERelease((PDEObject)ColorSpace);
        //ASTextDestroy(engAST);
        //ASTextDestroy(jnAST);
        //ASTextDestroy(frAST);

        //PDPageReleasePDEContent(outPage, 0);
        //PDPageRelease(outPage);




    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);                             //If there was an error, display it.

    END_HANDLER

    return errCode;                                            //lib's destructor terminates the library.
}
