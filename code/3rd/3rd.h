#include "3rd_stb_image.h"
#include "3rd_stb_image_write.h"
#include "3rd_stb_image_resize2.h"
#include "3rd_stb_vorbis.h"
#define stb_vorbis_free free

#ifndef __cplusplus
// following line exposes stb_truetype and stb_rectpack as well
// #include "cimgui/cimgui.h"
#include "cimgui/imgui/imstb_rectpack.h"
#include "cimgui/imgui/imstb_truetype.h"
#endif


#include "3rd_tfd.h"

#include "3rd_icon_ms.h"
//#include "3rd_icon_md.h"
//#include "3rd_icon_mdi.h"

#include "3rd_xml.h"
#include "3rd_json5.h"
#include "3rd_base64.h"

#include "3rd_compress.h"
#include "3rd_compress_deflate.h"

#include "3rd_archive_dir.h"
#include "3rd_archive_pak.h"
#include "3rd_archive_tar.h"
#include "3rd_archive_zip.h"

#include "3rd_swrap.h"

//#include "3rd_lua.h"
//#include "3rd_luadebugger.h"
