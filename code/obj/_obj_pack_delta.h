// ----------------------------------------------------------------------------
// delta en/decoder

API void delta8_encode(void *buffer, unsigned count);
API void delta8_decode(void *buffer, unsigned count);

API void delta16_encode(void *buffer, unsigned count);
API void delta16_decode(void *buffer, unsigned count);

API void delta32_encode(void *buffer, unsigned count);
API void delta32_decode(void *buffer, unsigned count);

API void delta64_encode(void *buffer, unsigned count);
API void delta64_decode(void *buffer, unsigned count);

// ----------------------------------------------------------------------------
// delta encoder

#define delta_expand_template(N) \
void delta##N##_encode(void *buffer_, unsigned count) { \
    uint##N##_t current, last = 0, *buffer = (uint##N##_t*)buffer_; \
    for( unsigned i = 0; i < count; i++ ) { \
        current = buffer[i]; \
        buffer[i] = current - last; \
        last = current; \
    } \
} \
void delta##N##_decode(void *buffer_, unsigned count) { \
    uint##N##_t delta, last = 0, *buffer = (uint##N##_t*)buffer_; \
    for( unsigned i = 0; i < count; i++ ) { \
        delta = buffer[i]; \
        buffer[i] = delta + last; \
        last = buffer[i]; \
    } \
}
delta_expand_template(8);
delta_expand_template(16);
delta_expand_template(32);
delta_expand_template(64);

#if 0
AUTORUN {
    char buf[] = "1231112223345555";
    int buflen = strlen(buf);

    char *dt = strdup(buf);
    printf("  delta8: ", dt);
    for( int i = 0; i < buflen; ++i ) printf("%c", dt[i] );
    printf("->");
    delta8_encode(dt, buflen);
    for( int i = 0; i < buflen; ++i ) printf("%02d,", dt[i] );
    printf("->");
    delta8_decode(dt, buflen);
    for( int i = 0; i < buflen; ++i ) printf("%c", dt[i] );
    printf("\r%c\n", 0 == strcmp(buf,dt) ? 'Y':'N');
}
#endif

