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
  assert(
      strcmp(
          output->message,
          "HTTP/1.1 200 OK \r\n\r\n<html><h1>Servidor On-line!</h1></html>") ==
      0);

  free(as.buf);
  return 0;
}
