#if !CODE

API int         dialog(const char *title, const char *message, int numbuttons); // returns 'Y'es/'N'o/'C'ancel
API const char* dialog_load(const char *title);
API const char* dialog_save(const char *title);
API const char* dialog_folder(const char *title);

API void        alert(const char *message);
API void        alert2(const char *title, const char *message);
API char*       prompt(const char *title, const char *message, const char *defaults ); // defined in sys_prompt.hh

#else // ------------------------------------------------------------------------

int dialog(const char *title, const char *message, int numbuttons) {
    const SDL_MessageBoxButtonData buttons[] = {
        // 1-button dialog
        { /* .flags, .buttonid, .text */        0, 0, "Ok" },
        // 2-button dialog
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "No" },
        { /* .flags, .buttonid, .text */        0, 0, "Yes" },
        // 3-button dialog
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "Cancel" },
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "No" },
        { /* .flags, .buttonid, .text */        0, 0, "Yes" },
    };
    SDL_MessageBoxData mb = {0};
    mb.flags = strstri(title, "error") ? SDL_MESSAGEBOX_ERROR :
        strstri(title, "warning") ? SDL_MESSAGEBOX_WARNING : SDL_MESSAGEBOX_INFORMATION;
    mb.title = title;
    mb.message = message;
    mb.numbuttons = numbuttons = max(min(numbuttons,3),1);
    mb.buttons = buttons + (numbuttons == 3 ? 3 : numbuttons == 2 ? 1 : 0);

    int clicked;
    if( SDL_ShowMessageBox(&mb, &clicked) )
        return "YNC"[ clicked ];

    return 0;
}

const char* dialog_load(const char *title) {
    // Not using these methods right now, as they're async:
    // SDL_ShowOpenFileDialog()
    // SDL_ShowOpenFolderDialog()

    char cwd[DIR_MAX] = {0}; getcwd(cwd, DIR_MAX);

    const char *windowTitle = title;
    const char *defaultPathFile = cwd;
    const char *filterHints = NULL; // "image files"
    const char *filters[] = { "*.*" };
    int allowMultipleSelections = 0;

    tinyfd_assumeGraphicDisplay = 1;
    return tinyfd_openFileDialog( windowTitle, defaultPathFile, COUNTOF(filters), filters, filterHints, allowMultipleSelections );
}
const char* dialog_save(const char *title) {
    // Not using these methods right now, as they're async:
    // SDL_ShowSaveFileDialog()
    
    char cwd[DIR_MAX] = {0}; getcwd(cwd, DIR_MAX);

    const char *windowTitle = title;
    const char *defaultPathFile = cwd;
    const char *filterHints = NULL; // "image files"
    const char *filters[] = { "*.*" };

    tinyfd_assumeGraphicDisplay = 1;
    return tinyfd_saveFileDialog( windowTitle, defaultPathFile, COUNTOF(filters), filters, filterHints );
}
const char* dialog_folder(const char *title) {
    char cwd[DIR_MAX] = {0}; getcwd(cwd, DIR_MAX);

    return tinyfd_selectFolderDialog(title, cwd);
}


void alert(const char *message) {
    alert2("Alert", message);
}

void alert2(const char *title, const char *message) {
    char joint[1024], *bt = message[0] == '!' ? trace(+16) : NULL;
    snprintf(joint, 1024, "%s\n%s\n", message + (message[0] == '!'), bt ? bt : "");
    SDL_ShowSimpleMessageBox(0, title, joint, NULL);
}

#endif
