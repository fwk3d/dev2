// dll ------------------------------------------------------------------------

/// !!! `filename` must contain extension
/// load dynamic library `file` and search for `symbol`
/// return: NULL if not found, found symbol otherwise.
/// filename: path to dynamic library file. must contain extension.
/// symbol: symbol name. must not be NULL.
/// see: dlopen^, dlclose^
/// > bool (*plugin_init)(void) = dll("plugin.dll", "init");
/// > assert(plugin_init());

API void* dll(const char *filename, const char *symbol);

#if CODE

void* dll(const char *fname, const char *symbol) {
    if( fname && !is_file(fname) ) {
        const char *buf, *path = file_path(fname), *base = file_name(fname);
        if( is_file(buf = va("%s%s.dll", path, base)) ||
            is_file(buf = va("%s%s.so", path, base)) ||
            is_file(buf = va("%slib%s.so", path, base)) ||
            is_file(buf = va("%s%s.dylib", path, base)) ) {
            fname = (const char *)buf;
        } else {
            return NULL;
        }
    }
#if is(win32)
    return (void*)GetProcAddress(fname ? LoadLibraryA(fname) : GetModuleHandleA(NULL), symbol);
#else
    return dlsym(fname ? dlopen(fname, RTLD_NOW|RTLD_LOCAL) : ifdef(osx, RTLD_SELF, NULL), symbol);
#endif
}

#if 0 // demo: cl demo.c /LD && REM dll
EXPORT int add2(int a, int b) { return a + b; }
int main() { int (*adder)() = dll("demo.dll", "add2"); printf("%d\n", adder(2,3)); }
#endif

#endif
