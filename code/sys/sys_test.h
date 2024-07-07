// ----------------------------------------------------------------------------
// tests

#if !CODE

API int test(const char *file, int line, const char *expr, bool result);

#if TESTS
#define test(expr) test(__FILE__,__LINE__,#expr,!!(expr))
#else
#define test(expr) 0
#endif

#if TESTS
#define AUTOTEST AUTORUN
#else
#define AUTOTEST static void CONCAT(CONCAT(CONCAT(disabled_test_, __LINE__), _), __COUNTER__)()
#endif

#else

static __thread int test_oks, test_errors, test_once;
static void test_exit(void) { fprintf(stderr, "%d/%d tests passed\n", test_oks, test_oks+test_errors); }
int (test)(const char *file, int line, const char *expr, bool result) {
    static int breakon = -1; if(breakon<0) breakon = optioni("--test-break", 0);
    if( breakon == (test_oks+test_errors+1) ) alert("user requested to break on this test"), breakpoint();
    test_once = test_once || !(atexit)(test_exit);
    test_oks += result, test_errors += !result;
    return (result || (fprintf(stderr, ANSI_RED( "(Test `%s` failed %s:%d)\n" ), expr, file, line), 0) );
}

AUTOTEST { test(1<2); }

#endif
