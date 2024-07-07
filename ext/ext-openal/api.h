#if !CODE

#include <AL/al.h>
#include <AL/alc.h>
//#include <AL/alut.h>

#if HAS_ALEXT
#include <AL/alext.h>
#include <AL/efx.h>
//#include <AL/efx-creative.h>
#define ALEXT_Effect_objects(X) \
    X(LPALGENEFFECTS,alGenEffects) \
    X(LPALDELETEEFFECTS,alDeleteEffects) \
    X(LPALISEFFECT,alIsEffect) \
    X(LPALEFFECTI,alEffecti) \
    X(LPALEFFECTIV,alEffectiv) \
    X(LPALEFFECTF,alEffectf) \
    X(LPALEFFECTFV,alEffectfv) \
    X(LPALGETEFFECTI,alGetEffecti) \
    X(LPALGETEFFECTIV,alGetEffectiv) \
    X(LPALGETEFFECTF,alGetEffectf) \
    X(LPALGETEFFECTFV,alGetEffectfv)
#define ALEXT_Filter_objects(X) \
    X(LPALGENFILTERS,alGenFilters) \
    X(LPALDELETEFILTERS,alDeleteFilters) \
    X(LPALISFILTER,alIsFilter) \
    X(LPALFILTERI,alFilteri) \
    X(LPALFILTERIV,alFilteriv) \
    X(LPALFILTERF,alFilterf) \
    X(LPALFILTERFV,alFilterfv) \
    X(LPALGETFILTERI,alGetFilteri) \
    X(LPALGETFILTERIV,alGetFilteriv) \
    X(LPALGETFILTERF,alGetFilterf) \
    X(LPALGETFILTERFV,alGetFilterfv)
#define ALEXT_Slot_objects(X) \
    X(LPALGENAUXILIARYEFFECTSLOTS,alGenAuxiliaryEffectSlots) \
    X(LPALDELETEAUXILIARYEFFECTSLOTS,alDeleteAuxiliaryEffectSlots) \
    X(LPALISAUXILIARYEFFECTSLOT,alIsAuxiliaryEffectSlot) \
    X(LPALAUXILIARYEFFECTSLOTI,alAuxiliaryEffectSloti) \
    X(LPALAUXILIARYEFFECTSLOTIV,alAuxiliaryEffectSlotiv) \
    X(LPALAUXILIARYEFFECTSLOTF,alAuxiliaryEffectSlotf) \
    X(LPALAUXILIARYEFFECTSLOTFV,alAuxiliaryEffectSlotfv) \
    X(LPALGETAUXILIARYEFFECTSLOTI,alGetAuxiliaryEffectSloti) \
    X(LPALGETAUXILIARYEFFECTSLOTIV,alGetAuxiliaryEffectSlotiv) \
    X(LPALGETAUXILIARYEFFECTSLOTF,alGetAuxiliaryEffectSlotf)
#define ALEXT_decl(t,n) t n = 0;
#define ALEXT_load(t,n) n = (t)alGetProcAddress(#n);
    ALEXT_Effect_objects(ALEXT_decl)
    ALEXT_Filter_objects(ALEXT_decl)
    ALEXT_Slot_objects(ALEXT_decl)
#endif

API bool load_efx();

#else

#pragma comment(lib, "OpenAL32")

bool load_efx() {
#if HAS_ALEXT
    ALEXT_Effect_objects(ALEXT_load)
    ALEXT_Filter_objects(ALEXT_load)
    ALEXT_Slot_objects(ALEXT_load)
    return 1;
#else
    return 0;
#endif
}

#endif
