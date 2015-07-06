//This driver will not be put in the final product, it is intended to demonstrate
//functionality and test methods.
#include "APDFLDoc.h"

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
            //Creating a document
            APDFLDoc A;
            //Set the name
            A.setDocumentName(L"A1.pdf");
            //Save document
            A.saveDoc();

            //Testing the open document constructor
            APDFLDoc B;
            //Set the asPathName
            B.setASPathName(L"B1.pdf");
            //Save the document
            B.saveWebOptimized();

            //Open a document and set the do repair flag
            APDFLDoc C(L"testfile.pdf", true);
            C.saveWebOptimized();

            //Testing insert page method
            APDFLDoc D;
            D.setDocumentName(L"D1.pdf");
            for (int i = 0; i < 10; ++ i)
                D.insertPage(500, 500);
            D.saveWebOptimized();

            //Checking case where file will not open
            //APDFLDoc E(L"testinghandleronopen.pdf", false);
            //E.insertPage(50, 100);
            //E.saveWebOptimized();


        HANDLER
            std::cout << "IN MAIN HANDLER";
            system("pause)");
        END_HANDLER


    system("pause");

    MyPDFLTerm();

    return EXIT_SUCCESS;
}
