@ECHO OFF
REM ***
REM ***  Copyright (c) 2015, Datalogics, Inc. All rights reserved.
REM ***

REM **********************************************************************************
REM *** Sample: All - Builds and runs each DataLogics APDFL sample.
REM ***
REM *** Note: By default, this occurs with the debug configuration.
REM *** Pass in "release" as an argument to use the release configuration.
REM *** It is important to note that this program assumes that if any sample was 
REM *** built, it was built in the correct directory. The final summary will be  
REM *** incorrect if this is untrue.
REM ***
REM *** Steps:
REM *** 1) Initialize
REM *** 2) Build each sample
REM *** 3) Run each sample via iteration
REM *** 4) Print the results
REM **********************************************************************************

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
REM *** REMain the sole and exclusive property of Datalogics and are protected by
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
REM ***

REM *************************************************
REM *** 1) Initialize
REM *************************************************

REM *** Initialize environment variables, enable delayed expansion.
SETLOCAL EnableDelayedExpansion
REM *** Filename of All project.
SET ALL_SLN=All.sln

REM *** The number of samples discovered
SET /A "NUM_SAMPLES=0"
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
REM *** A listing of descriptions of the failed runs.
SET DESC_FAIL_RUN=

REM *** Setting ARCH and STAGE portions of the pathname to build and run in ***
:rep1
IF /i "%1"=="release" (
	SET STAGE=Release
) ELSE ( 
	SET STAGE=Debug
)
SET ARCH=x64
:rep2

REM *** The .exe directory for each sample folder***
SET EXEDIR=%ARCH%\%STAGE%

REM *** Set up the visual studio environment. ***
IF "%VS120COMNTOOLS%" == "" GOTO Usage
CALL "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat" x64
IF "%VSINSTALLDIR%" == "" GOTO Usage

REM *************************************************
REM *** 2) Build each sample
REM *************************************************

devenv %ALL_SLN% /rebuild "%STAGE%|%ARCH%"
SET /A "NUM_FAIL_BUILD=%ERRORLEVEL%"

REM *************************************************
REM *** 3) Run each sample via iteration
REM *************************************************

REM *** Necessary for running.
SET PATH=..\..\Libs;%PATH%
cd ..
FOR /D %%G IN (*) DO (
	REM *** names of folders you don't want to search
	IF NOT %%G==All  (
		CD %%G
		IF EXIST *.sln (
			SET /A "NUM_SAMPLES+=1"
			ECHO #Running sample %%G...
			cd %ARCH%\%STAGE%
			IF !ERRORLEVEL! NEQ 0 (
				ECHO #^!Error^!^: .exe directory not found.
				SET "DESC_FAIL_BUILD=!DESC_FAIL_BUILD!^-%%G ^(no exe directory found^) ^& echo."
				CD ..
			) ELSE (
				IF EXIST %%G.exe (
					CALL %%G.exe
					REM *** If it didn't run successfully, update accordingly.
					IF !ERRORLEVEL! NEQ 0 (
						SET /A "NUM_FAIL_RUN+=1"
						ECHO #Failed with error code !ERRORLEVEL!
						SET "DESC_FAIL_RUN=!DESC_FAIL_RUN!^-%%G with error code !ERRORLEVEL!^& echo."
					) ELSE (
						SET /A "NUM_SUCCEED_RUN+=1"
						SET "DESC_SUCCEED_RUN=!DESC_SUCCEED_RUN!^-%%G^& echo."
					)
				) ELSE (
					ECHO #This sample failed to build.
					REM *** The number of failed builds was determined by the original devenv call,
					REM *** so does not need to be updated.
					SET "DESC_FAIL_BUILD=!DESC_FAIL_BUILD!^-%%G^& echo."
				)
				CD ..\..
			)
		) ELSE (
			REM *** No sample found.
		)
		ECHO.
		CD ..
	)
)

REM *************************************************
REM *** 3) Print the results
REM *************************************************

ECHO =====================================
ECHO =========Build/Run complete.
ECHO =========Total samples^: %NUM_SAMPLES%
ECHO =====================================

IF %NUM_SUCCEED_RUN% EQU %NUM_SAMPLES% (
	ECHO All samples built and ran successfuly.
	ECHO %DESC_SUCCEED_RUN%
) ELSE (
	IF %NUM_FAIL_BUILD% GTR 0 (
		ECHO Failed builds^: %NUM_FAIL_BUILD%
		ECHO %DESC_FAIL_BUILD%
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








