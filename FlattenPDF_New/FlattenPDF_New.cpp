// Copyright (c) 2015, Datalogics, Inc. All rights reserved.
//
// http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
//
//===================================================================================
// Sample: FlattenPDF - Uses the PDFlattener plugin to flatten the input document.
//             The sample also removes all the Optional-Content Groups (or, layers).
//
// Note:
// PDFlattener will only flatten pages that include transparent elements.
// Thus, the number of pages it reports to have flattened will not necessarily be
// equal to the number of pages you've asked it to work through.
//
// Steps:
// Step 1) Remove the Optional-Content Groups (layers) in the PDDoc.
// Step 2) Initialize the PDFLattener plugin.
// Step 3) Configure the PDFlattener parameters.
// Step 4) Call the PDFlattener.
// Step 5) Save the document, close it, and terminate the plugin.
//===================================================================================

#include <iomanip>

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#include "PagePDECntCalls.h"
#include "PDFlattenerCalls.h"    //Flattener plugin headers

//ASBool callback function: A function for PDFlattener which monitors the flattener's progress.
ASBool flattenerProgMon(ASInt32 pageNum, ASInt32 totalPages, float current, ASInt32 reserved, void *clientData);

//ASBool callback function: A function used to count the number of Optional-Content Groups (layers) in a PDDoc.
ASBool countOptionalContentGroups(PDOCG ocg, void *clientData);

//ASBool callback function: Destroys all the Optional-Content Groups (layers) in a PDDoc.
ASBool destroyOptionalContentGroups(PDOCG ocg, void *clientData);

int main(int argc, char** argv)
{
    APDFLib libInit;                                     //Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0;                             //Variable used to report any exceptions/errors if they occured.

    if (libInit.isValid() == false)                      //If there was a problem in initialization, return the error code.
        return libInit.getInitError();

    DURING

        APDFLDoc doc(L"../_Input/FlattenPDF_New.pdf", true);    //Open the input document, repairing it if it's damaged.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Step 1) Remove the Optional-Content Groups (layers) in the PDDoc.
//
// Note: This is equivalent to just destroying all the layers. This will make all invisible layers visible.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ASInt32 initialNumLayers = 0;
        PDDocEnumOCGs(doc.getPDDoc(), countOptionalContentGroups, (void*)&initialNumLayers);      //This enumerates through each Optional-Content Group and counts them.

        std::wcout << L"The input PDDOC had " << initialNumLayers << L" layers." << std::endl;

        if (initialNumLayers > 0)
        {
            std::wcout << L"Removing them..." << std::endl;                                       //The destruction enumeration will throw an error if it could not remove a layer.

            PDDocEnumOCGs(doc.getPDDoc(), destroyOptionalContentGroups, NULL);                    //This enumerates through each Optional-Content Group and destroys them.

            std::wcout << L"I removed them." << std::endl;                                        //The destruction enumeration will throw an error if it could not remove a layer.
        }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Step 2) Initialize the PDFLattener plugin.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        gPDFlattenerHFT = InitPDFlattenerHFT;    //Sets the correct location for the PDFlattener function table.

        if (!PDFlattenerInitialize())
        {
            std::wcout << L"The PDFlattener plugin failed to initialize." << std::endl;
            E_RETURN(-1);
        }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Step 3) Configure the PDFlattener parameters.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        PDFlattenerUserParamsRec flattenParams;
        memset(&flattenParams,0,sizeof (PDFlattenerUserParamsRec));
        flattenParams.size = sizeof(PDFlattenerUserParamsRec);

        //////////////////////////////
        // Appearance options.      //
        //////////////////////////////

        flattenParams.profileDesc      = ASTextFromUnicode((ASUTF16Val*)"DeviceRGB",kUTF8);    //This color space will be used for transparent objects. This translation works because ASCII is equivalent to UTF-8.
        flattenParams.colorCompression = kPDFlattenerJpegCompression;                          //The ZIP compression scheme (Flate encoding) for images.
        flattenParams.transQuality     = 000.0f;                                               //Raster/Vector balance. Use 0.00f for no vectors.

        //////////////////////////////
        // Callback options.        //
        //////////////////////////////

        ASInt32 currentPage = -1;
        flattenParams.progressClientData = (void*)&currentPage;                                //progmon callback data. I'm using this data to store the previous page the Flattener was working on, using -1 as "hasn't begun yet".
        flattenParams.flattenProgress    = flattenerProgMon;                                   //The progress monitor callback function.


        //////////////////////////////
        // Tile flattening options. //
        //////////////////////////////
        PDFlattenRec flattener;
        memset(&flattener,0,sizeof (PDFlattenRec));
        flattener.size = sizeof(PDFlattenRec);

        flattener.tilingMode  = 1;                                                             //See the definition; 1 is constant tiling, 2 is adaptive tiling.
        flattener.tileSizePts = 20;                                                            //Target tile size, in points.

        flattener.internalDPI = 10.0f;                                                         //Resolution for flattening the interior of an atomic region.
        flattener.externalDPI = 10.0f;                                                         //Resolution for flattening edges of atomic regions.

        flattener.clipComplexRegions = false;                                                  //If complex regions should be clipped.
        flattener.strokeToFill       = true;                                                   //If we convert stroked elements to filled elements.
        flattener.useTextOutlines    = true;                                                   //If we use rastered text instead of native text.
        flattener.preserveOverprint  = true;                                                   //If we attempt to preserve overprint

        flattener.allowShadingOutput       = false;                                            //Allow shading output.
        flattener.allowLevel3ShadingOutput = false;                                            //Allow level 3 shading output.

        flattener.maxFltnrImageSize = 0;                                                       //Maximum image size while flattening. 0 is default.
        flattener.adaptiveThreshold = 0;                                                       //Adaptive flattening threshold. Doesn't matter, since we're not doing adaptive tiling. See tilingMode.

        flattenParams.flattenParams = &flattener;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Step 4) Call the PDFlattener.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ASUns32 numFlattened = 0;

        PDDoc pddoc = doc.getPDDoc();
        ASInt32 result = 0;                                                       //PDFlattenerConvertEx2 will set this to 0 if flattening failed.
        result = PDFlattenerConvertEx2(pddoc,                                     //The document whose pages we wish to flatten.
                                       0,                                         //The first page to flatten.
                                       PDDocGetNumPages(pddoc)-1,                 //The last page to flatten.
                                       &numFlattened,                             //PDFlattener sets this to the number of pages it flattened. It will not flatten pages that do not contain transparent elements.
                                       &flattenParams);                           //Flattener options.

        if(result)
            std::wcout << L"I flattened " << result << " pages." << std::endl;
        else
        {
            std::wcout << L"Flattening failed." << std::endl;
            E_RETURN(result);
        }

//////////////////////////////////////////////////////////////////////////////////
// Step 5) Save the document, close it, and terminate the plugin.
//////////////////////////////////////////////////////////////////////////////////

        doc.saveDoc(L"FlattenedPDF.pdf");            //Save the document. APDFLDoc defaults to using the "PDSaveFull" flag while saving.
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

//////////////////////////////////////////////////////////////////////////////////////////////////
//ASBool callback function: A function used to count the number of Optional-Content Groups (layers) in a PDDoc.
//
//Note: clientData is just an ASInt32*, which is used to count the number of layers.
/////////////////////////////////////////////////////////////////////////////////////////////////
ASBool countOptionalContentGroups(PDOCG ocg, void *clientData)
{
    //Increase the Optional-Content Group count.
    ++(*((ASInt32*)clientData));

    //Return false to cease enumeration.
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//ASBool callback function: Destroys all the Optional-Content Groups (layers) in a PDDoc.
/////////////////////////////////////////////////////////////////////////////////////////
ASBool destroyOptionalContentGroups(PDOCG ocg, void *clientData)
{
    //This removes the layer. Since each element holds a reference to the OCG it is associated with, and not
    //vice versa, it's not necessary to enumerate through the elements associated with a layer to remove it.
    PDOCGDestroy(ocg);

    //Return false to cease enumeration.
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ASBool callback function: A function for PDFlattener which monitors the flattener's progress.
//
//Note:
//    clientData is an ASInt32* representing the previous page we were working on flattening. It must start with a page number that doesn't 
//    exist, like -1. With it, we have this function print the progress only every time a new page is begun, or when the Flattener is finished.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ASBool flattenerProgMon(ASInt32 pageNum, ASInt32 totalPages, float current, ASInt32 reserved, void *clientData)
{
    ASInt32* prevPage = (ASInt32*)clientData; //The previous page we were working on.

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
