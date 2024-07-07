#if CODE

AUTORUN {
	recipe("*.ase;*.aseprite", 0, "ext/ext-ase2ini/ase2ini.EXE \"INPUT\" > \"OUTPUT\"");
}

#endif
