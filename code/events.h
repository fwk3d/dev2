#if !CODE

typedef void (*event_fn_v)();
typedef int  (*event_fn_i)();

API int  event(const char *name);      // fire registered events for a given category
API void events(const char *fmt, ...); // register comma-separated event functions

API int  command(const char *cmd);
API int  command2(const char *cmd, const char *val);

#else

array(event_fn_v) inits = 0;
array(event_fn_v) ticks = 0;
array(event_fn_v) draws = 0;
array(event_fn_v) swaps = 0;
array(event_fn_v) quits = 0;
array(event_fn_i) execs = 0;

int event(const char *name) {
    /**/ if(name[0] == 't') for(int i = 0, e = array_count(ticks); i < e; ++i) ticks[i]();
    else if(name[0] == 'd') for(int i = 0, e = array_count(draws); i < e; ++i) draws[i]();
    else if(name[0] == 's') for(int i = 0, e = array_count(swaps); i < e; ++i) swaps[i]();
    else if(name[0] == 'i') for(int i = 0, e = array_count(inits); i < e; ++i) inits[i]();
    else if(name[0] == 'q') for(int i = 0, e = array_count(quits); i < e; ++i) quits[i]();
    else return 0;
    return 1;
}

void events(const char *fmt, ...) {
    const char *names[32] = {fmt}; // @fixme: 32
    int count = 1; while(*fmt++) if( fmt[-1] == ',' ) names[count++] = fmt;

    va_list list;
    va_start(list, fmt);

    for( int i = 0; i < count; ++i ) {
        event_fn_v fun = va_arg(list, event_fn_v);
        /**/ if(names[i][0] == 'i') array_push(inits, fun);
        else if(names[i][0] == 't') array_push(ticks, fun);
        else if(names[i][0] == 'd') array_push(draws, fun);
        else if(names[i][0] == 's') array_push(swaps, fun);
        else if(names[i][0] == 'q') array_push(quits, fun);
        else if(names[i][0] == 'e') array_push(execs, (event_fn_i)fun);
        else die("error: event hook not found '%s'\n", names[i]);
    }

    va_end(list);
}

int command2(const char *key, const char *val) {
    for(int i = 0, e = array_count(execs); i < e; ++i) 
        if( execs[i](key, val) )
            return 1;

    PRINTF("Warning: unknown command `%s=%s`\n", key, val);
    return 0;
}

int command(const char *cmd) {
    char *equal = strchr(cmd, '=');
    if( equal ) {
        char key[128];
        snprintf(key, 128, "%.*s", (int)(equal - cmd), cmd);
        return command2(key, equal+1);
    }
    return command2(cmd, "");
}

#endif
