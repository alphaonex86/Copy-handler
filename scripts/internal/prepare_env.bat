@echo off

call config.bat
if errorlevel 1 (
	exit /b 1
)

rem Setup environment
if not exist "%TmpDir%" (
	mkdir "%TmpDir%"
	if not exist "%TmpDir%" (
		echo ERROR: Creating temporary folder failed.
		exit /b 1
	)
)

if not exist "%OutputDir%" (
	mkdir "%OutputDir%"
	if not exist "%OutputDir%" (
		echo ERROR: Creating temporary folder failed. See the log below:
		type "%TmpDir%\command.log"
		exit /b 1
	)
)

rem Prepare directories
if not exist "%TmpDir%\zip32" (
	mkdir "%TmpDir%\zip32"
	if not exist "%TmpDir%\zip32" (
		echo ERROR: Creating temporary zip32 folder failed.
		goto error
	)
)

if not exist "%TmpDir%\zip64" (
	mkdir "%TmpDir%\zip64"
	if not exist "%TmpDir%\zip64" (
		echo ERROR: Creating temporary zip64 folder failed.
		goto error
	)
)

exit /b 0
