// libtcc sample
// - rlyeh, public domain

#if !CODE

struct TCCState;
typedef struct TCCState TCCState;

API extern TCCState *(*tcc_new)(void); // create a new TCC compilation context
API extern int (*tcc_set_output_type)(TCCState *s, int output_type); // set output type. MUST BE CALLED before any compilation
API extern int (*tcc_compile_string)(TCCState *s, const char *buf); // compile a string containing a C source. Return -1 if error.
API extern int (*tcc_relocate)(TCCState *s1, void *ptr); // do all relocations (needed before using tcc_get_symbol())
API extern void *(*tcc_get_symbol)(TCCState *s, const char *name); // return symbol value or NULL if not found
API extern void (*tcc_delete)(TCCState *s); // free a TCC compilation context

enum { TCC_OUTPUT_MEMORY =     1 }; /* output will be run in memory (default) */
enum { TCC_OUTPUT_EXE =        2 }; /* executable file */
enum { TCC_OUTPUT_DLL =        3 }; /* dynamic library */
enum { TCC_OUTPUT_OBJ =        4 }; /* object file */
enum { TCC_OUTPUT_PREPROCESS = 5 }; /* only preprocess (used internally) */

#define TCC_RELOCATE_AUTO (void*)1

#else

TCCState *(*tcc_new)(void); // create a new TCC compilation context
int (*tcc_set_output_type)(TCCState *s, int output_type); // set output type. MUST BE CALLED before any compilation
int (*tcc_compile_string)(TCCState *s, const char *buf); // compile a string containing a C source. Return -1 if error.
int (*tcc_relocate)(TCCState *s1, void *ptr); // do all relocations (needed before using tcc_get_symbol())
void *(*tcc_get_symbol)(TCCState *s, const char *name); // return symbol value or NULL if not found
void (*tcc_delete)(TCCState *s); // free a TCC compilation context

static
void tcc_init() {
    tcc_new = dll("libtcc.dll", "tcc_new");
    tcc_relocate = dll("libtcc.dll", "tcc_relocate");
    tcc_get_symbol = dll("libtcc.dll", "tcc_get_symbol");
    tcc_set_output_type = dll("libtcc.dll", "tcc_set_output_type");
    tcc_compile_string = dll("libtcc.dll", "tcc_compile_string");
    tcc_delete = dll("libtcc.dll", "tcc_delete");
}

AUTORUN {
    tcc_init();
}

#endif
