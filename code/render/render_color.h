// -----------------------------------------------------------------------------
// colors

#if !CODE

API unsigned rgba( uint8_t r, uint8_t g, uint8_t b, uint8_t a );
API unsigned bgra( uint8_t b, uint8_t g, uint8_t r, uint8_t a );
API unsigned rgbaf( float r, float g, float b, float a );
API unsigned bgraf( float b, float g, float r, float a );
API unsigned alpha( unsigned rgba );

#define RGBX(rgb,x)   ( ((rgb)&0xFFFFFF) | (((unsigned)(x))<<24) )
#define RGB3(r,g,b)   ( (255<<24) | ((b)<<16) | ((g)<<8) | (r) )
#define RGB4(r,g,b,a) ( ((a)<<24) | ((b)<<16) | ((g)<<8) | (r) )

#define BLACK   RGBX(0x000000,255)
#define WHITE   RGBX(0xE8F1FF,255)

#define RED     RGB3(   255, 0,48 )
#define GREEN   RGB3(  144,255,48 )
#define CYAN    RGB3(   0,192,255 )
#define ORANGE  RGB3(  255,144,48 )
#define PURPLE  RGB3(  102,77,102 ) // 178,128,255 )
#define YELLOW  RGB3(   255,224,0 )
#define GRAY    RGB3(    32,32,32 ) // dark gray
#define SILVER  RGB3( 149,149,149 ) // dark white, gray-ish
#define PINK    RGB3(  255,48,144 )
#define AQUA    RGB3(  48,255,144 )

#define BLUE    RGBX(0xB55A06,255)

API unsigned atorgba(const char *s);
API char *   rgbatoa(unsigned rgba);

#else

// -----------------------------------------------------------------------------
// colors

unsigned rgba( uint8_t r, uint8_t g, uint8_t b, uint8_t a ) {
    return (unsigned)a << 24 | b << 16 | g << 8 | r;
}
unsigned bgra( uint8_t b, uint8_t g, uint8_t r, uint8_t a ) {
    return rgba(r,g,b,a);
}
unsigned alpha( unsigned rgba ) {
    return rgba >> 24;
}

unsigned rgbaf(float r, float g, float b, float a) {
    return rgba(r * 255, g * 255, b * 255, a * 255);
}
unsigned bgraf(float b, float g, float r, float a) {
    return rgba(r * 255, g * 255, b * 255, a * 255);
}

unsigned atorgba(const char *s) {
    if( s[0] != '#' ) return 0;
    unsigned r = 0, g = 0, b = 0, a = 255;
    int slen = strspn(s+1, "0123456789abcdefABCDEF");
    if( slen > 8 ) slen = 8;
    /**/ if( slen == 6 ) sscanf(s+1, "%2x%2x%2x",    &r,&g,&b);
    else if( slen == 8 ) sscanf(s+1, "%2x%2x%2x%2x", &r,&g,&b,&a);
    else if( slen == 3 ) sscanf(s+1, "%1x%1x%1x",    &r,&g,&b   ), r=r<<4|r,g=g<<4|g,b=b<<4|b;
    else if( slen == 4 ) sscanf(s+1, "%1x%1x%1x%1x", &r,&g,&b,&a), r=r<<4|r,g=g<<4|g,b=b<<4|b,a=a<<4|a;
    return rgba(r,g,b,a);
}
char *rgbatoa(unsigned rgba) {
    unsigned a = rgba >> 24;
    unsigned b =(rgba >> 16) & 255;
    unsigned g =(rgba >>  8) & 255;
    unsigned r = rgba        & 255;
    char *s = va("#        ");
    sprintf(s+1, "%02x%02x%02x%02x", r,g,b,a);
    return s;
}

#endif
