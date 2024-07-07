// -----------------------------------------------------------------------------
// profiler & stats (@fixme: threadsafe)

#if !CODE

#if !ENABLE_PROFILER
#   define profile(section)             for(int macro(i) = 1; macro(i); macro(i) = 0)
#   define profile_incstat(name, accum) do {} while(0)
#   define profile_setstat(name, value) do {} while(0)
#   define profiler_init()              do {} while(0)
#   define profiler_enable(x)           0
#   define ui_profiler()                do {} while(0)
#else
#   define profile(section) for( \
        struct profile_t *found = profiler_enabled ? \
            map_find_or_add(profiler, section "@" FILELINE, (struct profile_t){NAN} ) : NULL, \
        *doit = found + ( found ? found->cost = -time_us(), 1 : 1 ); doit; \
        doit = found ? found->cost += time_us(), found->avg = found->cost * 0.25 + found->avg * 0.75, NULL : NULL)  ///+
#   define profile_incstat(name, accum) for( \
        struct profile_t *found = profiler_enabled ? map_find_or_add(profiler, name, (struct profile_t){0}) : NULL; \
        found; found->stat += accum, found = NULL) ///+
#   define profile_setstat(name, value) for( \
        struct profile_t *found = profiler_enabled ? map_find_or_add(profiler, name, (struct profile_t){0}) : NULL; \
        found; found->stat = value, found = NULL) ///+
API int profiler_enable(bool on);

struct profile_t { double stat; int32_t cost, avg; }; ///-
typedef map(char *, struct profile_t) profiler_t; ///-
extern API profiler_t profiler; ///-
extern API int profiler_enabled; ///-
#endif

#else

#if ENABLE_PROFILER
profiler_t profiler;
int profiler_enabled = 1;

void (profiler_init)() { map_init(profiler, less_str, hash_str); profiler_enabled &= !!profiler; }
int  (profiler_enable)(bool on) { return profiler_enabled = on; }
void (ui_profiler)() {
    // @todo: ui_plot()

    static float t = 0;
    t += window_delta();

    double fps = window_fps(), dt = window_delta() * 1000;
    profile_setstat("Render.frames_per_second", fps);
    profile_setstat("Render.delta_time (ms)", dt);

    // draw fps-meter: 300 samples, [0..70] range each, 70px height plot ...
    // ... unless filtering is enabled
    if(1) { // if( !(ui_filter && ui_filter[0]) ) {
        static array(vec2) channel[2] = {0}; static int offset[2] = {0}; // scrolling buffer

        #define channel_push(arr, Offset, v2, MaxSize) do { \
            if (array_count(arr) < MaxSize) array_push(arr, v2); \
            else arr[Offset] = v2, Offset = (Offset + 1) % MaxSize; \
        } while(0)

        // channel 0
        channel_push(channel[0], offset[0], vec2(t, fps), 2000);
        // channel 1
        channel_push(channel[1], offset[1], vec2(t, dt), 2000);

        static float history = 10.0f;
        igSliderFloat("History",&history,1,30,"%.1f s",0);

        const ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;
        if (ImPlot_BeginPlot("##Scrolling", ImVec2(-1,150), 0)) {
            ImPlot_SetupAxes(NULL, NULL, flags, flags);
            ImPlot_SetupAxisLimits(ImAxis_X1,t - history, t, ImGuiCond_Always);
            ImPlot_SetupAxisLimits(ImAxis_Y1,0,60,ImPlotCond_Once);
            ImPlot_PlotLine_FloatPtrFloatPtr("Frames Per Second", &channel[0][0].x, &channel[0][0].y, array_count(channel[0]), 0, offset[0], 2 * sizeof(float));
            ImPlot_PlotLine_FloatPtrFloatPtr("Frame (ms)", &channel[1][0].x, &channel[1][0].y, array_count(channel[1]), 0, offset[1], 2 * sizeof(float));
            ImPlot_EndPlot();
        }
    }


    for each_map_ptr_sorted(profiler, const char *, key, struct profile_t, val ) {
        if( isnan(val->stat) ) {
            float v = val->avg/1000.0;
            ui_slider2(*key, &v, va("%.2f ms", val->avg/1000.0));
        } else {
            float v = val->stat;
            ui_slider2(*key, &v, va("%.2f", val->stat));
            val->stat = 0;
        }
    }
}
#endif

#endif
