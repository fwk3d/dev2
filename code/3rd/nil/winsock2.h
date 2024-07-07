#pragma once
#include "in6addr.h"

typedef uint64_t SOCKET;

#define FD_SETSIZE 64

typedef struct fd_set {
  unsigned int  fd_count;
  SOCKET fd_array[FD_SETSIZE];
} fd_set, FD_SET, *PFD_SET, *LPFD_SET;

