@echo off

set aibotpath=%1
set mapname=%2

echo You are about to create play a match locally
echo.
echo.
echo.

REM CLEAN VARIABLES

set GOOGLE_DRIVE_DIR=
set PrerequisitesFound=
set googleDriveWellConfigured=
set TEAMNAME=

@echo off
call CheckInstall.bat

if ErrorLevel == 1 (
    goto fail
)

call :RetrieveGoogleDrivePath
call :RetrieveTeamName

echo.
echo aibotpath=%aibotpath%
echo mapname=%mapname%

set hour=%time:~0,2%
if "%hour:~0,1%" == " " set hour=0%hour:~1,1%
set min=%time:~3,2%
if "%min:~0,1%" == " " set min=0%min:~1,1%
set secs=%time:~6,2%
if "%secs:~0,1%" == " " set secs=0%secs:~1,1%

set JOBNAME=%DATE%_%hour%-%min%-%secs%

rem in milliseconds
set LoadTime=10000000
set InitTime=10000000
set TurnTime=10000000

"%AIBOOTCAMP_ENGINE_DIR%/AIBootCamp.exe" %mapname% -AIBot=%aibotpath% -AIBotLogPath=%~dp0/StandaloneMatchResults/aibot -GameLogPath=%~dp0/StandaloneMatchResults/gamelog -JobID=%JOBNAME% -TeamID=%TEAMNAME% -LoadTime=%LoadTime% -InitTime=%InitTime% -TurnTime=%TurnTime%

"%PYTHON_PATH%/python.exe" GenAIBootCampViewerFiles.py %~dp0/StandaloneMatchResults/gamelog/GameReplay.log %~dp0/StandaloneMatchResults/gamelog/GameReplay.js %~dp0/StandaloneMatchResults/gamelog/AIBootCampResults.html

goto success


:RetrieveTeamName
    set TEAMDRIVE=%GOOGLE_DRIVE_DIR%\Results\Team.txt
    IF EXIST "%TEAMDRIVE%" (
        FOR /F "usebackq delims== tokens=1,2" %%G IN ("%TEAMDRIVE%") DO set %%G=%%H 
    )
    exit /B 0

:RetrieveGoogleDrivePath
    IF EXIST "TeamInfo.team" (
        for /f "delims== tokens=1,2" %%G in (TeamInfo.team) do set %%G=%%H
    )
    exit /B 0

:success
    echo.
    echo SUCCESS : Match Completed
    goto end

:fail
    echo.
    echo FAILED : Match Execution Failed
    goto end

:end
