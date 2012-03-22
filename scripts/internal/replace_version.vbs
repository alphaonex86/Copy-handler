Const ForReading = 1
Const ForWriting = 2

strOldFileName = Wscript.Arguments(0)
strNewFileName = Wscript.Arguments(1)
strMajor = WScript.Arguments(2)
strMinor = WScript.Arguments(3)
strSVN = WScript.Arguments(4)
strCustom = WScript.Arguments(5)
strTextVersion = WScript.Arguments(6)

' Validate parameters
if strOldFileName = "" or strNewFileName = "" or strMajor = "" or strMinor = "" or strSVN = "" or strCustom ="" or strTextVersion="" then
	WScript.echo "Missing input parameters"
	return 1
end if

Set objFSO = CreateObject("Scripting.FileSystemObject")

' Read the source file
Set objFile = objFSO.OpenTextFile(strOldFileName, ForReading)

strText = objFile.ReadAll
objFile.Close

' Replace the text
strText = Replace(strText, "{major_version}", strMajor)
strText = Replace(strText, "{minor_version}", strMinor)
strText = Replace(strText, "{svn_version}", strSVN)
strText = Replace(strText, "{custom_version}", strCustom)
strText = Replace(strText, "{text_version}", strTextVersion)

' And write the text
Set objFile = objFSO.OpenTextFile(strNewFileName, ForWriting)
objFile.WriteLine strText
objFile.Close
