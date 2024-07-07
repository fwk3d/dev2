API void        traps_handled(void); // install handlers for diverse traps

#if CODE
#include <signal.h>

// trap signals ---------------------------------------------------------------

const char *trap_name(int signal) {
    if(signal == SIGABRT) return "SIGABRT: abnormal termination";
    if(signal == SIGFPE) return "SIGFPE: floating point exception";
    if(signal == SIGILL) return "SIGILL: illegal instruction";
    if(signal == SIGSEGV) return "SIGSEGV: segmentation fault / invalid memory access";
    if(signal == SIGINT) return "SIGINT: interrupted program";
    if(signal == SIGTERM) return "SIGTERM: termination request";
    ifndef(win32, if(signal == SIGBUS) return "SIGBUS");
    ifdef(linux, if(signal == SIGSTKFLT) return "SIGSTKFLT");
    ifndef(win32, if(signal == SIGQUIT) return "SIGQUIT");
    return "??";
}
void ignore_trap(int sgn) {
    signal(sgn, ignore_trap);
}
void shutdown_trap(int sgn) {
    signal(sgn, shutdown_trap);
    exit(0);
}
void abort_trap(int sgn) {
    char buffer[1024]; snprintf(buffer, 1024, "Error: unexpected signal %s (%d)\n%s", trap_name(sgn), sgn, trace(+16));
    fprintf(stderr, "%s\n", buffer), alert(buffer), breakpoint();
    signal(sgn, abort_trap);
    exit(-sgn);
}
void debug_trap(int sgn) { // @todo: rename to trap_on_choice() and ask the developer what to do next? abort, continue, debug
    char buffer[1024]; snprintf(buffer, 1024, "Error: unexpected signal %s (%d)\n%s", trap_name(sgn), sgn, trace(+16));
    fprintf(stderr, "%s\n", buffer), alert(buffer), breakpoint();
    signal(sgn, debug_trap);
}
#if is(win32)
LONG WINAPI seh_trap(PEXCEPTION_POINTERS pExceptionPtrs) {
    char buffer[1024]; snprintf(buffer, 1024, "Error: unexpected SEH exception\n%s", trace(+16));
    fprintf(stderr, "%s\n", buffer), alert(buffer), breakpoint();
    return EXCEPTION_EXECUTE_HANDLER; // Execute default exception handler next
}
#endif

void traps_handled(void) {
    // expected signals
    signal(SIGINT, shutdown_trap);
    signal(SIGTERM, shutdown_trap);
    ifndef(win32,
    signal(SIGQUIT, shutdown_trap));
    // unexpected signals
    signal(SIGABRT, abort_trap);
    signal(SIGFPE, abort_trap);
    signal(SIGILL, abort_trap);
    signal(SIGSEGV, abort_trap);
    ifndef(win32,
    signal(SIGBUS, abort_trap));
    ifdef(linux,
    signal(SIGSTKFLT, abort_trap));
    // others
    ifdef(win32,
    SetUnhandledExceptionFilter(seh_trap));
}

#ifdef TRAP_DEMO
AUTORUN {
    traps_handled();
    app_crash(); // app_hang();
}
#endif

AUTORUN {
    // install signal handlers
    if( optioni("--traps", 1) ) {
        traps_handled();
    }
}

#endif // CODE
