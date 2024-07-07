// built-ins

#if !CODE
typedef enum OBJTYPE_builtins {
    OBJTYPE_obj    =  0,
    OBJTYPE_entity =  1,
    OBJTYPE_vec2   =  2,
    OBJTYPE_vec3   =  3,
    OBJTYPE_vec4   =  4,
    OBJTYPE_quat   =  5,
    OBJTYPE_mat33  =  6,
    OBJTYPE_mat34  =  7,
    OBJTYPE_mat44  =  8,
    OBJTYPE_vec2i  =  9,
    OBJTYPE_vec3i  = 10,
} OBJTYPE_builtins;
#endif

#include "obj_pack.h"
#include "obj_reflect.h"
#include "obj_id.h"
#include "obj_obj.h" // after reflect and objid
#include "obj_pack_cobs.h"
#include "obj_pack_compress.h"

API int ui_obj(const char *fmt, obj *o);

#if CODE

int ui_obj(const char *fmt, obj *o) {
    int changed = 0, item = 1;
    for each_objmember(o, TYPE,NAME,PTR) {
        char *label = va(fmt, NAME);
        /**/ if(!strcmp(TYPE,"float"))    { if(ui_float(label, PTR)) changed = item; }
        else if(!strcmp(TYPE,"int"))      { if(ui_int(label, PTR)) changed = item; }
        else if(!strcmp(TYPE,"unsigned")) { if(ui_unsigned(label, PTR)) changed = item; }
        else if(!strcmp(TYPE,"vec2"))     { if(ui_float2(label, PTR)) changed = item; }
        else if(!strcmp(TYPE,"vec3"))     { if(ui_float3(label, PTR)) changed = item; }
        else if(!strcmp(TYPE,"vec4"))     { if(ui_float4(label, PTR)) changed = item; }
        else if(!strcmp(TYPE,"rgb"))      { if(ui_color3(label, PTR)) changed = item; }
        else if(!strcmp(TYPE,"rgba"))     { if(ui_color4(label, PTR)) changed = item; }
        else if(!strcmp(TYPE,"color"))    { if(ui_color4f(label, PTR)) changed = item; }
        else if(!strcmp(TYPE,"color3f"))  { if(ui_color3f(label, PTR)) changed = item; }
        else if(!strcmp(TYPE,"color4f"))  { if(ui_color4f(label, PTR)) changed = item; }
        else if(!strcmp(TYPE,"char*"))    { if(ui_string(label, PTR)) changed = item; }
        else ui_label2(label, va("(%s)", TYPE)); // INFO instead of (TYPE)?
        ++item;
    }
    return changed;
}

#endif
