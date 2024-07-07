// -----------------------------------------------------------------------------
// materials

#if !CODE

enum MATERIAL_ENUMS {
	MATERIAL_CHANNEL_ALBEDO,
	MATERIAL_CHANNEL_NORMALS,
	MATERIAL_CHANNEL_ROUGHNESS,
	MATERIAL_CHANNEL_METALLIC,
	MATERIAL_CHANNEL_AO,
	MATERIAL_CHANNEL_AMBIENT,
	MATERIAL_CHANNEL_EMISSIVE,
	MATERIAL_CHANNEL_PARALLAX,
    
    MAX_CHANNELS_PER_MATERIAL
};

typedef struct material_layer_t {
    char   texname[32];
    float  value;
    float  value2;
    colormap_t map;
} material_layer_t;

typedef struct material_t {
    char *name;
    material_layer_t layer[MAX_CHANNELS_PER_MATERIAL];
    float cutout_alpha;
    float ssr_strength;
    bool parallax_clip;
    bool disable_ibl;

    // internal
    bool _loaded;
} material_t;

API void material_texparams(material_t *m, unsigned texture_flags);
API uint32_t material_checksum(material_t *m);
API void ui_material(material_t *m);

#endif

