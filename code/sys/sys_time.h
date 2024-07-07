// ----------------------------------------------------------------------------
// time

API uint64_t    date();        // YYYYMMDDhhmmss
API uint64_t    date_epoch();  // linux epoch
API char*       date_string(); // "YYYY-MM-DD hh:mm:ss"

API double      time_hh();
API double      time_mm();
API double      time_ss();
API uint64_t    time_ms();
API uint64_t    time_us();
API uint64_t    time_ns();

API void        sleep_ss(double ss);
API void        sleep_ms(double ms);
API void        sleep_us(double us);
API void        sleep_ns(double us);

API void        yield();

#if CODE

#if 0
uint64_t time_gpu() {
    GLint64 t = 123456789;
    glGetInteger64v(GL_TIMESTAMP, &t);
    return (uint64_t)t;
}
#endif
uint64_t date() {
    time_t epoch = time(0);
    struct tm *ti = localtime(&epoch);
    return atoi64(va("%04d%02d%02d%02d%02d%02d",ti->tm_year+1900,ti->tm_mon+1,ti->tm_mday,ti->tm_hour,ti->tm_min,ti->tm_sec));
}
char *date_string() {
    time_t epoch = time(0);
    struct tm *ti = localtime(&epoch);
    return va("%04d-%02d-%02d %02d:%02d:%02d",ti->tm_year+1900,ti->tm_mon+1,ti->tm_mday,ti->tm_hour,ti->tm_min,ti->tm_sec);
}
uint64_t date_epoch() {
    time_t epoch = time(0);
    return epoch;
}

// high-perf functions

#define TIMER_E3 1000ULL
#define TIMER_E6 1000000ULL
#define TIMER_E9 1000000000ULL

uint64_t time_ns() {
    return SDL_GetTicksNS();
}
uint64_t time_us() {
    return time_ns() / TIMER_E3;
}
uint64_t time_ms() {
    return time_ns() / TIMER_E6;
}
double time_ss() {
    return time_ns() / 1e9; // TIMER_E9;
}
double time_mm() {
    return time_ss() / 60;
}
double time_hh() {
    return time_mm() / 60;
}

void sleep_ns( double ns ) {
    if(ns > 0) SDL_DelayPrecise(ns);
    else yield();
}
void sleep_us( double us ) {
    sleep_ns(us * 1e3);
}
void sleep_ms( double ms ) {
    sleep_ns(ms * 1e6);
}
void sleep_ss( double ss ) {
    sleep_ns(ss * 1e9);
}

void yield() {
#if is(win32)
    SwitchToThread(); // Sleep(0);
#else
    usleep(0);
#endif
}

#endif
