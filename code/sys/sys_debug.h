// -----------------------------------------------------------------------------
// debug utilities

//API void        beep();
API void        hang();
API void        crash();

API void        breakpoint();
API bool        has_debugger();

API void        hexdump( const void *ptr, unsigned len );
API void        hexdumpf( FILE *fp, const void *ptr, unsigned len, int width );

#if CODE
#include <stdio.h>
void hexdumpf( FILE *fp, const void *ptr, unsigned len, int width ) {
    unsigned char *data = (unsigned char*)ptr;
    for( unsigned jt = 0; jt <= len; jt += width ) {
        fprintf( fp, "; %05d%s", jt, jt == len ? "\n" : " " );
        for( unsigned it = jt, next = it + width; it < len && it < next; ++it ) {
            fprintf( fp, "%02x %s", (unsigned char)data[it], &" \n\0...\n"[ (1+it) < len ? 2 * !!((1+it) % width) : 3 ] );
        }
        fprintf( fp, "; %05d%s", jt, jt == len ? "\n" : " " );
        for( unsigned it = jt, next = it + width; it < len && it < next; ++it ) {
            fprintf( fp, " %c %s", (signed char)data[it] >= 32 ? (signed char)data[it] : (signed char)'.', &" \n\0..."[ (1+it) < len ? 2 * !!((1+it) % width) : 3 ] );
        }
    }
    fprintf(fp, " %d bytes\n", len);
}
void hexdump( const void *ptr, unsigned len ) {
    hexdumpf( stdout, ptr, len, 16 );
}

#if 0 // is(cl) only
static void debugbreak(void) {
    do { \
        __try { DebugBreak(); } \
        __except (GetExceptionCode() == EXCEPTION_BREAKPOINT ? \
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {} \
    } while(0);
}
#endif

#if is(win32)
static void debugbreak(void) { if(IsDebuggerPresent()) DebugBreak(); }
#else // is(unix)
static int is_debugger_present = -1;
static void _sigtrap_handler(int signum) {
    is_debugger_present = 0;
    signal(SIGTRAP, SIG_DFL);
}
static void debugbreak(void) { // break if debugger present
    // __builtin_trap(); //
    //raise(SIGABRT); // SIGTRAP);
    //__asm__ volatile("int $0x03");
    if( is_debugger_present < 0 ) {
        is_debugger_present = 1;
        signal(SIGTRAP, _sigtrap_handler);
        raise(SIGTRAP);
    }
}
#endif

void breakpoint() {
    debugbreak();
}

bool has_debugger() {
#if is(win32)
    return IsDebuggerPresent(); // SetLastError(123); OutputDebugStringA("\1"); enabled = GetLastError() != 123;
#else
    return false;
#endif
}

void hang() {
    for(;;);
}
void crash() {
    volatile int *p = 0;
    *p = 42;
}

#if 0
void beep() {
    ifdef(win32, app_spawn("rundll32 user32.dll,MessageBeep"); return; );
    ifdef(linux, app_spawn("paplay /usr/share/sounds/freedesktop/stereo/message.oga"); return; );
    ifdef(osx,   app_spawn("tput bel"); return; );

    //fallback:
    fputc('\x7', stdout);

    // win32:
    // rundll32 user32.dll,MessageBeep ; ok
    // rundll32 cmdext.dll,MessageBeepStub ; ok

    // osx:
    // tput bel
    // say "beep"
    // osascript -e 'beep'
    // osascript -e "beep 1"
    // afplay /System/Library/Sounds/Ping.aiff
    // /usr/bin/printf "\a"

    // linux:
    // paplay /usr/share/sounds/freedesktop/stereo/message.oga ; ok
    // paplay /usr/share/sounds/freedesktop/stereo/complete.oga ; ok
    // paplay /usr/share/sounds/freedesktop/stereo/bell.oga ; ok
    // beep ; apt-get
    // echo -e '\007' ; mute
    // echo -e "\007" >/dev/tty10 ; sudo
    // tput bel ; mute
}
#endif

#endif
