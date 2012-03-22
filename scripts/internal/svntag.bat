@echo off

rem Mark the changes as local ones
setlocal

if [%1] == [] (
	echo Usage: svntag.bat TextVersion
	exit /b 1
)

SET TextVersion=%1

rem Include config
call config.bat
if errorlevel 1 (
	exit /b 1
)

call internal\clear_env.bat
if errorlevel 1 (
	echo ERROR: Problem with preparing environment.
	goto error
)

echo    * Tagging projects...
call internal\svntag_single.bat "%ReposIcpf%" %TextVersion%
if errorlevel 1 (
	goto error
)

call internal\svntag_single.bat "%ReposIctranslate%" %TextVersion%
if errorlevel 1 (
	goto error
)

call internal\svntag_single.bat "%ReposCH%" %TextVersion%
if errorlevel 1 (
	goto error
)

echo    * Checking out the tagged ch repository...
svn --ignore-externals co "%ReposCH%/tags/%TextVersion%" "%MainProjectDir%" >"%TmpDir%\command.log"
if errorlevel 1 (
	echo ERROR: encountered a problem while checking out copyhandler project. See the log below:
	type "%TmpDir%\command.log"
	goto error
)

echo    * Creating new svn:externals definition...

echo src/libicpf %ReposIcpf%/tags/%TextVersion%/src/libicpf >"%TmpDir%\externals.txt"
echo src/libictranslate %ReposIctranslate%/tags/%TextVersion%/src/libictranslate >>"%TmpDir%\externals.txt"
echo src/rc2lng %ReposIctranslate%/tags/%TextVersion%/src/rc2lng >>"%TmpDir%\externals.txt"
echo src/ictranslate %ReposIctranslate%/tags/%TextVersion%/src/ictranslate >>"%TmpDir%\externals.txt"

svn propedit --editor-cmd "type %TmpDir%\externals.txt >" svn:externals "%MainProjectDir%"  >"%TmpDir%\command.log"
if errorlevel 1 (
	echo ERROR: encountered a problem while checking out copyhandler project. See the log below:
	type "%TmpDir%\command.log"
	goto error
)

echo    * Updating version information...
cscript //NoLogo internal\replace_version.vbs "%MainProjectDir%\src\common\version.h.template" "%MainProjectDir%\src\common\version.h" %MajorVersion% %MinorVersion% %SVNVersion% %CustomVersion% %TextVersion% >"%TmpDir%\command.log"
if errorlevel 1 (
	echo ERROR: encountered a problem while checking out copyhandler project. See the log below:
	type "%TmpDir%\command.log"
	goto error
)

echo    * Performing commit of the updated svn:externals...
svn commit -m "Updated svn:externals definition" "%MainProjectDir%" >"%TmpDir%\command.log"
if errorlevel 1 (
	echo ERROR: encountered a problem while committing changes to repository. See the log below:
	type "%TmpDir%\command.log"
	goto error
)

goto cleanup

:error
call internal\clear_env.bat /skip_create

exit /b 1

:cleanup
echo    * Cleaning up the temporary files...
call internal\clear_env.bat /skip_create

:end
exit /b 0
