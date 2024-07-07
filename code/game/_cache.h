#define IF_CACHED(str,type_t,name) \
    static map(char*,type_t) map = 0; if(!map) map_init_str(map); \
    for( type_t *name = map_find(str); name ; name = 0 )

#define CACHE(str,value) map_insert(map, STRDUP(str), value)

// -----------------------------------------------------------------------------
// cache

API void *       cache_insert(const char *key, void *value, int size);
API void *       cache_lookup(const char *key, int *size);

// -----------------------------------------------------------------------------

static thread_mutex_t cache_mutex; AUTORUN{ thread_mutex_init(&cache_mutex); }

void* cache_lookup(const char *pathfile, int *size) { // find key->value
    if( !MAX_CACHED_FILES ) return 0;
    void* data = 0;
    thread_mutex_lock(&cache_mutex);
    for(archive_dir *dir = dir_cache; dir; dir = dir->next) {
        if( !strcmp(dir->path, pathfile) ) {
            if(size) *size = dir->size;
            data = dir->data;
            break;
        }
    }
    thread_mutex_unlock(&cache_mutex);
    return data;
}
void* cache_insert(const char *pathfile, void *ptr, int size) { // append key/value; return LRU or NULL
    if( !MAX_CACHED_FILES ) return 0;
    if( !ptr || !size ) return 0;

    // keep cached files within limits
    thread_mutex_lock(&cache_mutex);

        // append to cache
        archive_dir zero = {0}, *old = dir_cache;
        *(dir_cache = REALLOC(0, sizeof(archive_dir))) = zero;
        dir_cache->next = old;
        dir_cache->path = STRDUP(pathfile);
        dir_cache->size = size;
        dir_cache->data = REALLOC(0, size+1);
        memcpy(dir_cache->data, ptr, size); size[(char*)dir_cache->data] = 0; // copy+terminator

        void *found = 0;

        static int added = 0;
        if( added < MAX_CACHED_FILES ) {
            ++added;
        } else {
            // remove oldest cache entry
            for( archive_dir *prev = dir_cache, *dir = prev; dir ; prev = dir, dir = dir->next ) {
                if( !dir->next ) {
                    prev->next = 0; // break link
                    found = dir->data;
                    dir->path = REALLOC(dir->path, 0);
                    dir->data = REALLOC(dir->data, 0);
                    dir = REALLOC(dir, 0);
                    break;
                }
            }
        }

    thread_mutex_unlock(&cache_mutex);

    return found;
}
