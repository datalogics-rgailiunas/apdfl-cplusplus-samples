//
// Copyright (c) 2007-2017, Datalogics, Inc. All rights reserved.
//
// For complete copyright information, see:
// http://dev.datalogics.com/adobe-pdf-library/adobe-pdf-library-c-language-interface/license-for-downloaded-pdf-samples/
//
// Sample project: importPages -- demonstrates how to copy the contents of one PDF page and place these
// into a different PDF page, by creating a PDEForm containing the page contents.
// The PDEForm is created with the PDEContentAddPage PDF Library API call, and is scaled
// so that it occupies one-quarter of the page it's being inserted into.
//                                                                                       
// Note that this sample uses the media boxes of the input pages -- this may pick up
// artifacts outside of the page's crop boxes (such as cut marks and color bars) that
// are not usually visible.  If you want these to be omitted, use the page's crop box
// to process and set (not the media boxes, as are used here).
//
// Command-line arguments (all optional): <Input-File-1> <Input-File-2> <Output-File>
//

#include "PEWCalls.h"
#include "PERCalls.h"
#include "PagePDECntCalls.h"

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#define INPUT_DIR "../../Samples/_Input/"
#define PDF_FNAME1  "mergepdf1.pdf"
#define PDF_FNAME2  "mergepdf2.pdf"
#define OUTPUT_FILE "importPages-out.pdf"

int main (int argc, char **argv)
{
    APDFLib libInit;       // Initialize the Adobe PDF Library.  Termination will be automatic when scope is lost
    if (libInit.isValid() == false)
    {
        ASErrorCode errCode = libInit.getInitError();
        APDFLib::displayError ( errCode );
        return errCode;
    }

    std::string csInputFile1 ( argc > 1 ? argv[1] : INPUT_DIR PDF_FNAME1 );
    std::string csInputFile2 ( argc > 2 ? argv[2] : INPUT_DIR PDF_FNAME2 );
    std::string csOutputFile ( argc > 3 ? argv[3] : OUTPUT_FILE );
    std::cout << "Will combine files " << csInputFile1.c_str() << " and " << csInputFile2.c_str() 
              << " and write to file " << csOutputFile.c_str() << std::endl;

DURING

    // Open input documents
    APDFLDoc apdflDoc1 ( csInputFile1.c_str(), true ); 
    APDFLDoc apdflDoc2 ( csInputFile2.c_str(), true ); 
    PDDoc inputPDDoc1 = apdflDoc1.getPDDoc();
    PDDoc inputPDDoc2 = apdflDoc2.getPDDoc();

    PDDoc outputPDDoc = PDDocCreate();
    CosDoc outputCosDoc = PDDocGetCosDoc(outputPDDoc);

    PDPage inputPage1 = PDDocAcquirePage(inputPDDoc1, 0);
    PDPage inputPage2 = PDDocAcquirePage(inputPDDoc2, 0);
    ASFixedRect inputPageRect1, inputPageRect2;
    PDPageGetMediaBox(inputPage1, &inputPageRect1);
    PDPageGetMediaBox(inputPage2, &inputPageRect2);
    PDRotate inputRotate1 = PDPageGetRotate(inputPage1);
    PDRotate inputRotate2 = PDPageGetRotate(inputPage2);

    ASFixedRect outputPageRect;
    outputPageRect.left = outputPageRect.bottom = fixedZero;
    outputPageRect.right = outputPageRect.top = 500 * fixedOne;
    PDPage outputPDPage = PDDocCreatePage(outputPDDoc, PDBeforeFirstPage, outputPageRect);
    PDEContent outputCont = PDPageAcquirePDEContent(outputPDPage, 0);

    // Position this page in the lower-left corner
    ASFixedMatrix outputMatrix1;
    outputMatrix1.a = ASFixedDiv((outputPageRect.right - outputPageRect.left) / 2,
                                 (inputPageRect1.right - inputPageRect1.left));
    outputMatrix1.b = outputMatrix1.c = fixedZero;
    outputMatrix1.d = ASFixedDiv((outputPageRect.top - outputPageRect.bottom) / 2,
                                 (inputPageRect1.top - inputPageRect1.bottom));
    outputMatrix1.h = outputMatrix1.v = fixedZero;

    // If the input page was rotated 90 degrees (clockwise or counter-clockwise),
    // swap the a and d values in the output matrix --
    // the call to add the content will auto-rotate the created PDEForm, but we need
    // to adjust so that the form takes up the bottom one-quarter of the page */
    if ((inputRotate1 == pdRotate90) || (inputRotate1 == pdRotate270))
    {
        ASFixed tempVal = outputMatrix1.a;
        outputMatrix1.a = outputMatrix1.d;
        outputMatrix1.d = tempVal;
    }

    PDEContentAddPage(outputCont,
                      PDEContentGetNumElems(outputCont) - 1,
                      outputCosDoc,
                      inputPage1,
                      &outputMatrix1,
                      NULL,              // ASAtom[] of annotation types to copy
                      0,                 // flag to specify annotation types to copy
                      &inputPageRect1);  // bounding box - use NULL to specify page's crop box

    // Position this page in the upper-right corner
    ASFixedMatrix outputMatrix2;
    outputMatrix2.a = ASFixedDiv((outputPageRect.right - outputPageRect.left) / 2,
                                 (inputPageRect2.right - inputPageRect2.left));
    outputMatrix2.b = outputMatrix2.c = fixedZero;
    outputMatrix2.d = ASFixedDiv((outputPageRect.top - outputPageRect.bottom) / 2,
                                 (inputPageRect2.top - inputPageRect2.bottom));
    outputMatrix2.h = (outputPageRect.right - outputPageRect.left) / 2;
    outputMatrix2.v = (outputPageRect.top - outputPageRect.bottom) / 2;
    // And adjust this page's boundaries if it's rotated also...
    if ((inputRotate2 == pdRotate90) || (inputRotate2 == pdRotate270))
    {
        ASFixed tempVal = outputMatrix2.a;
        outputMatrix2.a = outputMatrix2.d;
        outputMatrix2.d = tempVal;
    }

    PDEContentAddPage(outputCont,
                      PDEContentGetNumElems(outputCont) - 1,
                      outputCosDoc,
                      inputPage2,
                      &outputMatrix2,
                      NULL,
                      0,
                      &inputPageRect2);

    PDPageSetPDEContent(outputPDPage, 0);
    PDPageReleasePDEContent(outputPDPage, 0);
    PDPageRelease(inputPage1);
    PDPageRelease(inputPage2);

    ASPathName path = APDFLDoc::makePath ( csOutputFile.c_str() );
    PDDocSave(outputPDDoc, PDSaveFull, path, ASGetDefaultFileSys(), NULL, NULL);
    ASFileSysReleasePath(NULL, path);
    PDDocClose(outputPDDoc);

HANDLER
    APDFLib::displayError(ERRORCODE);
    return ERRORCODE;
END_HANDLER

    return 0;
}

