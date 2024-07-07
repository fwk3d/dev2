cd /d "%~dp0"

if not exist luajit git clone https://github.com/luajit/luajit luajit --depth=1
if not exist luajit\src\*.lib pushd luajit\src && call msvcbuild.bat && popd

if not exist x64 md x64
if not exist inc md inc

copy /y luajit\src\*.dll x64\
copy /y luajit\src\*.lib x64\
copy /y luajit\src\*.h   inc\

rd /q /s luajit
