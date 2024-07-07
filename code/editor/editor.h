API bool editor_active();
API bool editor_hovered();

// transform gizmos

API int  gizmo(vec3 *pos, vec3 *rot, vec3 *sca);
API bool gizmo_active();
API bool gizmo_hovered();

#if CODE

vec3 editor_pick() {
    return vec3(0,0,0);
}

bool editor_active() {
    return 0;
}
bool editor_hovered() {
    return 0;
}

static int gizmo__mode;
static int gizmo__active;
static int gizmo__hovered;
bool gizmo_active() {
    return gizmo__active;
}
bool gizmo_hovered() {
    return gizmo__hovered;
}
int gizmo(vec3 *pos, vec3 *rot, vec3 *sca) {
#if 0
    ddraw_flush();
    mat44 copy; copy44(copy, camera_get_active()->view);
    if( 1 ) {
        float *mv = camera_get_active()->view;
        float d = sqrt(mv[4*0+0] * mv[4*0+0] + mv[4*1+1] * mv[4*1+1] + mv[4*2+2] * mv[4*2+2]);
        if(4) mv[4*0+0] = d, mv[4*0+1] = 0, mv[4*0+2] = 0;
        if(2) mv[4*1+0] = 0, mv[4*1+1] = d, mv[4*1+2] = 0;
        if(1) mv[4*2+0] = 0, mv[4*2+1] = 0, mv[4*2+2] = d;
    }
#endif

    ddraw_color_push(dd_color);
    ddraw_ontop_push(1);

    int enabled = !ui_active() && !ui_hovered();
    vec3 mouse = enabled ? vec3(input(MOUSE_X),input(MOUSE_Y),input_down(MOUSE_L)) : vec3(0,0,0); // x,y,l
    vec3 from = camera_get_active()->position;
    vec3 to = editor_pick(mouse.x, mouse.y);
    ray r = ray(from, to);

    static vec3 src3, hit3, off3; static vec2 src2;
    #define on_gizmo_dragged(X,Y,Z,COLOR,DRAWCMD, ...) do { \
        vec3 dir = vec3(X,Y,Z); \
        line axis = {add3(*pos, scale3(dir,100)), add3(*pos, scale3(dir,-100))}; \
        plane ground = { vec3(0,0,0), vec3(Y?1:0,Y?0:1,0) }; \
        vec3 unit = vec3(X+(1.0-X)*0.3,Y+(1.0-Y)*0.3,Z+(1.0-Z)*0.3); \
        aabb arrow = { sub3(*pos,unit), add3(*pos,unit) }; \
        hit *hit_arrow = ray_hit_aabb(r, arrow), *hit_ground = ray_hit_plane(r, ground); \
        ddraw_color( hit_arrow || gizmo__active == (X*4+Y*2+Z) ? gizmo__hovered = 1, YELLOW : COLOR ); \
        DRAWCMD; \
        if( !gizmo__active && hit_arrow && mouse.z ) src2 = vec2(mouse.x,mouse.y), src3 = *pos, hit3 = hit_ground->p, off3 = mul3(sub3(src3,hit3),vec3(X,Y,Z)), gizmo__active = X*4+Y*2+Z; \
        if( (gizmo__active && gizmo__active==(X*4+Y*2+Z)) || (!gizmo__active && hit_arrow) ) { ddraw_color( COLOR ); ( 1 ? ddraw_line : ddraw_line_dashed)(axis.a, axis.b); } \
        if( gizmo__active == (X*4+Y*2+Z) && hit_ground ) {{ __VA_ARGS__ }; modified = 1; gizmo__active *= !!input(MOUSE_L); } \
    } while(0)
    #define gizmo_translate(X,Y,Z,COLOR) \
        on_gizmo_dragged(X,Y,Z,COLOR, ddraw_arrow(*pos,add3(*pos,vec3(X,Y,Z))), { \
            *pos = add3(line_closest_point(axis, hit_ground->p), off3); \
        } )
    #define gizmo_scale(X,Y,Z,COLOR) \
        on_gizmo_dragged(X,Y,Z,COLOR, (ddraw_line(*pos,add3(*pos,vec3(X,Y,Z))),ddraw_sphere(add3(*pos,vec3(X-0.1*X,Y-0.1*Y,Z-0.1*Z)),0.1)), { /*ddraw_aabb(arrow.min,arrow.max)*/ \
            int component = (X*1+Y*2+Z*3)-1; \
            float mag = len2(sub2(vec2(mouse.x, mouse.y), src2)); \
            float magx = (mouse.x - src2.x) * (mouse.x - src2.x); \
            float magy = (mouse.y - src2.y) * (mouse.y - src2.y); \
            float sgn = (magx > magy ? mouse.x > src2.x : mouse.y > src2.y) ? 1 : -1; \
            sca->v3[component] -= sgn * mag * 0.01; \
            src2 = vec2(mouse.x, mouse.y); \
        } )
    #define gizmo_rotate(X,Y,Z,COLOR) do { \
            vec3 dir = vec3(X,Y,Z); \
            line axis = {add3(*pos, scale3(dir,100)), add3(*pos, scale3(dir,-100))}; \
            plane ground = { vec3(0,0,0), vec3(0,1,0) }; \
                vec3 unit = vec3(X+(1.0-X)*0.3,Y+(1.0-Y)*0.3,Z+(1.0-Z)*0.3); \
                aabb arrow = { sub3(*pos,unit), add3(*pos,unit) }; \
                hit *hit_arrow = ray_hit_aabb(r, arrow), *hit_ground = ray_hit_plane(r, ground); \
                int hover = (hit_arrow ? (X*4+Y*2+Z) : 0); \
            if( gizmo__active == (X*4+Y*2+Z) ) { ddraw_color(gizmo__active ? gizmo__hovered = 1, YELLOW : WHITE); ddraw_circle(*pos, vec3(X,Y,Z), 1); } \
            else if( !gizmo__active && hover == (X*4+Y*2+Z) ) { gizmo__hovered = 1; ddraw_color(COLOR); ddraw_circle(*pos, vec3(X,Y,Z), 1); } \
            else if( !gizmo__active ) { ddraw_color(WHITE); ddraw_circle(*pos, vec3(X,Y,Z), 1); } \
            if( !gizmo__active && hit_arrow && mouse.z ) src2 = vec2(mouse.x,mouse.y), gizmo__active = hover; \
            if( (!gizmo__active && hover == (X*4+Y*2+Z)) || gizmo__active == (X*4+Y*2+Z) ) { gizmo__hovered = 1; ddraw_color( COLOR ); ( 1 ? ddraw_line_thin : ddraw_line_dashed)(axis.a, axis.b); } \
            if( gizmo__active && gizmo__active == (X*4+Y*2+Z) && hit_ground && enabled ) { \
                int component = (Y*1+X*2+Z*3)-1; /*pitch,yaw,roll*/ \
                float mag = len2(sub2(vec2(mouse.x, mouse.y), src2)); \
                float magx = (mouse.x - src2.x) * (mouse.x - src2.x); \
                float magy = (mouse.y - src2.y) * (mouse.y - src2.y); \
                float sgn = (magx > magy ? mouse.x > src2.x : mouse.y > src2.y) ? 1 : -1; \
                rot->v3[component] += sgn * mag; \
                /*rot->v3[component] = clampf(rot->v3[component], -360, +360);*/ \
                src2 = vec2(mouse.x, mouse.y); \
                \
            } \
            gizmo__active *= enabled && !!input(MOUSE_L); \
        } while(0)

    gizmo__hovered = 0;

    int modified = 0;
    if(enabled && input_down(KEY_SPACE)) gizmo__active = 0, gizmo__mode = (gizmo__mode + 1) % 3;
    if(gizmo__mode == 0) gizmo_translate(1,0,0, RED);
    if(gizmo__mode == 0) gizmo_translate(0,1,0, GREEN);
    if(gizmo__mode == 0) gizmo_translate(0,0,1, BLUE);
    if(gizmo__mode == 1) gizmo_scale(1,0,0, RED);
    if(gizmo__mode == 1) gizmo_scale(0,1,0, GREEN);
    if(gizmo__mode == 1) gizmo_scale(0,0,1, BLUE);
    if(gizmo__mode == 2) gizmo_rotate(1,0,0, RED);
    if(gizmo__mode == 2) gizmo_rotate(0,1,0, GREEN);
    if(gizmo__mode == 2) gizmo_rotate(0,0,1, BLUE);

#if 0
    ddraw_flush();
    copy44(camera_get_active()->view, copy);
#endif

    ddraw_ontop_pop();
    ddraw_color_pop();

    return modified;
}

#endif
