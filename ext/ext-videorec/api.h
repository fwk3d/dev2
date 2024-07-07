// -----------------------------------------------------------------------------
// ffmpeg video recording
// [src] http://blog.mmacklin.com/2013/06/11/real-time-video-capture-with-ffmpeg/
// -----------------------------------------------------------------------------
// video recorder (uses external ffmpeg and fallbacks to built-in mpeg1 encoder)
// - rlyeh, public domain
//
// @fixme: MSAA can cause some artifacts with Intel PBOs: either use glDisable(GL_MULTISAMPLE) before recording or do not create window with WINDOW_MSAA at all.

API bool       record_start(const char *outfile_mp4);
API bool        record_active();
API void       record_stop(void);

#if CODE

#define JO_MPEG_COMPONENTS 3                  // jo_mpeg
#include "3rd_jo_mpeg.h"

static FILE* rec_ffmpeg;
static FILE* rec_mpeg1;

void record_stop(void) {
    if(rec_ffmpeg) ifdef(win32, _pclose, pclose)(rec_ffmpeg);
    rec_ffmpeg = 0;

    if(rec_mpeg1) fclose(rec_mpeg1);
    rec_mpeg1 = 0;
}

bool record_active() {
    return rec_ffmpeg || rec_mpeg1;
}

bool record_start(const char *outfile_mp4) {
    do_once atexit(record_stop);

    record_stop();

    // first choice: external ffmpeg encoder
    if( !rec_ffmpeg ) {
        extern const char *TOOLS;

        char *tools_native_path = "ext/ext-videorec/"; // strswap( va("%s/", TOOLS), ifdef(win32, "/", "\\"), ifdef(win32, "\\", "/") );

        char *cmd = va("%sffmpeg%s "
                    "-hide_banner -loglevel error " // less verbose
                    "-r %d -f rawvideo -pix_fmt bgr24 -s %dx%d " // raw BGR WxH-60Hz frames
                    // "-framerate 30 " // interpolating new video output frames from the source frames
                    "-i - "              // read frames from stdin
                    //"-draw_mouse 1 "
                    "-threads 0 "
                    //"-vsync vfr "
                    "-preset ultrafast " // collection of options that will provide a certain encoding speed [fast,ultrafast]
                    // "-tune zerolatency " // change settings based upon the specifics of your input
                    //"-crf 21 "           // range of the CRF scale [0(lossless)..23(default)..51(worst quality)]
                    "-pix_fmt yuv420p "  // compatible with Windows Media Player and Quicktime
                    "-vf vflip "         // flip Y
//                  "-vf \"pad=ceil(iw/2)*2:ceil(ih/2)*2\" "
                    "-y \"%s\"", tools_native_path, ifdef(win32, ".exe", ifdef(osx, ".osx",".linux")),
                    (int)window_fps(), window_width(), window_height(), outfile_mp4);    // overwrite output file

        // -rtbufsize 100M (https://trac.ffmpeg.org/wiki/DirectShow#BufferingLatency) Prevent some frames in the buffer from being dropped.
        // -probesize 10M (https://www.ffmpeg.org/ffmpeg-formats.html#Format-Options) Set probing size in bytes, i.e. the size of the data to analyze to get stream information. A higher value will enable detecting more information in case it is dispersed into the stream, but will increase latency. Must be an integer not lesser than 32. It is 5000000 by default.
        // -c:v libx264 (https://www.ffmpeg.org/ffmpeg.html#Main-options) Select an encoder (when used before an output file) or a decoder (when used before an input file) for one or more streams. codec is the name of a decoder/encoder or a special value copy (output only) to indicate that the stream is not to be re-encoded.

        // open pipe to ffmpeg's stdin in binary write mode
        rec_ffmpeg = ifdef(win32, _popen(cmd, "wb"), popen(cmd, "w"));
    }

    // fallback: built-in mpeg1 encoder
    if( !rec_ffmpeg ) {
        rec_mpeg1 = fopen(outfile_mp4, "wb"); // "a+b"
    }

    return record_active();
}

void record_frame() {
    if( record_active() ) {
        void* pixels = screenshot_async(-3); // 3 RGB, 4 RGBA, -3 BGR, -4 BGRA. ps: BGR is fastest on my intel discrete gpu

        if( rec_ffmpeg ) {
            fwrite(pixels, 3 * window_width() * window_height(), 1, rec_ffmpeg);
        }
        if( rec_mpeg1 ) {
            jo_write_mpeg(rec_mpeg1, pixels, window_width(), window_height(), 24);  // 24fps
        }
    }
}

#endif
