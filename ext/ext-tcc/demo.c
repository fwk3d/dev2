// libtcc sample
// - rlyeh, public domain

#include "engine.h"

void* c_call(const char *program, const char *sym) {
    TCCState *s = tcc_new();
    if( !s ) die("Cannot create a tcc context");

    tcc_set_output_type(s, TCC_OUTPUT_MEMORY);
    if( tcc_compile_string(s, program) > 0 ) {
        die("C compilation error");
    }

    tcc_relocate(s, TCC_RELOCATE_AUTO);
    return tcc_get_symbol(s, sym);
    // tcc_delete(s); // @leak
}

int main() {
    const char* program =
    "int foo(int value){ return printf(\"Hello from a C script. This is a test: %d\n\", value); }";

    int (*foo)(int in_value) = c_call(program, "foo");
    foo(42);
}
