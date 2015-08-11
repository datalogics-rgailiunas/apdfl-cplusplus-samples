// Copyright(c) 2015, Datalogics, Inc.All rights reserved.

//======================================================================
// Sample: CopyContent - Copies the contents of the input PDF file into
//             a new PDF file. You can specify which kinds of content 
//             you want to copy.
//
//
//Steps:
// 1) Determine what kinds of content will be copied.
// 2) Open the input PDF, create the output PDF.
// 3) Copy the specified content from the input PDF into the output PDF.
// 4) Save the output PDF and close both PDFs.
//======================================================================

// This agreement is between Datalogics, Inc. 101 N.Wacker Drive, Suite 1800,
// Chicago, IL 60606 ("Datalogics") and you, an end user who downloads
// source code examples for integrating to the Adobe PDF Library
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

#include <iostream>
#include <map>
#include <utility>
#include <string>
#include "InitializeLibrary.h"
#include "APDFLDoc.h"
#include "ASExtraCalls.h"
#include "PDCalls.h"
#include "ASCalls.h"
#include "PEExpT.h"
#include "PagePDECntCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"

//To reduce verbosity.
#define WC_PAIR std::make_pair<ASInt32, bool>

//Copies all elements in "from" into "to".
void copyElements(PDEContent* to, PDEContent* from, const std::map<ASInt32, bool>* willCopyList);

int main(int argc, char** argv)
{
    APDFLib lib;                                        //Initialize the Adobe PDF Library.

    if (lib.isValid() == false)                         //If it failed to initialize, return the error code.
        return lib.getInitError();

    //Paths to input and output documents.
    wchar_t* inPath  = L"../_Input/CopyContent.pdf";
    wchar_t* outPath = L"CopiedContent.pdf";

    ASErrorCode errCode = 0;                            //This will catch error codes thrown during library usage.

//======================================================================================================================================================================================================================
//Step 1) Determine what kinds of content will be copied.
//======================================================================================================================================================================================================================

    //The kind of content we'll copy:
    //For each PDEType/boolean pair here, content of the key type will be copied to the output document if and only if it is mapped to true.
    std::map<ASInt32, bool> typeCopyMap;
    typeCopyMap.insert(WC_PAIR(kPDEContainer, true));
    typeCopyMap.insert(WC_PAIR(kPDEForm,      true));
    typeCopyMap.insert(WC_PAIR(kPDEGroup,     true));
    typeCopyMap.insert(WC_PAIR(kPDEImage,     true));
    typeCopyMap.insert(WC_PAIR(kPDEPath,      true));
    typeCopyMap.insert(WC_PAIR(kPDEPlace,     true));
    typeCopyMap.insert(WC_PAIR(kPDEPS,        true));
    typeCopyMap.insert(WC_PAIR(kPDEShading,   true));
    typeCopyMap.insert(WC_PAIR(kPDEText,      true));
    typeCopyMap.insert(WC_PAIR(kPDEUnknown,   true));
    typeCopyMap.insert(WC_PAIR(kPDEXObject,   true));

    //Which pages we'll copy:
    ASInt32 pagesToCopy[] = { 0, 1, 3, 5 };                                                     //If WILL_COPY_ALL_PAGES is 0, we will copy only the pages specified here, in order of appearance. The first page is 0.
#define WILL_COPY_ALL_PAGES 0                                                                   //If this is defined as 1, then all pages will be copied, regardless of what pagesToCopy is set to.

    DURING

//======================================================================================================================================================================================================================
//Step 2) Open the input PDF, create the output PDF.
//======================================================================================================================================================================================================================

    std::wcout << L"Opening the input document." << std::endl;

    APDFLDoc inAPDoc(inPath, true);                                                             //Open the document specified by inPath.
    PDDoc inDoc = inAPDoc.getPDDoc();

    APDFLDoc outAPDoc;                                                                          //Create a blank document.
    PDDoc outDoc = outAPDoc.getPDDoc();

//======================================================================================================================================================================================================================
//Step 3) Copy the specified content from the input PDF into the output PDF.
//======================================================================================================================================================================================================================

#if WILL_COPY_ALL_PAGES == 0
    for (ASInt32 i : pagesToCopy)
    if (i < PDDocGetNumPages(inDoc) && i >= 0)                                                  //Make sure the page number is valid...
    {
#else
    for (int i = 0; i < PDDocGetNumPages(inDoc); i++)
#endif
    {
        //Give the output document a new page, with the same size as page i of the input.
        PDPage inPage = inAPDoc.getPage(i);
        ASFixedRect inPageSize;                                                                 //Stores the size of input page i.
        PDPageGetSize(inPage, &(inPageSize.right), &(inPageSize.top));

        outAPDoc.insertPage(inPageSize.right, inPageSize.top, PDDocGetNumPages(outDoc) - 1);    //Insert the new blank page.
        PDPage outPage = outAPDoc.getPage(PDDocGetNumPages(outDoc) - 1);                        //A reference to the new blank page.
         
        //Now copy the content.
        std::wcout << L"Copying page " << i << "'s elements." << std::endl;

        PDEContent inContent = PDPageAcquirePDEContent(inPage, 0);                              //The content of input page i.
        PDEContent outContent = PDPageAcquirePDEContent(outPage, 0);                            //The content of output page i (it's blank right now).
        copyElements(&outContent, &inContent, &typeCopyMap);                                   //A recursive function which copies the specified elements of inContent to outContent.
        PDPageSetPDEContentCanRaise(outPage, 0);                                                //Set the content into the page.

        //Release the resources of this iteration.
        PDPageReleasePDEContent(outPage, 0);
        PDPageRelease(outPage);
        PDPageReleasePDEContent(inPage, 0);
        PDPageRelease(inPage);
#if WILL_COPY_ALL_PAGES == 0
    }                                                                                           //If the page index given is valid.
#endif
    };

//======================================================================================================================================================================================================================
//Step 4) Save the output PDF and close both PDFs.
//======================================================================================================================================================================================================================

    std::wcout << L"Done. Saving the new document." << std::endl;

    outAPDoc.saveDoc(outPath);

    std::wcout << L"Success." << std::endl;

    //APDFLDoc's constructor will properly close both documents.

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode);                                                              //If there was an error, display it.

    END_HANDLER

    return errCode;                                                                             //lib's destructor terminates the library.
};

//======================================================================================================================================================================================================================
//void function: Recursively copies PDEElements in "from" into "to", if willCopyList maps elements of that type to true.
//
//For each element i:
//      a) If i's type is true for willCopyList and:
//          a1)It is a PDEContainer, PDEGroup, or PDEForm, construct a new PDE<Container/Group/Form> and copy i's
//             elements into it, following willCopyList. Then copy the new PDE<Container/Group/Form> into "to".
//          a2)It is not, just copy i to "to".
//      b) If i's type is false for willCopyList, ignore it.
//======================================================================================================================================================================================================================
void copyElements(PDEContent* to, PDEContent* from, const std::map<ASInt32, bool>* typeCopyMap)
{
    DURING

    for (int i = 0; i < PDEContentGetNumElems(*from); ++i)                                               //For each element i...
    {

//======================================================================================================================================================================================================================
//a) If i's type is true for willCopyList...
//======================================================================================================================================================================================================================

        PDEElement nextElem = PDEContentGetElem(*from, i);                                               //The next element to consider copying.
        ASInt32 type = PDEObjectGetType(reinterpret_cast<PDEObject>(nextElem));                          //Its type.
        std::map<ASInt32, bool>::const_iterator doesCopy = typeCopyMap->find(type);                     //The pair at which this type is referenced.

        if (doesCopy != typeCopyMap->end())                                                             //If this type is even mentioned in willCopyList.
        {
            if (doesCopy->second)                                                                        //if willCopyList matches this type to true.
            {
                if (type == kPDEContainer)
                {

//======================================================================================================================================================================================================================
//a1) It is a PDEContainer, PDEGroup, or PDEForm, (so) construct a new PDE<Container/Group/Form> and copy i's
//    elements into it, following willCopyList. Then copy the new PDE<Container/Group/Form> into "to".
//======================================================================================================================================================================================================================

                    PDEContainer fromContainer = reinterpret_cast<PDEContainer>(nextElem);               //Our container element,
                    PDEContent fromContent = PDEContainerGetContent(fromContainer);                      //and its contents.

                    //The new container, to which which we give blank contents, and give to the output document.
                    PDEContainer toContainer = PDEContainerCreate(PDEContainerGetMCTag(fromContainer), NULL, true);
                    PDEContainerSetContent(toContainer, PDEContentCreate());
                    PDEContent toContent = PDEContainerGetContent(toContainer);

                    copyElements(&toContent, &fromContent, typeCopyMap);                                //Copy our container's elements into a new one.

                    PDEContentAddElem(*to, kPDEAfterLast, reinterpret_cast<PDEElement>(toContainer));    //Now copy the new container into "to".

                    PDERelease(reinterpret_cast<PDEObject>(toContainer));                                                         //Now release the output container, since we're done with it.

                }
                else if (type == kPDEGroup)
                {

//======================================================================================================================================================================================================================
//a1) It is a PDEContainer, PDEGroup, or PDEForm, (so) construct a new PDE<Container/Group/Form> and copy i's
//    elements into it, following willCopyList. Then copy the new PDE<Container/Group/Form> into "to".
//======================================================================================================================================================================================================================

                    PDEGroup fromGroup = reinterpret_cast<PDEGroup>(nextElem);                           //Our group element,
                    PDEContent fromContent = PDEGroupGetContent(fromGroup);                              //and its content.

                    PDEGroup toGroup = PDEGroupCreate();                                                 //The new container,
                    PDEGroupSetContent(toGroup, PDEContentCreate());                                     //To which which we give blank contents,
                    PDEContent toContent = PDEGroupGetContent(toGroup);                                  //and give to the output document.

                    copyElements(&toContent, &fromContent, typeCopyMap);                                //Copy our group's elements into a new one.

                    PDEContentAddElem(*to, kPDEAfterLast, reinterpret_cast<PDEElement>(toGroup));        //Now copy the new group  into "to".

                    PDERelease(reinterpret_cast<PDEObject>(toGroup));                                                             //Now release the output container, since we're done with it.

                }
                else if (type == kPDEForm)
                {

//======================================================================================================================================================================================================================
//a1) It is a PDEContainer, PDEGroup, or PDEForm, (so) construct a new PDE<Container/Group/Form> and copy i's
//    elements into it, following willCopyList. Then copy the new PDE<Container/Group/Form> into "to".
//======================================================================================================================================================================================================================

                    PDEForm fromForm = reinterpret_cast<PDEForm>(nextElem);
                    PDEContent fromFormContent = PDEFormGetContent(fromForm);

                    PDEForm toForm = PDEFormCreateClone(fromForm);                                       //Note: this also clones the underlying xObject Cos Object(s), for each occurence.

                    //Replace the contents of toForm with a new one, with only the elements we want copied 
                    PDEContent toContent = PDEContentCreate();                                           //A blank content to copy the desired elements into.
                    copyElements(&toContent, &fromFormContent, typeCopyMap);
                    PDEFormSetContent(toForm, toContent);
                    
                    PDEContentAddElem(*to, kPDEAfterLast, reinterpret_cast<PDEElement>(toForm));         //Now copy the new form  into "to".

                    //Release the containers, since we're done with them.
                    PDERelease(reinterpret_cast<PDEObject>(fromFormContent));
                    PDERelease(reinterpret_cast<PDEObject>(toForm));

                }
                else
                {

//======================================================================================================================================================================================================================
//a2) It is not, (so) just copy i to "to".
//======================================================================================================================================================================================================================

                    PDEElement copyNextElem = PDEElementCopy(nextElem, kPDEElementCopyClipping);
                    PDEContentAddElem(*to, kPDEAfterLast, reinterpret_cast<PDEElement>(copyNextElem));

                    PDERelease(reinterpret_cast<PDEObject>(copyNextElem));

                }
            }
        }
        else
            std::wcout << L"Error: Unknown PDEElement type encountered: " << type << std::endl;
    }

    HANDLER

        ASRaise(ERRORCODE);                                                                              //If there was an exception, let the caller handle it.

    END_HANDLER

    return;
};
