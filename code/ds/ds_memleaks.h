// memory leaks detector ------------------------------------------------------
// you watch() every allocation you make, then forget() every pointer you free.
// since the api is very manual and error prone, it's recommended that you use
// a memory allocator that already integrates both calls. our default allocator
// already integrates this api as long as you compile with -DENABLE_MEMORY_LEAKS

API void*  watch( void *ptr, int sz );
API void*  forget( void *ptr );

#if CODE

// leaks ----------------------------------------------------------------------

void* watch( void *ptr, int sz ) {
    static __thread int open = 1;
    if( ptr && open ) {
        open = 0;

        char buf[256];
        sprintf(buf, "%p.mem", ptr);
        for( FILE *fp = fopen(buf, "a+"); fp; fclose(fp), fp = 0 ) {
            fseek(fp, 0L, SEEK_END);
            const char *cs = trace( +16 ); // +48
            fprintf(fp, "Built %s %s\n", __DATE__, __TIME__); // today() instead?
            fprintf(fp, "Memleak address: [%p], size: %d\n%s\n", ptr, sz, cs ? cs : "No callstack.");
        }

        open = 1;
    }
    return ptr;
}
void* forget( void *ptr ) {
    if( ptr ) {
        char buf[256];
        sprintf(buf, "%p.mem", ptr);
        unlink(buf);
    }
    return ptr;
}

#endif
