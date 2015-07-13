#include "InitializeLibrary.h"

#include "MyPDFLibUtils.h"
#include "ASExtraCalls.h"

int main()
{
    //This is a small test procedure which ensures the class inits the library at all,
    //and which demonstrates how to use APDFLib.
    //This file will not be included in release and should not be merged.

    APDFLib lib;

    if (!lib.isValid())
        std::wcout << "The initialization procedure did not complete." << std::endl;

    if (lib.getInitError())
        lib.displayError(lib.getInitError());

    ASErrorCode b = 0;

    DURING

    PDDoc a = PDDocCreate();
    PDDocClose(a);

    HANDLER
        b = ERRORCODE;
    END_HANDLER

    DisplayError(b);

    //The deconstructor of APDFLib terminates the library.
    return 0;
}
