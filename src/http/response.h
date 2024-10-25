#pragma once

#include "../alloc/arena.h"
#include "request.h"

typedef struct {
  char *accept_ranges;
  char *age;
  char *etag;
  char *location;
  char *proxy_authenticate;
  char *retry_after;
  char *server;
  char *vary;
  char *www_authenticate;
} ResponseHeader;

typedef struct {
  // entity header
  // general header
  ResponseHeader response_header;
  char *message_body;
} Response;

typedef struct {
  char *message;
  unsigned long b_size;
} ResponseOutput;

ResponseOutput *Home(Request *request, ArenaSimples *as);

ResponseOutput *NotFound404View(Request *request, ArenaSimples *arena);
