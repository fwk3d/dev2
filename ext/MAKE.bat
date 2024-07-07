#!/bin/bash 2>nul || goto :windows

# linux + osx -----------------------------------------------------------------
cd `dirname $0`
exit

: -----------------------------------------------------------------------------
:windows
@echo off

setlocal enabledelayedexpansion

pushd .
cd /d "%~dp0"

rem copy dlls/libs to root folder
for /D /R %%i in (*) do if exist %%i\x64\* xcopy /ys %%i\x64\* .. >nul 2>nul

rem generate ext.h file
echo // auto-generated file. do not edit  > ext.h
echo // list of extensions included here >> ext.h
echo. >> ext.h
for /D /R %%i in (*) do if exist %%i\api.h (
set DIR=%%i
set DIR=!DIR:%CD%\=!
set DIR=!DIR:\=/!
echo #if __has_include^("!DIR!/api.h"^) >> ext.h
echo #         include "!DIR!/api.h" >> ext.h
echo #endif >> ext.h
)

rem generate ext-demos.c file
echo // auto-generated file. do not edit  > ext-demos.c
echo // list of extensions included here >> ext-demos.c
echo. >> ext-demos.c
for /D /R %%i in (*) do if exist %%i\demo.c (
set DIR=%%i
set DIR=!DIR:%CD%\=!
set DIR=!DIR:\=/!
echo #if __has_include^("!DIR!/demo.c"^) >> ext-demos.c
echo #         include "!DIR!/demo.c" >> ext-demos.c
echo #endif >> ext-demos.c
)

popd

endlocal
