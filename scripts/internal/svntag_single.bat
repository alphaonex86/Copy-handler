@echo off

if [%1] == [] (
	echo Usage: svntag_single.bat ReposAddress TextVersion 
	exit /b 1
)

SET ReposAddress=%1
SET TextVersion=%2

call config.bat
if errorlevel 1 (
	exit /b 1
)

call internal\clear_env.bat
if errorlevel 1 (
	exit /b 1
)

rem check if the project isn't already tagged
svn co "%ReposAddress%/tags/%TextVersion%" "%MainProjectDir%" 2>nul
if errorlevel 1 (
	rem when error, it probably means that the tag does not exist
	goto create
)

rem if we're here, then the tag already exist
echo    * The repository already had a tag %TextVersion%. Skipping tagging.
exit /b 0

:create
echo    * Tagging %ReposAddress% as %TextVersion%...
svn cp -m "Tagged project to %TextVersion%" "%ReposAddress%/trunk/" "%ReposAddress%/tags/%TextVersion%" >"%TmpDir%\command.log"
if errorlevel 1 (
	echo ERROR: encountered a problem while tagging libicpf project. See the log below:
	type "%TmpDir%\command.log"
	exit /b 1
)

exit /b 0
