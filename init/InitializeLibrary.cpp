#include "InitializeLibrary.h"

//*************************************
//Sets the PDFLDataRec and inits
//with that data.
//************************************
APDFLib::APDFLib()
{
    PDFLDataRec	pdflData;
    fillDirectories();
    memset(&pdflData, 0, sizeof(PDFLDataRec));

    pdflData.size = sizeof(PDFLDataRec);

    pdflData.allocator = NULL;

    pdflData.dirList = fontDirList;
    pdflData.listLen = fontDirListLen;

    pdflData.cMapDirectory = cMapDir;

    pdflData.unicodeDirectory = unicodeDir;

    pdflData.colorProfileDirList = colorProfDirList;
    pdflData.colorProfileDirListLen = colorProfDirListLen;

    pdflData.pluginDirList = pluginDirList;
    pdflData.pluginDirListLen = pluginDirListLen;

    initError = PDFLInitHFT(&pdflData);
}


//*************************************
//Reports whether an error happened
//and returns that error.
//************************************
ASInt32 APDFLib::getInitError()
{
    if (initError)
    {
        std::wcerr << L"Initialization error. See \"AcroErr.h\" for more info.\n" << std::endl;
        std::wcerr << L"Error system: " << ErrGetSystem(initError) << std::endl;
        std::wcerr << L"Error Severity: " << ErrGetSeverity(initError) << std::endl;
        std::wcerr << L"Error Code: " << ErrGetCode(initError) << std::endl;
    }

    return initError;
}

//*************************************
//Sets directory information
//************************************
void APDFLib::fillDirectories()
{
    fontDirList[0] = (ASUTF16Val*)L"..\\..\\..\\APDFL\\Resource\\Font";
    fontDirList[1] = (ASUTF16Val*)L"..\\..\\..\\APDFL\\Resource\\CMap";
    fontDirListLen = NUM_FONTS;

    colorProfDirList[0] = (ASUTF16Val*)L"..\\..\\..\\APDFL\\Resource\\Color\\Profiles";
    colorProfDirListLen = NUM_COLOR_PROFS;

    cMapDir = fontDirList[1];
    unicodeDir = (ASUTF16Val*)L"..\\..\\..\\APDFL\\Resource\\Unicode";


    pluginDirList[0] = (ASUTF16Val*)L"..\\..\\..\\APDFL\\Libs";
    pluginDirListLen = NUM_PLUGIN_DIRS;

}

//*************************************
//Deconstructor terminates the
//APDFL library.
//************************************
APDFLib::~APDFLib()
{
    PDFLTermHFT();
}

void APDFLib::displayError(ASErrorCode errCode)
{
    if (errCode == 0)	return;

    char errStr[250];
    fprintf(stderr, "[Error %ld] %s\n", errCode, ASGetErrorString(errCode, errStr, sizeof(errStr)));
}
