if not exist *.lib (
	del *.obj
	cl src\*.c -Iinc /MT %*
	lib /nologo /out:x64\spine-mt.lib *.obj
	del *.obj
	cl src\*.c -Iinc /MD %*
	lib /nologo /out:x64\spine-md.lib *.obj
	del *.obj
)
