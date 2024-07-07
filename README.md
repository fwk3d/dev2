<h1 align="center"><a href="#">F·W·K</a></h1>
<p align="center">
3D game engine/framework in C.<br/>
<br/>
DEV REPOSITORY.<br/>
</p>

## About

- 3D game engine v2, written in C.
- Major overhaul from [old v1 engine](https://github.com/fwk3d/v1).
- In general, v2 is faster, smaller and stronger than v1.
- v2 is still WIP, though.

<details>
<summary><h2>How does v2 compare to v1?</h2></summary>

- Smaller demos.
- Smaller codebase.
- Smaller repository size.
- Faster warm-up/boot times.
- Smoother experience. Higher framerates, less CPU usage.
- New backends: SDL3, Luajit, OpenAL, DearImgui, ImPlot, etc.
- New redesigned UI. Docking and multi-viewports ready.
- New redesigned Game APIs: Loop, App, 3D Audio, Script v2, ...
- New ext/PLUG plugin system. Community-driven, Github based.
- All APIs exposed to Lua scripts. New bindings generator.
- Editor, Rendering and Scene APIs decoupled from other APIs.
- Resources in embed/ folder can be optionally embedded now.
- Simplified code structure: old split/joint concepts are gone.
- Simplified cook process. Asset tools/ folder no longer required.
- Simplified build process. MAKE file scans automatically ext dependencies.
- Simplified implementation details for many apis: file, memory, panic, logger, ...
- And more.

During the process, many things got broken compared to v1:

- Windows only: no support for Linux, OSX, Emscripten targets at the moment.
- Visual Studio only: no support for clang, clang-cl, gcc compilers at the moment.
- Not so great compilation times: the engine is still C, but there are C++ dependencies now.
- No python bindings.
- No single-header distributions.
- No support for fused zipfiles.
- Generated documentation is broken.
- And more.

</details>

## Unlicense
This software is released into the [public domain](https://unlicense.org/). Also dual-licensed as [0-BSD](https://opensource.org/licenses/0BSD) or [MIT (No Attribution)](https://github.com/aws/mit-0) for those countries where public domain is a concern (sigh). Any contribution to this repository is implicitly subjected to the same release conditions aforementioned.

## Links
Still looking for alternatives? [amulet](https://github.com/ianmaclarty/amulet), [aroma](https://github.com/leafo/aroma/), [astera](https://github.com/tek256/astera), [blendelf](https://github.com/jesterKing/BlendELF), [bullordengine](https://github.com/MarilynDafa/Bulllord-Engine), [candle](https://github.com/EvilPudding/candle), [cave](https://github.com/kieselsteini/cave), [chickpea](https://github.com/ivansafrin/chickpea), [corange](https://github.com/orangeduck/Corange), [cute](https://github.com/RandyGaul/cute_framework), [dos-like](https://github.com/mattiasgustavsson/dos-like), [ejoy2d](https://github.com/ejoy/ejoy2d), [exengine](https://github.com/exezin/exengine), [gunslinger](https://github.com/MrFrenik/gunslinger), [hate](https://github.com/excessive/hate), [island](https://github.com/island-org/island), [juno](https://github.com/rxi/juno), [l](https://github.com/Lyatus/L), [lgf](https://github.com/Planimeter/lgf), [limbus](https://github.com/redien/limbus), [love](https://github.com/love2d/love/), [lovr](https://github.com/bjornbytes/lovr), [mini3d](https://github.com/mini3d/mini3d), [mintaro](https://github.com/mackron/mintaro), [mio](https://github.com/ccxvii/mio), [olive.c](https://github.com/tsoding/olive.c), [opensource](https://github.com/w23/OpenSource), [ouzel](https://github.com/elnormous/ouzel/), [pez](https://github.com/prideout/pez), [pixie](https://github.com/mattiasgustavsson/pixie), [punity](https://github.com/martincohen/Punity), [r96](https://github.com/badlogic/r96), [ricotech](https://github.com/dbechrd/RicoTech), [rizz](https://github.com/septag/rizz), [tigr](https://github.com/erkkah/tigr), [yourgamelib](https://github.com/duddel/yourgamelib)

<a href="https://github.com/fwk3d/v2/issues"><img alt="Issues" src="https://img.shields.io/github/issues-raw/fwk3d/v2.svg"/></a> <a href="https://discord.gg/UpB7nahEFU"><img alt="Discord" src="https://img.shields.io/discord/270565488365535232?color=5865F2&label=chat&logo=discord&logoColor=white"/></a>
