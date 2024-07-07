// ----------------------------------------------------------------------------
// threads

API void*       thread( int (*thread_func)(void* user_data), void* user_data, const char *thread_name );
API int         thread_join( void *thd );
API void        thread_detach( void *thd );

#if CODE

void* thread( int (*function)(void* user_data), void* user_data, const char *thread_name ) {
    SDL_Thread *thread = SDL_CreateThread(function, thread_name, user_data);
    // 
    return thread;
}

int thread_join( void *thd ) {
    int status;
    SDL_WaitThread((SDL_Thread*)thd, &status);
    return status;
}

void thread_detach( void *thd ) {
    SDL_DetachThread((SDL_Thread*)thd);
}

#endif
