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
#if __has_include("MaterialDesignIconsWebfont")
#include "MaterialDesignIconsWebfont"
#endif
#if __has_include("MaterialIconsSharpRegular")
#include "MaterialIconsSharpRegular"
#endif
struct resource_t { const char *name, *data; unsigned size; } resources[] = {
#if __has_include("B612Bold")
{ "B612Bold.ttf", B612Bold, (unsigned)sizeof(B612Bold) },
#endif
#if __has_include("B612BoldItalic")
{ "B612BoldItalic.ttf", B612BoldItalic, (unsigned)sizeof(B612BoldItalic) },
#endif
#if __has_include("B612Italic")
{ "B612Italic.ttf", B612Italic, (unsigned)sizeof(B612Italic) },
#endif
#if __has_include("B612MonoBold")
{ "B612MonoBold.ttf", B612MonoBold, (unsigned)sizeof(B612MonoBold) },
#endif
#if __has_include("B612MonoBoldItalic")
{ "B612MonoBoldItalic.ttf", B612MonoBoldItalic, (unsigned)sizeof(B612MonoBoldItalic) },
#endif
#if __has_include("B612MonoItalic")
{ "B612MonoItalic.ttf", B612MonoItalic, (unsigned)sizeof(B612MonoItalic) },
#endif
#if __has_include("B612MonoRegular")
{ "B612MonoRegular.ttf", B612MonoRegular, (unsigned)sizeof(B612MonoRegular) },
#endif
#if __has_include("B612Regular")
{ "B612Regular.ttf", B612Regular, (unsigned)sizeof(B612Regular) },
#endif
#if __has_include("MaterialDesignIconsWebfont")
{ "MaterialDesignIconsWebfont.ttf", MaterialDesignIconsWebfont, (unsigned)sizeof(MaterialDesignIconsWebfont) },
#endif
#if __has_include("MaterialIconsSharpRegular")
{ "MaterialIconsSharpRegular.otf", MaterialIconsSharpRegular, (unsigned)sizeof(MaterialIconsSharpRegular) },
#endif
{ NULL, NULL, 0u },
};

