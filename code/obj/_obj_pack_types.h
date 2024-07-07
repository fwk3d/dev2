
// -----------------------------------------------------------------------------
// semantic versioning in a single byte (octal)

API int semver( int major, int minor, int patch );
API int semvercmp( int v1, int v2 );

#define SEMVER(major,minor,patch) (0100 * (major) + 010 * (minor) + (patch))
#define SEMVERCMP(v1,v2) (((v1) & 0110) - ((v2) & 0110))
#define SEMVERFMT "%03o"

// -----------------------------------------------------------------------------
// storage types. refer to vec2i/3i, vec2/3/4 if you plan to do math operations

typedef struct byte2 { uint8_t x,y; } byte2;
typedef struct byte3 { uint8_t x,y,z; } byte3;
typedef struct byte4 { uint8_t x,y,z,w; } byte4;

typedef struct int2 { int x,y; } int2;
typedef struct int3 { int x,y,z; } int3;
typedef struct int4 { int x,y,z,w; } int4;

typedef struct uint2 { unsigned int x,y; } uint2;
typedef struct uint3 { unsigned int x,y,z; } uint3;
typedef struct uint4 { unsigned int x,y,z,w; } uint4;

typedef struct float2 { float x,y; } float2;
typedef struct float3 { float x,y,z; } float3;
typedef struct float4 { float x,y,z,w; } float4;

typedef struct double2 { double x,y; } double2;
typedef struct double3 { double x,y,z; } double3;
typedef struct double4 { double x,y,z,w; } double4;

#define byte2(x,y)       CAST(byte2, (uint8_t)(x), (uint8_t)(y) )
#define byte3(x,y,z)     CAST(byte3, (uint8_t)(x), (uint8_t)(y), (uint8_t)(z) )
#define byte4(x,y,z,w)   CAST(byte4, (uint8_t)(x), (uint8_t)(y), (uint8_t)(z), (uint8_t)(w) )

#define int2(x,y)        CAST(int2, (int)(x), (int)(y) )
#define int3(x,y,z)      CAST(int3, (int)(x), (int)(y), (int)(z) )
#define int4(x,y,z,w)    CAST(int4, (int)(x), (int)(y), (int)(z), (int)(w) )

#define uint2(x,y)       CAST(uint2, (unsigned)(x), (unsigned)(y) )
#define uint3(x,y,z)     CAST(uint3, (unsigned)(x), (unsigned)(y), (unsigned)(z) )
#define uint4(x,y,z,w)   CAST(uint4, (unsigned)(x), (unsigned)(y), (unsigned)(z), (unsigned)(w) )

#define float2(x,y)      CAST(float2, (float)(x), (float)(y) )
#define float3(x,y,z)    CAST(float3, (float)(x), (float)(y), (float)(z) )
#define float4(x,y,z,w)  CAST(float4, (float)(x), (float)(y), (float)(z), (float)(w) )

#define double2(x,y)     CAST(double2, (double)(x), (double)(y) )
#define double3(x,y,z)   CAST(double3, (double)(x), (double)(y), (double)(z) )
#define double4(x,y,z,w) CAST(double4, (double)(x), (double)(y), (double)(z), (double)(w) )

// -----------------------------------------------------------------------------
// semantic versioning in a single byte (octal)
// - rlyeh, public domain.
//
// - single octal byte that represents semantic versioning (major.minor.patch).
// - allowed range [0000..0377] ( <-> [0..255] decimal )
// - comparison checks only major.minor tuple as per convention.

int semver( int major, int minor, int patch ) {
    return SEMVER(major, minor, patch);
}
int semvercmp( int v1, int v2 ) {
    return SEMVERCMP(v1, v2);
}

#if 0
AUTORUN {
    for( int i= 0; i <= 255; ++i) printf(SEMVERFMT ",", i);
    puts("");

    printf(SEMVERFMT "\n", semver(3,7,7));
    printf(SEMVERFMT "\n", semver(2,7,7));
    printf(SEMVERFMT "\n", semver(1,7,7));
    printf(SEMVERFMT "\n", semver(0,7,7));

    printf(SEMVERFMT "\n", semver(3,7,1));
    printf(SEMVERFMT "\n", semver(2,5,3));
    printf(SEMVERFMT "\n", semver(1,3,5));
    printf(SEMVERFMT "\n", semver(0,1,7));

    assert( semvercmp( 0357, 0300 )  > 0 );
    assert( semvercmp( 0277, 0300 )  < 0 );
    assert( semvercmp( 0277, 0200 )  > 0 );
    assert( semvercmp( 0277, 0100 )  < 0 );
    assert( semvercmp( 0076, 0070 ) == 0 );
    assert( semvercmp( 0076, 0077 ) == 0 );
    assert( semvercmp( 0176, 0170 ) == 0 );
    assert( semvercmp( 0176, 0177 ) == 0 );
    assert( semvercmp( 0276, 0270 ) == 0 );
    assert( semvercmp( 0276, 0277 ) == 0 );
    assert( semvercmp( 0376, 0370 ) == 0 );
    assert( semvercmp( 0376, 0377 ) == 0 );
}
#endif
