
// ----------------------------------------------------------------------------
// array de/interleaving
//
// results:
// R0G0B0   R1G1B1   R2G2B2...   -> R0R1R2... B0B1B2... G0G1G2...
// R0G0B0A0 R1G1B1A1 R2G2B2A2... -> R0R1R2... A0A1A2... B0B1B2... G0G1G2...

API void *interleave( void *out, const void *list, int list_count, int sizeof_item, unsigned columns );




// ----------------------------------------------------------------------------
// arc4 en/decryptor

API void *arc4( void *buffer, unsigned buflen, const void *pass, unsigned passlen );

// ----------------------------------------------------------------------------
// crc64

API uint64_t crc64(uint64_t h, const void *ptr, uint64_t len);

// ----------------------------------------------------------------------------
// entropy encoder

API void entropy( void *buf, unsigned n );











// ----------------------------------------------------------------------------
// ARC4 en/decryptor. Based on code by Mike Shaffer.
// - rlyeh, public domain.

void *arc4( void *buf_, unsigned buflen, const void *pass_, unsigned passlen ) {
    // [ref] http://www.4guysfromrolla.com/webtech/code/rc4.inc.html
    assert(passlen);
    int sbox[256], key[256];
    char *buf = (char*)buf_;
    const char *pass = (const char*)pass_;
    for( unsigned a = 0; a < 256; a++ ) {
        key[a] = pass[a % passlen];
        sbox[a] = a;
    }
    for( unsigned a = 0, b = 0; a < 256; a++ ) {
        b = (b + sbox[a] + key[a]) % 256;
        int swap = sbox[a]; sbox[a] = sbox[b]; sbox[b] = swap;
    }
    for( unsigned a = 0, b = 0, i = 0; i < buflen; ++i ) {
        a = (a + 1) % 256;
        b = (b + sbox[a]) % 256;
        int swap = sbox[a]; sbox[a] = sbox[b]; sbox[b] = swap;
        buf[i] ^= sbox[(sbox[a] + sbox[b]) % 256];
    }
    return buf_;
}

#if 0
AUTORUN {
    char buffer[] = "Hello world."; int buflen = strlen(buffer);
    char *password = "abc123"; int passlen = strlen(password);

    printf("Original: %s\n", buffer);
    printf("Password: %s\n", password);

    char *encrypted = arc4( buffer, buflen, password, passlen );
    printf("ARC4 Encrypted text: '%s'\n", encrypted);

    char *decrypted = arc4( buffer, buflen, password, passlen );
    printf("ARC4 Decrypted text: '%s'\n", decrypted);
}
#endif


// ----------------------------------------------------------------------------
// entropy encoder

#if is(win32)
#include <winsock2.h>
#include <wincrypt.h>
#pragma comment(lib, "advapi32")

void entropy( void *buf, unsigned n ) {
    HCRYPTPROV provider;
    if( CryptAcquireContext( &provider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) == 0 ) {
        assert(!"CryptAcquireContext failed");
    }

    int rc = CryptGenRandom( provider, n, (BYTE *)buf );
    assert( rc != 0 );
    CryptReleaseContext( provider, 0 );
}

#elif is(linux) || is(osx)

void entropy( void *buf, unsigned n ) {
    FILE *fp = fopen( "/dev/urandom", "r" );
    if( !fp ) assert(!"/dev/urandom open failed");

    size_t read = n * fread( buf, n, 1, fp );
    assert( read == n && "/dev/urandom read failed" );
    fclose( fp );
}

#else // unused for now. likely emscripten will hit this

// pseudo random number generator with 128 bit internal state... probably not suited for cryptographical usage.
// [src] http://github.com/kokke (UNLICENSE)
// [ref] http://burtleburtle.net/bob/rand/smallprng.html

#include <time.h>

#if is(win32)
#include <process.h>
#else
#include <unistd.h>
#endif

static uint32_t prng_next(void) {
    #define prng_rotate(x,k) (x << k) | (x >> (32 - k))
    #define prng_shuffle() do { \
    uint32_t e = ctx[0] - prng_rotate(ctx[1], 27); \
    ctx[0] = ctx[1] ^ prng_rotate(ctx[2], 17); \
    ctx[1] = ctx[2] + ctx[3]; \
    ctx[2] = ctx[3] + e; \
    ctx[3] = e + ctx[0]; } while(0)
    static __thread uint32_t ctx[4], *once = 0; if( !once ) {
        uint32_t seed = (uint32_t)( ifdef(win32,_getpid,getpid)() + time(0) + ((uintptr_t)once) );
        ctx[0] = 0xf1ea5eed;
        ctx[1] = ctx[2] = ctx[3] = seed;
        for (int i = 0; i < 31; ++i) {
            prng_shuffle();
        }
        once = ctx;
    }
    prng_shuffle();
    return ctx[3];
}

void entropy( void *buf, unsigned n ) {
    for( ; n >= 4 ; n -= 4 ) {
        uint32_t a = prng_next();
        memcpy(buf, &a, 4);
        buf = ((char*)buf) + 4;
    }
    if( n > 0 ) {
        uint32_t a = prng_next();
        memcpy(buf, &a, n);
    }
}

#endif

#if 0
AUTORUN {
    unsigned char buf[128];
    entropy(buf, 128);
    for( int i = 0; i < 128; ++i ) {
        printf("%02x", buf[i]);
    }
    puts("");
}
#endif
