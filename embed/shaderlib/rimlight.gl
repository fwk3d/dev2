#ifndef RIMLIGHT_GLSL
#define RIMLIGHT_GLSL

vec3 rimlight(vec3 range, vec3 color, vec3 pivot, bool ambient) {
    vec3 n = normalize(mat3(M) * v_normal);  // convert normal to view space
    vec3 p = (M * vec4(v_position,1.0)).xyz; // convert position to view space
    vec3 v = vec3(0,-1,0);
    if (!ambient) {
        v = normalize(pivot-p);
    }
    float rim = 1.0 - max(dot(v,n), 0.0);
    vec3 col = color*(pow(smoothstep(1.0-range.x,range.y,rim), range.z));
    return col;
}

#endif