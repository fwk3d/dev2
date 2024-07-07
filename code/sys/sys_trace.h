// Note: Windows users add `/Zi` compilation flags, else add `-g` and/or `-ldl` flags
// Note: If you are linking your binary using GNU ld you need to add --export-dynamic

API char*       trace( int traces ); // write callstack into a temporary string. <0 traces to invert order. do not free().
API int         tracef( FILE *fp, int traces ); // write callstack to file. <0 traces to invert order.


#if CODE

#if is(osx)
#include <execinfo.h> // backtrace, backtrace_symbols
#include <dlfcn.h>    // dladdr, Dl_info
#elif is(gcc) && !is(ems) && !is(mingw) // maybe is(linux) is enough?
#include <execinfo.h>  // backtrace, backtrace_symbols
#elif is(win32) // && !defined __TINYC__
#include <winsock2.h>  // windows.h alternative
#include <dbghelp.h>
#pragma comment(lib, "DbgHelp")
#pragma comment(lib, "Kernel32")
static int backtrace( void **addr, int maxtraces ) {
    static bool init = 0;
    do_once SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_INCLUDE_32BIT_MODULES);
    do_once init = SymInitialize(GetCurrentProcess(), NULL, TRUE);
    if(!init) return 0; // error: cannot initialize DbgHelp.lib

    //typedef USHORT (WINAPI *pFN)(__in ULONG, __in ULONG, __out PVOID*, __out_opt PULONG); // _MSC_VER
    typedef USHORT (WINAPI *pFN)(); // TINYC
    static pFN rtlCaptureStackBackTrace = 0;
    if( !rtlCaptureStackBackTrace ) {
        rtlCaptureStackBackTrace = (pFN)GetProcAddress(LoadLibraryA("kernel32.dll"), "RtlCaptureStackBackTrace");
    }
    if( !rtlCaptureStackBackTrace ) {
        return 0;
    }
    return rtlCaptureStackBackTrace(1, maxtraces, (PVOID *)addr, (DWORD *) 0);
}
static char **backtrace_symbols(void *const *list,int size) {
    HANDLE process = GetCurrentProcess();

    struct symbol_t {
        SYMBOL_INFO info;
        TCHAR symbolname[256], terminator;
    } si = { {0} };
    si.info.SizeOfStruct = sizeof(SYMBOL_INFO);
    si.info.MaxNameLen = sizeof(si.symbolname) / sizeof(TCHAR); // number of chars, not bytes

    IMAGEHLP_LINE l64 = { 0 };
    l64.SizeOfStruct = sizeof(IMAGEHLP_LINE);

    static __thread char **symbols = 0; //[32][64] = {0};
    if( !symbols ) {
        symbols = SYS_MEM_REALLOC(0, 128 * sizeof(char*));
        for( int i = 0; i < 128; ++i) symbols[i] = SYS_MEM_REALLOC(0, 128 * sizeof(char));
    }

    if(size > 128) size = 128;
    for( int i = 0; i < size; ++i ) {

        char *ptr = symbols[i];
        *ptr = '\0';

        if (SymFromAddr(process, (DWORD64)(uintptr_t)list[i], 0, &si.info)) {
            //char undecorated[1024];
            //UnDecorateSymbolName(si.info.Name, undecorated, sizeof(undecorated)-1, UNDNAME_COMPLETE);
            char* undecorated = (char*)si.info.Name;
            ptr += snprintf(ptr, 128, "%s", undecorated);
        } else {
            ptr += snprintf(ptr, 128, "%s", "(?""?)");
        }

        DWORD dw = 0;
        if (SymGetLineFromAddr(process, (DWORD64)(uintptr_t)list[i], &dw, &l64)) {
            ptr += snprintf(ptr, 128 - (ptr - symbols[i]), " (%s:%u)", l64.FileName, (unsigned)l64.LineNumber);
        }
    }

    return symbols;
}
#else
static int backtrace(void **heap, int num) { return 0; }
static char **backtrace_symbols(void *const *sym,int num) { return 0; }
#endif

char *trace( int traces ) {
#if is(tcc) && is(linux)
    return "";
#endif
#if is(ems) // there is a stack overflow failure somewhere in the impl below
    static char empty[1]; return empty[0] = '\0', empty;
#endif

    enum { skip = 1 }; /* exclude 1 trace from stack (this function) */
    enum { maxtraces = 96 };

    static __thread char *output = 0;
    if(!output ) output = SYS_MEM_REALLOC( 0, maxtraces * (128+2) );
    if( output ) output[0] = '\0';
    char *ptr = output;

    int inc = 1;
    if( traces < 0 ) traces = -traces, inc = -1;
    if( traces == 0 ) return "";
    if( traces > maxtraces ) traces = maxtraces;

    void* stacks[maxtraces + 1]; stacks[maxtraces] = NULL; // = { 0 };
    traces = backtrace( stacks, traces );
    char **symbols = backtrace_symbols( stacks, traces ); // @todo: optimization: map(void*,char*) cache; and retrieve only symbols not in cache

    char demangled[1024] = "??";
    int L = 0, B = inc>0 ? skip - 1 : traces, E = inc>0 ? traces : skip - 1;
    for( int i = B; ( i += inc ) != E; ) {
#if is(linux)
        #if ENABLE_LINUX_CALLSTACKS
        // @fixme: following snippet works if compiled with '-g', albeit terribly slow
        // should concat addresses into a multi-address line

        char *binary = symbols[i];
        char *address = strchr( symbols[i], '(' ) + 1;
        *strrchr( address, ')') = '\0'; *(address - 1) = '\0';

        char addr2line[128]; snprintf(addr2line, 128, "addr2line -e %s %s", binary, address);

        for( FILE *fp = popen(addr2line, "r"); fp ; pclose(fp), fp = 0 ) { //addr2line -e binary -f -C address
            fgets(demangled, sizeof(demangled), fp);
            int len = strlen(demangled); while( len > 0 && demangled[len-1] < 32 ) demangled[--len] = 0;
        }
        symbols[i] = demangled;
        #else
        // make it shorter. ie, `0x00558997ccc87e ./a.out(+0x20187e) [0x00558997ccc87e]`
        strchr(symbols[i], ')')[1] = '\0';
        #endif
#elif is(osx)
        /*struct*/ Dl_info info;
        if( dladdr(stacks[i], &info) && info.dli_sname ) {
            const char *dmgbuf = info.dli_sname[0] != '_' ? NULL :
                 ifdef(cpp, __cxa_demangle(info.dli_sname, NULL, 0, NULL), info.dli_sname);
            strcpy( demangled, dmgbuf ? dmgbuf : info.dli_sname );
            symbols[i] = demangled;
            ifdef(cpp, dmgbuf && free( (void*)dmgbuf ) );
        }
#endif
        if( symbols[i] )
        ptr += sprintf(ptr, "%03d: %p %s\n", ++L, (void*)(uintptr_t)stacks[i], symbols[i]); // format gymnastics because %p is not standard when printing pointers
    }

#if is(linux) || is(osx)
     if(symbols) free(symbols);
#endif

     return output ? output : "";
}

int tracef( FILE *fp, int traces ) {
    char *buf = trace(traces);
    fputs(buf, fp);
    fflush(fp);
    return 0;
}

#endif
