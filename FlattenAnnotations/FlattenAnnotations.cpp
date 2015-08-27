// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//=====================================================================================
// Sample: FlattenAnnotations - Demonstrates flattening annotations.
//
// Note:
// This sample merges the appearance (AP) dictionaries of all annotations on the page 
// (if they have AP dictionaries) into the page's content stream, by converting them 
// into Form Xobjects.
//
// Steps:
// 1) Convert each Annotation into a Form XObject, and remove the annotation.
// 2) Save and close.
//=====================================================================================

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#include "PEWCalls.h"
#include "PERCalls.h"
#include "PagePDECntCalls.h"
#include "CosCalls.h"

int main(int argc, char** argv)
{
    APDFLib libInit;                                               //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;                                       //Variable used to report any exceptions or errors, if they occur.

    if (libInit.isValid() == false)                                //If there was a problem in initialization, return the error code.
        return libInit.getInitError();

    DURING

    APDFLDoc doc(L"../_Input/FlattenAnnotations.pdf", true);       //Open the input document, repairing it if necessary.

    PDPage page = doc.getPage(0);
    PDEContent  pageContent = PDPageAcquirePDEContent(page, 0);

//======================================================================================================================================================================================================================================================================
// 1) Convert each Annotation into a Form Xobject, and remove the annotation.
//======================================================================================================================================================================================================================================================================

    std::wcout << L"This page has " << PDPageGetNumAnnots(page) << L" annotations." << std::endl;
    std::wcout << L"Flattening them..." << std::endl;

    for (int i = PDPageGetNumAnnots(page) - 1; i >= 0; --i)                                              //Must be done in reverse order, because the annotation array is updated with each removal. See the documentation for PDPageRemoveAnnot.
    {
        //Get the next annotation and its data.
        PDAnnot next = PDPageGetAnnot(page, i);                                                          //The annotation itself.
        ASFixedRect nextLoc;                                                                             //Its location on the page.
        PDAnnotGetRect(next, &nextLoc);
        CosObj  appearanceDict = CosDictGetKeyString(PDAnnotGetCosObj(next), "AP");                      //The annotation's appearance dictionary.

        //Not all Annotations have Appearance dictionaries. Indeed, Links do not, and annotations created with APDFL will not until the PDF is opened in Acrobat.
        if (CosObjGetType(appearanceDict) != CosNull)
        {
            CosObj NormalAppearanceDict = CosDictGetKeyString(appearanceDict, "N");                      //Appearance streams for annotations have three types: N for normal appearance, R for rollover appearance, and D for down appearance. Including N is required.
            CosObj resources = CosDictGetKeyString(NormalAppearanceDict, "Resources");                   //The resources stream of that type.

            ASFixedMatrix unity;                                                                         //The transformation matrix for placing the PDEForm. We'll place it exactly where the annotation was.
            unity.a = unity.d = fixedOne;
            unity.b = unity.c = 0;
            unity.h = nextLoc.left;
            unity.v = nextLoc.bottom;

            PDEForm formXObject = PDEFormCreateFromCosObj(&NormalAppearanceDict, &resources, &unity);    //Create the Form XObject to match the annotation's normal appearance.

            PDEContentAddElem(pageContent, kPDEAfterLast, (PDEElement)formXObject);                      //Add it to the page.

            PDERelease((PDEObject)formXObject);
        }
        else
        {
            std::wcout << L"Annotation " << i + 1 << L" (" << ASAtomGetString(PDAnnotGetSubtype(next)) << L")"
                << L" has no Appearance Dictionary, so it cannot be flattened. It will be removed." << std::endl;
        }

        PDPageRemoveAnnot(page, i);

    }                                                                                                    //See the documentation for PDPageRemoveAnnot.

    PDPageSetPDEContentCanRaise(page, 0);                                                                //Set all this new content into the page.

//======================================================================================================================================================================================================================================================================
// 2) Save and close.
//======================================================================================================================================================================================================================================================================

    //Release resources.
    PDPageReleasePDEContent(page, 0);
    PDPageRelease(page);

    doc.saveDoc(L"Flattened.pdf");                 //APDFLDoc's destructor takes care of closing the rest of the PDDoc's resources.

    HANDLER

        errCode = ERRORCODE;
        libInit.displayError(errCode);             //If there was an error, display it.

    END_HANDLER

    if (!errCode)
        std::wcout << L"Success!" << std::endl;

    return errCode;                                //APDFLib's destructor terminates the library.
}
