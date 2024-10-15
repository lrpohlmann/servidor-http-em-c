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

  char expected[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html; "
                    "charset=UTF-8\r\nContent-Length: "
                    "39\r\n\r\n<html><h1>Servidor On-line!</h1></html>";

  int test = strcmp(output->message, expected);
  assert(test == 0);

  free(as.buf);
  return 0;
}
