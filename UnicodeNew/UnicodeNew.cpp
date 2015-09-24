// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//======================================================================================================================
// Sample: UnicodeNew - This sample demonstrates the use of Unicode text in APDFL. We place a variety of texts
//     in different languages and Unicode encodings, both vertically and horizontally placed, to the output document.
//
//Steps:
//Step 1) Declare the Unicode strings.
//Step 2) Load the necessary fonts.
//Step 3) Prepare the text objects and check for font compatibility.
//Step 4) Prepare to draw the texts to a new document.
//Step 5) Draw the texts, and subset the fonts we used.
//Step 6) Release resources, save, and close.
//======================================================================================================================

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

//========================================================================================================================================================================================================================================================================
//Step 1) Declare the Unicode strings.
//========================================================================================================================================================================================================================================================================

    //The following are all the unicode strings we intend to place. Note that every string lacks a BOM, lacks other identifiers, and is big endian.
    //These are all required characteristics for creating an ASText object from a unicode string (minus the endianness - one could also use the host machine's endianness,
    //but that is not garaunteed to be little-endian, so little endian is not demonstrated.)

    //Our English text is in UTF-8.
    //This could have been a regular, double-quoted string, but what the hey.
    ASUTF8Val englishStr_U8[] {
            0x55, 0x6E, 0x69, 0x76, 0x65, 0x72, 0x73, 0x61,
            0x6C, 0x20, 0x44, 0x65, 0x63, 0x6C, 0x61, 0x72,
            0x61, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x6F, 0x66,
            0x20, 0x48, 0x75, 0x6D, 0x61, 0x6E, 0x20, 0x52,
            0x69, 0x67, 0x68, 0x74, 0x73, 0x00
    };

    //Our French text is in UTF-16B.
    ASUTF8Val frenchStr_U16B[] {
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
            0x00, 0x65, 0x00, 0x00
    };

    //Our Russian (cyrillic script) text is in UTF-16B.
    ASUTF8Val cyrillicStr_U16B[] {
            0x04, 0x12, 0x04, 0x41, 0x04, 0x35, 0x04, 0x3E,
            0x04, 0x31, 0x04, 0x49, 0x04, 0x30, 0x04, 0x4F,
            0x00, 0x20, 0x04, 0x34, 0x04, 0x35, 0x04, 0x3A,
            0x04, 0x3B, 0x04, 0x30, 0x04, 0x40, 0x04, 0x30,
            0x04, 0x46, 0x04, 0x38, 0x04, 0x4F, 0x00, 0x20,
            0x04, 0x3F, 0x04, 0x40, 0x04, 0x30, 0x04, 0x32,
            0x00, 0x20, 0x04, 0x47, 0x04, 0x35, 0x04, 0x3B,
            0x04, 0x3E, 0x04, 0x32, 0x04, 0x35, 0x04, 0x3A,
            0x04, 0x30, 0x00, 0x00
    };

    //Our Japanese text is in UTF-32B.
    ASUTF8Val japaneseStr_U32B[] {
            0x00, 0x00, 0x30, 0x0E, 0x00, 0x00, 0x4E, 0x16,
            0x00, 0x00, 0x75, 0x4C, 0x00, 0x00, 0x4E, 0xBA,
            0x00, 0x00, 0x6A, 0x29, 0x00, 0x00, 0x5B, 0xA3,
            0x00, 0x00, 0x8A, 0x00, 0x00, 0x00, 0x30, 0x0F,
            0x00, 0x00, 0x00, 0x00,
    };

    //Our Korean text is in UTF-32B.
    ASUTF8Val koreanStr_U32B[] {
            0x00, 0x00, 0xC1, 0x38, 0x00, 0x00, 0x00, 0x20,
            0x00, 0x00, 0xAC, 0xC4, 0x00, 0x00, 0x00, 0x20,
            0x00, 0x00, 0xC7, 0x78, 0x00, 0x00, 0x00, 0x20,
            0x00, 0x00, 0xAD, 0x8C, 0x00, 0x00, 0x00, 0x20,
            0x00, 0x00, 0xC1, 0x20, 0x00, 0x00, 0x00, 0x20,
            0x00, 0x00, 0xC5, 0xB8, 0x00, 0x00, 0x00, 0x00
    };

    DURING

//================================================================================================================================================================================================================
//Step 2) Load the necessary fonts.
//================================================================================================================================================================================================================

#define NUM_FONTS_SAMPLE 3                                                                                          //The number of fonts we're using. Must be accurate, of course.

        std::wcout << L"Initializing fonts." << std::endl;

        //The following define the font attribute objects as well as a horizontal and vertical version of each font we'll use.
        PDEFont courVFont, courHFont;                                                                               //Vertical and horizontal fonts for Courier, which we'll use for the English and French text.
        PDEFont kozgVFont, kozgHFont;                                                                               //For Kozgo, which we'll use for the Japanese and Cyrillic text.
        PDEFont myunVFont, myunHFont;                                                                               //For Myungjo, which we'll use for the Korean Text.

        //These arrays will be used to initialize our fonts. Each column here is for one font.
        const char *fontNames[] {"CourierStd",   "KozGoPr6N-Medium",   "AdobeMyungjoStd-Medium"};                   //These fonts should all be included with your PDFL distribution.
        PDEFont       *fontsV[] {&courVFont,     &kozgVFont,           &myunVFont              };                   //The vertical form of the font.
        PDEFont       *fontsH[] {&courHFont,     &kozgHFont,           &myunHFont              };                   //The horizontal form of the font.

        //These properties are common to all fonts, so are specified out here before the initialization loop.
        PDSysEncoding  iHEnc = PDSysEncodingCreateFromCMapName(ASAtomFromString("Identity-H"));                     //The system encoding used to translate code points to font GIDs for horizontally-texts.
        PDSysEncoding  iVEnc = PDSysEncodingCreateFromCMapName(ASAtomFromString("Identity-V"));                     //The same, for vertically-placed texts.
        const char *fontType = "Type0";                                                                             //All our fonts are this type, otherwise this would have been another array.
        PDEFontCreateFlags fontFlags = (PDEFontCreateFlags)
            (kPDEFontCreateEmbedded | kPDEFontWillSubset | kPDEFontCreateToUnicode | kPDEFontEncodeByGID);          //The flags CreateEmbedded, WillSubset, and CreateToUnicode are promises
                                                                                                                    //    on our part to do these things. See the "PDEFontSubsetNow" calls.

        //This loop will load each font according to the properties just specified. A PDFL exception will be thrown here and caught in the handler if a font can't be found.
        PDEFontAttrs nextFontAttrs;                                                                                 //Used to find the system font.
        PDSysFont nextSysFont;                                                                                      //Used in PDEFont creation.
        for (int i = 0; i < NUM_FONTS_SAMPLE; ++i)
        {
            memset(&nextFontAttrs, 0, sizeof(nextFontAttrs));
            nextFontAttrs.name = ASAtomFromString(fontNames[i]);
            nextFontAttrs.type = ASAtomFromString(fontType);

            nextSysFont = PDFindSysFont(&nextFontAttrs, sizeof(PDEFontAttrs), 0);

            *fontsV[i] = PDEFontCreateFromSysFontAndEncoding(nextSysFont, iVEnc, nextFontAttrs.name, fontFlags);
            *fontsH[i] = PDEFontCreateFromSysFontAndEncoding(nextSysFont, iHEnc, nextFontAttrs.name, fontFlags);
        }

//================================================================================================================================================================================================================
//Step 3) Prepare the text objects and check for font compatibility.
//================================================================================================================================================================================================================

#define NUM_TEXTS 5                                                                                                     //The number of Unicode text strings we'll draw. This must be accurate, of course.

        ASText enAST = ASTextFromUnicode((ASUTF16Val*)englishStr_U8, kUTF8);
        ASText enTTL = ASTextFromUnicode((ASUTF16Val*)"English",     kUTF8);                                            //This takes advantage of the fact that all ASCII code points are identical to their
                                                                                                                        //    UTF-8 counterparts. It's simpler if it's all ASText objects.
        ASText frAST = ASTextFromUnicode((ASUTF16Val*)frenchStr_U16B, kUTF16BigEndian);
        ASText frTTL = ASTextFromUnicode((ASUTF16Val*)"French",       kUTF8);

        ASText jnAST = ASTextFromUnicode((ASUTF16Val*)japaneseStr_U32B, kUTF32BigEndian);
        ASText jnTTL = ASTextFromUnicode((ASUTF16Val*)"Japanese",       kUTF8);

        ASText krAST = ASTextFromUnicode((ASUTF16Val*)koreanStr_U32B,   kUTF32BigEndian);
        ASText krTTL = ASTextFromUnicode((ASUTF16Val*)"Korean",         kUTF8);

        ASText ruAST = ASTextFromUnicode((ASUTF16Val*)cyrillicStr_U16B, kUTF16BigEndian);
        ASText ruTTL = ASTextFromUnicode((ASUTF16Val*)"Russian",        kUTF8);

        struct textAndFont {
            ASText*  text;                                                                                              //The ASText object containing the text we'll draw to the PDF.
            ASText* title;                                                                                              //The title for this font will be drawn with the horizontal text.
            PDEFont* fontH;                                                                                             //the horizontally-oriented font to use.
            PDEFont* fontV;                                                                                             //The vertically-oriented font to use.
        };

        //This array associates each ASText object with the vertical (V) and horizontal (H) fonts we want to use for it. This is the order they will be drawn in.
        textAndFont textsAndFonts[] {
            { &enAST, &enTTL, &courHFont, &courVFont},                                                                  //The English ASText and associated fonts.
            { &jnAST, &jnTTL, &kozgHFont, &kozgVFont},                                                                  //Japanese...
            { &frAST, &frTTL, &courHFont, &courVFont},                                                                  //French...
            { &krAST, &krTTL, &myunHFont, &myunVFont},                                                                  //Korean...
            { &ruAST, &ruTTL, &kozgHFont, &kozgVFont},                                                                  //Russian...
        };

        std::wcout << L"Checking font compatibility." << std::endl;
        
        //We need to check that each text is actually representable in the fonts we associated with it.
        //It is entirely possible, and indeed usual, for fonts to not support every possible Unicode character, even for common languages.
        //We will however assume that Courier, the font we're using for the titles, can support the titles, since they're just ASCII.
        ASUns32 firstBadGlyph = 0;                                                                                      //If the text is not representable, this will be set to the index in the
        for (int i = 0; i < NUM_TEXTS; ++i)                                                                             //     string of the first unrepresentable character.
        {
            for (int f = 0; f < 2; f++)                                                                                 //To iterate through each font
            {
                wchar_t* errString; PDEFont* nextFont;
                
                switch (f)
                {
                case (0):
                    errString = L"horizontal";
                    nextFont = textsAndFonts[i].fontH;
                    break;
                case (1) :
                    errString = L"vertical";
                    nextFont = textsAndFonts[i].fontV;
                    break;
                }

                if (!PDEFontCheckASTextIsRepresentable(*nextFont, *textsAndFonts[i].text, &firstBadGlyph))
                {
                    PDEFontAttrs badFontAttrs;
                    PDEFontGetAttrs(*nextFont, &badFontAttrs, sizeof(PDEFontAttrs));

                    std::wcout << L"Error: The " << errString << "font associated with the" << i << L"th text " << ASAtomGetString(badFontAttrs.name)
                        << L" has no glyph for the text's " << firstBadGlyph << L"th character." << std::endl;

                    return -1;
                }
            }
        }

//================================================================================================================================================================================================================
//Step 4) Prepare to draw the texts to a new document.
//================================================================================================================================================================================================================

        std::wcout << L"Creating a new document." << std::endl;

        //Create the output document
        APDFLDoc outDoc;

        outDoc.insertPage(FloatToASFixed(8.5 * 72),FloatToASFixed(11 * 72),kPDEBeforeFirst);     //Insert an 8.5" by 7.5" page for the horizontal text.
        outDoc.insertPage(FloatToASFixed(8.5 * 72),FloatToASFixed(11 * 72),kPDEBeforeFirst);     //Insert another for the vertical text.

        PDPage horzPage = outDoc.getPage(0);
        PDEContent horzPageCont = PDPageAcquirePDEContent(horzPage, 0);
        PDEText horzTexts = PDETextCreate();                                                     //We'll fill this with the horizontal texts and add it to the first page.

        PDPage vertPage = outDoc.getPage(1);
        PDEContent vertPageCont = PDPageAcquirePDEContent(vertPage, 0);
        PDEText vertTexts = PDETextCreate();                                                     //We'll fill this with the vertical texts and add it to the second page.

        ASFixedRect pageRect;                                                                    //We must know the bounds of the page to place text on it.
        PDPageGetCropBox(horzPage, &pageRect);                                                   //The page rect is the same for both pages.

        //We will track two matrices for placing text: one for the horizontal text on the first page, one for the horizontal text on the second page.
        ASFixedMatrix horzPlaceMatrix;
        memset(&horzPlaceMatrix, 0, sizeof(horzPlaceMatrix));
        ASFixedMatrix vertPlaceMatrix;
        memset(&vertPlaceMatrix, 0, sizeof(vertPlaceMatrix));

        //They'll both start in the same (relative) place and define the same font size, but they will be moved around for new text differently.
        vertPlaceMatrix.a = horzPlaceMatrix.a = Int16ToFixed(14);                                //Font width, in points.
        vertPlaceMatrix.d = horzPlaceMatrix.d = Int16ToFixed(14);                                //Font height, in points.
        vertPlaceMatrix.h = horzPlaceMatrix.h = Int16ToFixed(1 * 72);                            //Starting X coordinate on page. One inch from left side.
        vertPlaceMatrix.v = horzPlaceMatrix.v = pageRect.top - Int16ToFixed(1 * 72);             //Starting Y coordinate on page.One inch from the top.

        //We'll draw the text with whatever the default graphics state is.
        PDEGraphicState graphics;
        PDEDefaultGState(&graphics, sizeof(PDEGraphicState));

//================================================================================================================================================================================================================
//Step 5) Draw the texts, and subset the fonts we used.
//================================================================================================================================================================================================================

        std::wcout << L"Placing the Unicode text." << std::endl;

        //This loop will iterate through each ASText, placing both the horizontal and vertical forms, as well as the title for the horizontal text.
        for (int i = 0; i < NUM_TEXTS; ++i)
        {
            //The title for the horizontal text. We'll use the Courier horizontal font for consistency.
            PDETextAddASText(horzTexts, kPDETextRun, i, *textsAndFonts[i].title, courHFont, &graphics, sizeof(graphics), NULL, 0, &horzPlaceMatrix);
            horzPlaceMatrix.v -= horzPlaceMatrix.d + Int16ToFixed(10);                //Advance the y value by the height of the font, plus a little padding.

            //The horizontal text. This procedure converts the Unicode into Glyph IDs for specified font.
            PDETextAddASText(horzTexts, kPDETextRun, i+NUM_TEXTS, *textsAndFonts[i].text, *textsAndFonts[i].fontH, &graphics, sizeof(graphics), NULL, 0, &horzPlaceMatrix);
            horzPlaceMatrix.v -= horzPlaceMatrix.d + Int16ToFixed(30);                //Advance the y value by the height of the font, plus a lot of padding.

            //The vertical text. Same as above.
            PDETextAddASText(vertTexts, kPDETextRun, i, *textsAndFonts[i].text, *textsAndFonts[i].fontV, &graphics, sizeof(graphics), NULL, 0, &vertPlaceMatrix);
            vertPlaceMatrix.h += vertPlaceMatrix.a + Int16ToFixed(20);                //Advance the x value by the width of the font, plus some padding.
        }

        //Add the text elements to the pages.
        PDEContentAddElem(horzPageCont, kPDEBeforeFirst, (PDEElement) horzTexts);     //Add the text element to the page.
        PDPageSetPDEContentCanRaise(horzPage, 0);                                     //Set the new content into the page.

        PDEContentAddElem(vertPageCont, kPDEBeforeFirst, (PDEElement) vertTexts);
        PDPageSetPDEContentCanRaise(vertPage, 0);

        std::wcout << L"Subsetting the fonts." << std::endl;

        //Simply calling "SubsetNow" subsets the font, creates a ToUnicode table (which maps Glyph IDs to Unicode), 
        //and creates widths. You don't need to call all three methods separately anymore.
        //Remember, if you do this, you must use the kPDEFontWillSubset font flag during font retrieval, and vice versa.
        CosDoc cosDoc = PDDocGetCosDoc(outDoc.getPDDoc());
        for (int i = 0; i < NUM_TEXTS; ++i)
        {
            PDEFontSubsetNow(*textsAndFonts[i].fontV, cosDoc);
            PDEFontSubsetNow(*textsAndFonts[i].fontH, cosDoc);
        }

//================================================================================================================================================================================================================
//Step 6) Release resources, save, and close.
//================================================================================================================================================================================================================

        //Release the text objects.
        PDERelease((PDEObject)horzTexts);
        PDERelease((PDEObject)vertTexts);

        //Release the PDEContents so we can release the pages.
        PDPageReleasePDEContent(horzPage, 0);
        PDPageReleasePDEContent(vertPage, 0);

        //Release the pages so we can close the document.
        PDPageRelease(horzPage);
        PDPageRelease(vertPage);

        //Destroy all the ASText objects we made.
        //We can't also release the fonts with textsAndFonts, since some texts use the same fonts.
        for (int i = 0; i < NUM_TEXTS; ++i)
        {
            ASTextDestroy(*textsAndFonts[i].text);             
            ASTextDestroy(*textsAndFonts[i].title);
        }

        //Release the PDSysEncodings
        PDERelease((PDEObject)iHEnc);
        PDERelease((PDEObject)iVEnc);

        //Release all the fonts.
        for (int i = 0; i < NUM_FONTS_SAMPLE; ++i)
        {
            PDERelease((PDEObject)*fontsH[i]);
            PDERelease((PDEObject)*fontsV[i]);
        }

        //And graphics we used.
        PDERelease((PDEObject)graphics.fillColorSpec.space);
        PDERelease((PDEObject)graphics.strokeColorSpec.space);

        outDoc.saveDoc(L"UnicodeNew.pdf");                        //APDFLDoc's destructor will take care of closing the document.

        std::wcout << L"Success." << std::endl;

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);                             //If there was an error, display it.

    END_HANDLER

    return errCode;                                            //lib's destructor terminates the library.
}
