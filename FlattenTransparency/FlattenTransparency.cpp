// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//===================================================================================
// Sample: FlattenPDF - Uses the PDFlattener plugin to flatten the input document.
//
// Note:
// PDFlattener will only flatten pages that include transparent elements.
//
// Steps:
// Step 1) Initialize the PDFLattener plugin.
// Step 2) Configure the PDFlattener parameters.
// Step 3) Call the PDFlattener.
// Step 4) Save the document, close it, and terminate the plugin.
//===================================================================================

#include <iomanip>

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#include "PagePDECntCalls.h"
#include "PDFlattenerCalls.h"    //Flattener plugin headers

//ASBool callback function: A function for PDFlattener which monitors the flattener's progress.
ASBool flattenerProgMon(ASInt32 pageNum, ASInt32 totalPages, float current, ASInt32 reserved, void *clientData);

int main(int argc, char** argv)
{
    APDFLib libInit;                                            //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;                                    //Variable used to report any exceptions/errors if they occured.

    if (libInit.isValid() == false)                             //If there was a problem in initialization, return the error code.
        return libInit.getInitError();

    DURING

        APDFLDoc doc(L"../_Input/FlattenTransparency.pdf", true);    //Open the input document, repairing it if it's damaged.

//===================================================================================================================================================================================================================================
// Step 1) Initialize the PDFLattener plugin.
//===================================================================================================================================================================================================================================

        gPDFlattenerHFT = InitPDFlattenerHFT;                                              //Sets the correct location for the PDFlattener function table.

        if (!PDFlattenerInitialize())
        {
            std::wcout << L"The PDFlattener plugin failed to initialize." << std::endl;
            E_RETURN(-1);
        }

//===================================================================================================================================================================================================================================
// Step 2) Configure the PDFlattener parameters.
//===================================================================================================================================================================================================================================

        PDFlattenerUserParamsRec flattenParams;
        memset(&flattenParams,0,sizeof (PDFlattenerUserParamsRec));
        flattenParams.size = sizeof(PDFlattenerUserParamsRec);

        //////////////////////////////
        // Appearance options.      //
        //////////////////////////////

        flattenParams.profileDesc = ASTextFromUnicode((ASUTF16Val*)"sRGB IEC61966-2.1", kUTF8);    //A profiled color space to use for transparent objects. For CMYK, use "U.S. Web Coated (SWOP)v2".
        flattenParams.colorCompression = kPDFlattenerZipCompression;                               //The ZIP compression scheme (Flate encoding) for images.
        flattenParams.transQuality     = 100.0f;                                                   //Raster/Vector balance. Use 0.00f for no vectors.

        //////////////////////////////
        // Callback options.        //
        //////////////////////////////

        ASInt32 currentPage = -1;
        flattenParams.progressClientData = (void*)&currentPage;                                    //Progress monitor callback data. I'm using this data to store the previous page the Flattener was working on, using -1 as "hasn't begun yet".
        flattenParams.flattenProgress    = flattenerProgMon;                                       //The progress monitor callback function.


        //////////////////////////////
        // Tile flattening options. //
        //////////////////////////////
        PDFlattenRec flattener;
        memset(&flattener,0,sizeof (PDFlattenRec));
        flattener.size = sizeof(PDFlattenRec);

        flattener.tilingMode  = kPDNoTiling;                                                       //The tiling mode.
        flattener.tileSizePts = 0;                                                                 //Target tile size, in points.

        flattener.internalDPI = 800.0f;                                                            //Resolution for flattening the interior of an atomic region.
        flattener.externalDPI = 200.0f;                                                            //Resolution for flattening edges of atomic regions.

        flattener.clipComplexRegions = false;                                                      //If complex regions should be clipped.
        flattener.strokeToFill       = true;                                                       //If we convert stroked elements to filled elements.
        flattener.useTextOutlines    = false;                                                      //If we use rastered text instead of native text.
        flattener.preserveOverprint  = true;                                                       //If we attempt to preserve overprint

        flattener.allowShadingOutput       = true;                                                 //Allow shading output.
        flattener.allowLevel3ShadingOutput = true;                                                 //Allow level 3 shading output.

        flattener.maxFltnrImageSize = 0;                                                           //Maximum image size while flattening. 0 is default.
        flattener.adaptiveThreshold = 0;                                                           //Adaptive flattening threshold. Doesn't matter, since we're not doing adaptive tiling. See tilingMode.

        flattenParams.flattenParams = &flattener;

//===================================================================================================================================================================================================================================
// Step 3) Call the PDFlattener.
//===================================================================================================================================================================================================================================

        ASUns32 numFlattened = 0;

        PDDoc pddoc = doc.getPDDoc();
        ASInt32 result = 0;                                                       //PDFlattenerConvertEx2 will set this to 0 if flattening failed.
        result = PDFlattenerConvertEx2(pddoc,                                     //The document whose pages we wish to flatten.
                                       0,                                         //The first page to flatten.
                                       PDDocGetNumPages(pddoc)-1,                 //The last page to flatten.
                                       &numFlattened,                             //PDFlattener sets this to the number of pages it flattened. It will not flatten pages that do not contain transparent elements.
                                       &flattenParams);                           //Flattener options.

        if(result)
            std::wcout << L"I flattened " << numFlattened << " pages." << std::endl;
        else
        {
            std::wcout << L"Flattening failed." << std::endl;
            E_RETURN(result);
        }

//===================================================================================================================================================================================================================================
// Step 4) Save the document, close it, and terminate the plugin.
//===================================================================================================================================================================================================================================

        doc.saveDoc(L"FlattenedTransparency.pdf");            //Save the document. APDFLDoc defaults to using the "PDSaveFull" flag while saving.
                                                     //APDFLDoc's destructor takes care of closing the document and releasing the rest of its resources.

        ASTextDestroy(flattenParams.profileDesc);    //Release resources. Hey, we only needed one creation!

        PDFlattenerTerminate();                      //Terminate the PDFlattener plugin.

    HANDLER

        errCode = ERRORCODE;
            libInit.displayError(errCode);           //If there was an error, display it.

    END_HANDLER

    if (!errCode)
        std::wcout << L"Success!" << std::endl;

    return errCode;                                  //APDFLib's destructor terminates the library.
}

//===================================================================================================================================================================================================================================
//ASBool callback function: A function for PDFlattener which monitors the flattener's progress.
//
//Note:
//    clientData is an ASInt32* representing the previous page we were working on flattening. It must start with a page number that doesn't 
//    exist, like -1. With it, we have this function print the progress only every time a new page is begun, or when the Flattener is finished.
//===================================================================================================================================================================================================================================
ASBool flattenerProgMon(ASInt32 pageNum, ASInt32 totalPages, float current, ASInt32 reserved, void *clientData)
{
    //The previous page we were working on.
    ASInt32* prevPage = (ASInt32*)clientData;

    //If we've begun a new page, or if we've finished.
    if (pageNum != (*prevPage) || current == 100.0f)
    {
        //Print the completion percentage.
        std::wcout << L"[" << std::fixed << std::setw(6) << std::setfill(L'0') << std::setprecision(2) << current << L"%] ";

        //Print the current page.
        std::wcout << L"Flattening page " << pageNum+1 << L" of " << totalPages << L". " << std::endl;

        //Update previous page.
        *prevPage = pageNum;
    }

    //Return 1 to cancel Flattening.
    return 0;
}
