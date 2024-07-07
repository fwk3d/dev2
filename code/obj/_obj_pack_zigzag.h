// ----------------------------------------------------------------------------
// zigzag en/decoder

API uint64_t zig64( int64_t value ); // convert sign|magnitude to magnitude|sign
API int64_t zag64( uint64_t value ); // convert magnitude|sign to sign|magnitude

API uint32_t enczig32u( int32_t n);
API uint64_t enczig64u( int64_t n);
API  int32_t deczig32i(uint32_t n);
API  int64_t deczig64i(uint64_t n);


// ----------------------------------------------------------------------------
// zigzag en/decoder
// - rlyeh, public domain

uint64_t zig64( int64_t value ) { // convert sign|magnitude to magnitude|sign
    return (value >> 63) ^ (value << 1);
}
int64_t zag64( uint64_t value ) { // convert magnitude|sign to sign|magnitude
    return (value >> 1) ^ -(value & 1);
}

// branchless zigzag encoding 32/64
// sign|magnitude to magnitude|sign and back
// [ref] https://developers.google.com/protocol-buffers/docs/encoding
uint32_t enczig32u( int32_t n) { return ((n << 1) ^ (n >> 31)); }
uint64_t enczig64u( int64_t n) { return ((n << 1) ^ (n >> 63)); }
 int32_t deczig32i(uint32_t n) { return ((n >> 1) ^  -(n & 1)); }
 int64_t deczig64i(uint64_t n) { return ((n >> 1) ^  -(n & 1)); }

#if 0
AUTORUN {
    int16_t x = -1000;
    printf("%d -> %llu %llx -> %lld\n", x, zig64(x), zig64(x), zag64(zig64(x)));
}
AUTORUN {
    #define CMP32(signedN) do { \
        int32_t reconverted = deczig32i( enczig32u(signedN) ); \
        int equal = signedN == reconverted; \
        printf("[%s] %d vs %d\n", equal ? " OK " : "FAIL", signedN, reconverted ); \
    } while(0)

    #define CMP64(signedN) do { \
        int64_t reconverted = deczig64i( enczig64u(signedN) ); \
        int equal = signedN == reconverted; \
        printf("[%s] %lld vs %lld\n", equal ? " OK " : "FAIL", signedN, reconverted ); \
    } while(0)

    CMP32( 0);
    CMP32(-1);
    CMP32(+1);
    CMP32(-2);
    CMP32(+2);
    CMP32(INT32_MAX - 1);
    CMP32(INT32_MIN + 1);
    CMP32(INT32_MAX);
    CMP32(INT32_MIN);

    CMP64( 0ll);
    CMP64(-1ll);
    CMP64(+1ll);
    CMP64(-2ll);
    CMP64(+2ll);
    CMP64(INT64_MAX - 1);
    CMP64(INT64_MIN + 1);
    CMP64(INT64_MAX);
    CMP64(INT64_MIN);
}
void TESTU( uint64_t N ) {
    uint8_t buf[9] = {0};
    enczig64i(buf, (N));
    uint64_t reconstructed = deczig64i(buf, 0);
    if( reconstructed != (N) ) printf("[FAIL] %llu vs %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", (N), buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8] );
    else if( 0xffffff == ((N) & 0xffffff) ) printf("[ OK ] %llx\n", (N));
}
void TESTI( int64_t N ) {
    TESTU( enczig64u(N) );
}
AUTORUN {
    TESTU(0LLU);
    TESTU(1LLU);
    TESTU(2LLU);
    TESTU(UINT64_MAX/8);
    TESTU(UINT64_MAX/4);
    TESTU(UINT64_MAX/2);
    TESTU(UINT64_MAX-2);
    TESTU(UINT64_MAX-1);
    TESTU(UINT64_MAX);

   #pragma omp parallel for  // compile with /openmp
   for( int64_t N = INT64_MIN; N < INT64_MAX; ++N ) {
        TESTU(N);
        TESTI((int64_t)N);
   }
}
#endif

