# pragma once
//# ifndef IMGUI_DEFINE_MATH_OPERATORS
//#     define IMGUI_DEFINE_MATH_OPERATORS
//# endif
//# include <imgui.h>
//# include <imgui_internal.h>

/*
MIT License

Copyright (c) 2024 Gabriele Torelli

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

//------------------------------------------------------------------------------
// VERSION 0.9.1
//
// LICENSE
//   This software is dual-licensed to the public domain and under the following
//   license: you are granted a perpetual, irrevocable license to copy, modify,
//   publish, and distribute this file as you see fit.
//
// CREDITS
//   Written by Michal Cichon
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
struct ImLine
{
    ImVec2 A, B;
};


//------------------------------------------------------------------------------
# if IMGUI_VERSION_NUM < 19002
inline bool operator==(const ImVec2& lhs, const ImVec2& rhs);
inline bool operator!=(const ImVec2& lhs, const ImVec2& rhs);
# endif
inline ImVec2 operator*(const float lhs, const ImVec2& rhs);
# if IMGUI_VERSION_NUM < 18955
inline ImVec2 operator-(const ImVec2& lhs);
# endif


//------------------------------------------------------------------------------
inline float  ImLength(float v);
inline float  ImLength(const ImVec2& v);
inline float  ImLengthSqr(float v);
inline ImVec2 ImNormalized(const ImVec2& v);


//------------------------------------------------------------------------------
inline bool   ImRect_IsEmpty(const ImRect& rect);
inline ImVec2 ImRect_ClosestPoint(const ImRect& rect, const ImVec2& p, bool snap_to_edge);
inline ImVec2 ImRect_ClosestPoint(const ImRect& rect, const ImVec2& p, bool snap_to_edge, float radius);
inline ImVec2 ImRect_ClosestPoint(const ImRect& rect, const ImRect& b);
inline ImLine ImRect_ClosestLine(const ImRect& rect_a, const ImRect& rect_b);
inline ImLine ImRect_ClosestLine(const ImRect& rect_a, const ImRect& rect_b, float radius_a, float radius_b);



//------------------------------------------------------------------------------
namespace ImEasing {

template <typename V, typename T>
inline V EaseOutQuad(V b, V c, T t)
{
    return b - c * (t * (t - 2));
}

} // namespace ImEasing

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// VERSION 0.1
//
// LICENSE
//   This software is dual-licensed to the public domain and under the following
//   license: you are granted a perpetual, irrevocable license to copy, modify,
//   publish, and distribute this file as you see fit.
//
// CREDITS
//   Written by Michal Cichon
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
template <typename T>
struct ImCubicBezierPointsT
{
    T P0;
    T P1;
    T P2;
    T P3;
};
using ImCubicBezierPoints = ImCubicBezierPointsT<ImVec2>;


//------------------------------------------------------------------------------
// Low-level Bezier curve sampling.
template <typename T> inline T ImLinearBezier(const T& p0, const T& p1, float t);
template <typename T> inline T ImLinearBezierDt(const T& p0, const T& p1, float t);
template <typename T> inline T ImQuadraticBezier(const T& p0, const T& p1, const T& p2, float t);
template <typename T> inline T ImQuadraticBezierDt(const T& p0, const T& p1, const T& p2, float t);
template <typename T> inline T ImCubicBezier(const T& p0, const T& p1, const T& p2, const T& p3, float t);
template <typename T> inline T ImCubicBezierDt(const T& p0, const T& p1, const T& p2, const T& p3, float t);


// High-level Bezier sampling, automatically collapse to lower level Bezier curves if control points overlap.
template <typename T> inline T ImCubicBezierSample(const T& p0, const T& p1, const T& p2, const T& p3, float t);
template <typename T> inline T ImCubicBezierSample(const ImCubicBezierPointsT<T>& curve, float t);
template <typename T> inline T ImCubicBezierTangent(const T& p0, const T& p1, const T& p2, const T& p3, float t);
template <typename T> inline T ImCubicBezierTangent(const ImCubicBezierPointsT<T>& curve, float t);


// Calculate approximate length of Cubic Bezier curve.
template <typename T> inline float ImCubicBezierLength(const T& p0, const T& p1, const T& p2, const T& p3);
template <typename T> inline float ImCubicBezierLength(const ImCubicBezierPointsT<T>& curve);


// Splits Cubic Bezier curve into two curves.
template <typename T>
struct ImCubicBezierSplitResultT
{
    ImCubicBezierPointsT<T> Left;
    ImCubicBezierPointsT<T> Right;
};
using ImCubicBezierSplitResult = ImCubicBezierSplitResultT<ImVec2>;

template <typename T> inline ImCubicBezierSplitResultT<T> ImCubicBezierSplit(const T& p0, const T& p1, const T& p2, const T& p3, float t);
template <typename T> inline ImCubicBezierSplitResultT<T> ImCubicBezierSplit(const ImCubicBezierPointsT<T>& curve, float t);


// Returns bounding rectangle of Cubic Bezier curve.
inline ImRect ImCubicBezierBoundingRect(const ImVec2& p0, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3);
inline ImRect ImCubicBezierBoundingRect(const ImCubicBezierPoints& curve);


// Project point on Cubic Bezier curve.
struct ImProjectResult
{
    ImVec2 Point;    // Point on curve
    float  Time;     // [0 - 1]
    float  Distance; // Distance to curve
};

inline ImProjectResult ImProjectOnCubicBezier(const ImVec2& p, const ImVec2& p0, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const int subdivisions = 100);
inline ImProjectResult ImProjectOnCubicBezier(const ImVec2& p, const ImCubicBezierPoints& curve, const int subdivisions = 100);


// Calculate intersection between line and a Cubic Bezier curve.
struct ImCubicBezierIntersectResult
{
    int Count;
    ImVec2 Points[3];
};

inline ImCubicBezierIntersectResult ImCubicBezierLineIntersect(const ImVec2& p0, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& a0, const ImVec2& a1);
inline ImCubicBezierIntersectResult ImCubicBezierLineIntersect(const ImCubicBezierPoints& curve, const ImLine& line);


// Adaptive Cubic Bezier subdivision.
enum ImCubicBezierSubdivideFlags
{
    ImCubicBezierSubdivide_None        = 0,
    ImCubicBezierSubdivide_SkipFirst   = 1
};

struct ImCubicBezierSubdivideSample
{
    ImVec2 Point;
    ImVec2 Tangent;
};

using ImCubicBezierSubdivideCallback = void (*)(const ImCubicBezierSubdivideSample& p, void* user_pointer);

inline void ImCubicBezierSubdivide(ImCubicBezierSubdivideCallback callback, void* user_pointer, const ImVec2& p0, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, float tess_tol = -1.0f, ImCubicBezierSubdivideFlags flags = ImCubicBezierSubdivide_None);
inline void ImCubicBezierSubdivide(ImCubicBezierSubdivideCallback callback, void* user_pointer, const ImCubicBezierPoints& curve, float tess_tol = -1.0f, ImCubicBezierSubdivideFlags flags = ImCubicBezierSubdivide_None);


// F has signature void(const ImCubicBezierSubdivideSample& p)
template <typename F> inline void ImCubicBezierSubdivide(F& callback, const ImVec2& p0, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, float tess_tol = -1.0f, ImCubicBezierSubdivideFlags flags = ImCubicBezierSubdivide_None);
template <typename F> inline void ImCubicBezierSubdivide(F& callback, const ImCubicBezierPoints& curve, float tess_tol = -1.0f, ImCubicBezierSubdivideFlags flags = ImCubicBezierSubdivide_None);

// Fixed step Cubic Bezier subdivision.
struct ImCubicBezierFixedStepSample
{
    float  T;
    float  Length;
    ImVec2 Point;
    bool   BreakSearch;
};

using ImCubicBezierFixedStepCallback = void (*)(ImCubicBezierFixedStepSample& sample, void* user_pointer);

inline void ImCubicBezierFixedStep(ImCubicBezierFixedStepCallback callback, void* user_pointer, const ImVec2& p0, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, float step, bool overshoot = false, float max_value_error = 1e-3f, float max_t_error = 1e-5f);
inline void ImCubicBezierFixedStep(ImCubicBezierFixedStepCallback callback, void* user_pointer, const ImCubicBezierPoints& curve, float step, bool overshoot = false, float max_value_error = 1e-3f, float max_t_error = 1e-5f);


// F has signature void(const ImCubicBezierFixedStepSample& p)
template <typename F> inline void ImCubicBezierFixedStep(F& callback, const ImVec2& p0, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, float step, bool overshoot = false, float max_value_error = 1e-3f, float max_t_error = 1e-5f);
template <typename F> inline void ImCubicBezierFixedStep(F& callback, const ImCubicBezierPoints& curve, float step, bool overshoot = false, float max_value_error = 1e-3f, float max_t_error = 1e-5f);


//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// VERSION 0.1
//
// LICENSE
//   This software is dual-licensed to the public domain and under the following
//   license: you are granted a perpetual, irrevocable license to copy, modify,
//   publish, and distribute this file as you see fit.
//
// CREDITS
//   Written by Michal Cichon
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
# include <map> // used in ImCubicBezierFixedStep


//------------------------------------------------------------------------------
template <typename T>
inline T ImLinearBezier(const T& p0, const T& p1, float t)
{
    return p0 + t * (p1 - p0);
}

template <typename T>
inline T ImLinearBezierDt(const T& p0, const T& p1, float t)
{
    IM_UNUSED(t);

    return p1 - p0;
}

template <typename T>
inline T ImQuadraticBezier(const T& p0, const T& p1, const T& p2, float t)
{
    const auto a = 1 - t;

    return a * a * p0 + 2 * t * a * p1 + t * t * p2;
}

template <typename T>
inline T ImQuadraticBezierDt(const T& p0, const T& p1, const T& p2, float t)
{
    return 2 * (1 - t) * (p1 - p0) + 2 * t * (p2 - p1);
}

template <typename T>
inline T ImCubicBezier(const T& p0, const T& p1, const T& p2, const T& p3, float t)
{
    const auto a = 1 - t;
    const auto b = a * a * a;
    const auto c = t * t * t;

    return b * p0 + 3 * t * a * a * p1 + 3 * t * t * a * p2 + c * p3;
}

template <typename T>
inline T ImCubicBezierDt(const T& p0, const T& p1, const T& p2, const T& p3, float t)
{
    const auto a = 1 - t;
    const auto b = a * a;
    const auto c = t * t;
    const auto d = 2 * t * a;

    return -3 * p0 * b + 3 * p1 * (b - d) + 3 * p2 * (d - c) + 3 * p3 * c;
}

template <typename T>
inline T ImCubicBezierSample(const T& p0, const T& p1, const T& p2, const T& p3, float t)
{
    const auto cp0_zero = ImLengthSqr(p1 - p0) < 1e-5f;
    const auto cp1_zero = ImLengthSqr(p3 - p2) < 1e-5f;

    if (cp0_zero && cp1_zero)
        return ImLinearBezier(p0, p3, t);
    else if (cp0_zero)
        return ImQuadraticBezier(p0, p2, p3, t);
    else if (cp1_zero)
        return ImQuadraticBezier(p0, p1, p3, t);
    else
        return ImCubicBezier(p0, p1, p2, p3, t);
}

template <typename T>
inline T ImCubicBezierSample(const ImCubicBezierPointsT<T>& curve, float t)
{
    return ImCubicBezierSample(curve.P0, curve.P1, curve.P2, curve.P3, t);
}

template <typename T>
inline T ImCubicBezierTangent(const T& p0, const T& p1, const T& p2, const T& p3, float t)
{
    const auto cp0_zero = ImLengthSqr(p1 - p0) < 1e-5f;
    const auto cp1_zero = ImLengthSqr(p3 - p2) < 1e-5f;

    if (cp0_zero && cp1_zero)
        return ImLinearBezierDt(p0, p3, t);
    else if (cp0_zero)
        return ImQuadraticBezierDt(p0, p2, p3, t);
    else if (cp1_zero)
        return ImQuadraticBezierDt(p0, p1, p3, t);
    else
        return ImCubicBezierDt(p0, p1, p2, p3, t);
}

template <typename T>
inline T ImCubicBezierTangent(const ImCubicBezierPointsT<T>& curve, float t)
{
    return ImCubicBezierTangent(curve.P0, curve.P1, curve.P2, curve.P3, t);
}

template <typename T>
inline float ImCubicBezierLength(const T& p0, const T& p1, const T& p2, const T& p3)
{
    // Legendre-Gauss abscissae with n=24 (x_i values, defined at i=n as the roots of the nth order Legendre polynomial Pn(x))
    static const float t_values[] =
    {
        -0.0640568928626056260850430826247450385909f,
         0.0640568928626056260850430826247450385909f,
        -0.1911188674736163091586398207570696318404f,
         0.1911188674736163091586398207570696318404f,
        -0.3150426796961633743867932913198102407864f,
         0.3150426796961633743867932913198102407864f,
        -0.4337935076260451384870842319133497124524f,
         0.4337935076260451384870842319133497124524f,
        -0.5454214713888395356583756172183723700107f,
         0.5454214713888395356583756172183723700107f,
        -0.6480936519369755692524957869107476266696f,
         0.6480936519369755692524957869107476266696f,
        -0.7401241915785543642438281030999784255232f,
         0.7401241915785543642438281030999784255232f,
        -0.8200019859739029219539498726697452080761f,
         0.8200019859739029219539498726697452080761f,
        -0.8864155270044010342131543419821967550873f,
         0.8864155270044010342131543419821967550873f,
        -0.9382745520027327585236490017087214496548f,
         0.9382745520027327585236490017087214496548f,
        -0.9747285559713094981983919930081690617411f,
         0.9747285559713094981983919930081690617411f,
        -0.9951872199970213601799974097007368118745f,
         0.9951872199970213601799974097007368118745f
    };

    // Legendre-Gauss weights with n=24 (w_i values, defined by a function linked to in the Bezier primer article)
    static const float c_values[] =
    {
        0.1279381953467521569740561652246953718517f,
        0.1279381953467521569740561652246953718517f,
        0.1258374563468282961213753825111836887264f,
        0.1258374563468282961213753825111836887264f,
        0.1216704729278033912044631534762624256070f,
        0.1216704729278033912044631534762624256070f,
        0.1155056680537256013533444839067835598622f,
        0.1155056680537256013533444839067835598622f,
        0.1074442701159656347825773424466062227946f,
        0.1074442701159656347825773424466062227946f,
        0.0976186521041138882698806644642471544279f,
        0.0976186521041138882698806644642471544279f,
        0.0861901615319532759171852029837426671850f,
        0.0861901615319532759171852029837426671850f,
        0.0733464814110803057340336152531165181193f,
        0.0733464814110803057340336152531165181193f,
        0.0592985849154367807463677585001085845412f,
        0.0592985849154367807463677585001085845412f,
        0.0442774388174198061686027482113382288593f,
        0.0442774388174198061686027482113382288593f,
        0.0285313886289336631813078159518782864491f,
        0.0285313886289336631813078159518782864491f,
        0.0123412297999871995468056670700372915759f,
        0.0123412297999871995468056670700372915759f
    };

    static_assert(sizeof(t_values) / sizeof(*t_values) == sizeof(c_values) / sizeof(*c_values), "");

    auto arc = [p0, p1, p2, p3](float t)
    {
        const auto p = ImCubicBezierDt(p0, p1, p2, p3, t);
        const auto l = ImLength(p);
        return l;
    };

    const auto z = 0.5f;
    const auto n = sizeof(t_values) / sizeof(*t_values);

    auto accumulator = 0.0f;
    for (size_t i = 0; i < n; ++i)
    {
        const auto t = z * t_values[i] + z;
        accumulator += c_values[i] * arc(t);
    }

    return z * accumulator;
}

template <typename T>
inline float ImCubicBezierLength(const ImCubicBezierPointsT<T>& curve)
{
    return ImCubicBezierLength(curve.P0, curve.P1, curve.P2, curve.P3);
}

template <typename T>
inline ImCubicBezierSplitResultT<T> ImCubicBezierSplit(const T& p0, const T& p1, const T& p2, const T& p3, float t)
{
    const auto z1 = t;
    const auto z2 = z1 * z1;
    const auto z3 = z1 * z1 * z1;
    const auto s1 = z1 - 1;
    const auto s2 = s1 * s1;
    const auto s3 = s1 * s1 * s1;

    return ImCubicBezierSplitResultT<T>
    {
        ImCubicBezierPointsT<T>
        {
                                                                 p0,
                                             z1      * p1 - s1 * p0,
                          z2      * p2 - 2 * z1 * s1 * p1 + s2 * p0,
            z3 * p3 - 3 * z2 * s1 * p2 + 3 * z1 * s2 * p1 - s3 * p0
        },
        ImCubicBezierPointsT<T>
        {
            z3 * p0 - 3 * z2 * s1 * p1 + 3 * z1 * s2 * p2 - s3 * p3,
                          z2      * p1 - 2 * z1 * s1 * p2 + s2 * p3,
                                             z1      * p2 - s1 * p3,
                                                                 p3,
        }
    };
}

template <typename T>
inline ImCubicBezierSplitResultT<T> ImCubicBezierSplit(const ImCubicBezierPointsT<T>& curve, float t)
{
    return ImCubicBezierSplit(curve.P0, curve.P1, curve.P2, curve.P3, t);
}

inline ImRect ImCubicBezierBoundingRect(const ImVec2& p0, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3)
{
    auto a = 3 * p3 - 9 * p2 + 9 * p1 - 3 * p0;
    auto b = 6 * p0 - 12 * p1 + 6 * p2;
    auto c = 3 * p1 - 3 * p0;
    auto delta_squared = ImMul(b, b) - 4 * ImMul(a, c);

    auto tl = ImMin(p0, p3);
    auto rb = ImMax(p0, p3);

# define IM_VEC2_INDEX(v, i) *(&v.x + i)

    for (int i = 0; i < 2; ++i)
    {
        if (IM_VEC2_INDEX(a, i) == 0.0f)
            continue;

        if (IM_VEC2_INDEX(delta_squared, i) >= 0)
        {
            auto delta = ImSqrt(IM_VEC2_INDEX(delta_squared, i));

            auto t0 = (-IM_VEC2_INDEX(b, i) + delta) / (2 * IM_VEC2_INDEX(a, i));
            if (t0 > 0 && t0 < 1)
            {
                auto p = ImCubicBezier(IM_VEC2_INDEX(p0, i), IM_VEC2_INDEX(p1, i), IM_VEC2_INDEX(p2, i), IM_VEC2_INDEX(p3, i), t0);
                IM_VEC2_INDEX(tl, i) = ImMin(IM_VEC2_INDEX(tl, i), p);
                IM_VEC2_INDEX(rb, i) = ImMax(IM_VEC2_INDEX(rb, i), p);
            }

            auto t1 = (-IM_VEC2_INDEX(b, i) - delta) / (2 * IM_VEC2_INDEX(a, i));
            if (t1 > 0 && t1 < 1)
            {
                auto p = ImCubicBezier(IM_VEC2_INDEX(p0, i), IM_VEC2_INDEX(p1, i), IM_VEC2_INDEX(p2, i), IM_VEC2_INDEX(p3, i), t1);
                IM_VEC2_INDEX(tl, i) = ImMin(IM_VEC2_INDEX(tl, i), p);
                IM_VEC2_INDEX(rb, i) = ImMax(IM_VEC2_INDEX(rb, i), p);
            }
        }
    }

# undef IM_VEC2_INDEX

    return ImRect(tl, rb);
}

inline ImRect ImCubicBezierBoundingRect(const ImCubicBezierPoints& curve)
{
    return ImCubicBezierBoundingRect(curve.P0, curve.P1, curve.P2, curve.P3);
}

inline ImProjectResult ImProjectOnCubicBezier(const ImVec2& point, const ImVec2& p0, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const int subdivisions)
{
    // http://pomax.github.io/bezierinfo/#projections

    const float epsilon    = 1e-5f;
    const float fixed_step = 1.0f / static_cast<float>(subdivisions - 1);

    ImProjectResult result;
    result.Point    = point;
    result.Time     = 0.0f;
    result.Distance = FLT_MAX;

    // Step 1: Coarse check
    for (int i = 0; i < subdivisions; ++i)
    {
        auto t = i * fixed_step;
        auto p = ImCubicBezier(p0, p1, p2, p3, t);
        auto s = point - p;
        auto d = ImDot(s, s);

        if (d < result.Distance)
        {
            result.Point    = p;
            result.Time     = t;
            result.Distance = d;
        }
    }

    if (result.Time == 0.0f || ImFabs(result.Time - 1.0f) <= epsilon)
    {
        result.Distance = ImSqrt(result.Distance);
        return result;
    }

    // Step 2: Fine check
    auto left  = result.Time - fixed_step;
    auto right = result.Time + fixed_step;
    auto step  = fixed_step * 0.1f;

    for (auto t = left; t < right + step; t += step)
    {
        auto p = ImCubicBezier(p0, p1, p2, p3, t);
        auto s = point - p;
        auto d = ImDot(s, s);

        if (d < result.Distance)
        {
            result.Point    = p;
            result.Time     = t;
            result.Distance = d;
        }
    }

    result.Distance = ImSqrt(result.Distance);

    return result;
}

inline ImProjectResult ImProjectOnCubicBezier(const ImVec2& p, const ImCubicBezierPoints& curve, const int subdivisions)
{
    return ImProjectOnCubicBezier(p, curve.P0, curve.P1, curve.P2, curve.P3, subdivisions);
}

inline ImCubicBezierIntersectResult ImCubicBezierLineIntersect(const ImVec2& p0, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& a0, const ImVec2& a1)
{
    auto cubic_roots = [](float a, float b, float c, float d, float* roots) -> int
    {
        int count = 0;

        auto sign = [](float x) -> float { return x < 0 ? -1.0f : 1.0f; };

        auto A = b / a;
        auto B = c / a;
        auto C = d / a;

        auto Q = (3 * B - ImPow(A, 2)) / 9;
        auto R = (9 * A * B - 27 * C - 2 * ImPow(A, 3)) / 54;
        auto D = ImPow(Q, 3) + ImPow(R, 2);               // polynomial discriminant

        if (D >= 0) // complex or duplicate roots
        {
            auto S = sign(R + ImSqrt(D)) * ImPow(ImFabs(R + ImSqrt(D)), (1.0f / 3.0f));
            auto T = sign(R - ImSqrt(D)) * ImPow(ImFabs(R - ImSqrt(D)), (1.0f / 3.0f));

            roots[0] = -A / 3 + (S + T);                // real root
            roots[1] = -A / 3 - (S + T) / 2;            // real part of complex root
            roots[2] = -A / 3 - (S + T) / 2;            // real part of complex root
            auto Im = ImFabs(ImSqrt(3) * (S - T) / 2);  // complex part of root pair

                                                        // discard complex roots
            if (Im != 0)
                count = 1;
            else
                count = 3;
        }
        else                                            // distinct real roots
        {
            auto th = ImAcos(R / ImSqrt(-ImPow(Q, 3)));

            roots[0] = 2 * ImSqrt(-Q) * ImCos(th / 3) - A / 3;
            roots[1] = 2 * ImSqrt(-Q) * ImCos((th + 2 * IM_PI) / 3) - A / 3;
            roots[2] = 2 * ImSqrt(-Q) * ImCos((th + 4 * IM_PI) / 3) - A / 3;

            count = 3;
        }

        return count;
    };

    // https://github.com/kaishiqi/Geometric-Bezier/blob/master/GeometricBezier/src/kaishiqi/geometric/intersection/Intersection.as
    //
    // Start with Bezier using Bernstein polynomials for weighting functions:
    //     (1-t^3)P0 + 3t(1-t)^2P1 + 3t^2(1-t)P2 + t^3P3
    //
    // Expand and collect terms to form linear combinations of original Bezier
    // controls.  This ends up with a vector cubic in t:
    //     (-P0+3P1-3P2+P3)t^3 + (3P0-6P1+3P2)t^2 + (-3P0+3P1)t + P0
    //             /\                  /\                /\       /\
    //             ||                  ||                ||       ||
    //             c3                  c2                c1       c0

    // Calculate the coefficients
    auto c3 =     -p0 + 3 * p1 - 3 * p2 + p3;
    auto c2 =  3 * p0 - 6 * p1 + 3 * p2;
    auto c1 = -3 * p0 + 3 * p1;
    auto c0 =      p0;

    // Convert line to normal form: ax + by + c = 0
    auto a = a1.y - a0.y;
    auto b = a0.x - a1.x;
    auto c = a0.x * (a0.y - a1.y) + a0.y * (a1.x - a0.x);

    // Rotate each cubic coefficient using line for new coordinate system?
    // Find roots of rotated cubic
    float roots[3];
    auto rootCount = cubic_roots(
        a * c3.x + b * c3.y,
        a * c2.x + b * c2.y,
        a * c1.x + b * c1.y,
        a * c0.x + b * c0.y + c,
        roots);

    // Any roots in closed interval [0,1] are intersections on Bezier, but
    // might not be on the line segment.
    // Find intersections and calculate point coordinates

    auto min = ImMin(a0, a1);
    auto max = ImMax(a0, a1);

    ImCubicBezierIntersectResult result;
    auto points = result.Points;

    for (int i = 0; i < rootCount; ++i)
    {
        auto root = roots[i];

        if (0 <= root && root <= 1)
        {
            // We're within the Bezier curve
            // Find point on Bezier
            auto p = ImCubicBezier(p0, p1, p2, p3, root);

            // See if point is on line segment
            // Had to make special cases for vertical and horizontal lines due
            // to slight errors in calculation of p00
            if (a0.x == a1.x)
            {
                if (min.y <= p.y && p.y <= max.y)
                    *points++ = p;
            }
            else if (a0.y == a1.y)
            {
                if (min.x <= p.x && p.x <= max.x)
                    *points++ = p;
            }
            else if (p.x >= min.x && p.y >= min.y && p.x <= max.x && p.y <= max.y)
            {
                *points++ = p;
            }
        }
    }

    result.Count = static_cast<int>(points - result.Points);

    return result;
}

inline ImCubicBezierIntersectResult ImCubicBezierLineIntersect(const ImCubicBezierPoints& curve, const ImLine& line)
{
    return ImCubicBezierLineIntersect(curve.P0, curve.P1, curve.P2, curve.P3, line.A, line.B);
}

inline void ImCubicBezierSubdivide(ImCubicBezierSubdivideCallback callback, void* user_pointer, const ImVec2& p0, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, float tess_tol, ImCubicBezierSubdivideFlags flags)
{
    return ImCubicBezierSubdivide(callback, user_pointer, ImCubicBezierPoints{ p0, p1, p2, p3 }, tess_tol, flags);
}

inline void ImCubicBezierSubdivide(ImCubicBezierSubdivideCallback callback, void* user_pointer, const ImCubicBezierPoints& curve, float tess_tol, ImCubicBezierSubdivideFlags flags)
{
    struct Tesselator
    {
        ImCubicBezierSubdivideCallback  Callback;
        void*                           UserPointer;
        float                           TesselationTollerance;
        ImCubicBezierSubdivideFlags     Flags;

        void Commit(const ImVec2& p, const ImVec2& t)
        {
            ImCubicBezierSubdivideSample sample;
            sample.Point = p;
            sample.Tangent = t;
            Callback(sample, UserPointer);
        }

        void Subdivide(const ImCubicBezierPoints& curve, int level = 0)
        {
            float dx = curve.P3.x - curve.P0.x;
            float dy = curve.P3.y - curve.P0.y;
            float d2 = ((curve.P1.x - curve.P3.x) * dy - (curve.P1.y - curve.P3.y) * dx);
            float d3 = ((curve.P2.x - curve.P3.x) * dy - (curve.P2.y - curve.P3.y) * dx);
            d2 = (d2 >= 0) ? d2 : -d2;
            d3 = (d3 >= 0) ? d3 : -d3;
            if ((d2 + d3) * (d2 + d3) < TesselationTollerance * (dx * dx + dy * dy))
            {
                Commit(curve.P3, ImCubicBezierTangent(curve, 1.0f));
            }
            else if (level < 10)
            {
                const auto p12 = (curve.P0 + curve.P1) * 0.5f;
                const auto p23 = (curve.P1 + curve.P2) * 0.5f;
                const auto p34 = (curve.P2 + curve.P3) * 0.5f;
                const auto p123 = (p12 + p23) * 0.5f;
                const auto p234 = (p23 + p34) * 0.5f;
                const auto p1234 = (p123 + p234) * 0.5f;

                Subdivide(ImCubicBezierPoints { curve.P0, p12, p123, p1234 }, level + 1);
                Subdivide(ImCubicBezierPoints { p1234, p234, p34, curve.P3 }, level + 1);
            }
        }
    };

    if (tess_tol < 0)
        tess_tol = 1.118f; // sqrtf(1.25f)

    Tesselator tesselator;
    tesselator.Callback              = callback;
    tesselator.UserPointer           = user_pointer;
    tesselator.TesselationTollerance = tess_tol * tess_tol;
    tesselator.Flags                 = flags;

    if (!(tesselator.Flags & ImCubicBezierSubdivide_SkipFirst))
        tesselator.Commit(curve.P0, ImCubicBezierTangent(curve, 0.0f));

    tesselator.Subdivide(curve, 0);
}

template <typename F> inline void ImCubicBezierSubdivide(F& callback, const ImVec2& p0, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, float tess_tol, ImCubicBezierSubdivideFlags flags)
{
    auto handler = [](const ImCubicBezierSubdivideSample& p, void* user_pointer)
    {
        auto& callback = *reinterpret_cast<F*>(user_pointer);
        callback(p);
    };

    ImCubicBezierSubdivide(handler, &callback, ImCubicBezierPoints{ p0, p1, p2, p3 }, tess_tol, flags);
}

template <typename F> inline void ImCubicBezierSubdivide(F& callback, const ImCubicBezierPoints& curve, float tess_tol, ImCubicBezierSubdivideFlags flags)
{
    auto handler = [](const ImCubicBezierSubdivideSample& p, void* user_pointer)
    {
        auto& callback = *reinterpret_cast<F*>(user_pointer);
        callback(p);
    };

    ImCubicBezierSubdivide(handler, &callback, curve, tess_tol, flags);
}

inline void ImCubicBezierFixedStep(ImCubicBezierFixedStepCallback callback, void* user_pointer, const ImVec2& p0, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, float step, bool overshoot, float max_value_error, float max_t_error)
{
    if (step <= 0.0f || !callback || max_value_error <= 0 || max_t_error <= 0)
        return;

    ImCubicBezierFixedStepSample sample;
    sample.T           = 0.0f;
    sample.Length      = 0.0f;
    sample.Point       = p0;
    sample.BreakSearch = false;

    callback(sample, user_pointer);
    if (sample.BreakSearch)
        return;

    const auto total_length = ImCubicBezierLength(p0, p1, p2, p3);
    const auto point_count  = static_cast<int>(total_length / step) + (overshoot ? 2 : 1);
    const auto t_min        = 0.0f;
    const auto t_max        = step * point_count / total_length;
    const auto t_0          = (t_min + t_max) * 0.5f;

    // #todo: replace map with ImVector + binary search
    std::map<float, float> cache;
    for (int point_index = 1; point_index < point_count; ++point_index)
    {
        const auto targetLength = point_index * step;

        float t_start = t_min;
        float t_end   = t_max;
        float t       = t_0;

        float t_best     = t;
        float error_best = total_length;

        while (true)
        {
            auto cacheIt = cache.find(t);
            if (cacheIt == cache.end())
            {
                const auto front        = ImCubicBezierSplit(p0, p1, p2, p3, t).Left;
                const auto split_length = ImCubicBezierLength(front);

                cacheIt = cache.emplace(t, split_length).first;
            }

            const auto length   = cacheIt->second;
            const auto error    = targetLength - length;

            if (error < error_best)
            {
                error_best = error;
                t_best     = t;
            }

            if (ImFabs(error) <= max_value_error || ImFabs(t_start - t_end) <= max_t_error)
            {
                sample.T      = t;
                sample.Length = length;
                sample.Point  = ImCubicBezier(p0, p1, p2, p3, t);

                callback(sample, user_pointer);
                if (sample.BreakSearch)
                    return;

                break;
            }
            else if (error < 0.0f)
                t_end = t;
            else // if (error > 0.0f)
                t_start = t;

            t = (t_start + t_end) * 0.5f;
        }
    }
}

inline void ImCubicBezierFixedStep(ImCubicBezierFixedStepCallback callback, void* user_pointer, const ImCubicBezierPoints& curve, float step, bool overshoot, float max_value_error, float max_t_error)
{
    ImCubicBezierFixedStep(callback, user_pointer, curve.P0, curve.P1, curve.P2, curve.P3, step, overshoot, max_value_error, max_t_error);
}

// F has signature void(const ImCubicBezierFixedStepSample& p)
template <typename F>
inline void ImCubicBezierFixedStep(F& callback, const ImVec2& p0, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, float step, bool overshoot, float max_value_error, float max_t_error)
{
    auto handler = [](ImCubicBezierFixedStepSample& sample, void* user_pointer)
    {
        auto& callback = *reinterpret_cast<F*>(user_pointer);
        callback(sample);
    };

    ImCubicBezierFixedStep(handler, &callback, p0, p1, p2, p3, step, overshoot, max_value_error, max_t_error);
}

template <typename F>
inline void ImCubicBezierFixedStep(F& callback, const ImCubicBezierPoints& curve, float step, bool overshoot, float max_value_error, float max_t_error)
{
    auto handler = [](ImCubicBezierFixedStepSample& sample, void* user_pointer)
    {
        auto& callback = *reinterpret_cast<F*>(user_pointer);
        callback(sample);
    };

    ImCubicBezierFixedStep(handler, &callback, curve.P0, curve.P1, curve.P2, curve.P3, step, overshoot, max_value_error, max_t_error);
}


//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// VERSION 0.9.1
//
// LICENSE
//   This software is dual-licensed to the public domain and under the following
//   license: you are granted a perpetual, irrevocable license to copy, modify,
//   publish, and distribute this file as you see fit.
//
// CREDITS
//   Written by Michal Cichon
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
# if IMGUI_VERSION_NUM < 19002
inline bool operator==(const ImVec2& lhs, const ImVec2& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(const ImVec2& lhs, const ImVec2& rhs)
{
    return lhs.x != rhs.x || lhs.y != rhs.y;
}
# endif

inline ImVec2 operator*(const float lhs, const ImVec2& rhs)
{
    return ImVec2(lhs * rhs.x, lhs * rhs.y);
}

# if IMGUI_VERSION_NUM < 18955
inline ImVec2 operator-(const ImVec2& lhs)
{
    return ImVec2(-lhs.x, -lhs.y);
}
# endif


//------------------------------------------------------------------------------
inline float ImLength(float v)
{
    return v;
}

inline float ImLength(const ImVec2& v)
{
    return ImSqrt(ImLengthSqr(v));
}

inline float ImLengthSqr(float v)
{
    return v * v;
}

inline ImVec2 ImNormalized(const ImVec2& v)
{
    return v * ImInvLength(v, 0.0f);
}




//------------------------------------------------------------------------------
inline bool ImRect_IsEmpty(const ImRect& rect)
{
    return rect.Min.x >= rect.Max.x
        || rect.Min.y >= rect.Max.y;
}

inline ImVec2 ImRect_ClosestPoint(const ImRect& rect, const ImVec2& p, bool snap_to_edge)
{
    if (!snap_to_edge && rect.Contains(p))
        return p;

    return ImVec2(
        (p.x > rect.Max.x) ? rect.Max.x : (p.x < rect.Min.x ? rect.Min.x : p.x),
        (p.y > rect.Max.y) ? rect.Max.y : (p.y < rect.Min.y ? rect.Min.y : p.y)
    );
}

inline ImVec2 ImRect_ClosestPoint(const ImRect& rect, const ImVec2& p, bool snap_to_edge, float radius)
{
    auto point = ImRect_ClosestPoint(rect, p, snap_to_edge);

    const auto offset      = p - point;
    const auto distance_sq = offset.x * offset.x + offset.y * offset.y;
    if (distance_sq <= 0)
        return point;

    const auto distance = ImSqrt(distance_sq);

    return point + offset * (ImMin(distance, radius) * (1.0f / distance));
}

inline ImVec2 ImRect_ClosestPoint(const ImRect& rect, const ImRect& other)
{
    ImVec2 result;
    if (other.Min.x >= rect.Max.x)
        result.x = rect.Max.x;
    else if (other.Max.x <= rect.Min.x)
        result.x = rect.Min.x;
    else
        result.x = (ImMax(rect.Min.x, other.Min.x) + ImMin(rect.Max.x, other.Max.x)) / 2;

    if (other.Min.y >= rect.Max.y)
        result.y = rect.Max.y;
    else if (other.Max.y <= rect.Min.y)
        result.y = rect.Min.y;
    else
        result.y = (ImMax(rect.Min.y, other.Min.y) + ImMin(rect.Max.y, other.Max.y)) / 2;

    return result;
}

inline ImLine ImRect_ClosestLine(const ImRect& rect_a, const ImRect& rect_b)
{
    ImLine result;
    result.A = ImRect_ClosestPoint(rect_a, rect_b);
    result.B = ImRect_ClosestPoint(rect_b, rect_a);

    auto distribute = [](float& a, float& b, float a0, float a1, float b0, float b1)
    {
        if (a0 >= b1 || a1 <= b0)
            return;

        const auto aw = a1 - a0;
        const auto bw = b1 - b0;

        if (aw > bw)
        {
            b = b0 + bw - bw * (a - a0) / aw;
            a = b;
        }
        else if (aw < bw)
        {
            a = a0 + aw - aw * (b - b0) / bw;
            b = a;
        }
    };

    distribute(result.A.x, result.B.x, rect_a.Min.x, rect_a.Max.x, rect_b.Min.x, rect_b.Max.x);
    distribute(result.A.y, result.B.y, rect_a.Min.y, rect_a.Max.y, rect_b.Min.y, rect_b.Max.y);

    return result;
}

inline ImLine ImRect_ClosestLine(const ImRect& rect_a, const ImRect& rect_b, float radius_a, float radius_b)
{
    auto line = ImRect_ClosestLine(rect_a, rect_b);
    if (radius_a < 0)
        radius_a = 0;
    if (radius_b < 0)
        radius_b = 0;

    if (radius_a == 0 && radius_b == 0)
        return line;

    const auto offset      = line.B - line.A;
    const auto length_sq   = offset.x * offset.x + offset.y * offset.y;
    const auto radius_a_sq = radius_a * radius_a;
    const auto radius_b_sq = radius_b * radius_b;

    if (length_sq <= 0)
        return line;

    const auto length    = ImSqrt(length_sq);
    const auto direction = ImVec2(offset.x / length, offset.y / length);

    const auto total_radius_sq = radius_a_sq + radius_b_sq;
    if (total_radius_sq > length_sq)
    {
        const auto scale = length / (radius_a + radius_b);
        radius_a *= scale;
        radius_b *= scale;
    }

    line.A = line.A + (direction * radius_a);
    line.B = line.B - (direction * radius_b);

    return line;
}


//------------------------------------------------------------------------------


//------------------------------------------------------------------------------


inline static void CopyIOEvents(ImGuiContext* src, ImGuiContext* dst, ImVec2 origin, float scale)
{
    dst->InputEventsQueue = src->InputEventsTrail;
    for (ImGuiInputEvent& e : dst->InputEventsQueue) {
        if (e.Type == ImGuiInputEventType_MousePos) {
            e.MousePos.PosX = (e.MousePos.PosX - origin.x) / scale;
            e.MousePos.PosY = (e.MousePos.PosY - origin.y) / scale;
        }
    }
}

inline static void AppendDrawData(ImDrawList* src, ImVec2 origin, float scale)
{
    // TODO optimize if vtx_start == 0 || if idx_start == 0
    ImDrawList* dl = ImGui::GetWindowDrawList();
    const int vtx_start = dl->VtxBuffer.size();
    const int idx_start = dl->IdxBuffer.size();
    dl->VtxBuffer.resize(dl->VtxBuffer.size() + src->VtxBuffer.size());
    dl->IdxBuffer.resize(dl->IdxBuffer.size() + src->IdxBuffer.size());
    dl->CmdBuffer.reserve(dl->CmdBuffer.size() + src->CmdBuffer.size());
    dl->_VtxWritePtr = dl->VtxBuffer.Data + vtx_start;
    dl->_IdxWritePtr = dl->IdxBuffer.Data + idx_start;
    const ImDrawVert* vtx_read = src->VtxBuffer.Data;
    const ImDrawIdx* idx_read = src->IdxBuffer.Data;
    for (int i = 0, c = src->VtxBuffer.size(); i < c; ++i) {
        dl->_VtxWritePtr[i].uv = vtx_read[i].uv;
        dl->_VtxWritePtr[i].col = vtx_read[i].col;
        dl->_VtxWritePtr[i].pos = vtx_read[i].pos * scale + origin;
    }
    for (int i = 0, c = src->IdxBuffer.size(); i < c; ++i) {
        dl->_IdxWritePtr[i] = idx_read[i] + vtx_start;
    }
    for (auto cmd : src->CmdBuffer) {
        cmd.IdxOffset += idx_start;
        IM_ASSERT(cmd.VtxOffset == 0);
        cmd.ClipRect.x = cmd.ClipRect.x * scale + origin.x;
        cmd.ClipRect.y = cmd.ClipRect.y * scale + origin.y;
        cmd.ClipRect.z = cmd.ClipRect.z * scale + origin.x;
        cmd.ClipRect.w = cmd.ClipRect.w * scale + origin.y;
        dl->CmdBuffer.push_back(cmd);
    }

    dl->_VtxCurrentIdx += src->VtxBuffer.size();
    dl->_VtxWritePtr = dl->VtxBuffer.Data + dl->VtxBuffer.size();
    dl->_IdxWritePtr = dl->IdxBuffer.Data + dl->IdxBuffer.size();
}

struct ContainedContextConfig
{
    bool extra_window_wrapper = false;
    ImVec2 size = {0.f, 0.f};
    ImU32 color = IM_COL32_WHITE;
    bool zoom_enabled = true;
    float zoom_min = 0.3f;
    float zoom_max = 2.f;
    float zoom_divisions = 10.f;
    float zoom_smoothness = 5.f;
    float default_zoom = 1.f;
    ImGuiKey reset_zoom_key = ImGuiKey_R;
    ImGuiMouseButton scroll_button = ImGuiMouseButton_Middle;
};

class ContainedContext
{
public:
    ~ContainedContext();
    ContainedContextConfig& config() { return m_config; }
    void begin();
    void end();
    [[nodiscard]] ImVec2 size() const { return m_size; }
    [[nodiscard]] float scale() const { return m_scale; }
    [[nodiscard]] const ImVec2& origin() const { return m_origin; }
    [[nodiscard]] bool hovered() const { return m_hovered; }
    [[nodiscard]] const ImVec2& scroll() const { return m_scroll; }
    ImGuiContext* getRawContext() { return m_ctx; }
private:
    ContainedContextConfig m_config;

    ImVec2 m_origin;
    ImVec2 m_pos;
    ImVec2 m_size;
    ImGuiContext* m_ctx = nullptr;
    ImGuiContext* m_original_ctx = nullptr;

    bool m_anyWindowHovered = false;
    bool m_anyItemActive = false;
    bool m_hovered = false;

    float m_scale = m_config.default_zoom, m_scaleTarget = m_config.default_zoom;
    ImVec2 m_scroll = {0.f, 0.f}, m_scrollTarget = {0.f, 0.f};
};

inline ContainedContext::~ContainedContext()
{
    if (m_ctx) ImGui::DestroyContext(m_ctx);
}

inline void ContainedContext::begin()
{
    ImGui::PushID(this);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, m_config.color);
    ImGui::BeginChild("view_port", m_config.size, 0, ImGuiWindowFlags_NoMove);
    ImGui::PopStyleColor();
    m_pos = ImGui::GetWindowPos();

    m_size = ImGui::GetContentRegionAvail();
    m_origin = ImGui::GetCursorScreenPos();
    m_original_ctx = ImGui::GetCurrentContext();
    const ImGuiStyle& orig_style = ImGui::GetStyle();
    if (!m_ctx) m_ctx = ImGui::CreateContext(ImGui::GetIO().Fonts);
    ImGui::SetCurrentContext(m_ctx);
    ImGuiStyle& new_style = ImGui::GetStyle();
    new_style = orig_style;

    CopyIOEvents(m_original_ctx, m_ctx, m_origin, m_scale);

    ImGui::GetIO().DisplaySize = m_size / m_scale;
    ImGui::GetIO().ConfigInputTrickleEventQueue = false;
    ImGui::NewFrame();

    if (!m_config.extra_window_wrapper)
        return;
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImGui::GetMainViewport()->WorkSize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("viewport_container", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove
                                                | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::PopStyleVar();
}

inline void ContainedContext::end()
{
    m_anyWindowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
    if (m_config.extra_window_wrapper && ImGui::IsWindowHovered())
        m_anyWindowHovered = false;

    m_anyItemActive = ImGui::IsAnyItemActive();

    if (m_config.extra_window_wrapper)
        ImGui::End();

    ImGui::Render();

    ImDrawData* draw_data = ImGui::GetDrawData();

    ImGui::SetCurrentContext(m_original_ctx);
    m_original_ctx = nullptr;

    for (int i = 0; i < draw_data->CmdListsCount; ++i)
        AppendDrawData(draw_data->CmdLists[i], m_origin, m_scale);

    m_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && !m_anyWindowHovered;

    // Zooming
    if (m_config.zoom_enabled && m_hovered && ImGui::GetIO().MouseWheel != 0.f)
    {
        m_scaleTarget += ImGui::GetIO().MouseWheel / m_config.zoom_divisions;
        m_scaleTarget = m_scaleTarget < m_config.zoom_min ? m_config.zoom_min : m_scaleTarget;
        m_scaleTarget = m_scaleTarget > m_config.zoom_max ? m_config.zoom_max : m_scaleTarget;

        if (m_config.zoom_smoothness == 0.f)
        {
            m_scroll += (ImGui::GetMousePos() - m_pos) / m_scaleTarget - (ImGui::GetMousePos() - m_pos) / m_scale;
            m_scale = m_scaleTarget;
        }
    }
    if (abs(m_scaleTarget - m_scale) >= 0.015f / m_config.zoom_smoothness)
    {
        float cs = (m_scaleTarget - m_scale) / m_config.zoom_smoothness;
        m_scroll += (ImGui::GetMousePos() - m_pos) / (m_scale + cs) - (ImGui::GetMousePos() - m_pos) / m_scale;
        m_scale += (m_scaleTarget - m_scale) / m_config.zoom_smoothness;

        if (abs(m_scaleTarget - m_scale) < 0.015f / m_config.zoom_smoothness)
        {
            m_scroll += (ImGui::GetMousePos() - m_pos) / m_scaleTarget - (ImGui::GetMousePos() - m_pos) / m_scale;
            m_scale = m_scaleTarget;
        }
    }

    // Zoom reset
    if (ImGui::IsKeyPressed(m_config.reset_zoom_key, false))
        m_scaleTarget = m_config.default_zoom;

    // Scrolling
    if (m_hovered && !m_anyItemActive && ImGui::IsMouseDragging(m_config.scroll_button, 0.f))
    {
        m_scroll += ImGui::GetIO().MouseDelta / m_scale;
        m_scrollTarget = m_scroll;
    }

    ImGui::EndChild();
    ImGui::PopID();
}



#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <cmath>
#include <memory>
#include <algorithm>
#include <functional>
#include <unordered_map>

//#define ConnectionFilter_None       [](ImFlow::Pin* out, ImFlow::Pin* in){ return true; }
//#define ConnectionFilter_SameType   [](ImFlow::Pin* out, ImFlow::Pin* in){ return out->getDataType() == in->getDataType(); }
//#define ConnectionFilter_Numbers    [](ImFlow::Pin* out, ImFlow::Pin* in){ return out->getDataType() == typeid(double) || out->getDataType() == typeid(float) || out->getDataType() == typeid(int); }

namespace ImFlow
{
    // -----------------------------------------------------------------------------------------------------------------
    // HELPERS

    /**
     * @brief <BR>Draw a sensible bezier between two points
     * @param p1 Starting point
     * @param p2 Ending point
     * @param color Color of the curve
     * @param thickness Thickness of the curve
     */
    inline static void smart_bezier(const ImVec2& p1, const ImVec2& p2, ImU32 color, float thickness);

    /**
     * @brief <BR>Collider checker for smart_bezier
     * @details Projects the point "p" orthogonally onto the bezier curve and
     *          checks if the distance is less than the given radius.
     * @param p Point to be tested
     * @param p1 Starting point of smart_bezier
     * @param p2 Ending point of smart_bezier
     * @param radius Lateral width of the hit box
     * @return [TRUE] if "p" is inside the collider
     *
     * Intended to be used in union with smart_bezier();
     */
    inline static bool smart_bezier_collider(const ImVec2& p, const ImVec2& p1, const ImVec2& p2, float radius);

    // -----------------------------------------------------------------------------------------------------------------
    // CLASSES PRE-DEFINITIONS

    template<typename T> class InPin;
    template<typename T> class OutPin;
    class Pin; class BaseNode;
    class ImNodeFlow; class ConnectionFilter;

    // -----------------------------------------------------------------------------------------------------------------
    // PIN'S PROPERTIES

    typedef unsigned long long int PinUID;

    /**
     * @brief Extra pin's style setting
     */
    struct PinStyleExtras
    {
        /// @brief Top and bottom spacing
        ImVec2 padding = ImVec2(3.f, 1.f);
        /// @brief Border and background corner rounding
        float bg_radius = 8.f;
        /// @brief Border thickness
        float border_thickness = 1.f;
        /// @brief Background color
        ImU32 bg_color = IM_COL32(23, 16, 16, 0);
        /// @brief Background color when hovered
        ImU32 bg_hover_color = IM_COL32(100, 100, 255, 70);
        /// @brief Border color
        ImU32 border_color = IM_COL32(255, 255, 255, 0);

        /// @brief Link thickness
        float link_thickness = 2.6f;
        /// @brief Link thickness when dragged
        float link_dragged_thickness = 2.2f;
        /// @brief Link thickness when hovered
        float link_hovered_thickness = 3.5f;
        /// @brief Thickness of the outline of a selected link
        float link_selected_outline_thickness = 0.5f;
        /// @brief Color of the outline of a selected link
        ImU32 outline_color = IM_COL32(80, 20, 255, 200);

        /// @brief Spacing between pin content and socket
        float socket_padding = 6.6f;

    };

    /**
     * @brief Defines the visual appearance of a pin
     */
    class PinStyle
    {
    public:
        PinStyle(ImU32 color, int socket_shape, float socket_radius, float socket_hovered_radius, float socket_connected_radius, float socket_thickness)
                :color(color), socket_shape(socket_shape), socket_radius(socket_radius), socket_hovered_radius(socket_hovered_radius), socket_connected_radius(socket_connected_radius),  socket_thickness(socket_thickness) {}

        /// @brief Socket and link color
        ImU32 color;
        /// @brief Socket shape ID
        int socket_shape;
        /// @brief Socket radius
        float socket_radius;
        /// @brief Socket radius when hovered
        float socket_hovered_radius;
        /// @brief Socket radius when connected
        float socket_connected_radius;
        /// @brief Socket outline thickness when empty
        float socket_thickness;
        /// @brief List of less common properties
        PinStyleExtras extra;
    public:
        /// @brief <BR>Default cyan style
        static std::shared_ptr<PinStyle> cyan() { return std::make_shared<PinStyle>(PinStyle(IM_COL32(87,155,185,255), 0, 4.f, 4.67f, 3.7f, 1.f)); }
        /// @brief <BR>Default green style
        static std::shared_ptr<PinStyle> green() { return std::make_shared<PinStyle>(PinStyle(IM_COL32(90,191,93,255), 4, 4.f, 4.67f, 4.2f, 1.3f)); }
        /// @brief <BR>Default blue style
        static std::shared_ptr<PinStyle> blue() { return std::make_shared<PinStyle>(PinStyle(IM_COL32(90,117,191,255), 0, 4.f, 4.67f, 3.7f, 1.f)); }
        /// @brief <BR>Default brown style
        static std::shared_ptr<PinStyle> brown() { return std::make_shared<PinStyle>(PinStyle(IM_COL32(191,134,90,255), 0, 4.f, 4.67f, 3.7f, 1.f)); }
        /// @brief <BR>Default red style
        static std::shared_ptr<PinStyle> red() { return std::make_shared<PinStyle>(PinStyle(IM_COL32(191,90,90,255), 0, 4.f, 4.67f, 3.7f, 1.f)); }
        /// @brief <BR>Default white style
        static std::shared_ptr<PinStyle> white() { return std::make_shared<PinStyle>(PinStyle(IM_COL32(255,255,255,255), 5, 4.f, 4.67f, 4.2f, 1.f)); }
    };

    // -----------------------------------------------------------------------------------------------------------------
    // NODE'S PROPERTIES

    typedef uintptr_t NodeUID;

    /**
     * @brief Defines the visual appearance of a node
     */
    class NodeStyle
    {
    public:
        NodeStyle(ImU32 header_bg, ImColor header_title_color, float radius) :header_bg(header_bg), header_title_color(header_title_color), radius(radius) {}

        /// @brief Body's background color
        ImU32 bg = IM_COL32(55,64,75,255);
        /// @brief Header's background color
        ImU32 header_bg;
        /// @brief Header title color
        ImColor header_title_color;
        /// @brief Border color
        ImU32 border_color = IM_COL32(30,38,41,140);
        /// @brief Border color when selected
        ImU32 border_selected_color = IM_COL32(170, 190, 205, 230);

        /// @brief Body's content padding (Left Top Right Bottom)
        ImVec4 padding = ImVec4(13.7f, 6.f, 13.7f, 2.f);
        /// @brief Edges rounding
        float radius;
        /// @brief Border thickness
        float border_thickness = -1.35f;
        /// @brief Border thickness when selected
        float border_selected_thickness = 2.f;
    public:
        /// @brief <BR>Default cyan style
        static std::shared_ptr<NodeStyle> cyan() { return std::make_shared<NodeStyle>(IM_COL32(71,142,173,255), ImColor(233,241,244,255), 6.5f); }
        /// @brief <BR>Default green style
        static std::shared_ptr<NodeStyle> green() { return std::make_shared<NodeStyle>(IM_COL32(90,191,93,255), ImColor(233,241,244,255), 3.5f); }
        /// @brief <BR>Default red style
        static std::shared_ptr<NodeStyle> red() { return std::make_shared<NodeStyle>(IM_COL32(191,90,90,255), ImColor(233,241,244,255), 11.f); }
        /// @brief <BR>Default brown style
        static std::shared_ptr<NodeStyle> brown() { return std::make_shared<NodeStyle>(IM_COL32(191,134,90,255), ImColor(233,241,244,255), 6.5f); }
    };

    // -----------------------------------------------------------------------------------------------------------------
    // LINK

    /**
     * @brief Link between two Pins of two different Nodes
     */
    class Link
    {
    public:
        /**
         * @brief <BR>Construct a link
         * @param left Pointer to the output Pin of the Link
         * @param right Pointer to the input Pin of the Link
         * @param inf Pointer to the Handler that contains the Link
         */
        explicit Link(Pin* left, Pin* right, ImNodeFlow* inf) : m_left(left), m_right(right), m_inf(inf) {}

        /**
         * @brief <BR>Destruction of a link
         * @details Deletes references of this links form connected pins
         */
        ~Link();

        /**
         * @brief <BR>Looping function to update the Link
         * @details Draws the Link and updates Hovering and Selected status.
         */
        void update();

        /**
         * @brief <BR>Get Left pin of the link
         * @return Pointer to the Pin
         */
        [[nodiscard]] Pin* left() const { return m_left; }

        /**
         * @brief <BR>Get Right pin of the link
         * @return Pointer to the Pin
         */
        [[nodiscard]] Pin* right() const { return m_right; }

        /**
         * @brief <BR>Get hovering status
         * @return [TRUE] If the link is hovered in the current frame
         */
        [[nodiscard]] bool isHovered() const { return m_hovered; }

        /**
         * @brief <BR>Get selected status
         * @return [TRUE] If the link is selected in the current frame
         */
        [[nodiscard]] bool isSelected() const { return m_selected; }
    private:
        Pin* m_left;
        Pin* m_right;
        ImNodeFlow* m_inf;
        bool m_hovered = false;
        bool m_selected = false;
    };

    // -----------------------------------------------------------------------------------------------------------------
    // HANDLER

    /**
     * @brief Grid's the color parameters
     */
    struct InfColors
    {
        /// @brief Background of the grid
        ImU32 background = IM_COL32(33,41,45,255);
        /// @brief Main lines of the grid
        ImU32 grid = IM_COL32(200, 200, 200, 40);
        /// @brief Secondary lines
        ImU32 subGrid = IM_COL32(200, 200, 200, 10);
    };

    /**
     * @brief ALl the grid's appearance parameters. Sizes + Colors
     */
    struct InfStyler
    {
        /// @brief Size of main grid
        float grid_size = 50.f;
        /// @brief Sub-grid divisions for Node snapping
        float grid_subdivisions = 5.f;
        /// @brief ImNodeFlow colors
        InfColors colors;
    };

    /**
     * @brief Main node editor
     * @details Handles the infinite grid, nodes and links. Also handles all the logic.
     */
    class ImNodeFlow
    {
    private:
        static int m_instances;
    public:
        /**
         * @brief <BR>Instantiate a new editor with default name.
         * <BR> Editor name will be "FlowGrid + the number of editors"
         */
        ImNodeFlow() : ImNodeFlow("FlowGrid" + std::to_string(m_instances)) {}

        /**
         * @brief <BR>Instantiate a new editor with given name
         * @details Creates a new Node Editor with the given name.
         * @param name Name of the editor
         */
        explicit ImNodeFlow(std::string name) :m_name(std::move(name))
        {
            m_instances++;
            m_context.config().extra_window_wrapper = true;
            m_context.config().color = m_style.colors.background;
        }

        /**
         * @brief <BR>Handler loop
         * @details Main update function. Refreshes all the logic and draws everything. Must be called every frame.
         */
        void update();

        /**
         * @brief <BR>Add a node to the grid
         * @tparam T Derived class of <BaseNode> to be added
         * @tparam Params types of optional args to forward to derived class ctor
         * @param pos Position of the Node in grid coordinates
         * @param args Optional arguments to be forwarded to derived class ctor
         * @return Shared pointer of the pushed type to the newly added node
         *
         * Inheritance is checked at compile time, \<T> MUST be derived from BaseNode.
         */
        template<typename T, typename... Params>
        std::shared_ptr<T> addNode(const ImVec2& pos, Params&&... args);

        /**
         * @brief <BR>Add a node to the grid
         * @tparam T Derived class of <BaseNode> to be added
         * @tparam Params types of optional args to forward to derived class ctor
         * @param pos Position of the Node in screen coordinates
         * @param args Optional arguments to be forwarded to derived class ctor
         * @return Shared pointer of the pushed type to the newly added node
         *
         * Inheritance is checked at compile time, \<T> MUST be derived from BaseNode.
         */
        template<typename T, typename... Params>
        std::shared_ptr<T> placeNodeAt(const ImVec2& pos, Params&&... args);

        /**
         * @brief <BR>Add a node to the grid using mouse position
         * @tparam T Derived class of <BaseNode> to be added
         * @tparam Params types of optional args to forward to derived class ctor
         * @param args Optional arguments to be forwarded to derived class ctor
         * @return Shared pointer of the pushed type to the newly added node
         *
         * Inheritance is checked at compile time, \<T> MUST be derived from BaseNode.
         */
        template<typename T, typename... Params>
        std::shared_ptr<T> placeNode(Params&&... args);

        /**
         * @brief <BR>Add link to the handler internal list
         * @param link Reference to the link
         */
        void addLink(std::shared_ptr<Link>& link);

        /**
         * @brief <BR>Pop-up when link is "dropped"
         * @details Sets the content of a pop-up that can be displayed when dragging a link in the open instead of onto another pin.
         * @details If "key = ImGuiKey_None" the pop-up will always open when a link is dropped.
         * @param content Function or Lambda containing only the contents of the pop-up and the subsequent logic
         * @param key Optional key required in order to open the pop-up
         */
        void droppedLinkPopUpContent(std::function<void(Pin* dragged)> content, ImGuiKey key = ImGuiKey_None) { m_droppedLinkPopUp = std::move(content); m_droppedLinkPupUpComboKey = key; }

        /**
         * @brief <BR>Pop-up when right-clicking
         * @details Sets the content of a pop-up that can be displayed when right-clicking on the grid.
         * @param content Function or Lambda containing only the contents of the pop-up and the subsequent logic
         */
        void rightClickPopUpContent(std::function<void(BaseNode* node)> content) { m_rightClickPopUp = std::move(content); }

        /**
         * @brief <BR>Get mouse clicking status
         * @return [TRUE] if mouse is clicked and click hasn't been consumed
         */
        [[nodiscard]] bool getSingleUseClick() const { return m_singleUseClick; }

        /**
         * @brief <BR>Consume the click for the given frame
         */
        void consumeSingleUseClick() { m_singleUseClick = false; }

        /**
         * @brief <BR>Get editor's name
         * @return Const reference to editor's name
         */
        const std::string& getName() { return m_name; }

        /**
         * @brief <BR>Get editor's position
         * @return Const reference to editor's position in screen coordinates
         */
        const ImVec2& getPos() { return m_context.origin(); }

        /**
         * @brief <BR>Get editor's grid scroll
         * @details Scroll is the offset from the origin of the grid, changes while navigating the grid.
         * @return Const reference to editor's grid scroll
         */
        const ImVec2& getScroll() { return m_context.scroll(); }

        /**
         * @brief <BR>Get editor's list of nodes
         * @return Const reference to editor's internal nodes list
         */
        std::unordered_map<NodeUID, std::shared_ptr<BaseNode>>& getNodes() { return m_nodes; }

        /**
         * @brief <BR>Get nodes count
         * @return Number of nodes present in the editor
         */
        uint32_t getNodesCount() { return (uint32_t)m_nodes.size(); }

        /**
         * @brief <BR>Get editor's list of links
         * @return Const reference to editor's internal links list
         */
        const std::vector<std::weak_ptr<Link>>& getLinks() { return m_links; }

        /**
         * @brief <BR>Get zooming viewport
         * @return Const reference to editor's internal viewport for zoom support
         */
        ContainedContext& getGrid() { return m_context; }

        /**
         * @brief <BR>Get dragging status
         * @return [TRUE] if a Node is being dragged around the grid
         */
        [[nodiscard]] bool isNodeDragged() const { return m_draggingNode; }

        /**
         * @brief <BR>Get current style
         * @return Reference to style variables
         */
        InfStyler& getStyle() { return m_style; }

        /**
         * @brief <BR>Set editor's size
         * @param size Editor's size. Set to (0, 0) to auto-fit.
         */
        void setSize(const ImVec2& size) { m_context.config().size = size; }

        /**
         * @brief <BR>Set dragging status
         * @param state New dragging state
         *
         * The new state will only be updated one at the start of each frame.
         */
        void draggingNode(bool state) { m_draggingNodeNext = state; }

        /**
         * @brief <BR>Set what pin is being hovered
         * @param hovering Pointer to the hovered pin
         */
        void hovering(Pin* hovering) { m_hovering = hovering; }

        /**
         * @brief <BR>Set what node is being hovered
         * @param hovering Pointer to the hovered node
         */
        void hoveredNode(BaseNode* hovering) { m_hoveredNode = hovering; }

        /**
         * @brief <BR>Convert coordinates from screen to grid
         * @param p Point in screen coordinates to be converted
         * @return Point in grid's coordinates
         */
        ImVec2 screen2grid(const ImVec2& p);

        /**
         * @brief <BR>Convert coordinates from grid to screen
         * @param p Point in grid's coordinates to be converted
         * @return Point in screen coordinates
         */
        ImVec2 grid2screen(const ImVec2 &p);

        /**
         * @brief <BR>Check if mouse is on selected node
         * @return [TRUE] if the mouse is hovering a selected node
         */
        bool on_selected_node();

        /**
         * @brief <BR>Check if mouse is on a free point on the grid
         * @return [TRUE] if the mouse is not hovering a node or a link
         */
        bool on_free_space();

        /**
         * @brief <BR>Get recursion blacklist for nodes
         * @return Reference to blacklist
         */
        std::vector<std::string>& get_recursion_blacklist() { return m_pinRecursionBlacklist; }
    private:
        std::string m_name;
        ContainedContext m_context;

        bool m_singleUseClick = false;

        std::unordered_map<NodeUID, std::shared_ptr<BaseNode>> m_nodes;
        std::vector<std::string> m_pinRecursionBlacklist;
        std::vector<std::weak_ptr<Link>> m_links;

        std::function<void(Pin* dragged)> m_droppedLinkPopUp;
        ImGuiKey m_droppedLinkPupUpComboKey = ImGuiKey_None;
        Pin* m_droppedLinkLeft = nullptr;
        std::function<void(BaseNode* node)> m_rightClickPopUp;
        BaseNode* m_hoveredNodeAux = nullptr;

        BaseNode* m_hoveredNode = nullptr;
        bool m_draggingNode = false, m_draggingNodeNext = false;
        Pin* m_hovering = nullptr;
        Pin* m_dragOut = nullptr;

        InfStyler m_style;
    };

    // -----------------------------------------------------------------------------------------------------------------
    // BASE NODE

    /**
     * @brief Parent class for custom nodes
     * @details Main class from which custom nodes can be created. All interactions with the main grid are handled internally.
     */
    class BaseNode
    {
    public:
        virtual ~BaseNode() = default;
        BaseNode() = default;

        /**
         * @brief <BR>Main loop of the node
         * @details Updates position, hovering and selected status, and renders the node. Must be called each frame.
         */
        void update();

        /**
         * @brief <BR>Content of the node
         * @details Function to be implemented by derived custom nodes.
         *          Must contain the body of the node. If left empty the node will only have input and output pins.
         */
        virtual void draw() {}

        /**
         * @brief <BR>Add an Input to the node
         * @details Will add an Input pin to the node with the given name and data type.
         *          <BR> <BR> In this case the name of the pin will also be its UID.
         *          <BR> <BR> The UID must be unique only in the context of the current node's inputs.
         * @tparam T Type of the data the pin will handle
         * @param name Name of the pin
         * @param defReturn Default return value when the pin is not connected
         * @param filter Connection filter
         * @param style Style of the pin
         * @return Shared pointer to the newly added pin
         */
        template<typename T>
        std::shared_ptr<InPin<T>> addIN(const std::string& name, T defReturn, std::function<bool(Pin*, Pin*)> filter, std::shared_ptr<PinStyle> style = nullptr);

        /**
         * @brief <BR>Add an Input to the node
         * @details Will add an Input pin to the node with the given name and data type.
         *          <BR> <BR> The UID must be unique only in the context of the current node's inputs.
         * @tparam T Type of the data the pin will handle
         * @tparam U Type of the UID
         * @param uid Unique identifier of the pin
         * @param name Name of the pin
         * @param defReturn Default return value when the pin is not connected
         * @param filter Connection filter
         * @param style Style of the pin
         * @return Shared pointer to the newly added pin
         */
        template<typename T, typename U>
        std::shared_ptr<InPin<T>> addIN_uid(const U& uid, const std::string& name, T defReturn, std::function<bool(Pin*, Pin*)> filter, std::shared_ptr<PinStyle> style = nullptr);

        /**
         * @brief <BR>Remove input pin
         * @tparam U Type of the UID
         * @param uid Unique identifier of the pin
         */
        template<typename U>
        void dropIN(const U& uid);

        /**
         * @brief <BR>Remove input pin
         * @param uid Unique identifier of the pin
         */
        void dropIN(const char* uid);

        /**
         * @brief <BR>Show a temporary input pin
         * @details Will show an input pin with the given name.
         *          The pin is created the first time showIN is called and kept alive as long as showIN is called each frame.
         *          <BR> <BR> In this case the name of the pin will also be its UID.
         *          <BR> <BR> The UID must be unique only in the context of the current node's inputs.
         * @tparam T Type of the data the pin will handle
         * @param name Name of the pin
         * @param defReturn Default return value when the pin is not connected
         * @param filter Connection filter
         * @param style Style of the pin
         * @return Const reference to the value of the connected link for the current frame of defReturn
         */
        template<typename T>
        const T& showIN(const std::string& name, T defReturn, std::function<bool(Pin*, Pin*)> filter, std::shared_ptr<PinStyle> style = nullptr);

        /**
         * @brief <BR>Show a temporary input pin
         * @details Will show an input pin with the given name and UID.
         *          The pin is created the first time showIN_uid is called and kept alive as long as showIN_uid is called each frame.
         *          <BR> <BR> The UID must be unique only in the context of the current node's inputs.
         * @tparam T Type of the data the pin will handle
         * @tparam U Type of the UID
         * @param uid Unique identifier of the pin
         * @param name Name of the pin
         * @param defReturn Default return value when the pin is not connected
         * @param filter Connection filter
         * @param style Style of the pin
         * @return Const reference to the value of the connected link for the current frame of defReturn
         */
        template<typename T, typename U>
        const T& showIN_uid(const U& uid, const std::string& name, T defReturn, std::function<bool(Pin*, Pin*)> filter, std::shared_ptr<PinStyle> style = nullptr);

        /**
         * @brief <BR>Add an Output to the node
         * @details Must be called in the node constructor. WIll add an Output pin to the node with the given name and data type.
         *          <BR> <BR> In this case the name of the pin will also be its UID.
         *          <BR> <BR> The UID must be unique only in the context of the current node's outputs.
         * @tparam T Type of the data the pin will handle
         * @param name Name of the pin
         * @param filter Connection filter
         * @param style Style of the pin
         * @return Shared pointer to the newly added pin. Must be used to set the behaviour
         */
        template<typename T>
        [[nodiscard]] std::shared_ptr<OutPin<T>> addOUT(const std::string& name, std::shared_ptr<PinStyle> style = nullptr);

        /**
         * @brief <BR>Add an Output to the node
         * @details Must be called in the node constructor. WIll add an Output pin to the node with the given name and data type.
         *          <BR> <BR> The UID must be unique only in the context of the current node's outputs.
         * @tparam T Type of the data the pin will handle
         * @tparam U Type of the UID
         * @param uid Unique identifier of the pin
         * @param name Name of the pin
         * @param filter Connection filter
         * @param style Style of the pin
         * @return Shared pointer to the newly added pin. Must be used to set the behaviour
         */
        template<typename T, typename U>
        [[nodiscard]] std::shared_ptr<OutPin<T>> addOUT_uid(const U& uid, const std::string& name, std::shared_ptr<PinStyle> style = nullptr);

        /**
         * @brief <BR>Remove output pin
         * @tparam U Type of the UID
         * @param uid Unique identifier of the pin
         */
        template<typename U>
        void dropOUT(const U& uid);

        /**
         * @brief <BR>Remove output pin
         * @param uid Unique identifier of the pin
         */
        void dropOUT(const char* uid);

        /**
         * @brief <BR>Show a temporary output pin
         * @details Will show an output pin with the given name.
         *          The pin is created the first time showOUT is called and kept alive as long as showOUT is called each frame.
         *          <BR> <BR> In this case the name of the pin will also be its UID.
         *          <BR> <BR> The UID must be unique only in the context of the current node's outputs.
         * @tparam T Type of the data the pin will handle
         * @param name Name of the pin
         * @param behaviour Function or lambda expression used to calculate output value
         * @param filter Connection filter
         * @param style Style of the pin
         */
        template<typename T>
        void showOUT(const std::string& name, std::function<T()> behaviour, std::shared_ptr<PinStyle> style = nullptr);

        /**
         * @brief <BR>Show a temporary output pin
         * @details Will show an output pin with the given name.
         *          The pin is created the first time showOUT_uid is called and kept alive as long as showOUT_uid is called each frame.
         *          <BR> <BR> The UID must be unique only in the context of the current node's outputs.
         * @tparam T Type of the data the pin will handle
         * @tparam U Type of the UID
         * @param uid Unique identifier of the pin
         * @param name Name of the pin
         * @param behaviour Function or lambda expression used to calculate output value
         * @param filter Connection filter
         * @param style Style of the pin
         */
        template<typename T, typename U>
        void showOUT_uid(const U& uid, const std::string& name, std::function<T()> behaviour, std::shared_ptr<PinStyle> style = nullptr);

        /**
         * @brief <BR>Get Input value from an InPin
         * @details Get a reference to the value of an input pin, the value is stored in the output pin at the other end of the link.
         * @tparam T Data type
         * @tparam U Type of the UID
         * @param uid Unique identifier of the pin
         * @return Const reference to the value
         */
        template<typename T, typename U>
        const T& getInVal(const U& uid);

        /**
         * @brief <BR>Get Input value from an InPin
         * @details Get a reference to the value of an input pin, the value is stored in the output pin at the other end of the link.
         * @tparam T Data type
         * @param uid Unique identifier of the pin
         * @return Const reference to the value
         */
        template<typename T>
        const T& getInVal(const char* uid);

        /**
         * @brief <BR>Get generic reference to input pin
         * @tparam U Type of the UID
         * @param uid Unique identifier of the pin
         * @return Generic pointer to the pin
         */
        template<typename U>
        Pin* inPin(const U& uid);

        /**
         * @brief <BR>Get generic reference to input pin
         * @param uid Unique identifier of the pin
         * @return Generic pointer to the pin
         */
        Pin* inPin(const char* uid);

        /**
         * @brief <BR>Get generic reference to output pin
         * @tparam U Type of the UID
         * @param uid Unique identifier of the pin
         * @return Generic pointer to the pin
         */
        template<typename U>
        Pin* outPin(const U& uid);

        /**
         * @brief <BR>Get generic reference to output pin
         * @param uid Unique identifier of the pin
         * @return Generic pointer to the pin
         */
        Pin* outPin(const char* uid);

        /**
         * @brief <BR>Get internal input pins list
         * @return Const reference to node's internal list
         */
        const std::vector<std::shared_ptr<Pin>>& getIns() { return m_ins; }

        /**
         * @brief <BR>Get internal output pins list
         * @return Const reference to node's internal list
         */
        const std::vector<std::shared_ptr<Pin>>& getOuts() { return m_outs; }

        /**
         * @brief <BR>Delete itself
         */
        void destroy() { m_destroyed = true; }

        /*
         * @brief <BR>Get if node must be deleted
         */
        [[nodiscard]] bool toDestroy() const { return m_destroyed; }

        /**
         * @brief <BR>Get hovered status
         * @return [TRUE] if the mouse is hovering the node
         */
        bool isHovered();

        /**
         * @brief <BR>Get node's UID
         * @return Node's unique identifier
         */
        [[nodiscard]] NodeUID getUID() const { return m_uid; }

        /**
         * @brief <BR>Get node name
         * @return Const reference to the node's name
         */
        const std::string& getName() { return m_title; }

        /**
         * @brief <BR>Get node size
         * @return Const reference to the node's size
         */
        const ImVec2& getSize() { return  m_size; }

        /**
         * @brief <BR>Get node position
         * @return Const reference to the node's position
         */
        const ImVec2& getPos() { return  m_pos; }

        /**
         * @brief <BR>Get grid handler bound to node
         * @return Pointer to the handler
         */
        ImNodeFlow* getHandler() { return m_inf; }

        /**
         * @brief <BR>Get node's style
         * @return Shared pointer to the node's style
         */
        const std::shared_ptr<NodeStyle>& getStyle() { return m_style; }

        /**
         * @brief <BR>Get selected status
         * @return [TRUE] if the node is selected
         */
        [[nodiscard]] bool isSelected() const { return m_selected; }

        /**
         * @brief <BR>Get dragged status
         * @return [TRUE] if the node is being dragged
         */
        [[nodiscard]] bool isDragged() const { return m_dragged; }

        /**
         * @brief <BR>Set node's uid
         * @param uid Node's unique identifier
         */
        BaseNode* setUID(NodeUID uid) { m_uid = uid; return this; }

        /**
         * @brief <BR>Set node's name
         * @param name New title
         */
        BaseNode* setTitle(const std::string& title) { m_title = title; return this; }

        /**
         * @brief <BR>Set node's position
         * @param pos Position in grid coordinates
         */
        BaseNode* setPos(const ImVec2& pos) { m_pos = pos; m_posTarget = pos; return this; }

        /**
         * @brief <BR>Set ImNodeFlow handler
         * @param inf Grid handler for the node
         */
        BaseNode* setHandler(ImNodeFlow* inf) { m_inf = inf; return this; }

        /**
         * @brief Set node's style
         * @param style New style
         */
        BaseNode* setStyle(std::shared_ptr<NodeStyle> style) { m_style = std::move(style); return this; }

        /**
         * @brief <BR>Set selected status
         * @param state New selected state
         *
         * Status only updates when updatePublicStatus() is called
         */
        BaseNode* selected(bool state) { m_selectedNext = state; return this; }

        /**
         * @brief <BR>Update the isSelected status of the node
         */
        void updatePublicStatus() { m_selected = m_selectedNext; }
    private:
        NodeUID m_uid = 0;
        std::string m_title;
        ImVec2 m_pos, m_posTarget;
        ImVec2 m_size;
        ImNodeFlow* m_inf = nullptr;
        std::shared_ptr<NodeStyle> m_style;
        bool m_selected = false, m_selectedNext = false;
        bool m_dragged = false;
        bool m_destroyed = false;

        std::vector<std::shared_ptr<Pin>> m_ins;
        std::vector<std::pair<int, std::shared_ptr<Pin>>> m_dynamicIns;
        std::vector<std::shared_ptr<Pin>> m_outs;
        std::vector<std::pair<int, std::shared_ptr<Pin>>> m_dynamicOuts;
    };

    // -----------------------------------------------------------------------------------------------------------------
    // PINS

    /**
     * @brief Pins type identifier
     */
    enum PinType
    {
        PinType_Input,
        PinType_Output
    };

    /**
     * @brief Generic base class for pins
     */
    class Pin
    {
    public:
        /**
         * @brief <BR>Generic pin constructor
         * @param name Name of the pin
         * @param filter Connection filter
         * @param kind Specifies Input or Output
         * @param parent Pointer to the Node containing the pin
         * @param inf Pointer to the Grid Handler the pin is in (same as parent)
         * @param style Style of the pin
         */
        explicit Pin(PinUID uid, std::string name, std::shared_ptr<PinStyle> style, PinType kind, BaseNode* parent, ImNodeFlow** inf)
            :m_uid(uid), m_name(std::move(name)), m_style(std::move(style)), m_type(kind), m_parent(parent), m_inf(inf)
            {
                if(!m_style)
                    m_style = PinStyle::cyan();
            }

        virtual ~Pin() = default;

        /**
         * @brief <BR>Main loop of the pin
         * @details Updates position, hovering and dragging status, and renders the pin. Must be called each frame.
         */
        void update();

        /**
         * @brief <BR>Draw default pin's socket
         */
        void drawSocket();

        /**
         * @brief <BR>Draw default pin's decoration (border, bg, and hover overlay)
         */
        void drawDecoration();

        /**
         * @brief <BR>Used by output pins to calculate their values
         */
        virtual void resolve() {}

        /**
         * @brief <BR>Custom render function to override Pin appearance
         * @param r Function or lambda expression with new ImGui rendering
         */
        Pin* renderer(std::function<void(Pin* p)> r) { m_renderer = std::move(r); return this; }

        /**
         * @brief <BR>Create link between pins
         * @param other Pointer to the other pin
         */
        virtual void createLink(Pin* other) = 0;

        /**
         * @brief <BR>Set the reference to a link
         * @param link Smart pointer to the link
         */
        virtual void setLink(std::shared_ptr<Link>& link) {}

        /**
         * @brief <BR>Delete link reference
         */
        virtual void deleteLink() = 0;

        /**
         * @brief <BR>Get connected status
         * @return [TRUE] if the pin is connected
         */
        virtual bool isConnected() = 0;

        /**
         * @brief <BR>Get pin's link
         * @return Weak_ptr reference to pin's link
         */
        virtual std::weak_ptr<Link> getLink() { return std::weak_ptr<Link>{}; }

        /**
         * @brief <BR>Get pin's UID
         * @return Unique identifier of the pin
         */
        [[nodiscard]] PinUID getUid() const { return m_uid; }

        /**
         * @brief <BR>Get pin's name
         * @return Const reference to pin's name
         */
        const std::string& getName() { return m_name; }

        /**
         * @brief <BR>Get pin's position
         * @return Const reference to pin's position in grid coordinates
         */
        [[nodiscard]] const ImVec2& getPos() { return m_pos; }

        /**
         * @brief <BR>Get pin's hit-box size
         * @return Const reference to pin's hit-box size
         */
        [[nodiscard]] const ImVec2& getSize() { return m_size; }

        /**
         * @brief <BR>Get pin's parent node
         * @return Generic type pointer to pin's parent node. (Node that contains it)
         */
        BaseNode* getParent() { return m_parent; }

        /**
         * @brief <BR>Get pin's type
         * @return The pin type. Either Input or Output
         */
        PinType getType() { return m_type; }

        /**
         * @brief <BR>Get pin's data type (aka: \<T>)
         * @return String containing unique information identifying the data type
         */
        [[nodiscard]] virtual const std::type_info& getDataType() const = 0;

        /**
         * @brief <BR>Get pin's style
         * @return Smart pointer to pin's style
         */
        std::shared_ptr<PinStyle>& getStyle() { return m_style; }

        /**
         * @brief <BR>Get pin's link attachment point (socket)
         * @return Grid coordinates to the attachment point between the link and the pin's socket
         */
        virtual ImVec2 pinPoint() = 0;

        /**
         * @brief <BR>Calculate pin's width pre-rendering
         * @return The with of the pin once it will be rendered
         */
        float calcWidth() { return ImGui::CalcTextSize(m_name.c_str()).x; }

        /**
         * @brief <BR>Set pin's position
         * @param pos Position in screen coordinates
         */
        void setPos(ImVec2 pos) { m_pos = pos; }
    protected:
        PinUID m_uid;
        std::string m_name;
        ImVec2 m_pos = ImVec2(0.f, 0.f);
        ImVec2 m_size = ImVec2(0.f, 0.f);
        PinType m_type;
        BaseNode* m_parent = nullptr;
        ImNodeFlow** m_inf;
        std::shared_ptr<PinStyle> m_style;
        std::function<void(Pin* p)> m_renderer;
    };

    /**
     * @brief Collection of Pin's collection filters
     */
    class ConnectionFilter
    {
    public:
        static std::function<bool(Pin*, Pin*)> None() { return [](Pin* out, Pin* in){ return true; }; }
        static std::function<bool(Pin*, Pin*)> SameType() { return [](Pin* out, Pin* in) { return out->getDataType() == in->getDataType(); }; }
        static std::function<bool(Pin*, Pin*)> Numbers() { return [](Pin* out, Pin* in){ return out->getDataType() == typeid(double) || out->getDataType() == typeid(float) || out->getDataType() == typeid(int); }; }
    };

    /**
     * @brief Input specific pin
     * @details Derived from the generic class Pin. The input pin owns the link pointer.
     * @tparam T Data type handled by the pin
     */
    template<class T> class InPin : public Pin
    {
    public:
        /**
         * @brief <BR>Input pin constructor
         * @param name Name of the pin
         * @param filter Connection filter
         * @param parent Pointer to the Node containing the pin
         * @param defReturn Default return value when the pin is not connected
         * @param inf Pointer to the Grid Handler the pin is in (same as parent)
         * @param style Style of the pin
         */
        explicit InPin(PinUID uid, const std::string& name, T defReturn, std::function<bool(Pin*, Pin*)> filter, std::shared_ptr<PinStyle> style, BaseNode* parent, ImNodeFlow** inf)
            : Pin(uid, name, style, PinType_Input, parent, inf), m_emptyVal(defReturn), m_filter(std::move(filter)) {}

        /**
         * @brief <BR>Create link between pins
         * @param other Pointer to the other pin
         */
        void createLink(Pin* other) override;

        /**
        * @brief <BR>Delete the link connected to the pin
        */
        void deleteLink() override { m_link.reset(); }

        /**
         * @brief Specify if connections from an output on the same node are allowed
         * @param state New state of the flag
         */
        void allowSameNodeConnections(bool state) { m_allowSelfConnection = state; }

        /**
         * @brief <BR>Get connected status
         * @return [TRUE] is pin is connected to a link
         */
        bool isConnected() override { return m_link != nullptr; }

        /**
         * @brief <BR>Get pin's link
         * @return Weak_ptr reference to the link connected to the pin
         */
        std::weak_ptr<Link> getLink() override { return m_link; }

        /**
         * @brief <BR>Get InPin's connection filter
         * @return InPin's connection filter configuration
         */
        [[nodiscard]] const std::function<bool(Pin*, Pin*)>& getFilter() const { return m_filter; }

        /**
         * @brief <BR>Get pin's data type (aka: \<T>)
         * @return String containing unique information identifying the data type
         */
        [[nodiscard]] const std::type_info& getDataType() const override { return typeid(T); };

        /**
         * @brief <BR>Get pin's link attachment point (socket)
         * @return Grid coordinates to the attachment point between the link and the pin's socket
         */
        ImVec2 pinPoint() override { return m_pos + ImVec2(-m_style->extra.socket_padding, m_size.y / 2); }

        /**
         * @brief <BR>Get value carried by the connected link
         * @return Reference to the value of the connected OutPin. Or the default value if not connected
         */
        const T& val();
    private:
        std::shared_ptr<Link> m_link;
        T m_emptyVal;
        std::function<bool(Pin*, Pin*)> m_filter;
        bool m_allowSelfConnection = false;
    };

    /**
     * @brief Output specific pin
     * @details Derived from the generic class Pin. The output pin handles the logic.
     * @tparam T Data type handled by the pin
     */
    template<class T> class OutPin : public Pin
    {
    public:
        /**
         * @brief <BR>Output pin constructor
         * @param name Name of the pin
         * @param filter Connection filter
         * @param parent Pointer to the Node containing the pin
         * @param inf Pointer to the Grid Handler the pin is in (same as parent)
         * @param style Style of the pin
         */
        explicit OutPin(PinUID uid, const std::string& name, std::shared_ptr<PinStyle> style, BaseNode* parent, ImNodeFlow** inf)
            :Pin(uid, name, style, PinType_Output, parent, inf) {}

        /**
         * @brief <BR>When parent gets deleted, remove the links
         */
        ~OutPin() override {
            std::vector<std::weak_ptr<Link>> links = std::move(m_links);
            for (auto &l: links) if (!l.expired()) l.lock()->right()->deleteLink();
        }

        /**
         * @brief <BR>Create link between pins
         * @param other Pointer to the other pin
         */
        void createLink(Pin* other) override;

        /**
         * @brief <BR>Add a connected link to the internal list
         * @param link Pointer to the link
         */
        void setLink(std::shared_ptr<Link>& link) override;

        /**
         * @brief <BR>Delete any expired weak pointers to a (now deleted) link
         */
        void deleteLink() override;

        /**
         * @brief <BR>Get connected status
         * @return [TRUE] is pin is connected to one or more links
         */
        bool isConnected() override { return !m_links.empty(); }

        /**
         * @brief <BR>Get pin's link attachment point (socket)
         * @return Grid coordinates to the attachment point between the link and the pin's socket
         */
        ImVec2 pinPoint() override { return m_pos + ImVec2(m_size.x + m_style->extra.socket_padding, m_size.y / 2); }

        /**
         * @brief <BR>Get output value
         * @return Const reference to the internal value of the pin
         */
        const T& val();

        /**
         * @brief <BR>Set logic to calculate output value
         * @details Used to define the pin behaviour. This is what gets the data from the parent's inputs, and applies the needed logic.
         * @param func Function or lambda expression used to calculate output value
         */
        OutPin<T>* behaviour(std::function<T()> func) { m_behaviour = std::move(func); return this; }

        /**
         * @brief <BR>Get pin's data type (aka: \<T>)
         * @return String containing unique information identifying the data type
         */
        [[nodiscard]] const std::type_info& getDataType() const override { return typeid(T); };
    private:
        std::vector<std::weak_ptr<Link>> m_links;
        std::function<T()> m_behaviour;
        T m_val;
    };
}


namespace ImFlow {
    // -----------------------------------------------------------------------------------------------------------------
    // LINK

    void Link::update() {
        ImVec2 start = m_left->pinPoint();
        ImVec2 end = m_right->pinPoint();
        float thickness = m_left->getStyle()->extra.link_thickness;
        bool mouseClickState = m_inf->getSingleUseClick();

        if (!ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            m_selected = false;

        if (smart_bezier_collider(ImGui::GetMousePos(), start, end, 2.5)) {
            m_hovered = true;
            thickness = m_left->getStyle()->extra.link_hovered_thickness;
            if (mouseClickState) {
                m_inf->consumeSingleUseClick();
                m_selected = true;
            }
        } else { m_hovered = false; }

        if (m_selected)
            smart_bezier(start, end, m_left->getStyle()->extra.outline_color,
                         thickness + m_left->getStyle()->extra.link_selected_outline_thickness);
        smart_bezier(start, end, m_left->getStyle()->color, thickness);

        if (m_selected && ImGui::IsKeyPressed(ImGuiKey_Delete, false))
            m_right->deleteLink();
    }

    Link::~Link() {
        m_left->deleteLink();
    }

    // -----------------------------------------------------------------------------------------------------------------
    // BASE NODE

    bool BaseNode::isHovered() {
        ImVec2 paddingTL = {m_style->padding.x, m_style->padding.y};
        ImVec2 paddingBR = {m_style->padding.z, m_style->padding.w};
        return ImGui::IsMouseHoveringRect(m_inf->grid2screen(m_pos - paddingTL),
                                          m_inf->grid2screen(m_pos + m_size + paddingBR));
    }

    void BaseNode::update() {
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        ImGui::PushID(this);
        bool mouseClickState = m_inf->getSingleUseClick();
        ImVec2 offset = m_inf->grid2screen({0.f, 0.f});
        ImVec2 paddingTL = {m_style->padding.x, m_style->padding.y};
        ImVec2 paddingBR = {m_style->padding.z, m_style->padding.w};

        draw_list->ChannelsSetCurrent(1); // Foreground
        ImGui::SetCursorScreenPos(offset + m_pos);

        ImGui::BeginGroup();

        // Header
        ImGui::BeginGroup();
        ImGui::TextColored(m_style->header_title_color, "%s", m_title.c_str());
        ImGui::Spacing();
        ImGui::EndGroup();
        float headerH = ImGui::GetItemRectSize().y;
        float titleW = ImGui::GetItemRectSize().x;

        // Inputs
        ImGui::BeginGroup();
        for (auto &p: m_ins) {
            p->setPos(ImGui::GetCursorPos());
            p->update();
        }
        for (auto &p: m_dynamicIns) {
            if (p.first == 1) {
                p.second->setPos(ImGui::GetCursorPos());
                p.second->update();
                p.first = 0;
            }
        }
        ImGui::EndGroup();
        ImGui::SameLine();

        // Content
        ImGui::BeginGroup();
        draw();
        ImGui::Dummy(ImVec2(0.f, 0.f));
        ImGui::EndGroup();
        ImGui::SameLine();

        // Outputs
        float maxW = 0.0f;
        for (auto &p: m_outs) {
            float w = p->calcWidth();
            if (w > maxW)
                maxW = w;
        }
        for (auto &p: m_dynamicOuts) {
            float w = p.second->calcWidth();
            if (w > maxW)
                maxW = w;
        }
        ImGui::BeginGroup();
        for (auto &p: m_outs) {
            // FIXME: This looks horrible
            if ((m_pos + ImVec2(titleW, 0) + m_inf->getGrid().scroll()).x <
                ImGui::GetCursorPos().x + ImGui::GetWindowPos().x + maxW)
                p->setPos(ImGui::GetCursorPos() + ImGui::GetWindowPos() + ImVec2(maxW - p->calcWidth(), 0.f));
            else
                p->setPos(ImVec2((m_pos + ImVec2(titleW - p->calcWidth(), 0) + m_inf->getGrid().scroll()).x,
                                 ImGui::GetCursorPos().y + ImGui::GetWindowPos().y));
            p->update();
        }
        for (auto &p: m_dynamicOuts) {
            // FIXME: This looks horrible
            if ((m_pos + ImVec2(titleW, 0) + m_inf->getGrid().scroll()).x <
                ImGui::GetCursorPos().x + ImGui::GetWindowPos().x + maxW)
                p.second->setPos(
                        ImGui::GetCursorPos() + ImGui::GetWindowPos() + ImVec2(maxW - p.second->calcWidth(), 0.f));
            else
                p.second->setPos(
                        ImVec2((m_pos + ImVec2(titleW - p.second->calcWidth(), 0) + m_inf->getGrid().scroll()).x,
                               ImGui::GetCursorPos().y + ImGui::GetWindowPos().y));
            p.second->update();
            p.first -= 1;
        }

        ImGui::EndGroup();

        ImGui::EndGroup();
        m_size = ImGui::GetItemRectSize();
        ImVec2 headerSize = ImVec2(m_size.x + paddingBR.x, headerH);

        // Background
        draw_list->ChannelsSetCurrent(0);
        draw_list->AddRectFilled(offset + m_pos - paddingTL, offset + m_pos + m_size + paddingBR, m_style->bg,
                                 m_style->radius);
        draw_list->AddRectFilled(offset + m_pos - paddingTL, offset + m_pos + headerSize, m_style->header_bg,
                                 m_style->radius, ImDrawFlags_RoundCornersTop);

        ImU32 col = m_style->border_color;
        float thickness = m_style->border_thickness;
        ImVec2 ptl = paddingTL;
        ImVec2 pbr = paddingBR;
        if (m_selected) {
            col = m_style->border_selected_color;
            thickness = m_style->border_selected_thickness;
        }
        if (thickness < 0.f) {
            ptl.x -= thickness / 2;
            ptl.y -= thickness / 2;
            pbr.x -= thickness / 2;
            pbr.y -= thickness / 2;
            thickness *= -1.f;
        }
        draw_list->AddRect(offset + m_pos - ptl, offset + m_pos + m_size + pbr, col, m_style->radius, 0, thickness);


        if (ImGui::IsWindowHovered() && !ImGui::IsKeyDown(ImGuiKey_LeftCtrl) &&
            ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !m_inf->on_selected_node())
            selected(false);

        if (isHovered()) {
            m_inf->hoveredNode(this);
            if (mouseClickState) {
                selected(true);
                m_inf->consumeSingleUseClick();
            }
        }

        if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_Delete) && !ImGui::IsAnyItemActive() && isSelected())
            destroy();

        bool onHeader = ImGui::IsMouseHoveringRect(offset + m_pos - paddingTL, offset + m_pos + headerSize);
        if (onHeader && mouseClickState) {
            m_inf->consumeSingleUseClick();
            m_dragged = true;
            m_inf->draggingNode(true);
        }
        if (m_dragged || (m_selected && m_inf->isNodeDragged())) {
            float step = m_inf->getStyle().grid_size / m_inf->getStyle().grid_subdivisions;
            m_posTarget += ImGui::GetIO().MouseDelta;
            // "Slam" The position
            m_pos.x = round(m_posTarget.x / step) * step;
            m_pos.y = round(m_posTarget.y / step) * step;

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                m_dragged = false;
                m_inf->draggingNode(false);
                m_posTarget = m_pos;
            }
        }
        ImGui::PopID();

        // Deleting dead pins
        m_dynamicIns.erase(std::remove_if(m_dynamicIns.begin(), m_dynamicIns.end(),
                                          [](const std::pair<int, std::shared_ptr<Pin>> &p) { return p.first == 0; }),
                           m_dynamicIns.end());
        m_dynamicOuts.erase(std::remove_if(m_dynamicOuts.begin(), m_dynamicOuts.end(),
                                           [](const std::pair<int, std::shared_ptr<Pin>> &p) { return p.first == 0; }),
                            m_dynamicOuts.end());
    }

    // -----------------------------------------------------------------------------------------------------------------
    // HANDLER

    int ImNodeFlow::m_instances = 0;

    bool ImNodeFlow::on_selected_node() {
        return std::any_of(m_nodes.begin(), m_nodes.end(),
                           [](const auto &n) { return n.second->isSelected() && n.second->isHovered(); });
    }

    bool ImNodeFlow::on_free_space() {
        return std::all_of(m_nodes.begin(), m_nodes.end(),
                           [](const auto &n) { return !n.second->isHovered(); })
               && std::all_of(m_links.begin(), m_links.end(),
                              [](const auto &l) { return !l.lock()->isHovered(); });
    }

    ImVec2 ImNodeFlow::screen2grid(const ImVec2 &p) {
        if (ImGui::GetCurrentContext() == m_context.getRawContext())
            return p - m_context.scroll();
        else
            return p - m_context.origin() - m_context.scroll() * m_context.scale();
    }

    ImVec2 ImNodeFlow::grid2screen(const ImVec2 &p) {
        if (ImGui::GetCurrentContext() == m_context.getRawContext())
            return p + m_context.scroll();
        else
            return p + m_context.origin() + m_context.scroll() * m_context.scale();
    }

    void ImNodeFlow::addLink(std::shared_ptr<Link> &link) {
        m_links.push_back(link);
    }

    void ImNodeFlow::update() {
        // Updating looping stuff
        m_hovering = nullptr;
        m_hoveredNode = nullptr;
        m_draggingNode = m_draggingNodeNext;
        m_singleUseClick = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

        // Create child canvas
        m_context.begin();
        ImGui::GetIO().IniFilename = nullptr;

        ImDrawList *draw_list = ImGui::GetWindowDrawList();

        // Display grid
        ImVec2 gridSize = ImGui::GetWindowSize();
        float subGridStep = m_style.grid_size / m_style.grid_subdivisions;
        for (float x = fmodf(m_context.scroll().x, m_style.grid_size); x < gridSize.x; x += m_style.grid_size)
            draw_list->AddLine(ImVec2(x, 0.0f), ImVec2(x, gridSize.y), m_style.colors.grid);
        for (float y = fmodf(m_context.scroll().y, m_style.grid_size); y < gridSize.y; y += m_style.grid_size)
            draw_list->AddLine(ImVec2(0.0f, y), ImVec2(gridSize.x, y), m_style.colors.grid);
        if (m_context.scale() > 0.7f) {
            for (float x = fmodf(m_context.scroll().x, subGridStep); x < gridSize.x; x += subGridStep)
                draw_list->AddLine(ImVec2(x, 0.0f), ImVec2(x, gridSize.y), m_style.colors.subGrid);
            for (float y = fmodf(m_context.scroll().y, subGridStep); y < gridSize.y; y += subGridStep)
                draw_list->AddLine(ImVec2(0.0f, y), ImVec2(gridSize.x, y), m_style.colors.subGrid);
        }

        // Update and draw nodes
        // TODO: I don't like this
        draw_list->ChannelsSplit(2);
        for (auto &node: m_nodes) { node.second->update(); }
        // Remove "toDelete" nodes
        for (auto iter = m_nodes.begin(); iter != m_nodes.end();) {
            if (iter->second->toDestroy())
                iter = m_nodes.erase(iter);
            else
                ++iter;
        }
        draw_list->ChannelsMerge();
        for (auto &node: m_nodes) { node.second->updatePublicStatus(); }

        // Update and draw links
        for (auto &l: m_links) { if (!l.expired()) l.lock()->update(); }

        // Links drop-off
        if (m_dragOut && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            if (!m_hovering) {
                if (on_free_space() && m_droppedLinkPopUp) {
                    if (m_droppedLinkPupUpComboKey == ImGuiKey_None || ImGui::IsKeyDown(m_droppedLinkPupUpComboKey)) {
                        m_droppedLinkLeft = m_dragOut;
                        ImGui::OpenPopup("DroppedLinkPopUp");
                    }
                }
            } else
                m_dragOut->createLink(m_hovering);
        }

        // Links drag-out
        if (!m_draggingNode && m_hovering && !m_dragOut && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            m_dragOut = m_hovering;
        if (m_dragOut) {
            if (m_dragOut->getType() == PinType_Output)
                smart_bezier(m_dragOut->pinPoint(), ImGui::GetMousePos(), m_dragOut->getStyle()->color,
                             m_dragOut->getStyle()->extra.link_dragged_thickness);
            else
                smart_bezier(ImGui::GetMousePos(), m_dragOut->pinPoint(), m_dragOut->getStyle()->color,
                             m_dragOut->getStyle()->extra.link_dragged_thickness);

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                m_dragOut = nullptr;
        }

        // Right-click PopUp
        if (m_rightClickPopUp && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
            m_hoveredNodeAux = m_hoveredNode;
            ImGui::OpenPopup("RightClickPopUp");
        }
        if (ImGui::BeginPopup("RightClickPopUp")) {
            m_rightClickPopUp(m_hoveredNodeAux);
            ImGui::EndPopup();
        }

        // Dropped Link PopUp
        if (ImGui::BeginPopup("DroppedLinkPopUp")) {
            m_droppedLinkPopUp(m_droppedLinkLeft);
            ImGui::EndPopup();
        }

        // Removing dead Links
        m_links.erase(std::remove_if(m_links.begin(), m_links.end(),
                                     [](const std::weak_ptr<Link> &l) { return l.expired(); }), m_links.end());

        // Clearing recursion blacklist
        m_pinRecursionBlacklist.clear();

        m_context.end();
    }
}


namespace ImFlow
{
    inline void smart_bezier(const ImVec2& p1, const ImVec2& p2, ImU32 color, float thickness)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        float distance = sqrt(pow((p2.x - p1.x), 2.f) + pow((p2.y - p1.y), 2.f));
        float delta = distance * 0.45f;
        if (p2.x < p1.x) delta += 0.2f * (p1.x - p2.x);
        // float vert = (p2.x < p1.x - 20.f) ? 0.062f * distance * (p2.y - p1.y) * 0.005f : 0.f;
        float vert = 0.f;
        ImVec2 p22 = p2 - ImVec2(delta, vert);
        if (p2.x < p1.x - 50.f) delta *= -1.f;
        ImVec2 p11 = p1 + ImVec2(delta, vert);
        dl->AddBezierCubic(p1, p11, p22, p2, color, thickness);
    }

    inline bool smart_bezier_collider(const ImVec2& p, const ImVec2& p1, const ImVec2& p2, float radius)
    {
        float distance = sqrt(pow((p2.x - p1.x), 2.f) + pow((p2.y - p1.y), 2.f));
        float delta = distance * 0.45f;
        if (p2.x < p1.x) delta += 0.2f * (p1.x - p2.x);
        // float vert = (p2.x < p1.x - 20.f) ? 0.062f * distance * (p2.y - p1.y) * 0.005f : 0.f;
        float vert = 0.f;
        ImVec2 p22 = p2 - ImVec2(delta, vert);
        if (p2.x < p1.x - 50.f) delta *= -1.f;
        ImVec2 p11 = p1 + ImVec2(delta, vert);
        return ImProjectOnCubicBezier(p, p1, p11, p22, p2).Distance < radius;
    }

    // -----------------------------------------------------------------------------------------------------------------
    // HANDLER

    template<typename T, typename... Params>
    std::shared_ptr<T> ImNodeFlow::addNode(const ImVec2& pos, Params&&... args)
    {
        static_assert(std::is_base_of<BaseNode, T>::value, "Pushed type is not a subclass of BaseNode!");

        std::shared_ptr<T> n = std::make_shared<T>(std::forward<Params>(args)...);
        n->setPos(pos);
        n->setHandler(this);
        if (!n->getStyle())
            n->setStyle(NodeStyle::cyan());

        auto uid = reinterpret_cast<uintptr_t>(n.get());
        n->setUID(uid);
        m_nodes[uid] = n;
        return n;
    }

    template<typename T, typename... Params>
    std::shared_ptr<T> ImNodeFlow::placeNodeAt(const ImVec2& pos, Params&&... args)
    {
        return addNode<T>(screen2grid(pos), std::forward<Params>(args)...);
    }

    template<typename T, typename... Params>
    std::shared_ptr<T> ImNodeFlow::placeNode(Params&&... args)
    {
        return placeNodeAt<T>(ImGui::GetMousePos(), std::forward<Params>(args)...);
    }

    // -----------------------------------------------------------------------------------------------------------------
    // BASE NODE

    template<typename T>
    std::shared_ptr<InPin<T>> BaseNode::addIN(const std::string& name, T defReturn, std::function<bool(Pin*, Pin*)> filter, std::shared_ptr<PinStyle> style)
    {
        return addIN_uid(name, name, defReturn, std::move(filter), std::move(style));
    }

    template<typename T, typename U>
    std::shared_ptr<InPin<T>> BaseNode::addIN_uid(const U& uid, const std::string& name, T defReturn, std::function<bool(Pin*, Pin*)> filter, std::shared_ptr<PinStyle> style)
    {
        PinUID h = std::hash<U>{}(uid);
        auto p = std::make_shared<InPin<T>>(h, name, defReturn, std::move(filter), std::move(style), this, &m_inf);
        m_ins.emplace_back(p);
        return p;
    }

    template<typename U>
    void BaseNode::dropIN(const U& uid)
    {
        PinUID h = std::hash<U>{}(uid);
        for (auto it = m_ins.begin(); it != m_ins.end(); it++)
        {
            if (it->get()->getUid() == h)
            {
                m_ins.erase(it);
                return;
            }
        }
    }

    inline void BaseNode::dropIN(const char* uid)
    {
        dropIN<std::string>(uid);
    }

    template<typename T>
    const T& BaseNode::showIN(const std::string& name, T defReturn, std::function<bool(Pin*, Pin*)> filter, std::shared_ptr<PinStyle> style)
    {
        return showIN_uid(name, name, defReturn, std::move(filter), std::move(style));
    }

    template<typename T, typename U>
    const T& BaseNode::showIN_uid(const U& uid, const std::string& name, T defReturn, std::function<bool(Pin*, Pin*)> filter, std::shared_ptr<PinStyle> style)
    {
        PinUID h = std::hash<U>{}(uid);
        for (std::pair<int, std::shared_ptr<Pin>>& p : m_dynamicIns)
        {
            if (p.second->getUid() == h)
            {
                p.first = 1;
                return static_cast<InPin<T>*>(p.second.get())->val();
            }
        }

        m_dynamicIns.emplace_back(std::make_pair(1, std::make_shared<InPin<T>>(h, name, defReturn, std::move(filter), std::move(style), this, &m_inf)));
        return static_cast<InPin<T>*>(m_dynamicIns.back().second.get())->val();
    }

    template<typename T>
    std::shared_ptr<OutPin<T>> BaseNode::addOUT(const std::string& name, std::shared_ptr<PinStyle> style)
    {
        return addOUT_uid<T>(name, name, std::move(style));
    }

    template<typename T, typename U>
    std::shared_ptr<OutPin<T>> BaseNode::addOUT_uid(const U& uid, const std::string& name, std::shared_ptr<PinStyle> style)
    {
        PinUID h = std::hash<U>{}(uid);
        auto p = std::make_shared<OutPin<T>>(h, name, std::move(style), this, &m_inf);
        m_outs.emplace_back(p);
        return p;
    }

    template<typename U>
    void BaseNode::dropOUT(const U& uid)
    {
        PinUID h = std::hash<U>{}(uid);
        for (auto it = m_outs.begin(); it != m_outs.end(); it++)
        {
            if (it->get()->getUid() == h)
            {
                m_outs.erase(it);
                return;
            }
        }
    }

    inline void BaseNode::dropOUT(const char* uid)
    {
        dropOUT<std::string>(uid);
    }

    template<typename T>
    void BaseNode::showOUT(const std::string& name, std::function<T()> behaviour, std::shared_ptr<PinStyle> style)
    {
        showOUT_uid<T>(name, name, std::move(behaviour), std::move(style));
    }

    template<typename T, typename U>
    void BaseNode::showOUT_uid(const U& uid, const std::string& name, std::function<T()> behaviour, std::shared_ptr<PinStyle> style)
    {
        PinUID h = std::hash<U>{}(uid);
        for (std::pair<int, std::shared_ptr<Pin>>& p : m_dynamicOuts)
        {
            if (p.second->getUid() == h)
            {
                p.first = 2;
                return;
            }
        }

        m_dynamicOuts.emplace_back(std::make_pair(2, std::make_shared<OutPin<T>>(h, name, std::move(style), this, &m_inf)));
        static_cast<OutPin<T>*>(m_dynamicOuts.back().second.get())->behaviour(std::move(behaviour));
    }

    template<typename T, typename U>
    const T& BaseNode::getInVal(const U& uid)
    {
        PinUID h = std::hash<U>{}(uid);
        auto it = std::find_if(m_ins.begin(), m_ins.end(), [&h](std::shared_ptr<Pin>& p)
                            { return p->getUid() == h; });
        assert(it != m_ins.end() && "Pin UID not found!");
        return static_cast<InPin<T>*>(it->get())->val();
    }

    template<typename T>
    const T& BaseNode::getInVal(const char* uid)
    {
        return getInVal<T, std::string>(uid);
    }

    template<typename U>
    Pin* BaseNode::inPin(const U& uid)
    {
        PinUID h = std::hash<U>{}(uid);
        auto it = std::find_if(m_ins.begin(), m_ins.end(), [&h](std::shared_ptr<Pin>& p)
                            { return p->getUid() == h; });
        assert(it != m_ins.end() && "Pin UID not found!");
        return it->get();
    }

    inline Pin* BaseNode::inPin(const char* uid)
    {
        return inPin<std::string>(uid);
    }

    template<typename U>
    Pin* BaseNode::outPin(const U& uid)
    {
        PinUID h = std::hash<U>{}(uid);
        auto it = std::find_if(m_outs.begin(), m_outs.end(), [&h](std::shared_ptr<Pin>& p)
                            { return p->getUid() == h; });
        assert(it != m_outs.end() && "Pin UID not found!");
        return it->get();
    }

    inline Pin* BaseNode::outPin(const char* uid)
    {
        return outPin<std::string>(uid);
    }

    // -----------------------------------------------------------------------------------------------------------------
    // PIN

    inline void Pin::drawSocket()
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 tl = pinPoint() - ImVec2(m_style->socket_radius, m_style->socket_radius);
        ImVec2 br = pinPoint() + ImVec2(m_style->socket_radius, m_style->socket_radius);

        if (isConnected())
            draw_list->AddCircleFilled(pinPoint(), m_style->socket_connected_radius, m_style->color, m_style->socket_shape);
        else
        {
            if (ImGui::IsItemHovered() || ImGui::IsMouseHoveringRect(tl, br))
                draw_list->AddCircle(pinPoint(), m_style->socket_hovered_radius, m_style->color, m_style->socket_shape, m_style->socket_thickness);
            else
                draw_list->AddCircle(pinPoint(), m_style->socket_radius, m_style->color, m_style->socket_shape, m_style->socket_thickness);
        }

        if (ImGui::IsMouseHoveringRect(tl, br))
            (*m_inf)->hovering(this);
    }

    inline void Pin::drawDecoration()
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        if (ImGui::IsItemHovered())
            draw_list->AddRectFilled(m_pos - m_style->extra.padding, m_pos + m_size + m_style->extra.padding, m_style->extra.bg_hover_color, m_style->extra.bg_radius);
        else
            draw_list->AddRectFilled(m_pos - m_style->extra.padding, m_pos + m_size + m_style->extra.padding, m_style->extra.bg_color, m_style->extra.bg_radius);
        draw_list->AddRect(m_pos - m_style->extra.padding, m_pos + m_size + m_style->extra.padding, m_style->extra.border_color, m_style->extra.bg_radius, 0, m_style->extra.border_thickness);
    }

    inline void Pin::update()
    {
        // Custom rendering
        if (m_renderer)
        {
            ImGui::BeginGroup();
            m_renderer(this);
            ImGui::EndGroup();
            m_size = ImGui::GetItemRectSize();
            if (ImGui::IsItemHovered())
                (*m_inf)->hovering(this);
            return;
        }

        ImGui::SetCursorPos(m_pos);
        ImGui::Text("%s", m_name.c_str());
        m_size = ImGui::GetItemRectSize();

        drawDecoration();
        drawSocket();

        if (ImGui::IsItemHovered())
            (*m_inf)->hovering(this);
    }

    // -----------------------------------------------------------------------------------------------------------------
    // IN PIN

    template<class T>
    const T& InPin<T>::val()
    {
        if(!m_link)
            return m_emptyVal;

        return reinterpret_cast<OutPin<T>*>(m_link->left())->val();
    }

    template<class T>
    void InPin<T>::createLink(Pin *other)
    {
        if (other == this || other->getType() == PinType_Input)
            return;

        if (m_parent == other->getParent() && !m_allowSelfConnection)
            return;

        if (m_link && m_link->left() == other)
        {
            m_link.reset();
            return;
        }

        if (!m_filter(other, this)) // Check Filter
            return;

        m_link = std::make_shared<Link>(other, this, (*m_inf));
        other->setLink(m_link);
        (*m_inf)->addLink(m_link);
    }

    // -----------------------------------------------------------------------------------------------------------------
    // OUT PIN

    template<class T>
    const T &OutPin<T>::val()
    {
        std::string s = std::to_string(m_uid) + std::to_string(m_parent->getUID());
        if (std::find((*m_inf)->get_recursion_blacklist().begin(), (*m_inf)->get_recursion_blacklist().end(), s) == (*m_inf)->get_recursion_blacklist().end())
        {
            (*m_inf)->get_recursion_blacklist().emplace_back(s);
            m_val = m_behaviour();
        }

        return m_val;
    }

    template<class T>
    void OutPin<T>::createLink(ImFlow::Pin *other)
    {
        if (other == this || other->getType() == PinType_Output)
            return;

        other->createLink(this);
    }

    template<class T>
    void OutPin<T>::setLink(std::shared_ptr<Link>& link)
    {
        m_links.emplace_back(link);
    }

    template<class T>
    void OutPin<T>::deleteLink()
    {
        m_links.erase(std::remove_if(m_links.begin(), m_links.end(),
                                     [](const std::weak_ptr<Link>& l) { return l.expired(); }), m_links.end());
    }
}

// demo ---------------------------------------------------------


using namespace ImFlow;

class SimpleSum : public BaseNode
{

public:
    SimpleSum()
    {
        setTitle("Simple sum");
        setStyle(NodeStyle::green());
        BaseNode::addIN<int>("In", 0, ConnectionFilter::SameType());
        BaseNode::addOUT<int>("Out", nullptr)->behaviour([this](){ return getInVal<int>("In") + m_valB; });
    }

    void draw() override
    {
        if(BaseNode::isSelected()) {
          ImGui::SetNextItemWidth(100.f);
          ImGui::InputInt("##ValB", &m_valB);
          ImGui::Button("Hello");
        }
    }

private:
    int m_valB = 0;
};

struct NodeEditor : ImFlow::BaseNode
{
    ImFlow::ImNodeFlow mINF;
    NodeEditor(float d, std::size_t r)
    : BaseNode()
    {
        setTitle("glhf");
        mINF.setSize({d,d});
        if(r > 0) {
          mINF.addNode<SimpleSum>({0,0});
          mINF.addNode<SimpleSum>({10,10});
        }
    }

    void set_size(ImVec2 d)
    {
        mINF.setSize(d);
    }

    void draw() override
    {
        mINF.update();
    }
};

extern "C"
void igNodeFlowDemo() {
    static NodeEditor neditor(500, 1500);

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    const auto window_size = io.DisplaySize - ImVec2(1, 1);
    const auto window_pos = ImVec2(1, 1);
    const auto node_editor_size = window_size - ImVec2(16, 16);
    ImGui::SetNextWindowSize(window_size);
    ImGui::SetNextWindowPos(window_pos);
    ImGui::Begin("Node Editor", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    neditor.set_size(node_editor_size);
    neditor.draw();
    ImGui::End();
}
