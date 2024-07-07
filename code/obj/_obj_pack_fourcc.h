
// -----------------------------------------------------------------------------
// compile-time fourcc, eightcc

API char *cc4str(unsigned cc);
API char *cc8str(uint64_t cc);

enum {
#   define _(a,b,c,d,e) cc__##a, cc__##b, cc__##c, cc__##d, cc__##e
    cc__1 = '1', _(2,3,4,5,6),_(7,8,9,0,_), cc__ = ' ',
    cc__A = 'A', _(B,C,D,E,F),_(G,H,I,J,K),_(L,M,N,O,P),_(Q,R,S,T,U),_(V,W,X,Y,Z),
    cc__a = 'a', _(b,c,d,e,f),_(g,h,i,j,k),_(l,m,n,o,p),_(q,r,s,t,u),_(v,w,x,y,z),
#   undef _
};

#ifdef BIG
#define cc4(a,b,c,d) ((uint32_t)(cc__##a<<24) | (cc__##b<<16) | (cc__##c<<8) | (cc__##d<<0))
#define cc8(a,b,c,d,e,f,g,h) (((uint64_t)cc4(a,b,c,d) << 32ULL) | cc4(e,f,g,h))
#else
#define cc4(a,b,c,d) ((uint32_t)(cc__##d<<24) | (cc__##c<<16) | (cc__##b<<8) | (cc__##a<<0))
#define cc8(a,b,c,d,e,f,g,h) (((uint64_t)cc4(e,f,g,h) << 32ULL) | cc4(a,b,c,d))
#endif

#define cc3(a,b,c) cc4(,a,b,c)
#define cc5(a,b,c,d,e) cc6(,a,b,c,d,e)
#define cc6(a,b,c,d,e,f) cc7(,a,b,c,d,e,f)
#define cc7(a,b,c,d,e,f,g) cc8(,a,b,c,d,e,f,g)




// -----------------------------------------------------------------------------
// compile-time fourcc, eightcc

char *cc4str(unsigned x) {
    static __thread char type[4+1] = {0};
    type[3] = (x >> 24ULL) & 255;
    type[2] = (x >> 16ULL) & 255;
    type[1] = (x >>  8ULL) & 255;
    type[0] = (x >>  0ULL) & 255;
    return type;
}
char *cc8str(uint64_t x) {
    static __thread char type[8+1] = {0};
    type[7] = (x >> 56ULL) & 255;
    type[6] = (x >> 48ULL) & 255;
    type[5] = (x >> 40ULL) & 255;
    type[4] = (x >> 32ULL) & 255;
    type[3] = (x >> 24ULL) & 255;
    type[2] = (x >> 16ULL) & 255;
    type[1] = (x >>  8ULL) & 255;
    type[0] = (x >>  0ULL) & 255;
    return type;
}


