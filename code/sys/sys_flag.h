
API int         flag(const char *commalist); // --arg // app_flag?
API const char* option(const char *commalist, const char *defaults); // --arg=string or --arg string
API int         optioni(const char *commalist, int defaults); // --arg=integer or --arg integer
API float       optionf(const char *commalist, float defaults); // --arg=float or --arg float


#if CODE
// ----------------------------------------------------------------------------
// options

int flag(const char *commalist) {
    while( commalist[0] ) {
        const char *begin = commalist;
        while(*commalist != ',' && *commalist != '\0') ++commalist;
        const char *end = commalist;

        char token[128];
        snprintf(token,   128, "%.*s",  (int)(end - begin), begin);

        for( int i = 1; i < argc(); ++i ) {
            char *arg = argv(i);

            if( !strcmpi( arg, token ) ) {  // --arg
                return 1;
            }
        }

        commalist = end + !!end[0];
    }
    return 0;
}

const char *option(const char *commalist, const char *defaults) {
    while( commalist[0] ) {
        const char *begin = commalist;
        while(*commalist != ',' && *commalist != '\0') ++commalist;
        const char *end = commalist;

        char token[128], tokeneq[128];
        snprintf(token,   128, "%.*s",  (int)(end - begin), begin);
        snprintf(tokeneq, 128, "%.*s=", (int)(end - begin), begin);

        for( int i = 1; i < argc(); ++i ) {
            char *arg = argv(i);

            if( strbegi( arg, tokeneq ) ) { // --arg=value
                return argv(i) + strlen(tokeneq);
            }
            if( !strcmpi( arg, token ) ) {  // --arg value
                if( (i+1) < argc() ) {
                    return argv(i+1);
                }
            }
        }

        commalist = end + !!end[0];
    }
    return defaults;
}

int optioni(const char *commalist, int defaults) {
    const char *rc = option(commalist, 0);
    return rc ? atoi(rc) : defaults;
}
float optionf(const char *commalist, float defaults) {
    const char *rc = option(commalist, 0);
    return rc ? atof(rc) : defaults;
}

#endif // CODE

