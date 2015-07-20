@ECHO OFF
REM ***
REM ***  Copyright (c) 2015, Datalogics, Inc. All rights reserved.
REM ***

REM **********************************************************************************************************************
REM *** Sample: All - Builds and runs each DataLogics APDFL sample, and outputs the results.
REM ***
REM *** By default, this occurs with the debug configuration.
REM *** Pass in "release" as an argument to use the release configuration.
REM *** It is important to note that this program assumes: 
REM ***    1. A sample was built successfully <-> Its exe is located in <samplefolder>/<arch>/<stage>/<samplename>.exe
REM ***    2. In assumption 1, <samplename> is equal to <samplefolder>.
REM ***
REM *** Steps:
REM *** 1) Initialize
REM *** 2) Build each sample
REM *** 3) Run each sample
REM *** 4) Output the results
REM **********************************************************************************************************************

REM ***  This agreement is between Datalogics, Inc. 101 N. Wacker Drive, Suite 1800,
REM ***  Chicago, IL 60606 ("Datalogics") and you, an end user who downloads
REM ***  source code examples for integrating to the Adobe PDF Library
REM ***  ("the Example Code"). By accepting this agreement you agree to be bound
REM ***  by the following terms of use for the Example Code.
REM *** 
REM ***  LICENSE
REM ***  -------
REM ***  Datalogics hereby grants you a royalty-free, non-exclusive license to
REM ***  download and use the Example Code for any lawful purpose. There is no charge
REM ***  for use of Example Code.
REM *** 
REM ***  OWNERSHIP
REM ***  ---------
REM ***  The Example Code and any related documentation and trademarks are and shall
REM ***  remain the sole and exclusive property of Datalogics and are protected by
REM ***  the laws of copyright in the U.S. and other countries.
REM *** 
REM ***  Datalogics is a trademark of Datalogics, Inc.
REM *** 
REM ***  TERM
REM ***  ----
REM ***  This license is effective until terminated. You may terminate it at any
REM ***  other time by destroying the Example Code.
REM *** 
REM ***  WARRANTY DISCLAIMER
REM ***  -------------------
REM ***  THE EXAMPLE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
REM ***  EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES
REM ***  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
REM *** 
REM ***  DATALOGICS DISCLAIM ALL OTHER WARRANTIES, CONDITIONS, UNDERTAKINGS OR
REM ***  TERMS OF ANY KIND, EXPRESS OR IMPLIED, WRITTEN OR ORAL, BY OPERATION OF
REM ***  LAW, ARISING BY STATUTE, COURSE OF DEALING, USAGE OF TRADE OR OTHERWISE,
REM ***  INCLUDING, WARRANTIES OR CONDITIONS OF MERCHANTABILITY, FITNESS FOR A
REM ***  PARTICULAR PURPOSE, SATISFACTORY QUALITY, LACK OF VIRUSES, TITLE,
REM ***  NON-INFRINGEMENT, ACCURACY OR COMPLETENESS OF RESPONSES, RESULTS, AND/OR
REM ***  LACK OF WORKMANLIKE EFFORT. THE PROVISIONS OF THIS SECTION SET FORTH
REM ***  SUBLICENSEE'S SOLEREMEDY AND DATALOGICS'S SOLE LIABILITY WITH RESPECT
REM ***  TO THE WARRANTY SET FORTH HEREIN. NO REPRESENTATION OR OTHER AFFIRMATION
REM ***  OF FACT, INCLUDING STATEMENTS REGARDING PERFORMANCE OF THE EXAMPLE CODE,
REM ***  WHICH IS NOT CONTAINED IN THIS AGREEMENT, SHALL BE BINDING ON DATALOGICS.
REM ***  NEITHER DATALOGICS WARRANT AGAINST ANY BUG, ERROR, OMISSION, DEFECT,
REM ***  DEFICIENCY, OR NONCONFORMITY IN ANY EXAMPLE CODE.
REM ***  

REM *************************************************
REM *** 1) Initialize
REM *************************************************

REM *** Initialize environment variables, enable delayed expansion.
SETLOCAL EnableDelayedExpansion  
REM *** Filename of All project.
SET ALL_SLN=All.sln


REM ************* Initialize variables which track our progress ******************
REM *** The number of samples that failed to build
SET /A "NUM_FAIL_BUILD=0"
REM *** A list of the failed builds.
SET DESC_FAIL_BUILD=
REM *** The number of samples that successfully ran.
SET /A "NUM_SUCCEED_RUN=0"
REM *** A list of the successful runs.
SET DESC_SUCCEED_RUN=
REM *** The number of samples that failed to run.
SET /A "NUM_FAIL_RUN=0"
REM *** A list of the failed runs.
SET DESC_FAIL_RUN=

REM *** Configure self based on arguments.
REM *** Setting ARCH and STAGE portions of the pathname to build and run in.
REM *** (<arch> and <stage> in the description at the top)


REM *** Assume default settings.
SET STAGE=Debug
SET ONLY_BUILD=N

:AcceptCommands
REM *** Iterate through arguments, changing settings when needed.
IF /i "%1"=="release" (
    SET STAGE=Release
) 
IF /i "%1"=="-n" (
	SET ONLY_BUILD=Y
)
IF /i "%1"=="" (
	GOTO ArgumentsEnd
)
SHIFT
GOTO AcceptCommands

:ArgumentsEnd
SET ARCH=x64

REM *** Set up the visual studio environment
IF "%VS120COMNTOOLS%" == "" GOTO Usage
CALL "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat" x64
IF "%VSINSTALLDIR%" == "" GOTO Usage

REM *************************************************
REM *** 2) Build each sample
REM *************************************************

devenv %ALL_SLN% /rebuild "%STAGE%|%ARCH%"

REM *************************************************
REM *** 3) Run each sample
REM *************************************************

REM *** A list of sample names (<samplename> in the description at the top)
REM *** This program will run each of the samples listed here.
REM *** It is imperative that All.sln builds all and only these samples.
REM *** To add a sample to this script's functionality, include its name here,
REM *** and add the project to All.sln.
REM *** And don't forget to update NUM_SAMPLES!
SET "SAMPLE_LIST=(AddDocumentInformation addElements placeText SplitPDF WebOptimizedPDF)"
SET /A "NUM_SAMPLES=5"
REM *** i iterates over each sample.
SET /A "i=0"

REM ** Prepare to go through all the samples.
CD ..
REM *** The directory in which the sample folders are located.
SET SAMPLEDIR=%CD%
REM *** Necessary for running.
SET PATH=..\..\Libs;%PATH%
SET CURRENT_SAMPLE=

REM ************************************************************
REM ******************** MAIN LOOP *****************************
REM ************************************************************
:RunSampleLoop_START
REM *** If we've run all the samples, end
IF %i% GEQ %NUM_SAMPLES% GOTO RunSampleLoop_END
ECHO.

REM *** Retrieve the ith sample.
GOTO GetIthSample
:GotIthSample

CD %SAMPLEDIR%\%CURRENT_SAMPLE%\%ARCH%\%STAGE%

REM *** If the directory could not be found.
IF %ERRORLEVEL% NEQ 0 (GOTO CantFindExe)
REM *** If the exe file could not be found.
IF NOT EXIST %CURRENT_SAMPLE%.exe (GOTO CantFindExe)


If %ONLY_BUILD% == Y GOTO RunSampleLoop_Call_End 

:RunSampleLoop_Call
ECHO #%CURRENT_SAMPLE%.exe is running...
CALL %CURRENT_SAMPLE%.exe

REM *** If it failed to run
IF %ERRORLEVEL% NEQ 0 (GOTO FailedRun)
REM *** Otherwise, it succeeded!
GOTO SuccessfulRun
:RunSampleLoop_Call_End

:RunSampleLoop_NEXT_ITER
REM *** Prepare for next iteration
SET /A "i+=1"
GOTO RunSampleLoop_START

REM ************************************************************
REM ************************************************************
REM ************************************************************

REM ********************************
REM ******* SUBROUTINES ************
REM ********************************
:GetIthSample

SET /A "n=0"
FOR %%S IN %SAMPLE_LIST% DO (
	IF !n! EQU !i! SET CURRENT_SAMPLE=%%S
	SET /A "n+=1"
)

GOTO GotIthSample
REM ********************************
:SuccessfulRun

SET /A "NUM_SUCCEED_RUN+=1"
ECHO #Ran successfully.
SET "DESC_SUCCEED_RUN=!DESC_SUCCEED_RUN!^*!CURRENT_SAMPLE! ^& ECHO."

GOTO RunSampleLoop_NEXT_ITER
REM ********************************
:FailedRun

SET /A "NUM_FAIL_RUN+=1"
ECHO #Failed ^(%ERRORLEVEL%^)
SET "DESC_FAIL_RUN=!DESC_FAIL_RUN!^*!CURRENT_SAMPLE! !ERRORLEVEL! ^& ECHO."

GOTO RunSampleLoop_NEXT_ITER
REM ********************************
:CantFindExe

ECHO #%CURRENT_SAMPLE% failed to build.
SET /A "NUM_FAIL_BUILD+=1"
SET "DESC_FAIL_BUILD=!DESC_FAIL_BUILD!^*!CURRENT_SAMPLE! ^& ECHO."

GOTO RunSampleLoop_NEXT_ITER
REM ********************************
REM ********************************
REM ********************************
:RunSampleLoop_END



REM *************************************************
REM *** 3) Print the results
REM *************************************************

ECHO =====================================
IF %ONLY_BUILD% == N (
ECHO =========Build/Run complete.
) ELSE (
ECHO =========Build complete.
)
ECHO =========Total samples^: %NUM_SAMPLES%
ECHO =====================================

IF %NUM_SUCCEED_RUN% EQU %NUM_SAMPLES% (
    ECHO All samples built and ran successfuly.
    ECHO %DESC_SUCCEED_RUN%
) ELSE (
    IF %NUM_FAIL_BUILD% GTR 0 (
        ECHO Failed builds^: %NUM_FAIL_BUILD% 
        ECHO %DESC_FAIL_BUILD%
    ) ELSE (
		ECHO All samples built successfuly.
	)
    IF %NUM_FAIL_RUN% GTR 0 (
        ECHO Failed runs^: %NUM_FAIL_RUN%
        ECHO %DESC_FAIL_RUN%
    )
    IF %NUM_SUCCEED_RUN% GTR 0 (
        ECHO Successful runs^: %NUM_SUCCEED_RUN%
        ECHO %DESC_SUCCEED_RUN%
    )
)
ECHO =====================================
ECHO =====================================
GOTO End

:Usage
ECHO "You must have Visual Studio 2013 installed to use this executable."

:End
PAUSE
EXIT /b %ERRORLEVEL%
