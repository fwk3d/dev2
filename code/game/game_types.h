#if !CODE

typedef union vec2i{ struct { int X,Y; };       struct { int x,y; }; struct { int r,g; }; struct { int w,h; }; struct { int min,max; }; struct { int from,to; }; struct { int src,dst; }; int v2[2]; int array[1]; } vec2i;
typedef union vec3i{ struct { int X,Y,Z; };     struct { int x,y,z; }; struct { int r,g,b; }; struct { int w,h,d; }; struct { int min,max; }; struct { int from,to,step; }; struct { int src,dst; }; int v3[3]; int array[1]; } vec3i;
typedef union vec2 { struct { float X,Y; };     struct { float x,y; }; struct { float r,g; }; struct { float w,h; }; struct { float min,max; }; struct { float from,to; }; struct { float src,dst; }; float v2[2]; float array[1]; } vec2;
typedef union vec3 { struct { float X,Y,Z; };   struct { float x,y,z; }; struct { float r,g,b; }; struct { float min,max; }; struct { float from,to; }; vec2 xy; vec2 rg; vec2 wh; float v3[3]; float array[1]; } vec3;
typedef union vec4 { struct { float X,Y,Z,W; }; struct { float x,y,z,w; }; struct { float r,g,b,a; }; struct { float min,max; }; struct { float from,to; }; vec2 xy; vec3 xyz; vec2 rg; vec3 rgb; vec2 wh; vec3 whd; float v4[4]; float array[1]; } vec4;
typedef union quat { struct { float X,Y,Z,W; }; struct { float x,y,z,w; }; vec3 xyz; vec4 xyzw; float v4[4]; float array[1]; } quat;
typedef float mat33[9];
typedef float mat34[12];
typedef float mat44[16];

#define vec2i(x, y     )  CAST(vec2i,(int)(x),   (int)(y)                          )
#define vec3i(x, y, z  )  CAST(vec3i,(int)(x),   (int)(y),   (int)(z)              )
#define vec2(x, y      )  CAST(vec2, (float)(x), (float)(y)                        )
#define vec3(x, y, z   )  CAST(vec3, (float)(x), (float)(y), (float)(z),           )
#define vec4(x, y, z, w)  CAST(vec4, (float)(x), (float)(y), (float)(z), (float)(w))
#define quat(x, y, z, w)  CAST(quat, (float)(x), (float)(y), (float)(z), (float)(w))
#define axis(x, y, z)     CAST(axis, (float)(x), (float)(y), (float)(z))
#define mat33(...)        CAST(mat33, __VA_ARGS__ )
#define mat34(...)        CAST(mat34, __VA_ARGS__ )
#define mat44(...)        CAST(mat44, __VA_ARGS__ )

typedef struct line     { vec3 a, b;                                                  } line;
typedef struct sphere   { vec3 c; float r;                                            } sphere;
typedef struct aabb     { vec3 min, max;                                              } aabb;
typedef struct plane    { vec3 p, n;                                                  } plane;
typedef struct capsule  { vec3 a, b; float r;                                         } capsule;
typedef struct ray      { vec3 p, d;                                                  } ray;
typedef struct triangle { vec3 p0,p1,p2;                                              } triangle;
typedef struct poly     { vec3* verts; int cnt;                                       } poly;
typedef union  frustum  { struct { vec4 l, r, t, b, n, f; }; vec4 pl[6]; float v[24]; } frustum;

#define line(...)       CAST(line, __VA_ARGS__)
#define sphere(...)     CAST(sphere, __VA_ARGS__)
#define aabb(...)       CAST(aabb, __VA_ARGS__)
#define plane(...)      CAST(plane, __VA_ARGS__)
#define capsule(...)    CAST(capsule, __VA_ARGS__)
#define ray(...)        CAST(ray, __VA_ARGS__)
#define triangle(...)   CAST(triangle, __VA_ARGS__)
#define poly(...)       CAST(poly, __VA_ARGS__)
#define frustum(...)    CAST(frustum, __VA_ARGS__)

// ----------------------------------------------------------------------------
// text conversions

API char* ftoa1(float v);
API char* ftoa2(vec2  v);
API char* ftoa3(vec3  v);
API char* ftoa4(vec4  v);

API float atof1(const char *s);
API vec2  atof2(const char *s);
API vec3  atof3(const char *s);
API vec4  atof4(const char *s);

API char* itoa1(int   v);
API char* itoa2(vec2i v);
API char* itoa3(vec3i v);

API int   atoi1(const char *s);
API vec2i atoi2(const char *s);
API vec3i atoi3(const char *s);

#else

// ----------------------------------------------------------------------------
// float conversion (text)

char* itoa1(int v) {
    return va("%d", v);
}
char* itoa2(vec2i v) {
    return va("%d,%d", v.x,v.y);
}
char* itoa3(vec3i v) {
    return va("%d,%d,%d", v.x,v.y,v.z);
}

char* ftoa1(float v) {
    return va("%f", v);
}
char* ftoa2(vec2 v) {
    return va("%f,%f", v.x, v.y);
}
char* ftoa3(vec3 v) {
    return va("%f,%f,%f", v.x, v.y, v.z);
}
char* ftoa4(vec4 v) {
    return va("%f,%f,%f,%f", v.x, v.y, v.z, v.w);
}

float atof1(const char *s) {
    char buf[64];
    return sscanf(s, "%63[^]\r\n,}]", buf) == 1 ? (float)EVAL(buf) : (float)NAN;
}
vec2 atof2(const char *s) {
    vec2 v = { 0 };
    char buf1[64],buf2[64];
    int num = sscanf(s, "%63[^]\r\n,}],%63[^]\r\n,}]", buf1, buf2);
    if( num > 0 ) v.x = EVAL(buf1);
    if( num > 1 ) v.y = EVAL(buf2);
    return v;
}
vec3 atof3(const char *s) {
    vec3 v = {0};
    char buf1[64],buf2[64],buf3[64];
    int num = sscanf(s, "%63[^]\r\n,}],%63[^]\r\n,}],%63[^]\r\n,}]", buf1, buf2, buf3);
    if( num > 0 ) v.x = EVAL(buf1);
    if( num > 1 ) v.y = EVAL(buf2);
    if( num > 2 ) v.z = EVAL(buf3);
    return v;
}
vec4 atof4(const char *s) {
    vec4 v = {0};
    char buf1[64],buf2[64],buf3[64],buf4[64];
    int num = sscanf(s, "%63[^]\r\n,}],%63[^]\r\n,}],%63[^]\r\n,}],%63[^]\r\n,}]", buf1, buf2, buf3, buf4);
    if( num > 0 ) v.x = EVAL(buf1);
    if( num > 1 ) v.y = EVAL(buf2);
    if( num > 2 ) v.z = EVAL(buf3);
    if( num > 3 ) v.w = EVAL(buf4);
    return v;
}

// @todo: expand this to proper int parsers
int atoi1(const char *s) {
    return (int)atof1(s);
}
vec2i atoi2(const char *s) {
    vec2 v = atof2(s);
    return vec2i( v.x, v.y );
}
vec3i atoi3(const char *s) {
    vec3 v = atof3(s);
    return vec3i( v.x, v.y, v.z );
}

#endif
