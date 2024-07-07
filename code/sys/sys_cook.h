// register a recipe to cook a given asset type
API int recipe( const char *pathmasks, int errorlevel, const char *scripts );

// cook any asset. this call is generally not needed. used mostly internally.
API const char *cook( const char *inputfile );

#if CODE

typedef struct recipe_t {
    const char *pathmask;
    const char *script; // @todo: multiline scripts
    int errorlevel;
} recipe_t;

array(recipe_t) rules;

int recipe(const char *pathmasks, int errorlevel, const char *script) {
    for each_substring(pathmasks,";",pathmask) {
        recipe_t rule = { strdup(pathmask), strdup(script), errorlevel }; // @leak
        array_push(rules, rule);
    }
    return 1;
}

const char* cook(const char *infile) {

    char precooked[DIR_MAX];
    snprintf(precooked, DIR_MAX, "%s.%s", file_path(infile), file_name(infile));
    if( is_file(precooked) ) return va("%s", precooked);

    for each_array(rules, recipe_t, recipe) {
        if( strmatch(infile, recipe.pathmask) ) {
            printf("cooking asset: %s with recipe %s\n", infile, recipe.pathmask);

            char outfile[DIR_MAX];
            snprintf(outfile, DIR_MAX, "%s.%s", file_path(infile), file_name(infile));

            char* script = 0;
            for each_substring(recipe.script, " \n", token ) { // @todo: multiline support
                /**/ if( strstr(token, ".EXE") ) token = file_norm(token);
                else if( !strcmp(token, "REN") ) token = ifdef(win32, "move /y", "mv -f");
                strcatf(&script, "%s ", token);
            }

            strrepl(&script, "INPUT", file_norm(infile));
            strrepl(&script, "OUTPUT", file_norm(outfile));

            puts(script);

            int rc = system(script);
            FREE(script);

            if( rc == recipe.errorlevel ) return va("%s", outfile);
        }
    }
    return infile;
}

#endif
