cl mod.c -DAPI=__declspec(dllexport) -DAPISTR="\"dll\"" /MT /LD 
del *.obj & del *.lib & del *.exp
