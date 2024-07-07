// -----------------------------------------------------------------------------
// udp wrapper
// - rlyeh, public domain.

// server
API int   udp_bind(const char *address, const char *port);

// client
API int   udp_open(const char *address, const char *port);

// common
API int   udp_send(int, const void *buf, int len ); // <0 error, >0 bytes sent ok
API int   udp_sendto(int, const char *ip, const char *port, const void *buf, int len ); // <0 error, >0 bytes sent ok
API int   udp_recv(int, void *buf, int len ); // <0 error, 0 orderly shutdown, >0 received bytes
API int   udp_peek(int); // <0 error, 0 timeout, >0 data

#if CODE

// -----------------------------------------------------------------------------

#define UDP_DEBUG 0

static int udp_init() {
    do_once {
        int rc = swrapInit(); // atexit(swrapTerminate);
        if( rc ) die("udp_init: swrapInit error");
    }
    return 1;
}

int udp_open(const char *address, const char *port) {
    do_once udp_init();
    int fd = swrapSocket(SWRAP_UDP, SWRAP_CONNECT, 0, address, port);
    // if( fd == -1 ) die("udp_open: swrapSocket error");
    return fd;
}

int udp_bind(const char *address, const char *port) {
    do_once udp_init();
    int fd = swrapSocket(SWRAP_UDP, SWRAP_BIND, 0, address, port);
    // if( fd == -1 ) die("udp_bind: swrapSocket error");
    return fd;
}

int udp_send( int fd, const void *buf, int len ) { // returns bytes sent, or -1 if error
    int rc = -1;
    if( fd >= 0 ) for( ;; ) {
        rc = swrapSend(fd, (const char *)buf, len);
#if is(win32)
        if( rc == -1 && WSAGetLastError() == WSAEINTR ) continue;
        else break;
#else
        if( rc == -1 && errno == EINTR ) continue;
        else break;
#endif
    }
#if UDP_DEBUG
    if( rc > 0 ) {
        char host[128], serv[128];
        int rc2 = swrapAddressInfo(&sa, host, 128, serv, 128 );
        if( rc2 != 0 ) die("swrapAddressInfo error");
        printf("udp_send: %d bytes to %s:%s : %.*s\n", rc, host, serv, rc, buf );
        hexdump(buf, rc);
    }
#endif
    return rc;
}

int udp_close( int fd ) { // @todo: expose me? needed?
#if is(win32)
    // closesocket(fd);
#else
    // close(fd);
#endif
    fd = -1; // noop
    return 0;
}

#if 0
// use socket to send data to another address
int udp_sendto( int fd, const char *ip, const char *port, const void *buf, int len ) { // return number of bytes sent
#if 0
    int rc = swrapSendTo(fd, struct swrap_addr*, (const char*)buf, len);
    if( rc == -1 ) return -1; //die("udp_send: swrapSend error");
    return rc;
#else
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    // use inet_addr. tcc(win32) wont work otherwise.
    addr.sin_addr.s_addr = inet_addr(ip); // inet_pton(AF_INET, ip, &addr.sin_addr);
    addr.sin_port = htons(atoi(port));

    int n = sendto(fd, buf, len, 0, (struct sockaddr *)&addr, sizeof(addr));
    return n < 0 ? -1 : n;
#endif
}
#endif

int udp_peek( int fd ) { // <0 error, 0 timeout, >0 data
    int rc = swrapSelect(fd, 0.00001);
    if( rc < 0 ) return -1; // die("udp_peek: swrapSelect error");
    if( rc == 0 ) return 0; // timeout
    return 1; //> 0: new data is available
}

int udp_recv( int fd, void *buf, int len ) { // <0 error, 0 orderly shutdown, >0 received bytes
    struct swrap_addr sa = {0};
    int rc = swrapReceiveFrom(fd, &sa, buf, len);
    if( rc < 0 ) return -1; // die("udp_recv: swrapReceiveFrom error");
    if( rc == 0 ) return 0; // orderly shutdown

#if UDP_DEBUG
    char host[128], serv[128];
    int rc2 = swrapAddressInfo(&sa, host, 128, serv, 128 );
    if( rc2 != 0 ) die("swrapAddressInfo error");
    printf("udp_recv: %d bytes from %s:%s : %.*s\n", rc, host, serv, rc, buf );
    hexdump(buf, rc);
#endif

    return rc;
}


AUTORUN {
    udp_init();
}

#endif
