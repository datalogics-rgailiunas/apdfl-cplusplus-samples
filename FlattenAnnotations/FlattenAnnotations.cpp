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
// See chapter 8.4.4 in the PDF Reference, version 1.7, for more information on
// annotation appearances.
//
// Steps:
// 1) Convert each Annotation's appearance stream, if it has one, into a Form 
//      XObject and remove the annotation.
// 2) Save and close.
//=====================================================================================

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#include "PEWCalls.h"
#include "PERCalls.h"
#include "PagePDECntCalls.h"
#include "CosCalls.h"

//A CosObjEnumProc which puts the first entry of the CosDict obj, val, into clientData, and stops.
ASBool getFirstElement(CosObj obj, CosObj val, void* clientData);

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

    for (int i = PDPageGetNumAnnots(page) - 1; i >= 0; --i)                                                        //Must be done in reverse order, because the annotation array is updated with each removal. See the documentation for PDPageRemoveAnnot.
    {
        //Get the next annotation.
        PDAnnot next = PDPageGetAnnot(page, i);
        CosObj annotCos = PDAnnotGetCosObj(next);

        CosObj appearanceStrm = CosNewNull();  //Will default to CosNull if we can't find an appearance stream for this annotation.

        //Try to find this annotation's appearance stream, which may contain the resources CosObj we need to create the PDEForm of its appearance.
        if (CosDictKnownKeyString(annotCos, "AP"))                        //The appearance dictionary of our annotation.
        {
            CosObj APDict = CosDictGetKeyString(annotCos, "AP");
            if (CosDictKnownKeyString(APDict, "N"))
            {
                CosObj normal = CosDictGetKeyString(APDict, "N");       //The normal appearance of our annotation.

                //The normal appearance is either a stream, in which case it it what we want, or it is a dictionary, 
                //in which case we'll have to get the appearance stream from the appearance state ("AS").
                if (CosObjGetType(normal) == CosStream)
                    appearanceStrm = normal;
                else
                {
                    if (CosDictKnownKeyString(annotCos, "AS"))
                    {
                        ASAtom appearanceName = CosNameValue(CosDictGetKeyString(annotCos, "AS"));

                        if (CosDictKnown(normal, appearanceName))
                            appearanceStrm = CosDictGet(normal, appearanceName);
                    }
                }
            }
        }

        //If we found the appearance stream, we must find its resources entry. Otherwise the annotation has no appearance.
        CosObj resource = CosNewNull();
        if (CosObjGetType(appearanceStrm) != CosNull)
        {
            resource = CosDictGetKeyString(appearanceStrm, "Resources");

            //If the appearance stream doesn't have a Resources entry, we must look for an appearance
            //that might have been inherited from a parent page in the page tree.
            if (CosObjGetType(resource) == CosNull)
            {
                CosObj pageObj = PDPageGetCosObj(page);
                while (CosObjGetType(resource) == CosNull)
                {
                    resource = CosDictGetKeyString(pageObj, "Resources");
                    if (CosObjGetType(resource) == CosNull)
                    {
                        pageObj = CosDictGetKeyString(pageObj, "Parent");
                        if (CosObjGetType(pageObj) == CosNull)
                            break;
                    }
                    else
                        break;
                }
            }
        }
        
        if (CosObjGetType(resource) != CosNull)
        {
            // Place the annotation's resources in the page's content
            ASFixedRect nextLoc;
            PDAnnotGetRect(next, &nextLoc);
            ASDoubleMatrix unity;
            unity.a = unity.d = 1.0;
            unity.b = unity.c = 0.0;
            unity.h = (ASDouble)ASFixedToFloat(nextLoc.left);
            unity.v = (ASDouble)ASFixedToFloat(nextLoc.bottom);

            //Create and add the form xobject.
            PDEForm formXObject = PDEFormCreateFromCosObjEx(&appearanceStrm, &resource, &unity);
            PDEContentAddElem(pageContent, kPDEAfterLast, (PDEElement)formXObject);

            PDERelease((PDEObject)formXObject);
        }
        else
        {
            //This annotation has no appearance.
            std::wcout << L"Warning: The " << i << L"th annotation, a " << ASAtomGetString(PDAnnotGetSubtype(next)) << 
                L", has no contained or inherited resources entry, so has no appearance. It will still be removed." << std::endl;
        }

        PDPageRemoveAnnot(page, i);

    }
        PDPageSetPDEContentCanRaise(page, 0);                                                                          //Set all this new content into the page.

//======================================================================================================================================================================================================================================================================
// 2) Save and close.
//======================================================================================================================================================================================================================================================================

    std::wcout << L"Saving..." << std::endl;

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
