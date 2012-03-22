@echo off

rem setlocal ENABLEDELAYEDEXPANSION

rem Validate input parameters
if [%1] == [] (
	echo Usage: detect_version_internal.bat path
	goto end
)

echo    * Scanning directory for version number
rem Get SVN version
SET _ver_cmd=svnversion -n "%1"
for /f %%a in ('%_ver_cmd%') do set SVNVersion=%%a
if errorlevel 1 (
	echo Problem with scanning svn WC for current version.
	goto error
)
if "%SVNVersion%" == "" (
	echo Can't get the major version.
	goto error
)

echo    * Detecting version numbers
set _ver_cmd=type "%1\src\common\version.h"
for /f "tokens=3 delims= " %%a in ('%_ver_cmd% ^|find "define PRODUCT_VERSION1 "') do set MajorVersion=%%a
if errorlevel 1 (
	echo Problem with retrieving MajorVersion.
	goto error
)
if "%MajorVersion%" == "" (
	echo Can't get the major version.
	goto error
)

set _ver_cmd=type "%1\src\common\version.h"
for /f "tokens=3 delims= " %%a in ('%_ver_cmd% ^|find "define PRODUCT_VERSION2 "') do set MinorVersion=%%a
if errorlevel 1 (
	echo Problem with retrieving MinorVersion.
	goto error
)
if "%MinorVersion%" == "" (
	echo Can't get the major version.
	goto error
)

SET CustomVersion=1

SET TextVersion=%MajorVersion%.%MinorVersion%internal-svn%SVNVersion%
if "%TextVersion%" == "" (
	echo Cannot calculate the text version.
	goto error
)

echo    * Detected current trunk at %MajorVersion%.%MinorVersion%.%SVNVersion%.%CustomVersion%
goto end

:error
rem Get outside of the temp directory to be able to delete it
exit /b 1

:end
exit /b 0
