@echo off
setlocal EnableExtensions

if "%1" == "" GOTO Error

rmdir /s /q nuget
mkdir nuget 2> NUL
mkdir nuget\lib 2> NUL
mkdir nuget\src 2> NUL

copy SimpleActiveScriptHost\Release\bin\*.dll nuget\lib
copy SimpleActiveScriptHost\Release\bin\*.pdb nuget\lib
copy SimpleActiveScriptHost.nuspec nuget
robocopy SimpleActiveScriptHost nuget\src /S /XD Release Debug /XF *.user

pushd nuget
nuget pack SimpleActiveScriptHost.nuspec -properties id=%1 -symbols
popd
 
GOTO End

:Error
echo "Missing ID parameter"
pause

:End
