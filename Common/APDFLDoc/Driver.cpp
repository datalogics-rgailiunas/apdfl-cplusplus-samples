//This driver will not be put in the final product, it is intended to demonstrate
//functionality and test methods.
#include "APDFLDoc.h"
#include "PEWCalls.h"

#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"
#include "MyPDFLibUtils.h"
#include "PSFCalls.h"
#include "ASCalls.h"
#include "ASExtraCalls.h"
int main()
{
    //Initialize the PDF Library
    int  err = MyPDFLInit();

    if (err != 0)
    {
        std::cerr << "Initialization error. See \"AcroErr.h\" for more info.\n" << std::endl;
        std::cerr << "Error system: " << ErrGetSystem(err) << std::endl;
        std::cerr << "Error Severity: " << ErrGetSeverity(err) << std::endl;
        std::cerr << "Error Code: " << ErrGetCode(err) << std::endl;
        return EXIT_FAILURE;
    }

    DURING

        //Test 1: Attempt to create a document and save it with no name or page number specified.
        //This should return and print an error message.
        APDFLDoc A;
        A.saveDoc();

        //Test 2: Open a document and save it with a new path specified. This should save the document
        //at the new path specified.
        APDFLDoc B(L"testfile.pdf", false);
        B.saveDoc(L"testfile2.pdf", PDSaveFull | PDSaveLinearized);

        //Test 3: Create a new document, insert a page into it and save the document at the path specified. 
        APDFLDoc C;
        C.insertPage(700, 700, PDBeforeFirstPage);
        C.saveDoc(L"testfile3.pdf", PDSaveFull | PDSaveLinearized);

        //Test 4: This should open an existing document and save the document with saveDoc() default parameters
        //overwriting the testfile4.pdf with any changes that were made.
        APDFLDoc D(L"testfile4.pdf", true);
        D.saveDoc();
        
        //Test 5: Attempt to acquire a page and release it.
        PDPage pdPage = NULL;
        pdPage = D.getPageNumber(0);
        if (pdPage != NULL)
            std::cerr << "Acquired page " << std::endl;
        PDPageRelease(pdPage);

        //Test 6: Return a reference to the PDDoc object wrapped in the APDFLDoc class.
        //This may alternatively be accessed as APDFLDoc.PDDoc. Closing the returned document
        //is optional it will still be handled by the destructor.
        volatile PDDoc document = B.getPDDoc();
        B.insertPage(25, 25, PDBeforeFirstPage);
        PDDocClose(document);

    HANDLER
        std::cout << "IN MAIN HANDLER";
    END_HANDLER

    system("pause");

    MyPDFLTerm();

    return EXIT_SUCCESS;
}
