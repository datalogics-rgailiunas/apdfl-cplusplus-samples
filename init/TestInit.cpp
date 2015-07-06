#include "InitializeLibrary.h"

#include "MyPDFLibUtils.h"
#include "ASExtraCalls.h"

int main()
{
    APDFLib lib;
    ASErrorCode b = 0;

    DURING

    PDDoc a = PDDocCreate();
    PDDocClose(a);

    HANDLER
        b = ERRORCODE;
    END_HANDLER

        DisplayError(b);

    return 0;
}
