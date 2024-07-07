if "%1"=="tidy" (
	del *.obj
	del *.exe
	del *.pdb
	del *.ilk
	del *.dll
	exit /b
)

copy /y ..\..\..\ext\ext-sdl3\x64\SDL3.dll
cl *.cpp -I 3rd /std:c++20 /EHsc -I ..\..\..\ext\ext-sdl3\inc\ /link /LIBPATH:..\..\..\ext\ext-sdl3\x64 %*
