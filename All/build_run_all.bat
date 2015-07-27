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
REM ***    2. Above, <samplename> is equal to <samplefolder>.
REM ***
REM *** USAGE:
REM ***   See the documentation for maintenance (adding or removing samples from its build/run directives).
REM ***
REM ***   ARGUMENT      EFFECT
REM ***   -noRun        Don't run the samples, just build them.
REM ***   -noAD         Don't process the Adobe samples.
REM ***   -noDL         Don't process the Datalogics samples.
REM ***
REM *** Steps:
REM *** 1) Initialize.
REM *** 2) Build each sample.
REM *** 3) Decide which samples to run.
REM *** 4) Run the samples.
REM *** 4) Output the results.
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
REM *** 1) Initialize.
REM *************************************************

REM *** Initialize environment variables, enable delayed expansion.
SETLOCAL EnableDelayedExpansion  
REM *** Filename of All project.
SET ALL_AD_SLN=All_Adobe.sln
SET ALL_DL_SLN=All_Datalogics.sln


REM ************* Initialize variables which track our progress ******************
REM *** The number of samples that failed to build.
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

REM *** Assume default settings.

REM *** The configuration to use.
SET STAGE=Debug
REM *** Only build the samples, don't run them.
SET ONLY_BUILD=N
REM *** Process the Datalogics samples.
SET DO_DL=Y
REM *** Process the Adobe samples.
SET DO_AD=Y
REM *** Build for x64. No arguments can change this.
SET ARCH=x64

:AcceptCommands
REM *** Iterate through arguments, changing settings when needed.
IF /i "%1"=="" (
	GOTO ArgumentsEnd
)
IF /i "%1"=="release" (
    SET STAGE=Release
) 
IF /i "%1"=="-noRun" (
	SET ONLY_BUILD=Y
)
IF /i "%1"=="-noDL" (
	SET DO_DL=N
)
IF /i "%1"=="-noAD" (
	SET DO_AD=N
)
SHIFT
GOTO AcceptCommands

:ArgumentsEnd

IF %DO_DL% == N IF %DO_AD% == N GOTO MustIncludeFiles

REM *** Set up the visual studio environment.
IF "%VS120COMNTOOLS%" == "" GOTO Usage
CALL "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat" x64
IF "%VSINSTALLDIR%" == "" GOTO Usage

REM *************************************************
REM *** 2) Build each sample.
REM *************************************************

IF %DO_DL% == Y (
	ECHO #Building Datalogics samples...
	devenv %ALL_DL_SLN% /rebuild "%STAGE%|%ARCH%"
)
ECHO.
IF %DO_AD% == Y (
	ECHO #Building Adobe samples...
	devenv %ALL_AD_SLN% /rebuild "%STAGE%|%ARCH%"
)
 
REM *************************************************
REM *** 3) Decide which samples to run.
REM *************************************************

REM *** This needs to be accurate, of course.
SET /A "NUM_SAMPLES=5"

REM *** The lists of samples to process (<samplename> in the description at the top).

REM *** Datalogics Samples.
SET "DL_SAMPLE_LIST=("
SET "DL_SAMPLE_LIST=%DL_SAMPLE_LIST% AddDocumentInformation addElements placeText"
SET "DL_SAMPLE_LIST=%DL_SAMPLE_LIST% SplitPDF WebOptimizedPDF"
SET "DL_SAMPLE_LIST=%DL_SAMPLE_LIST%)"
REM *** This needs to be accurate, of course.
SET /A "NUM_DL_SAMPLES=5"
REM *** Di iterates over Datalogics samples.
SET /A "Di=0"

REM *** Adobe Samples.
SET "AD_SAMPLE_LIST=()"
REM *** This needs to be accurate, of course.
SET /A "NUM_AD_SAMPLES=0"
REM *** Ai iterates over Adobe samples.
REM (None yet.)
SET /A "Ai=0"

REM *** Arguments for all samples. The variable name before "_args"
REM *** is the name of the sample which requires the arguments.
rem (devnote: These samples are not currently included. These variables
rem are examples of what argument-passing is like.)
SET "MTInMemFS_args=..\_Data\input.txt outPath"
SET "MTSerialNums_args=..\_Data\addelem.pdf outDir outBase 2 3"
SET "SnippetRunner_args=DLautoInput.txt"


REM *************************************************
REM *** 4) Run the samples.
REM *************************************************

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
REM *** If we've run all the samples, end.
REM *** (AD samples are always done last.)
IF %DO_DL% == Y IF %DO_AD% == Y IF %Di% GEQ %NUM_DL_SAMPLES% IF %Ai% GEQ %NUM_AD_SAMPLES% GOTO RunSampleLoop_END
IF %DO_DL% == N IF %DO_AD% == Y IF %Ai% GEQ %NUM_AD_SAMPLES% GOTO RunSampleLoop_END
IF %DO_DL% == Y IF %DO_AD% == N IF %Di% GEQ %NUM_DL_SAMPLES% GOTO RunSampleLoop_END

ECHO.

REM *** Retrieve the next sample.
GOTO GetNextSample
:GotNextSample

CD %SAMPLEDIR%\%CURRENT_SAMPLE%\%ARCH%\%STAGE%

REM *** If the directory could not be found.
IF %ERRORLEVEL% NEQ 0 (GOTO CantFindExe)
REM *** If the exe file could not be found.
IF NOT EXIST %CURRENT_SAMPLE%.exe (GOTO CantFindExe)

If %ONLY_BUILD% == Y GOTO RunSampleLoop_Call_End 

:RunSampleLoop_Call
	ECHO #%CURRENT_SAMPLE%.exe is running...
	REM *** Call the sample with its arguments, if any.
	REM *** (undefined variables expand to nothing.)
	CALL %CURRENT_SAMPLE%.exe %!CURRENT_SAMPLE!_args%

	REM *** If it failed to run.
	IF %ERRORLEVEL% NEQ 0 (GOTO FailedRun)
	REM *** Otherwise, it succeeded!
	GOTO SuccessfulRun
:RunSampleLoop_Call_End

:RunSampleLoop_NEXT_ITER
GOTO RunSampleLoop_START

REM ************************************************************
REM ************************************************************
REM ************************************************************

REM ********************************
REM ******* SUBROUTINES ************
REM ********************************
:GetNextSample
SET /A "n=0"

IF %DO_DL% == Y IF %Di% LSS %NUM_DL_SAMPLES% GOTO NextDLSample
IF %DO_AD% == Y IF %Ai% LSS %NUM_AD_SAMPLES% GOTO NextAdobeSample

:NextDLSample
FOR %%S IN %DL_SAMPLE_LIST% DO (
	IF !n! EQU !Di! SET CURRENT_SAMPLE=%%S
	SET /A "n+=1"
)
SET /A "Di+=1"
GOTO GotNextSample

:NextAdobeSample
FOR %%S IN %AD_SAMPLE_LIST% DO (
	IF !n! EQU !Ai! SET CURRENT_SAMPLE=%%S
	SET /A "n+=1"
)
SET /A "Ai+=1"
GOTO GotNextSample

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
REM *** 5) Print the results.
REM *************************************************


SET /A "NUM_SAMPLES=0"
IF %DO_DL% == Y SET /A "NUM_SAMPLES+=%NUM_DL_SAMPLES%"
IF %DO_AD% == Y SET /A "NUM_SAMPLES+=%NUM_AD_SAMPLES%"

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
ECHO You must have Visual Studio 2013 installed to use this executable.
GOTO End

:MustIncludeFiles
ECHO You must choose to run the DL or the Adobe samples^!
GOTO End

:End
PAUSE
EXIT /b %ERRORLEVEL%
