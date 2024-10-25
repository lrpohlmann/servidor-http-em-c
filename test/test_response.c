#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../src/alloc/arena.h"
#include "../src/http/request.h"
#include "../src/http/response.h"

int main(void) {
  Request request;
  RequestInit(&request);
  ArenaSimples as;
  as.buf = malloc(2000);
  as.posicao = 0;
  as.capacidade = 2000;
  ResponseOutput *output = Home(&request, &as);

  char expected[] =
      "HTTP/1.1 200 OK\r\nServer: MulaServer/0.1\r\nContent-Type: text/html; "
      "charset=UTF-8\r\nContent-Length: "
      "67\r\n\r\n<html><head></head><body><h1>MulaServer/0.1 "
      "on!</h1></body></html>\n";

  int test = strcmp(output->message, expected);
  assert(test == 0);

  free(as.buf);
  return 0;
}
