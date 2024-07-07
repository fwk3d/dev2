// camera

#if !CODE

typedef struct camera_t { OBJ
    mat44 view, proj;
    vec3 position, updir, lookdir, rightdir;
    vec3 velocity;
    float yaw, pitch, roll; // mirror of (x,y) lookdir in deg;
    float speed, accel, fov, aspect; // fov in deg(45)
    float near_clip, far_clip;
    float frustum_fov_multiplier;

    float move_friction, move_damping;
    float look_friction, look_damping;
    vec3 last_look; vec3 last_move; // used for friction and damping
    bool damping;

    bool orthographic; // 0 perspective, 1 orthographic; when ortho: dimetric[if pitch == -30º], isometric[if pitch == 35.264º]
    float distance;    // distance to pivot, when orbiting
    // vec2 polarity = { +1,-1 }; // @todo
    // vec2 sensitivity = { 2,2 }; // @todo
} camera_t;

API camera_t camera();
API void camera_teleport(camera_t *cam, vec3 pos);
API void camera_moveby(camera_t *cam, vec3 inc);
API void camera_fov(camera_t *cam, float fov);
API void camera_fps(camera_t *cam, float yaw, float pitch);
API void camera_fps2(camera_t *cam, float yaw, float pitch, float roll);
API void camera_orbit(camera_t *cam, float yaw, float pitch, float inc_distance);
API void camera_lookat(camera_t *cam, vec3 target);
API void camera_enable(camera_t *cam);
API void camera_freefly(camera_t *cam, bool blocked);
API frustum camera_frustum_build(camera_t *cam);
API camera_t *camera_get_active();

API int  ui_camera(camera_t *cam);
API void ddraw_camera(camera_t *cam);

#else

static camera_t *last_camera;

static
void camera_ctor(camera_t *cam) {
    cam->speed = 0.50f;
    cam->accel = 0.5f;
    cam->position = vec3(10,10,10);
    cam->updir = vec3(0,1,0);
    cam->rightdir = vec3(1,0,0);
    cam->fov = 45;
    cam->aspect = 0.0f;
    cam->frustum_fov_multiplier = 1.0f;
    cam->orthographic = false;
    cam->distance = 3; // len3(cam->position);
    cam->near_clip = 0.1f;
    cam->far_clip = 1000.f;

    cam->damping = false;
    cam->move_friction = 0.09f;
    cam->move_damping = 0.96f;
    cam->look_friction = 0.30f;
    cam->look_damping = 0.96f;
    cam->last_look = vec3(0,0,0);
    cam->last_move = vec3(0,0,0);

    // update proj & view
    camera_lookat(cam,vec3(-5,0,-5));

    // @todo: remove this hack that is used to consolidate dampings
    if( 1 ) {
        vec3 zero = {0};
        for( int i = 0; i < 1000; ++i ) {
            camera_moveby(cam, zero);
            camera_fps(cam,0,0);
        }
    }
}

camera_t camera() {
    camera_t *old = last_camera;

    static camera_t cam = {0};
    do_once {
        camera_ctor(&cam);
    }

    if (!last_camera)
        *camera_get_active() = cam;
    last_camera = old;
    return cam;
}

camera_t *camera_get_active() {
    static camera_t defaults = {0};
    if( !last_camera ) {
        identity44(defaults.view);
        identity44(defaults.proj);
        last_camera = &defaults;
    }
    return last_camera;
}

void camera_moveby(camera_t *cam, vec3 inc) {
    // calculate camera damping
    if( cam->damping ) {
        float fr = cam->move_friction; fr *= fr; fr *= fr; fr *= fr;
        float sm = clampf(cam->move_damping, 0, 0.999f); sm *= sm; sm *= sm;

        cam->last_move = scale3(cam->last_move, 1 - fr);
        inc.x = cam->last_move.x = inc.x * (1 - sm) + cam->last_move.x * sm;
        inc.y = cam->last_move.y = inc.y * (1 - sm) + cam->last_move.y * sm;
        inc.z = cam->last_move.z = inc.z * (1 - sm) + cam->last_move.z * sm;
    }

    vec3 dir = norm3(cross3(cam->lookdir, cam->updir));
    vec3 abs_inc = scale3(dir, inc.x);
    abs_inc = add3(abs_inc, scale3(cam->updir, inc.y)); // up
    abs_inc = add3(abs_inc, scale3(cam->lookdir, inc.z)); // front
    cam->position = add3(cam->position, abs_inc);
    cam->velocity = abs_inc;

    camera_fps(cam, 0, 0);
}

void camera_teleport(camera_t *cam, vec3 pos) {
    bool damping = cam->damping;
    cam->damping = 0;
    cam->last_move = vec3(0,0,0);
    cam->position = pos;
    camera_fps(cam, 0, 0);
    cam->damping = damping;
}

void camera_lookat(camera_t *cam, vec3 target) {
    // invert expression that cam->lookdir = norm3(vec3(cos(y) * cos(p), sin(p), sin(y) * cos(p)));
    // look.y = sin p > y = asin(p)
    // look.x = cos y * cos p; -> cos p = look.x / cos y \ look.x / cos y = look.z / sin y
    // look.z = sin y * cos p; -> cos p = look.z / sin y /
    // so, sin y / cos y = look x / look z > tan y = look x / look z > y = atan(look x / look z)

    vec3 look = norm3(sub3(target, cam->position));
    const float rad2deg = 1 / 0.0174532f;
    float pitch = asin(look.y) * rad2deg;
    float yaw = atan2(look.z, look.x) * rad2deg; // coords swapped. it was (look.x, look.z) before. @todo: testme

    camera_fps(cam, yaw-cam->yaw, pitch-cam->pitch);
}

void camera_enable(camera_t *cam) {
    // camera_t *other = camera_get_active(); // init default camera in case there is none
    last_camera = cam;
    // trigger a dummy update -> update matrices
    camera_fps(cam, 0, 0);
}

void camera_freefly(camera_t *cam, bool blocked) {
    // static bool is_active = true;
    // if (input_down(KEY_HOME)) is_active = !is_active;
    // if (!is_active) return;
    bool active = ui_active() || ui_hovered() || blocked/*gizmo_active()*/ ? false : input(MOUSE_L) || input(MOUSE_M) || input(MOUSE_R);
    window_cursor( !active );
    int mult_speed = input(KEY_LSHIFT) || input(KEY_LALT);

#if 0
        // fps camera
        if( input(GAMEPAD_CONNECTED) ) {
            vec2 filtered_lpad = input_filter_deadzone(input2(GAMEPAD_LPAD), 0.15f/*do_gamepad_deadzone*/ + 1e-3 );
            vec2 filtered_rpad = input_filter_deadzone(input2(GAMEPAD_RPAD), 0.15f/*do_gamepad_deadzone*/ + 1e-3 );
            vec2 mouse = scale2(vec2(filtered_rpad.x, filtered_rpad.y), 1.0f);
            vec3 wasdec = scale3(vec3(filtered_lpad.x, input(GAMEPAD_LT) - input(GAMEPAD_RT), filtered_lpad.y), 1.0f);
            camera_moveby(&cam, scale3(wasdec, window_delta() * 60));
            camera_fps(&cam, mouse.x,mouse.y);
            window_cursor( true );
            return;
        }
#endif

    static float speed_buildup = 1.0f;
    if( active ) cam->speed = clampf(cam->speed + input_diff(MOUSE_W) / 10, 0.05f, 5.0f);
    vec2 mouse = scale2(vec2(input_diff(MOUSE_X), -input_diff(MOUSE_Y)), 0.2f * active);
    vec3 wasdecq = scale3(vec3(input(KEY_D)-input(KEY_A),input(KEY_E)-(input(KEY_C)||input(KEY_Q)),input(KEY_W)-input(KEY_S)), cam->speed);
    if ( len3sq(wasdecq) ) speed_buildup += (cam->speed * cam->accel * (2.0f * mult_speed + 1.0f) * window_delta());
    // if (!active) speed_buildup = 1.0f;
    else speed_buildup = 1.0f;
    camera_moveby(cam, scale3(wasdecq, window_delta() * 60 * speed_buildup * (2.0f * mult_speed + 1.0f)));
    camera_fps(cam, mouse.x,mouse.y);
}

frustum camera_frustum_build(camera_t *cam) {
    float aspect = cam->aspect ? cam->aspect : window_width() / ((float)window_height()+!window_height());
    float fov = cam->fov * cam->frustum_fov_multiplier;
    mat44 proj;
    if( cam->orthographic ) {
        ortho44(proj, -fov * aspect, fov * aspect, -fov, fov, cam->near_clip, cam->far_clip);
    } else {
        perspective44(proj, fov, aspect, cam->near_clip, cam->far_clip);
    }
    mat44 projview; multiply44x2(projview, proj, cam->view);
    return frustum_build(projview);
}

void camera_fov(camera_t *cam, float fov) {
    last_camera = cam;

    float aspect = cam->aspect ? cam->aspect : window_width() / ((float)window_height()+!window_height());

    cam->fov = fov;

    if( cam->orthographic ) {
        ortho44(cam->proj, -cam->fov * aspect, cam->fov * aspect, -cam->fov, cam->fov, cam->near_clip, cam->far_clip);
        // [ref] https://commons.wikimedia.org/wiki/File:Isometric_dimetric_camera_views.png
        // float pitch = cam->dimetric ? 30.000f : 35.264f; // dimetric or isometric
        // cam->pitch = -pitch; // quickly reorient towards origin
    } else {
        perspective44(cam->proj, cam->fov, aspect, cam->near_clip, cam->far_clip);
    }
}

void camera_fps2(camera_t *cam, float yaw, float pitch, float roll) {
    last_camera = cam;

    // camera damping
    if( cam->damping ) {
        float fr = cam->look_friction; fr *= fr; fr *= fr; fr *= fr;
        float sm = clampf(cam->look_damping, 0, 0.999f); sm *= sm; sm *= sm;

        cam->last_look = scale3(cam->last_look, 1 - fr);
        yaw = cam->last_look.y = yaw * (1 - sm) + cam->last_look.y * sm;
        pitch = cam->last_look.x = pitch * (1 - sm) + cam->last_look.x * sm;
        roll = cam->last_look.z = roll * (1 - sm) + cam->last_look.z * sm;
    }

    cam->yaw += yaw;
    cam->yaw = fmod(cam->yaw, 360);
    cam->pitch += pitch;
    cam->pitch = cam->pitch > 89 ? 89 : cam->pitch < -89 ? -89 : cam->pitch;
    cam->roll += roll;
    cam->roll += fmod(cam->roll, 360);

    const float deg2rad = 0.0174532f, y = cam->yaw * deg2rad, p = cam->pitch * deg2rad, r = cam->roll * deg2rad;
    cam->lookdir = norm3(vec3(cos(y) * cos(p), sin(p), sin(y) * cos(p)));
    vec3 up = vec3(0,1,0);
    // calculate updir
    {
        float cosfa = cosf(r);
        float sinfa = sinf(r);
        vec3 right = cross3(cam->lookdir, up);
        cam->rightdir = right;
        float th = dot3(cam->lookdir, up);

        cam->updir.x = up.x * cosfa + right.x * sinfa + cam->lookdir.x * th * (1.0f - cosfa);
        cam->updir.y = up.y * cosfa + right.y * sinfa + cam->lookdir.y * th * (1.0f - cosfa);
        cam->updir.z = up.z * cosfa + right.z * sinfa + cam->lookdir.z * th * (1.0f - cosfa);
    }

    lookat44(cam->view, cam->position, add3(cam->position, cam->lookdir), cam->updir); // eye,center,up

    camera_fov(cam, cam->fov);
}

void camera_fps(camera_t *cam, float yaw, float pitch) {
    camera_fps2(cam, yaw, pitch, 0.0f);
}

void camera_orbit( camera_t *cam, float yaw, float pitch, float inc_distance ) {
    last_camera = cam;

    // update dummy state
    camera_fps(cam, 0,0);

    // @todo: add damping
    vec3 _mouse = vec3(yaw, pitch, inc_distance);
    cam->yaw += _mouse.x;
    cam->pitch += _mouse.y;
    cam->distance += _mouse.z;

    // look: limit pitch angle [-89..89]
    cam->pitch = cam->pitch > 89 ? 89 : cam->pitch < -89 ? -89 : cam->pitch;

    // compute view matrix
    float x = rad(cam->yaw), y = rad(-cam->pitch), cx = cosf(x), cy = cosf(y), sx = sinf(x), sy = sinf(y);
    lookat44(cam->view, vec3( cx*cy*cam->distance, sy*cam->distance, sx*cy*cam->distance ), vec3(0,0,0), vec3(0,1,0) );

    // save for next call
    cam->last_move.x = _mouse.x;
    cam->last_move.y = _mouse.y;
}

int ui_camera( camera_t *cam ) {
    int changed = 0;
    changed |= ui_bool("Orthographic", &cam->orthographic);
    changed |= ui_bool("Damping", &cam->damping);
    if( !cam->damping ) ui_enable(0);
    changed |= ui_slider2("Move friction", &cam->move_friction, va("%5.3f", cam->move_friction));
    changed |= ui_slider2("Move damping", &cam->move_damping, va("%5.3f", cam->move_damping));
    changed |= ui_slider2("View friction", &cam->look_friction, va("%5.3f", cam->look_friction));
    changed |= ui_slider2("View damping", &cam->look_damping, va("%5.3f", cam->look_damping));
    if( !cam->damping ) ui_enable(1);
    ui_separator();
    changed |= ui_float("Speed", &cam->speed);
    changed |= ui_float3("Position", cam->position.v3);
    changed |= ui_float("Pitch", &cam->pitch);
    changed |= ui_float("Yaw", &cam->yaw);
    changed |= ui_float("Roll", &cam->roll);
    changed |= ui_float3("LookDir", cam->lookdir.v3);
    changed |= ui_float3("UpDir", cam->updir.v3);
    ui_enable(0);
    changed |= ui_mat44("View matrix", cam->view);
    ui_enable(1);
    ui_separator();
    changed |= ui_float("FOV (degrees)", &cam->fov);
    changed |= ui_float("Orbit distance", &cam->distance);
    ui_enable(0);
    changed |= ui_mat44("Projection matrix", cam->proj);
    ui_enable(1);
    return changed;
}

OBJTYPEDEF(camera_t,OBJTYPE_camera);

AUTORUN {
    STRUCT(camera_t,mat44,view,"View matrix");
    STRUCT(camera_t,mat44,proj,"Projection matrix");
    STRUCT(camera_t,vec3,position,"Camera position");
    STRUCT(camera_t,vec3,updir,"Up direction vector");
    STRUCT(camera_t,vec3,lookdir,"Look direction vector");
    STRUCT(camera_t,vec3,rightdir,"Right direction vector");
    STRUCT(camera_t,float,yaw,"Yaw angle in degrees");
    STRUCT(camera_t,float,pitch,"Pitch angle in degrees");
    STRUCT(camera_t,float,roll,"Roll angle in degrees");
    STRUCT(camera_t,float,speed,"Camera movement speed");
    STRUCT(camera_t,float,accel,"Camera acceleration");
    STRUCT(camera_t,float,fov,"Field of view in degrees");
    STRUCT(camera_t,float,aspect,"Aspect ratio");
    STRUCT(camera_t,float,near_clip,"Near clipping plane distance");
    STRUCT(camera_t,float,far_clip,"Far clipping plane distance");
    STRUCT(camera_t,float,frustum_fov_multiplier,"Frustum FOV multiplier");
    STRUCT(camera_t,float,move_friction,"Movement friction");
    STRUCT(camera_t,float,move_damping,"Movement damping");
    STRUCT(camera_t,float,look_friction,"Look friction");
    STRUCT(camera_t,float,look_damping,"Look damping");
    STRUCT(camera_t,vec3,last_look,"Last look direction for friction and damping");
    STRUCT(camera_t,vec3,last_move,"Last movement for friction and damping");
    STRUCT(camera_t,bool,damping,"Damping enabled flag");
    STRUCT(camera_t,bool,orthographic,"Orthographic projection flag");
    STRUCT(camera_t,float,distance,"Distance to pivot when orbiting");

    obj_ctor[OBJTYPE_camera] = camera_ctor;
}

#endif
