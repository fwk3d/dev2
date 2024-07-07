#if __has_include("api")
#include "api"
#endif
#if __has_include("B612Bold")
#include "B612Bold"
#endif
#if __has_include("B612BoldItalic")
#include "B612BoldItalic"
#endif
#if __has_include("B612Italic")
#include "B612Italic"
#endif
#if __has_include("B612MonoBold")
#include "B612MonoBold"
#endif
#if __has_include("B612MonoBoldItalic")
#include "B612MonoBoldItalic"
#endif
#if __has_include("B612MonoItalic")
#include "B612MonoItalic"
#endif
#if __has_include("B612MonoRegular")
#include "B612MonoRegular"
#endif
#if __has_include("B612Regular")
#include "B612Regular"
#endif
#if __has_include("MaterialSymbolsSharp_Filled_28pt_Regular")
#include "MaterialSymbolsSharp_Filled_28pt_Regular"
#endif
#if __has_include("brdf")
#include "brdf"
#endif
#if __has_include("compat")
#include "compat"
#endif
#if __has_include("fog")
#include "fog"
#endif
#if __has_include("light")
#include "light"
#endif
#if __has_include("model_fs")
#include "model_fs"
#endif
#if __has_include("model_vs")
#include "model_vs"
#endif
#if __has_include("parallax")
#include "parallax"
#endif
#if __has_include("rimlight")
#include "rimlight"
#endif
#if __has_include("shadowmap")
#include "shadowmap"
#endif
#if __has_include("sh_lighting")
#include "sh_lighting"
#endif
#if __has_include("surface")
#include "surface"
#endif
#if __has_include("utils")
#include "utils"
#endif
#if __has_include("brdf_lut")
#include "brdf_lut"
#endif
#if __has_include("cubemap_sh")
#include "cubemap_sh"
#endif
#if __has_include("font_fs")
#include "font_fs"
#endif
#if __has_include("font_vs")
#include "font_vs"
#endif
#if __has_include("fullscreen_quad_A_vs")
#include "fullscreen_quad_A_vs"
#endif
#if __has_include("fullscreen_quad_B_flipped_vs")
#include "fullscreen_quad_B_flipped_vs"
#endif
#if __has_include("fullscreen_quad_B_vs")
#include "fullscreen_quad_B_vs"
#endif
#if __has_include("header_shadertoy")
#include "header_shadertoy"
#endif
#if __has_include("model_fs")
#include "model_fs"
#endif
#if __has_include("model_material_fs")
#include "model_material_fs"
#endif
#if __has_include("model_vs")
#include "model_vs"
#endif
#if __has_include("preamble_fs")
#include "preamble_fs"
#endif
#if __has_include("query_point_fs")
#include "query_point_fs"
#endif
#if __has_include("query_point_vs")
#include "query_point_vs"
#endif
#if __has_include("rect2d_fs")
#include "rect2d_fs"
#endif
#if __has_include("rect2d_vs")
#include "rect2d_vs"
#endif
#if __has_include("shadertoy_flip_vs")
#include "shadertoy_flip_vs"
#endif
#if __has_include("shadertoy_main_fs")
#include "shadertoy_main_fs"
#endif
#if __has_include("shadertoy_vs")
#include "shadertoy_vs"
#endif
#if __has_include("shadow_pass_fs")
#include "shadow_pass_fs"
#endif
#if __has_include("skybox_fs")
#include "skybox_fs"
#endif
#if __has_include("skybox_rayleigh_fs")
#include "skybox_rayleigh_fs"
#endif
#if __has_include("skybox_vs")
#include "skybox_vs"
#endif
#if __has_include("sprite_fs")
#include "sprite_fs"
#endif
#if __has_include("sprite_vs")
#include "sprite_vs"
#endif
#if __has_include("texel_inv_gamma_fs")
#include "texel_inv_gamma_fs"
#endif
#if __has_include("texel_ycbr_gamma_saturation_fs")
#include "texel_ycbr_gamma_saturation_fs"
#endif
#if __has_include("texel_y_gamma_saturation_fs")
#include "texel_y_gamma_saturation_fs"
#endif
struct resource_t { const char *name, *data; unsigned size; } resources[] = {
#if __has_include(".embed/api")
{ "C:\prj\2\code\embed\.embed\api", api, (unsigned)sizeof(api) },
#endif
#if __has_include(".embed/B612Bold")
{ "C:\prj\2\code\embed\.embed\B612Bold", B612Bold, (unsigned)sizeof(B612Bold) },
#endif
#if __has_include(".embed/B612BoldItalic")
{ "C:\prj\2\code\embed\.embed\B612BoldItalic", B612BoldItalic, (unsigned)sizeof(B612BoldItalic) },
#endif
#if __has_include(".embed/B612Italic")
{ "C:\prj\2\code\embed\.embed\B612Italic", B612Italic, (unsigned)sizeof(B612Italic) },
#endif
#if __has_include(".embed/B612MonoBold")
{ "C:\prj\2\code\embed\.embed\B612MonoBold", B612MonoBold, (unsigned)sizeof(B612MonoBold) },
#endif
#if __has_include(".embed/B612MonoBoldItalic")
{ "C:\prj\2\code\embed\.embed\B612MonoBoldItalic", B612MonoBoldItalic, (unsigned)sizeof(B612MonoBoldItalic) },
#endif
#if __has_include(".embed/B612MonoItalic")
{ "C:\prj\2\code\embed\.embed\B612MonoItalic", B612MonoItalic, (unsigned)sizeof(B612MonoItalic) },
#endif
#if __has_include(".embed/B612MonoRegular")
{ "C:\prj\2\code\embed\.embed\B612MonoRegular", B612MonoRegular, (unsigned)sizeof(B612MonoRegular) },
#endif
#if __has_include(".embed/B612Regular")
{ "C:\prj\2\code\embed\.embed\B612Regular", B612Regular, (unsigned)sizeof(B612Regular) },
#endif
#if __has_include(".embed/brdf")
{ "C:\prj\2\code\embed\.embed\brdf", brdf, (unsigned)sizeof(brdf) },
#endif
#if __has_include(".embed/brdf_lut")
{ "C:\prj\2\code\embed\.embed\brdf_lut", brdf_lut, (unsigned)sizeof(brdf_lut) },
#endif
#if __has_include(".embed/compat")
{ "C:\prj\2\code\embed\.embed\compat", compat, (unsigned)sizeof(compat) },
#endif
#if __has_include(".embed/cubemap_sh")
{ "C:\prj\2\code\embed\.embed\cubemap_sh", cubemap_sh, (unsigned)sizeof(cubemap_sh) },
#endif
#if __has_include(".embed/fog")
{ "C:\prj\2\code\embed\.embed\fog", fog, (unsigned)sizeof(fog) },
#endif
#if __has_include(".embed/font_fs")
{ "C:\prj\2\code\embed\.embed\font_fs", font_fs, (unsigned)sizeof(font_fs) },
#endif
#if __has_include(".embed/font_vs")
{ "C:\prj\2\code\embed\.embed\font_vs", font_vs, (unsigned)sizeof(font_vs) },
#endif
#if __has_include(".embed/fullscreen_quad_A_vs")
{ "C:\prj\2\code\embed\.embed\fullscreen_quad_A_vs", fullscreen_quad_A_vs, (unsigned)sizeof(fullscreen_quad_A_vs) },
#endif
#if __has_include(".embed/fullscreen_quad_B_flipped_vs")
{ "C:\prj\2\code\embed\.embed\fullscreen_quad_B_flipped_vs", fullscreen_quad_B_flipped_vs, (unsigned)sizeof(fullscreen_quad_B_flipped_vs) },
#endif
#if __has_include(".embed/fullscreen_quad_B_vs")
{ "C:\prj\2\code\embed\.embed\fullscreen_quad_B_vs", fullscreen_quad_B_vs, (unsigned)sizeof(fullscreen_quad_B_vs) },
#endif
#if __has_include(".embed/header_shadertoy")
{ "C:\prj\2\code\embed\.embed\header_shadertoy", header_shadertoy, (unsigned)sizeof(header_shadertoy) },
#endif
#if __has_include(".embed/light")
{ "C:\prj\2\code\embed\.embed\light", light, (unsigned)sizeof(light) },
#endif
#if __has_include(".embed/MaterialSymbolsSharp_Filled_28pt_Regular")
{ "C:\prj\2\code\embed\.embed\MaterialSymbolsSharp_Filled_28pt_Regular", MaterialSymbolsSharp_Filled_28pt_Regular, (unsigned)sizeof(MaterialSymbolsSharp_Filled_28pt_Regular) },
#endif
#if __has_include(".embed/model_fs")
{ "C:\prj\2\code\embed\.embed\model_fs", model_fs, (unsigned)sizeof(model_fs) },
#endif
#if __has_include(".embed/model_material_fs")
{ "C:\prj\2\code\embed\.embed\model_material_fs", model_material_fs, (unsigned)sizeof(model_material_fs) },
#endif
#if __has_include(".embed/model_vs")
{ "C:\prj\2\code\embed\.embed\model_vs", model_vs, (unsigned)sizeof(model_vs) },
#endif
#if __has_include(".embed/parallax")
{ "C:\prj\2\code\embed\.embed\parallax", parallax, (unsigned)sizeof(parallax) },
#endif
#if __has_include(".embed/preamble_fs")
{ "C:\prj\2\code\embed\.embed\preamble_fs", preamble_fs, (unsigned)sizeof(preamble_fs) },
#endif
#if __has_include(".embed/query_point_fs")
{ "C:\prj\2\code\embed\.embed\query_point_fs", query_point_fs, (unsigned)sizeof(query_point_fs) },
#endif
#if __has_include(".embed/query_point_vs")
{ "C:\prj\2\code\embed\.embed\query_point_vs", query_point_vs, (unsigned)sizeof(query_point_vs) },
#endif
#if __has_include(".embed/rect2d_fs")
{ "C:\prj\2\code\embed\.embed\rect2d_fs", rect2d_fs, (unsigned)sizeof(rect2d_fs) },
#endif
#if __has_include(".embed/rect2d_vs")
{ "C:\prj\2\code\embed\.embed\rect2d_vs", rect2d_vs, (unsigned)sizeof(rect2d_vs) },
#endif
#if __has_include(".embed/rimlight")
{ "C:\prj\2\code\embed\.embed\rimlight", rimlight, (unsigned)sizeof(rimlight) },
#endif
#if __has_include(".embed/shadertoy_flip_vs")
{ "C:\prj\2\code\embed\.embed\shadertoy_flip_vs", shadertoy_flip_vs, (unsigned)sizeof(shadertoy_flip_vs) },
#endif
#if __has_include(".embed/shadertoy_main_fs")
{ "C:\prj\2\code\embed\.embed\shadertoy_main_fs", shadertoy_main_fs, (unsigned)sizeof(shadertoy_main_fs) },
#endif
#if __has_include(".embed/shadertoy_vs")
{ "C:\prj\2\code\embed\.embed\shadertoy_vs", shadertoy_vs, (unsigned)sizeof(shadertoy_vs) },
#endif
#if __has_include(".embed/shadowmap")
{ "C:\prj\2\code\embed\.embed\shadowmap", shadowmap, (unsigned)sizeof(shadowmap) },
#endif
#if __has_include(".embed/shadow_pass_fs")
{ "C:\prj\2\code\embed\.embed\shadow_pass_fs", shadow_pass_fs, (unsigned)sizeof(shadow_pass_fs) },
#endif
#if __has_include(".embed/sh_lighting")
{ "C:\prj\2\code\embed\.embed\sh_lighting", sh_lighting, (unsigned)sizeof(sh_lighting) },
#endif
#if __has_include(".embed/skybox_fs")
{ "C:\prj\2\code\embed\.embed\skybox_fs", skybox_fs, (unsigned)sizeof(skybox_fs) },
#endif
#if __has_include(".embed/skybox_rayleigh_fs")
{ "C:\prj\2\code\embed\.embed\skybox_rayleigh_fs", skybox_rayleigh_fs, (unsigned)sizeof(skybox_rayleigh_fs) },
#endif
#if __has_include(".embed/skybox_vs")
{ "C:\prj\2\code\embed\.embed\skybox_vs", skybox_vs, (unsigned)sizeof(skybox_vs) },
#endif
#if __has_include(".embed/sprite_fs")
{ "C:\prj\2\code\embed\.embed\sprite_fs", sprite_fs, (unsigned)sizeof(sprite_fs) },
#endif
#if __has_include(".embed/sprite_vs")
{ "C:\prj\2\code\embed\.embed\sprite_vs", sprite_vs, (unsigned)sizeof(sprite_vs) },
#endif
#if __has_include(".embed/surface")
{ "C:\prj\2\code\embed\.embed\surface", surface, (unsigned)sizeof(surface) },
#endif
#if __has_include(".embed/texel_inv_gamma_fs")
{ "C:\prj\2\code\embed\.embed\texel_inv_gamma_fs", texel_inv_gamma_fs, (unsigned)sizeof(texel_inv_gamma_fs) },
#endif
#if __has_include(".embed/texel_ycbr_gamma_saturation_fs")
{ "C:\prj\2\code\embed\.embed\texel_ycbr_gamma_saturation_fs", texel_ycbr_gamma_saturation_fs, (unsigned)sizeof(texel_ycbr_gamma_saturation_fs) },
#endif
#if __has_include(".embed/texel_y_gamma_saturation_fs")
{ "C:\prj\2\code\embed\.embed\texel_y_gamma_saturation_fs", texel_y_gamma_saturation_fs, (unsigned)sizeof(texel_y_gamma_saturation_fs) },
#endif
#if __has_include(".embed/utils")
{ "C:\prj\2\code\embed\.embed\utils", utils, (unsigned)sizeof(utils) },
#endif
{ NULL, NULL, 0u },
};

