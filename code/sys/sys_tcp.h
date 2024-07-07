// -----------------------------------------------------------------------------
// tcp wrapper
// - rlyeh, public domain

// client
API int   tcp_open(const char *address, const char *port);

// server
API int   tcp_bind(const char *interface_, const char *port, int queue);
API int   tcp_peek(int, int(*callback)(int));

// common
API int   tcp_send(int, const void* buf, int len);
API int   tcp_recv(int, void* buf, int len);
API char* tcp_host(int); // info
API char* tcp_port(int); // info

API int   tcp_close(int);

// extras
API int   tcp_debug(int); // toggle traffic monitoring on/off for given socket
//API int   tcp_printf(int, const char *fmt, ...); // printf message in remote end
//API int   tcp_crypt(int,uint64_t);               // set shared secret

#if CODE

// -----------------------------------------------------------------------------

#define TCP_DEBUG 1

#if TCP_DEBUG
static set(int) tcp_set;
#endif

void tcp_init(void) {
    do_once {
        udp_init();
#if TCP_DEBUG
        set_init(tcp_set, less_int, hash_int);
#endif
    }
}
int tcp_open(const char *address, const char *port) {
    do_once tcp_init();
    int fd = swrapSocket(SWRAP_TCP, SWRAP_CONNECT, 0/*|SWRAP_NODELAY*/, address, port);
    return fd;
}
int tcp_bind(const char *interface_, const char *port, int backlog) {
    do_once tcp_init();
    int fd = swrapSocket(SWRAP_TCP, SWRAP_BIND, 0/*|SWRAP_NODELAY*//*|SWRAP_NOBLOCK*/, interface_, port);
    if( fd >= 0 ) swrapListen(fd, backlog);
    return fd;
}
int tcp_peek(int fd, int(*callback)(int)) {
    struct swrap_addr sa;
    int fd2 = swrapAccept(fd, &sa);
    if( fd2 >= 0 ) return callback(fd2);
    return -1;
}
int tcp_send(int fd, const void *buf, int len) {
    int rc = swrapSend(fd, (const char *)buf, len);
#if TCP_DEBUG
    if( set_find(tcp_set, fd) ) {
        printf("send -> %11d (status: %d) %s:%s\n", len, rc, tcp_host(fd), tcp_port(fd));
        if( rc > 0 ) hexdump(buf, rc);
    }
#endif
    return rc;
}
int tcp_recv(int fd, void *buf, int len) {
    int rc = swrapReceive(fd, (char*)buf, len);
#if TCP_DEBUG
    if( rc != 0 && set_find(tcp_set, fd) ) {
        printf("recv <- %11d (status: %d) %s:%s\n", len, rc, tcp_host(fd), tcp_port(fd));
        if( rc > 0 ) hexdump(buf, rc);
    }
#endif
    return rc;
}
char* tcp_host(int fd) {
    char buf[1024];
    struct swrap_addr sa;
    swrapAddress(fd, &sa);
    swrapAddressInfo(&sa, buf, 512, buf+512, 512);
    return va("%s", buf);
}
char* tcp_port(int fd) {
    char buf[1024];
    struct swrap_addr sa;
    swrapAddress(fd, &sa);
    swrapAddressInfo(&sa, buf, 512, buf+512, 512);
    return va("%s", buf+512);
}
int tcp_close(int fd) {
    swrapClose(fd);
    return 0;
}
int tcp_debug(int fd) {
#if TCP_DEBUG
    if( set_find(tcp_set, fd) ) {
        set_erase(tcp_set, fd);
        return 0;
    } else {
        set_insert(tcp_set, fd);
        return 1;
    }
#else
    return 0;
#endif
}

AUTORUN {
	tcp_init();
}

#endif