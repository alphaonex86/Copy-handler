@echo off

call config.bat
if errorlevel 1 (
	exit /b 1
)

rem Setup environment
if exist "%TmpDir%" (
	echo    * Removing temporary directory...

	cd "%TmpDir%\.."

	rmdir /S /Q "%TmpDir%" >nul
	if exist "%TmpDir%" (
		echo ERROR: Deleting the old temporary folder failed.
		exit /b 1
	)
)

if NOT "%1" == "/skip_create" (
	if exist "%OutputDir%" (
		echo    * Removing OutputDirectory...
		rmdir /S /Q "%OutputDir%" >nul
		if exist "%OutputDir%" (
			echo ERROR: Deleting the old output folder failed.
			exit /b 1
		)
	)

	mkdir "%TmpDir%"
	if not exist "%TmpDir%" (
		echo ERROR: Creating temporary folder failed.
		exit /b 1
	)

	mkdir "%OutputDir%"
	if not exist "%OutputDir%" (
		echo ERROR: Creating temporary folder failed.
		exit /b 1
	)

	mkdir "%TmpDir%\zip32"
	if not exist "%TmpDir%\zip32" (
		echo ERROR: Creating temporary zip32 folder failed.
		goto error
	)

	mkdir "%TmpDir%\zip64"
	if not exist "%TmpDir%\zip64" (
		echo ERROR: Creating temporary zip64 folder failed.
		goto error
	)
)

exit /b 0
