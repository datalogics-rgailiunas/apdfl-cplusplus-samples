// Copyright(c) 2015, Datalogics, Inc.All rights reserved.

//======================================================================
// Sample: CopyContent - Copies the contents of the input PDF file into
//                       a new PDF file. You can specify which kinds
//                       of content you want to copy, and which
//                       you want to ignore.
//
//Steps:
// 1) Configure sample functionality
// 2) Open input PDF, create output PDF
// 3) Copy the content from input into output
// 4) Save and close
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

//Some defines to reduce verbosity
#define I_B_PAIR std::make_pair<ASInt32, bool>
#define RELEASE_PDEOBJ(o) PDERelease(reinterpret_cast<PDEObject>(o))

//Copies all elements in "from" into "to". See definition below.
void copyElements(PDEContent* to, PDEContent* from, const std::map<ASInt32, bool>* willCopyList);

int main(int argc, char** argv)
{
    //Initialize the APDF Library.
    APDFLib lib;
    if (!lib.isValid())
        return lib.getInitError();    //Will display the error, if any.

    ASErrorCode errCode = 0;       //Tracks APDFL errors
    //Paths to in/out documents.
    wchar_t* inPath = L"../_Input/CopyContent.pdf";
    wchar_t* outPath = L"CopiedContent.pdf";

//=============================================================================
//Step 1) Configure sample functionality
//=============================================================================

    //This map defines the kinds of content that
    //will be copied into the output.
    //Set a pair's value to false if you do not want
    //elements of the key's type to be copied.
    std::map<ASInt32, bool> willCopyList;
    willCopyList.insert(I_B_PAIR(kPDEContainer, true));
    willCopyList.insert(I_B_PAIR(kPDEForm,      true));
    willCopyList.insert(I_B_PAIR(kPDEGroup,     true));
    willCopyList.insert(I_B_PAIR(kPDEImage,     true));
    willCopyList.insert(I_B_PAIR(kPDEPath,      true));
    willCopyList.insert(I_B_PAIR(kPDEPlace,     true));
    willCopyList.insert(I_B_PAIR(kPDEPS,        true));
    willCopyList.insert(I_B_PAIR(kPDEShading,   true));
    willCopyList.insert(I_B_PAIR(kPDEText,      true));
    willCopyList.insert(I_B_PAIR(kPDEUnknown,   true));
    willCopyList.insert(I_B_PAIR(kPDEXObject,   true));

    ASInt32 pagesToCopy[] = { 0, 1, 3, 5 };                  //Which pages we'll copy. First page is 0.
#define WILL_COPY_ALL_PAGES 0                                //Set to 1 if you just want to copy every page

    DURING

//=============================================================================
//Step 2) Open input PDF, create output PDF
//=============================================================================

    std::wcout << L"Opening the input document." << std::endl;

    APDFLDoc inAPDoc(inPath,true);
    PDDoc inDoc = inAPDoc.getPDDoc();
    APDFLDoc outAPDoc;
    PDDoc outDoc = outAPDoc.getPDDoc();

//=============================================================================
//Step 3) Copy the content from input into output
//=============================================================================

    ASInt32 numPages = PDDocGetNumPages(inDoc);

#if WILL_COPY_ALL_PAGES == 0
    for (ASInt32 i : pagesToCopy)
#else
    for (int i = 0; i < numPages; i++)
#endif
    {
        if (i < numPages)    //Make sure the page number is valid
        {
            //Give the output document a new page with input page i's dimensions
            PDPage inPage = inAPDoc.getPageNumber(i);
            ASFixedRect inPageSize;                                                //Stores the size of page i

            PDPageGetSize(inPage, &(inPageSize.right), &(inPageSize.top));

            outAPDoc.insertPage(inPageSize.right, inPageSize.top, PDDocGetNumPages(outDoc) - 1);
            PDPage copyPage = outAPDoc.getPageNumber(PDDocGetNumPages(outDoc) - 1);

            //Now copy the content
            PDEContent inContent = PDPageAcquirePDEContent(inPage, 0);

            PDEContent copyContent = PDPageAcquirePDEContent(copyPage, 0);

            std::wcout << L"Copying page " << i << "'s elements." << std::endl;
            copyElements(&copyContent, &inContent, &willCopyList);                 //Copy the contents of page i

            PDPageSetPDEContentCanRaise(copyPage, 0);                              //Set the content into the page

            //Release resources
            PDPageReleasePDEContent(copyPage,0);
            PDPageRelease(copyPage);
            PDPageReleasePDEContent(inPage,0);
            PDPageRelease(inPage);
        }
    };
    PDDocClose(inDoc);

//=============================================================================
//Step 4) Save and close
//=============================================================================

    std::wcout << L"Done. Saving the new document." << std::endl;

    outAPDoc.saveDoc(outPath, PDDocNeedsSave | PDSaveFull);

    HANDLER

        errCode = ERRORCODE;

    END_HANDLER

    if (errCode) lib.displayError(errCode);

    return errCode;    //End. Lib's destructor terminates the library.
};

//===============================================================
//Copy all elements from "from" into "to".
//
//For each element i:
//      a) If i's type is true for willCopyList and:
//          a1)It is a PDEContainer, PDEGroup, or PDEForm,
//             construct a new PDE<Container/Group/Form> and copy
//             i's elements into it, following willCopyList.
//             Then copy the new object into "to".
//          a2)It is not, just copy i to "to".
//===============================================================
void copyElements(PDEContent* to, PDEContent* from, const std::map<ASInt32,bool>* willCopyList)
{
    DURING

    for (int i = 0; i < PDEContentGetNumElems(*from); ++i)
    {
//===================================================================================
//a) If i's type is true for willCopyList...
//===================================================================================

        PDEElement nextElem = PDEContentGetElem(*from, i);                                //The next element to consider copying
        ASInt32 type = PDEObjectGetType(reinterpret_cast<PDEObject>(nextElem));           //Its type
        std::map<ASInt32, bool>::const_iterator doesCopy = willCopyList->find(type);      //Whether we copy this type

        if (doesCopy != willCopyList->end())
        {
            if (willCopyList->find(type)->second)
            {
                if (type == kPDEContainer)
                {
//===================================================================================
//a1) Construct a new PDEContainer and copy nextElem's elements into it,
//    then add the new container into "to".
//===================================================================================

                    PDEContainer fromContainer = 
                        reinterpret_cast<PDEContainer>(nextElem);                        //Our container element,
                    PDEContent fromContent = PDEContainerGetContent(fromContainer);      //and its contents

                    PDEContainer toContainer = PDEContainerCreate(                       //The new container,
                                    PDEContainerGetMCTag(fromContainer), NULL, true);
                    PDEContainerSetContent(toContainer, PDEContentCreate());             //which we give blank contents
                    PDEContent toContent = PDEContainerGetContent(toContainer);

                    copyElements(&toContent, &fromContent, willCopyList);                //Copy our container's elements into a new one

                    PDEContentAddElem(*to, kPDEAfterLast,                                //Now copy the new container into "to".
                            reinterpret_cast<PDEElement>(toContainer)); 

                    RELEASE_PDEOBJ(toContainer);
                }
                else if (type == kPDEGroup)
                {
//===================================================================================
//a1) Construct a new PDEGroup and copy nextElem's elements into it,
//    then add the new group into "to".
//===================================================================================

                    PDEGroup fromGroup = reinterpret_cast<PDEGroup>(nextElem);    //Our group element,
                    PDEContent fromContent = PDEGroupGetContent(fromGroup);       //and its content

                    PDEGroup toGroup = PDEGroupCreate();                          //The new container,
                    PDEGroupSetContent(toGroup, PDEContentCreate());              //which we give blank contents
                    PDEContent toContent = PDEGroupGetContent(toGroup);

                    copyElements(&toContent, &fromContent, willCopyList);         //Copy our group's elements into a new one

                    PDEContentAddElem(*to, kPDEAfterLast,                         //Now copy the new group  into "to"
                        reinterpret_cast<PDEElement>(toGroup));

                    RELEASE_PDEOBJ(toGroup);

                }
                else if (type == kPDEForm)
                {
//===================================================================================
//a1) Clone the PDEForm into a new form, and remove undesired content
//    elements.
//    Then add the new form into "to".
//===================================================================================

                    PDEForm fromForm = reinterpret_cast<PDEForm>(nextElem);
                    PDEContent fromFormContent = PDEFormGetContent(fromForm);

                    PDEForm toForm = PDEFormCreateClone(fromForm);           //Note: this also clones the underlying xObject Cos Object(s), for each occurence.
                    //Replace the contents of toForm with a new one,
                    //with only the elements we want copied 
                    PDEContent toContent = PDEContentCreate();               //A blank content to copy the desired elements into.
                    copyElements(&toContent, &fromFormContent, willCopyList);
                    PDEFormSetContent(toForm, toContent);
                    
                    PDEContentAddElem(*to, kPDEAfterLast,                    //Now copy the new form  into "to"
                        reinterpret_cast<PDEElement>(toForm));

                    RELEASE_PDEOBJ(fromFormContent);
                    RELEASE_PDEOBJ(toForm);
                }
                else
                {
//===================================================================================
//a2) Copy nextElem to "to".
//===================================================================================

                    PDEElement copyNextElem = PDEElementCopy(nextElem, kPDEElementCopyClipping);
                    PDEContentAddElem(*to, kPDEAfterLast, 
                        reinterpret_cast<PDEElement>(copyNextElem));

                    RELEASE_PDEOBJ(copyNextElem);
                }
            }
        }
        else
            std::wcout << L"Error: Unknown PDEElement type encountered: " << type << std::endl;
    }
    HANDLER

        RERAISE();

    END_HANDLER

    return;
};
