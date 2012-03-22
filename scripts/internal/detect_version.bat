@echo off

rem Include config
call config.bat
if errorlevel 1 (
	exit /b 1
)

rem Validate input parameters
if "%1" == "" (
	echo Usage: script beta^|final
	goto end
)

echo    * Cleaning environment
call internal\clear_env.bat
if errorlevel 1 (
	echo ERROR: Problem with preparing environment.
	goto error
)

echo    * Checking out '/trunk/src/common' directory of CH
svn co "%ReposCH%/trunk/src/common" "%MainProjectDir%" >"%TmpDir%\command.log"
if errorlevel 1 (
	echo Cannot retrieve the part of copy handler sources to establish svn version. See the log below:
	type "%TmpDir%\command.log"
	goto error
)

echo    * Scanning directory for version number
rem Get SVN version +2 to correct the nearest commit
set _ver_cmd=svnversion -n "%MainProjectDir%"
for /f %%a in ('%_ver_cmd%') do set /a TrunkSVNVersion=%%a
if errorlevel 1 (
	echo Problem with scanning svn WC for current version.
	goto error
)
if "%TrunkSVNVersion%" == "" (
	echo Can't get the major version.
	goto error
)

SET /a SVNVersion=%TrunkSVNVersion%+2

echo    * Detecting version numbers
set _ver_cmd=type "%MainProjectDir%\version.h"
for /f "tokens=3 delims= " %%a in ('%_ver_cmd% ^|find "define PRODUCT_VERSION1 "') do set MajorVersion=%%a
if errorlevel 1 (
	echo Problem with retrieving MajorVersion.
	goto error
)
if "%MajorVersion%" == "" (
	echo Can't get the major version.
	goto error
)

set _ver_cmd=type "%MainProjectDir%\version.h"
for /f "tokens=3 delims= " %%a in ('%_ver_cmd% ^|find "define PRODUCT_VERSION2 "') do set MinorVersion=%%a
if errorlevel 1 (
	echo Problem with retrieving MinorVersion.
	goto error
)
if "%MinorVersion%" == "" (
	echo Can't get the major version.
	goto error
)

SET CustomVersion=0

if "%1" == "beta" (
	SET TrunkTextVersion=%MajorVersion%.%MinorVersion%beta-svn%TrunkSVNVersion%
	SET TextVersion=%MajorVersion%.%MinorVersion%beta-svn%SVNVersion%
) else (
	SET TrunkTextVersion=%MajorVersion%.%MinorVersion%Final
	SET TextVersion=%MajorVersion%.%MinorVersion%Final
)

if "%TextVersion%" == "" (
	echo Cannot calculate the text version.
	goto error
)

echo    * Detected current trunk at %MajorVersion%.%MinorVersion%.%TrunkSVNVersion%.%CustomVersion%, preparing to tag as %MajorVersion%.%MinorVersion%.%SVNVersion%.%CustomVersion%
goto cleanup

:error
rem Get outside of the temp directory to be able to delete it
call internal\clear_env.bat /skip_create
exit /b 1

:cleanup
call internal\clear_env.bat /skip_create

:end
exit /b 0
