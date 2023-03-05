@REM #change current directory to this file
@%~d0
@cd %~dp0

cmake -G "Visual Studio 17 2022"  -B build/SimpleGameEngine-x64-windows
cmake -G "Visual Studio 17 2022"  -B build/SimpleGameEngine-client-x64-windows -DBUILD_TARGET=_TARGET_CLIENT
cmake -G "Visual Studio 17 2022"  -B build/SimpleGameEngine-server-x64-windows -DBUILD_TARGET=_TARGET_SERVER
cmake -G "Visual Studio 17 2022"  -B build/SimpleGameEngine-editor-x64-windows -DBUILD_TARGET=_TARGET_EDITOR

@pause