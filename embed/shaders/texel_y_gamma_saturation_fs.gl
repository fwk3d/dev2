uniform sampler2D u_texture_y;  /*unit0*/
// uniform float u_gamma; /// set:2.2


in vec2 uv;
out vec4 fragcolor;


void main() {
    float y = texture(u_texture_y, uv).r;
    
    vec4 texel = vec4(y, y, y, 1.0);

    // gamma correction
    // texel.rgb = pow(texel.rgb, vec3(u_gamma));
    
    // saturation (algorithm from Chapter 16 of OpenGL Shading Language)
    if(false) { float saturation = 2.0; const vec3 W = vec3(0.2125, 0.7154, 0.0721);
    vec3 intensity = vec3(dot(texel.rgb, W));
    texel.rgb = mix(intensity, texel.rgb, saturation); }
    fragcolor = vec4(texel.rgb, 1.0);
}