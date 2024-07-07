// ----------------------------------------------------------------------------
// ease

#if !CODE

enum EASE_FLAGS {
    EASE_SINE,
    EASE_QUAD,
    EASE_CUBIC,
    EASE_QUART,
    EASE_QUINT,
    EASE_EXPO,
    EASE_CIRC,
    EASE_BACK,
    EASE_ELASTIC,
    EASE_BOUNCE,

    EASE_IN,
    EASE_OUT = 0,
    EASE_INOUT = EASE_IN * 2,

    EASE_ZERO = EASE_INOUT | (EASE_BOUNCE + 1),
    EASE_ONE,
    EASE_LINEAR,
    EASE_INOUT_PERLIN,

    EASE_NUM
};

API float ease(float t01, unsigned fn); // / 0-to-1
API float ease_pong(float t01, unsigned fn); // \ 1-to-0
API float ease_ping_pong(float t, unsigned fn1, unsigned fn2); // /\ 0-to-1-to-0
API float ease_pong_ping(float t, unsigned fn1, unsigned fn2); // \/ 1-to-0-to-1

API const char *ease_enum(unsigned fn);
API const char**ease_enums();

API float ease_zero(float t);
API float ease_one(float t);
API float ease_linear(float t);

API float ease_out_sine(float t);
API float ease_out_quad(float t);
API float ease_out_cubic(float t);
API float ease_out_quart(float t);
API float ease_out_quint(float t);
API float ease_out_expo(float t);
API float ease_out_circ(float t);
API float ease_out_back(float t);
API float ease_out_elastic(float t);
API float ease_out_bounce(float t);

API float ease_in_sine(float t);
API float ease_in_quad(float t);
API float ease_in_cubic(float t);
API float ease_in_quart(float t);
API float ease_in_quint(float t);
API float ease_in_expo(float t);
API float ease_in_circ(float t);
API float ease_in_back(float t);
API float ease_in_elastic(float t);
API float ease_in_bounce(float t);

API float ease_inout_sine(float t);
API float ease_inout_quad(float t);
API float ease_inout_cubic(float t);
API float ease_inout_quart(float t);
API float ease_inout_quint(float t);
API float ease_inout_expo(float t);
API float ease_inout_circ(float t);
API float ease_inout_back(float t);
API float ease_inout_elastic(float t);
API float ease_inout_bounce(float t);

API float ease_inout_perlin(float t);

#else

// ----------------------------------------------------------------------------
// ease

float ease_zero(float t) { return 0; }
float ease_one(float t) { return 1; }
float ease_linear(float t) { return t; }

float ease_out_sine(float t) { return sinf(t*(C_PI*0.5f)); }
float ease_out_quad(float t) { return -(t*(t-2)); }
float ease_out_cubic(float t) { float f=t-1; return f*f*f+1; }
float ease_out_quart(float t) { float f=t-1; return f*f*f*(1-t)+1; }
float ease_out_quint(float t) { float f=(t-1); return f*f*f*f*f+1; }
float ease_out_expo(float t) { return (t >= 1) ? t : 1-powf(2,-10*t); }
float ease_out_circ(float t) { return sqrtf((2-t)*t); }
float ease_out_back(float t) { float f=1-t; return 1-(f*f*f-f*sinf(f*C_PI)); }
float ease_out_elastic(float t) { return sinf(-13*(C_PI*0.5f)*(t+1))*powf(2,-10*t)+1; }
float ease_out_bounce(float t) { return (t < 4.f/11) ? (121.f*t*t)/16 : (t < 8.f/11) ? (363.f/40*t*t)-(99.f/10*t)+17.f/5 : (t < 9.f/10) ? (4356.f/361*t*t)-(35442.f/1805*t)+16061.f/1805 : (54.f/5*t*t)-(513.f/25*t)+268.f/25; }

float ease_in_sine(float t) { return 1+sinf((t-1)*(C_PI*0.5f)); }
float ease_in_quad(float t) { return t*t; }
float ease_in_cubic(float t) { return t*t*t; }
float ease_in_quart(float t) { return t*t*t*t; }
float ease_in_quint(float t) { return t*t*t*t*t; }
float ease_in_expo(float t) { return (t <= 0) ? t : powf(2,10*(t-1)); }
float ease_in_circ(float t) { return 1-sqrtf(1-(t*t)); }
float ease_in_back(float t) { return t*t*t-t*sinf(t*C_PI); }
float ease_in_elastic(float t) { return sinf(13*(C_PI*0.5f)*t)*powf(2,10*(t-1)); }
float ease_in_bounce(float t) { return 1-ease_out_bounce(1-t); }

float ease_inout_sine(float t) { return 0.5f*(1-cosf(t*C_PI)); }
float ease_inout_quad(float t) { return (t < 0.5f) ? 2*t*t : (-2*t*t)+(4*t)-1; }
float ease_inout_cubic(float t) { float f; return (t < 0.5f) ? 4*t*t*t : (f=(2*t)-2,0.5f*f*f*f+1); }
float ease_inout_quart(float t) { float f; return (t < 0.5f) ? 8*t*t*t*t : (f=(t-1),-8*f*f*f*f+1); }
float ease_inout_quint(float t) { float f; return (t < 0.5f) ? 16*t*t*t*t*t : (f=((2*t)-2),0.5f*f*f*f*f*f+1); }
float ease_inout_expo(float t) { return (t <= 0 || t >= 1) ? t : t < 0.5f ? 0.5f*powf(2,(20*t)-10) : -0.5f*powf(2,(-20*t)+10)+1; }
float ease_inout_circ(float t) { return t < 0.5f ? 0.5f*(1-sqrtf(1-4*(t*t))) : 0.5f*(sqrtf(-((2*t)-3)*((2*t)-1))+1); }
float ease_inout_back(float t) { float f; return t < 0.5f ? (f=2*t,0.5f*(f*f*f-f*sinf(f*C_PI))) : (f=(1-(2*t-1)),0.5f*(1-(f*f*f-f*sinf(f*C_PI)))+0.5f); }
float ease_inout_elastic(float t) { return t < 0.5f ? 0.5f*sinf(13*(C_PI*0.5f)*(2*t))*powf(2,10*((2*t)-1)) : 0.5f*(sinf(-13*(C_PI*0.5f)*((2*t-1)+1))*powf(2,-10*(2*t-1))+2); }
float ease_inout_bounce(float t) { return t < 0.5f ? 0.5f*ease_in_bounce(t*2) : 0.5f*ease_out_bounce(t*2-1)+0.5f; }

float ease_inout_perlin(float t) { float t3=t*t*t,t4=t3*t,t5=t4*t; return 6*t5-15*t4+10*t3; }

float ease(float t01, unsigned mode) {
    typedef float (*easing)(float);
    easing modes[] = {
        ease_out_sine,
        ease_out_quad,
        ease_out_cubic,
        ease_out_quart,
        ease_out_quint,
        ease_out_expo,
        ease_out_circ,
        ease_out_back,
        ease_out_elastic,
        ease_out_bounce,

        ease_in_sine,
        ease_in_quad,
        ease_in_cubic,
        ease_in_quart,
        ease_in_quint,
        ease_in_expo,
        ease_in_circ,
        ease_in_back,
        ease_in_elastic,
        ease_in_bounce,

        ease_inout_sine,
        ease_inout_quad,
        ease_inout_cubic,
        ease_inout_quart,
        ease_inout_quint,
        ease_inout_expo,
        ease_inout_circ,
        ease_inout_back,
        ease_inout_elastic,
        ease_inout_bounce,

        ease_zero,
        ease_one,
        ease_linear,
        ease_inout_perlin,
    };
    return modes[clampi(mode, 0, COUNTOF(modes))](clampf(t01,0,1));
}

float ease_pong(float t, unsigned fn) { return 1 - ease(t, fn); }
float ease_ping_pong(float t, unsigned fn1, unsigned fn2) { return t < 0.5 ? ease(t*2,fn1) : ease(1-(t-0.5)*2,fn2); }
float ease_pong_ping(float t, unsigned fn1, unsigned fn2) { return 1 - ease_ping_pong(t,fn1,fn2); }


const char **ease_enums() {
    static const char *list[] = {
        "ease_out_sine",
        "ease_out_quad",
        "ease_out_cubic",
        "ease_out_quart",
        "ease_out_quint",
        "ease_out_expo",
        "ease_out_circ",
        "ease_out_back",
        "ease_out_elastic",
        "ease_out_bounce",

        "ease_in_sine",
        "ease_in_quad",
        "ease_in_cubic",
        "ease_in_quart",
        "ease_in_quint",
        "ease_in_expo",
        "ease_in_circ",
        "ease_in_back",
        "ease_in_elastic",
        "ease_in_bounce",

        "ease_inout_sine",
        "ease_inout_quad",
        "ease_inout_cubic",
        "ease_inout_quart",
        "ease_inout_quint",
        "ease_inout_expo",
        "ease_inout_circ",
        "ease_inout_back",
        "ease_inout_elastic",
        "ease_inout_bounce",

        "ease_zero",
        "ease_one",
        "ease_linear",
        "ease_inout_perlin",

        0
    };
    return list;
}

const char *ease_enum(unsigned mode) {
    return mode[ ease_enums() ];
}

/*AUTORUN {
    ENUM(EASE_LINEAR|EASE_OUT);
    ENUM(EASE_SINE|EASE_OUT);
    ENUM(EASE_QUAD|EASE_OUT);
    ENUM(EASE_CUBIC|EASE_OUT);
    ENUM(EASE_QUART|EASE_OUT);
    ENUM(EASE_QUINT|EASE_OUT);
    ENUM(EASE_EXPO|EASE_OUT);
    ENUM(EASE_CIRC|EASE_OUT);
    ENUM(EASE_BACK|EASE_OUT);
    ENUM(EASE_ELASTIC|EASE_OUT);
    ENUM(EASE_BOUNCE|EASE_OUT);

    ENUM(EASE_SINE|EASE_IN);
    ENUM(EASE_QUAD|EASE_IN);
    ENUM(EASE_CUBIC|EASE_IN);
    ENUM(EASE_QUART|EASE_IN);
    ENUM(EASE_QUINT|EASE_IN);
    ENUM(EASE_EXPO|EASE_IN);
    ENUM(EASE_CIRC|EASE_IN);
    ENUM(EASE_BACK|EASE_IN);
    ENUM(EASE_ELASTIC|EASE_IN);
    ENUM(EASE_BOUNCE|EASE_IN);

    ENUM(EASE_SINE|EASE_INOUT);
    ENUM(EASE_QUAD|EASE_INOUT);
    ENUM(EASE_CUBIC|EASE_INOUT);
    ENUM(EASE_QUART|EASE_INOUT);
    ENUM(EASE_QUINT|EASE_INOUT);
    ENUM(EASE_EXPO|EASE_INOUT);
    ENUM(EASE_CIRC|EASE_INOUT);
    ENUM(EASE_BACK|EASE_INOUT);
    ENUM(EASE_ELASTIC|EASE_INOUT);
    ENUM(EASE_BOUNCE|EASE_INOUT);

    ENUM(EASE_ZERO);
    ENUM(EASE_ONE);
    ENUM(EASE_LINEAR);
    ENUM(EASE_INOUT_PERLIN);
};*/

#endif
