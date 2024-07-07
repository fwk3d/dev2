@echo off
if "%1"=="tidy" (
	del *.pdb
	del *.exe
	del *.obj
	del *.ilk
	del *.exp
	del *.lib
	del *.dll
	exit /b
)

@echo on
copy /y \prj\2\code\3rd\SDL3\x64\SDL3.dll
for %%i in (tests\*.c) do cl /nologo %%i mojoal.c -I . -I \prj\2\code\3rd \prj\2\code\3rd\SDL3\x64\SDL3.lib -DAL_LIBTYPE_STATIC /Zi %*
