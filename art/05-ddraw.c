#include "engine.h"

void collide_demo(); // debug draw collisions

int main() {
    // create window
    app_create(0.75, APP_MSAA8);

    // create camera
    camera_t cam = camera();

    while( app_swap() ) {
        // fps camera
        camera_freefly(&cam, 0);

        // world
        ddraw_grid(0);

        // demos
        ddraw_demo();
        collide_demo();
    }
}

void collide_demo() { // debug draw collisions

    // animation
    static float dx = 0, dy = 0;
    float delta = (0.25f / 60.f);
    dx = dx + delta * 2.0f;
    dy = dy + delta * 0.8f;

#if 0
    // 3D
    glEnable(GL_DEPTH_TEST);

    // grid
    ddraw_grid(0);
#endif

    {
        // Triangle-Ray Intersection*/
        vec3 ro, rd;
        int suc;

        triangle tri = { vec3(-9,1,28), vec3(-10,0,28), vec3(-11,1,28) };

        // ray
        ro = vec3(-10,-1,20);
        rd = vec3(-10+0.4f*sin(dx), 2.0f*cos(dy), 29.81023f);
        rd = sub3(rd, ro);
        rd = norm3(rd);

        ray r = ray(ro, rd);
        hit *hit = ray_hit_triangle(r, tri);
        if (hit) {
            // point of intersection
            ddraw_color(RED);
            ddraw_box(hit->p, vec3(0.10f, 0.10f, 0.10f));

            // intersection normal
            ddraw_color(BLUE);
            vec3 v = add3(hit->p, hit->n);
            ddraw_arrow(hit->p, v);
        }

        // line
        ddraw_color(RED);
        rd = scale3(rd,10);
        rd = add3(ro,rd);
        ddraw_line(ro, rd);

        // triangle
        if (hit) ddraw_color(RED);
        else ddraw_color(WHITE);
        ddraw_triangle(tri.p0,tri.p1,tri.p2);
    }
    {
        // Plane-Ray Intersection*/
        vec3 ro, rd;
        mat33 rot;

        // ray
        static float d = 0;
        d += delta * 2.0f;
        ro = vec3(0,-1,20);
        rd = vec3(0.1f, 0.5f, 9.81023f);
        rd = sub3(rd, ro);
        rd = norm3(rd);

        // rotation
        rotation33(rot, deg(d), 0,1,0);
        rd = mulv33(rot, rd);

        // intersection
        ray r = ray(ro, rd);
        plane pl = plane(vec3(0,0,28), vec3(0,0,1));
        hit *hit = ray_hit_plane(r, pl);
        if (hit) {
            // point of intersection
            ddraw_color(RED);
            ddraw_box(hit->p, vec3(0.10f, 0.10f, 0.10f));

            // intersection normal
            ddraw_color(BLUE);
            vec3 v = add3(hit->p, hit->n);
            ddraw_arrow(hit->p, v);
            ddraw_color(RED);
        }
        // line
        ddraw_color(RED);
        rd = scale3(rd,9);
        rd = add3(ro,rd);
        ddraw_line(ro, rd);

        // plane
        if (hit) ddraw_color(RED);
        else ddraw_color(WHITE);
        ddraw_plane(vec3(0,0,28), vec3(0,0,1), 3.0f);
    }
    {
        // Sphere-Ray Intersection*/
        vec3 ro, rd;
        sphere s;

        // ray
        ro = vec3(0,-1,0);
        rd = vec3(0.4f*sin(dx), 2.0f*cos(dy), 9.81023f);
        rd = sub3(rd, ro);
        rd = norm3(rd);

        ray r = ray(ro, rd);
        s = sphere(vec3(0,0,8), 1);
        hit *hit = ray_hit_sphere(r, s);
        if(hit) {
            // points of intersection
            vec3 in = add3(ro,scale3(rd,hit->t0));

            ddraw_color(GREEN);
            ddraw_box(in, vec3(0.05f, 0.05f, 0.05f));

            in = add3(ro,scale3(rd,hit->t1));

            ddraw_color(YELLOW);
            ddraw_box(in, vec3(0.05f, 0.05f, 0.05f));

            // intersection normal
            ddraw_color(BLUE);
            vec3 v = add3(hit->p, hit->n);
            ddraw_arrow(hit->p, v);
            ddraw_color(RED);
        }
        // line
        ddraw_color(RED);
        rd = scale3(rd,10);
        rd = add3(ro,rd);
        ddraw_line(ro, rd);

        // sphere
        if (hit) ddraw_color(RED);
        else ddraw_color(WHITE);
        ddraw_sphere(vec3(0,0,8), 1);
    }
    {   // ray-aabb
        aabb bounds = aabb(vec3(10-0.5f,-0.5f,7.5f), vec3(10.5f,0.5f,8.5f));

        vec3 ro = vec3(10,-1,0);
        vec3 rd = vec3(10+0.4f*sin(dx), 2.0f*cos(dy), 9.81023f);
        rd = norm3(sub3(rd, ro));
        ray r = ray(ro, rd);

        hit *hit = ray_hit_aabb(r, bounds);
        if(hit) {
            // points of intersection
            vec3 in;
            in = scale3(rd,hit->t0);
            in = add3(ro,in);

            ddraw_color(RED);
            ddraw_box(in, vec3(0.05f, 0.05f, 0.05f));

            in = scale3(rd,hit->t1);
            in = add3(ro,in);

            ddraw_color(RED);
            ddraw_box(in, vec3(0.05f, 0.05f, 0.05f));

            // intersection normal
            ddraw_color(BLUE);
            vec3 v = add3(hit->p, hit->n);
            ddraw_arrow(hit->p, v);
            ddraw_color(RED);
        } else ddraw_color(WHITE);
        ddraw_box(vec3(10,0,8), vec3(1,1,1));

        // line
        ddraw_color(RED);
        rd = scale3(rd,10);
        rd = add3(ro,rd);
        ddraw_line(ro, rd);
    }
    {
        // Sphere-Sphere intersection*/
        sphere a = sphere(vec3(-10,0,8), 1);
        sphere b = sphere(vec3(-10+0.6f*sin(dx), 3.0f*cos(dy),8), 1);
        hit *m = sphere_hit_sphere(a, b);
        if (m) {
            vec3 v;
            ddraw_color(BLUE);
            ddraw_box(m->contact_point, vec3(0.05f, 0.05f, 0.05f));
            v = add3(m->contact_point, m->normal);
            ddraw_arrow(m->contact_point, v);
            ddraw_color(RED);
        } else ddraw_color(WHITE);

        ddraw_sphere(a.c, 1);
        ddraw_sphere(b.c, 1);
    }
    {
        // AABB-AABB intersection*/
        const float x = 10+0.6f*sin(dx);
        const float y = 3.0f*cos(dy);
        const float z = 20.0f;

        aabb a = aabb(vec3(10-0.5f,-0.5f,20-0.5f), vec3(10+0.5f,0.5f,20.5f));
        aabb b = aabb(vec3(x-0.5f,y-0.5f,z-0.5f), vec3(x+0.5f,y+0.5f,z+0.5f));
        hit *m = aabb_hit_aabb(a, b);
        if(m) {
            vec3 v;
            ddraw_color(BLUE);
            ddraw_box(m->contact_point, vec3(0.05f, 0.05f, 0.05f));
            v = add3(m->contact_point, m->normal);
            ddraw_arrow(m->contact_point, v);
            ddraw_color(RED);
        } else ddraw_color(WHITE);

        ddraw_box(vec3(10,0,20), vec3(1,1,1));
        ddraw_box(vec3(x,y,z), vec3(1,1,1));
    }
    {
        // Capsule-Capsule intersection*/
        const float x = 20+0.4f*sin(dx);
        const float y = 3.0f*cos(dy);
        const float z = 28.5f;

        capsule a = capsule(vec3(20.0f,-1.0f,28.0f), vec3(20.0f,1.0f,28.0f), 0.2f);
        capsule b = capsule(vec3(x,y-1.0f,z), vec3(x,y+1.0f,z-1.0f), 0.2f);
        hit *m = capsule_hit_capsule(a, b);
        if( m ) {
            vec3 v;
            ddraw_color(BLUE);
            ddraw_box(m->contact_point, vec3(0.05f, 0.05f, 0.05f));
            v = add3(m->contact_point, m->normal);
            ddraw_arrow(m->contact_point, v);
            ddraw_color(RED);
        } else ddraw_color(WHITE);
        ddraw_capsule(vec3(x,y-1.0f,z), vec3(x,y+1.0f,z-1.0f), 0.2f);
        ddraw_capsule(vec3(20.0f,-1.0f,28.0f), vec3(20.0f,1.0f,28.0f), 0.2f);
    }
    {
        // AABB-Sphere intersection*/
        aabb a = aabb(vec3(20-0.5f,-0.5f,7.5f), vec3(20.5f,0.5f,8.5f));
        sphere s = sphere(vec3(20+0.6f*sin(dx), 3.0f*cos(dy),8), 1);
        hit *m  = aabb_hit_sphere(a, s);
        if(m) {
            vec3 v;
            ddraw_color(BLUE);
            ddraw_box(m->contact_point, vec3(0.05f, 0.05f, 0.05f));
            v = add3(m->contact_point, m->normal);
            ddraw_arrow(m->contact_point, v);
            ddraw_color(RED);
        } else ddraw_color(WHITE);

        ddraw_box(vec3(20,0,8), vec3(1,1,1));
        ddraw_sphere(s.c, 1);
    }
    {
        // Sphere-AABB intersection*/
        const float x = 10+0.6f*sin(dx);
        const float y = 3.0f*cos(dy);
        const float z = -8.0f;

        sphere s = sphere(vec3(10,0,-8), 1);
        aabb a = aabb(vec3(x-0.5f,y-0.5f,z-0.5f), vec3(x+0.5f,y+0.5f,z+0.5f));
        hit *m = sphere_hit_aabb(s, a);
        if(m) {
            vec3 v;
            ddraw_color(BLUE);
            ddraw_box(m->contact_point, vec3(0.05f, 0.05f, 0.05f));
            v = add3(m->contact_point, m->normal);
            ddraw_arrow(m->contact_point, v);
            ddraw_color(RED);
        } else ddraw_color(WHITE);

        ddraw_box(vec3(x,y,z), vec3(1,1,1));
        ddraw_sphere(s.c, 1);
    }
    {
        // Capsule-Sphere intersection*/
        capsule c = capsule(vec3(-20.5f,-1.0f,7.5f), vec3(-20+0.5f,1.0f,8.5f), 0.2f);
        sphere b = sphere(vec3(-20+0.6f*sin(dx), 3.0f*cos(dy),8), 1);
        hit *m = capsule_hit_sphere(c, b);
        if(m) {
            vec3 v;
            ddraw_color(BLUE);
            ddraw_box(m->contact_point, vec3(0.05f, 0.05f, 0.05f));
            v = add3(m->contact_point, m->normal);
            ddraw_arrow(m->contact_point, v);
            ddraw_color(RED);
        } else ddraw_color(WHITE);
        ddraw_sphere(b.c, 1);
        ddraw_capsule(vec3(-20.5f,-1.0f,7.5f), vec3(-20+0.5f,1.0f,8.5f), 0.2f);
    }
    {
        // Sphere-Capsule intersection*/
        const float x = 20+0.4f*sin(dx);
        const float y = 3.0f*cos(dy);
        const float z = -8;

        sphere s = sphere(vec3(20,0,-8), 1);
        capsule c = capsule(vec3(x,y-1.0f,z), vec3(x,y+1.0f,z-1.0f), 0.2f);
        hit *m = sphere_hit_capsule(s, c);
        if(m) {
            vec3 v;
            ddraw_color(BLUE);
            ddraw_box(m->contact_point, vec3(0.05f, 0.05f, 0.05f));
            v = add3(m->contact_point, m->normal);
            ddraw_arrow(m->contact_point, v);
            ddraw_color(RED);
        } else ddraw_color(WHITE);

        ddraw_capsule(vec3(x,y-1.0f,z), vec3(x,y+1.0f,z-1.0f), 0.2f);
        ddraw_sphere(s.c, 1);
    }
    {
        // Capsule-AABB intersection*/
        const float x = -20+0.6f*sin(dx);
        const float y = 3.0f*cos(dy);
        const float z = 28.0f;

        capsule c = capsule(vec3(-20.5f,-1.0f,27.5f), vec3(-20+0.5f,1.0f,28.5f), 0.2f);
        aabb b = aabb(vec3(x-0.5f,y-0.5f,z-0.5f), vec3(x+0.5f,y+0.5f,z+0.5f));
        hit *m = capsule_hit_aabb(c, b);
        if(m) {
            vec3 v;
            ddraw_color(BLUE);
            ddraw_box(m->contact_point, vec3(0.05f, 0.05f, 0.05f));
            v = add3(m->contact_point, m->normal);
            ddraw_arrow(m->contact_point, v);
            ddraw_color(RED);
        } else ddraw_color(WHITE);
        ddraw_box(vec3(x,y,z), vec3(1,1,1));
        ddraw_capsule(vec3(-20.5f,-1.0f,27.5f), vec3(-20+0.5f,1.0f,28.5f), 0.2f);
    }
    {
        // AABB-Capsule intersection*/
        const float x = 0.4f*sin(dx);
        const float y = 3.0f*cos(dy);
        const float z = -8;

        aabb a = aabb(vec3(-0.5f,-0.5f,-8.5f), vec3(0.5f,0.5f,-7.5f));
        capsule c = capsule(vec3(x,y-1.0f,z), vec3(x,y+1.0f,z-1.0f), 0.2f);
        hit *m = aabb_hit_capsule(a, c);
        if(m) {
            ddraw_color(RED);
            ddraw_box(m->contact_point, vec3(0.05f, 0.05f, 0.05f));
            ddraw_arrow(m->contact_point, add3(m->contact_point, m->normal));
        } else ddraw_color(WHITE);

        ddraw_capsule(vec3(x,y-1.0f,z), vec3(x,y+1.0f,z-1.0f), 0.2f);
        ddraw_box(vec3(0,0,-8.0f), vec3(1,1,1));
    }
}

#if 0

// ddraw demo: fps camera, renderdd, collide, math, ui, fx, boids
// - rlyeh, public domain.
//
// Compile with:
//    `make     demos\02-ddraw.c` (windows)
// `sh MAKE.bat demos/02-ddraw.c` (linux, osx)

#include "v4k.h"

int main() {
    bool do_boids_demo = 1;
    bool do_colliders_demo = 1;
    bool do_debugdraw_demo = 1;

    // 75% size, MSAAx2
    app_create(75.0, APP_MSAA2);
    app_title(__FILE__);

    // camera that points to origin
    camera_t cam = camera();
    // load skybox folder (no flags)
    skybox_t sky = skybox("cubemaps/stardust", 0);
    // load all postfx files in all subdirs
    fx_load("fx**.fs");

    // main loop
    while( app_swap() ) {

        // input handler
        if (input_down(KEY_F11) ) app_fullscreen( app_has_fullscreen()^1 );
        if (input_down(KEY_ESC) ) break;

        // fps camera
        profile("FPS camera") {
            if( input(GAMEPAD_CONNECTED) ) {
                vec2 filtered_lpad = input_filter_deadzone(input2(GAMEPAD_LPAD), 0.15f/*do_gamepad_deadzone*/ + 1e-3 );
                vec2 filtered_rpad = input_filter_deadzone(input2(GAMEPAD_RPAD), 0.15f/*do_gamepad_deadzone*/ + 1e-3 );
                vec2 mouse = scale2(vec2(filtered_rpad.x, filtered_rpad.y), 1.0f);
                vec3 wasdec = scale3(vec3(filtered_lpad.x, input(GAMEPAD_LT) - input(GAMEPAD_RT), filtered_lpad.y), 1.0f);
                camera_moveby(&cam, scale3(wasdec, app_delta() * 60));
                camera_fps(&cam, mouse.x,mouse.y);
                app_cursor( true );
            } else {
                bool active = ui_active() || ui_hover() || gizmo_active() ? false : input(MOUSE_L) || input(MOUSE_M) || input(MOUSE_R);
                if( active ) cam.speed = clampf(cam.speed + input_diff(MOUSE_W) / 10, 0.05f, 5.0f);
                vec2 mouse = scale2(vec2(input_diff(MOUSE_X), -input_diff(MOUSE_Y)), 0.2f * active);
                vec3 wasdecq = scale3(vec3(input(KEY_D)-input(KEY_A),input(KEY_E)-(input(KEY_C)||input(KEY_Q)),input(KEY_W)-input(KEY_S)), cam.speed);
                camera_moveby(&cam, scale3(wasdecq, app_delta() * 60));
                camera_fps(&cam, mouse.x,mouse.y);
                app_cursor( !active );
            }
        }

        // draw skybox
        skybox_render(&sky, cam.proj, cam.view);

        // boids
        static swarm_t sw;
        if( do_boids_demo ) profile("boids") {
            do_once sw = swarm();
            do_once array_push(sw.steering_targets, vec3(0,0,0));
            do_once for(int i = 0; i < 100; ++i)
                array_push(sw.boids, boid(scale3(rnd3(),10), scale3(rnd3(),.10))); // pos,vel

            // move
            sw.steering_targets[0] = cam.position;
            swarm_update(&sw, app_delta()/60);

            // draw
            for (int j = 0, end = array_count(sw.boids); j < end; ++j) {
                vec3 dir = norm3(sub3(sw.boids[j].position, sw.boids[j].prev_position));
                ddraw_boid(sw.boids[j].position, dir);
            }
        }

        // showcase many debugdraw shapes
        if( do_debugdraw_demo ) {
            ddraw_demo();
        }

        // showcase many colliding tests
        if( do_colliders_demo ) {
            collide_demo();
        }

        // ui
        if( ui_panel("App", 0) ) {
            ui_bool("Boids demo", &do_boids_demo);
            ui_bool("Collide demo", &do_colliders_demo);
            ui_bool("DebugDraw demo", &do_debugdraw_demo);
            ui_panel_end();
        }
        if( ui_panel("Swarm", 0) ) {
            ui_swarm(&sw);
            ui_panel_end();
        }
    }
}

// this demo supersedes following old sources:
// https://github.com/r-lyeh/FWK/blob/45e34d7890b2b8fe1f4994f4b76e496280d83cb6/demos/00-collide.c

#endif
