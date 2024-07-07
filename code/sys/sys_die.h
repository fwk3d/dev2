API extern void (*die)(const char *error);
API extern void (*threw)(const char *exception); // fatal if first [0] char is '!'

#ifndef die
#define die(...) die(va(__VA_ARGS__))
#endif

#if CODE

static
char *oom_reserve;

static
void die_handler(const char *message) {
   if(oom_reserve) FREE(oom_reserve);
   fprintf(stderr, "%s\n", message);
   fflush(stderr);

   SDL_SetClipboardText(message);

#if 0
   ifdef(win32, if(IsDebuggerPresent()) DebugBreak());
#else
   abort();
#endif
   exit(-1);
}

void (*die)(const char *error) = die_handler;
void (*threw)(const char *exception) = fn_void_str;

AUTORUN {
   oom_reserve = MALLOC(1<<20); // 1 MiB
}

#endif
