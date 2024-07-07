API int         argc();
API char*       argv(int);
API void        argvadd(const char *arg);

#if CODE

#if (is(tcc) && is(linux)) || (is(gcc) && !is(mingw)) // || is(clang)
int __argc; char **__argv;
#if !is(ems)
__attribute__((constructor)) void init_argcv(int argc, char **argv) { __argc = argc; __argv = argv; }
#endif
#endif

// ----------------------------------------------------------------------------
// argc/v

static void argc_init() {
#if is(tcc) && is(linux)
    do_once {
        char buffer[128], arg0[128] = {0};
        for( FILE *fp = fopen("/proc/self/status", "rb"); fp; fclose(fp), fp = 0) {
            while( fgets(buffer, 128, fp) ) {
                if( strbeg(buffer, "Name:") ) {
                    sscanf(buffer + 5, "%s", arg0 );
                    break;
                }
            }
        }
        extern char **environ;
        __argv = environ - 2; // last argv, as stack is [argc][argv0][argv1][...][NULL][envp]
        while( !strend(*__argv,arg0) ) --__argv;
        __argc = *(int*)(__argv-1);
    }
#endif
#if is(tcc)
    // skip tcc argvs (if invoked as tcc file.c engine.c -g -run) (win)
    if( __argc > 1 ) if( strstr(__argv[0], "/tcc") || strstr(__argv[0], "\\tcc") ) {
        __argc = 0;
    }
#endif
}

int argc() {
    do_once argc_init();
    return __argc;
}
char* argv(int arg) {
    do_once argc_init();
    static __thread char empty[1];
    return (unsigned)arg < __argc ? __argv[arg] : (empty[0] = '\0', empty);
}

void argvadd(const char *arg) {
    char **argv = MALLOC( sizeof(char*) * (__argc+1) );
    for( int i = 0; i < __argc; ++i ) {
        argv[i] = __argv[i];
    }
    argv[__argc] = STRDUP(arg);
    __argv = argv;
    ++__argc;
}

static
void cwdexe(void) {
    // relocate cwd to exe folder (relative paths may not work from this point)
    static char buffer[MAX_PATH] = {0};
    if( !buffer[0] ) {
        #ifdef __APPLE__
        realpath(__argv[0],buffer);
        if(strrchr(buffer, '/')) 1[strrchr(buffer,'/')] = '\0';
        #elif defined _WIN32
        GetModuleFileName(0,buffer,MAX_PATH);
        *strrchr(buffer, '\\') = '\0';
        #else
        char path[32] = {0};
        sprintf(path, "/proc/%d/exe", getpid());
        readlink(path, buffer, sizeof(buffer));
        if(strrchr(buffer, '/')) 1[strrchr(buffer,'/')] = '\0';
        #endif
    }

    ifdef(win32, SetCurrentDirectoryA, chdir)(buffer);
}

AUTORUN {
    // relocate cwd to executable location
    cwdexe();

    // remove paths from argv[0]: ../home/prj/my_app > my_app
    char *a = strrchr(__argv[0], '/');
    char *b = strrchr(__argv[0], '\\');
    __argv[0] = a > b ? a+1 : b > a ? b+1 : __argv[0];
}

#endif // CODE
