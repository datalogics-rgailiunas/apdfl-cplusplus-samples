#include <iostream>

#include "MyPDFLibUtils.h"

static int errorCode;

int main()
{
	//Initialize the pdf library
	errorCode = MyPDFLInit();

	//Check for errors upon initialization of APDFL
	if (errorCode != 0)
	{
		std::cerr << "Initialization error. See \"AcroErr.h\" for more info.\n" << std::endl;
		std::cerr << "Error system: " << ErrGetSystem(errorCode) << std::endl;
		std::cerr << "Error Severity: " << ErrGetSeverity(errorCode) << std::endl;
		std::cerr << "Error Code: " << ErrGetCode(errorCode) << std::endl;
		return errorCode;
	}

	DURING





	HANDLER
		//If there was an exception generate an error code 
		errorCode = ERRORCODE;
		char buf[256];
		ASGetErrorString(ERRORCODE, buf, sizeof(buf));
		//Print out error code
		std::cerr << "Error Code:" << errorCode << "Error Message:" << buf << std::endl;
	END_HANDLER

	return errorCode;
}

