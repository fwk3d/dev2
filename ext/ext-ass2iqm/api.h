#if CODE
AUTORUN {
    // iqe->iqm case

    recipe("**.iqe", 0, "ext/ext-ass2iqm/iqe2iqm.EXE OUTPUT INPUT > NUL");

    // fbx->iqe->iqm case
    // fbx->animlist case

    recipe("**.fbx", 0, "ext/ext-ass2iqm/ass2iqe.EXE -Z -o OUTPUT.iqe INPUT 2> NUL" "&& ext/ext-ass2iqm/iqe2iqm.EXE OUTPUT OUTPUT.iqe > NUL");
    recipe("**.fbx", 0, "ext/ext-ass2iqm/ass2iqe.EXE FLAGS -L -o OUTPUT@animlist.txt INPUT 2> NUL");

    // gltf/gltf2/glb/obj->iqe->iqm case

    recipe("**.obj;**.glb;**.gltf;**.gltf2", 0, "ext/ext-ass2iqm/ass2iqe.EXE -o OUTPUT.iqe INPUT 2> NUL" "&& ext/ext-ass2iqm/iqe2iqm.EXE OUTPUT OUTPUT.iqe > NUL");
}
#endif
