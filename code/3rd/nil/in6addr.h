#pragma once
typedef struct in6_addr {
  union {
    uint8_t  Byte[16];
    uint16_t Word[8];
  } u;
} IN6_ADDR, *PIN6_ADDR, *LPIN6_ADDR;
