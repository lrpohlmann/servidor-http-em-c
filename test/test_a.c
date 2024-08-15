#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/alloc/arena.h"
#include "../src/http/request.h"

int main(void) {
  printf("%s\n", __FILE__);
  char buf_recebido[] =
      "GET / HTTP/1.1\r\nHost: localhost:9500\r\nUser-Agent: Mozilla/5.0 (X11; "
      "Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0\r\nAccept: "
      "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/"
      "webp,image/png,image/svg+xml,*/*;q=0.8\r\nAccept-Language: "
      "en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate, br, "
      "zstd\r\nConnection: keep-alive\r\nUpgrade-Insecure-Requests: "
      "1\r\nSec-Fetch-Dest: document\r\nSec-Fetch-Mode: "
      "navigate\r\nSec-Fetch-Site: none\r\nSec-Fetch-User: ?1\r\nPriority: "
      "u=0, i\r\n\r\n";
  Request *request_obj = NULL;
  ErroRequest *err_request = NULL;
  ArenaSimples as = {.posicao = 0, .capacidade = 2048};
  char *b = malloc(2048);
  as.buf = b;
  HTTP_AnaliseRequest(buf_recebido, strlen(buf_recebido), &request_obj,
                      &err_request, &as);
  assert(strcmp(request_obj->method, "GET") == 0);
  assert(strcmp(request_obj->url->segmento, "/") == 0);
  assert(strcmp(request_obj->http_version, "HTTP/1.1") == 0);

  assert(strcmp(request_obj->general_header.connection, "keep-alive"));
  free(b);
  printf("%s ... OK\n", __FILE__);
}
