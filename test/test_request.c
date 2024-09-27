#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/alloc/arena.h"
#include "../src/http/request.h"

int main(int argc, char *argv[]) {
  char buf_recebido[] =
      "GET /user?name=leonardo HTTP/1.1\r\nHost: localhost:9500\r\nUser-Agent: "
      "Mozilla/5.0 (X11; "
      "Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0\r\nAccept: "
      "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/"
      "webp,image/png,image/svg+xml,*/*;q=0.8\r\nAccept-Language: "
      "en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate, br, "
      "zstd\r\nConnection: keep-alive\r\nUpgrade-Insecure-Requests: "
      "1\r\nSec-Fetch-Dest: document\r\nSec-Fetch-Mode: "
      "navigate\r\nSec-Fetch-Site: none\r\nSec-Fetch-User: ?1\r\nPriority: "
      "u=0, i\r\n\r\n";
  Request *request_obj = NULL;
  ErrorRequest *err_request = NULL;
  ArenaSimples as = {.posicao = 0, .capacidade = 2048};
  char *b = malloc(2048);
  as.buf = b;
  HTTP_ParseRequest(buf_recebido, strlen(buf_recebido), &request_obj,
                    &err_request, &as);
  assert(strcmp(request_obj->method, "GET") == 0);

  // url
  assert(strcmp(request_obj->url->segment, "/") == 0);
  assert(strcmp(request_obj->url->next->segment, "user") == 0);
  assert(request_obj->url->next->next == NULL);
  // querystring
  assert(strcmp(request_obj->querystring->key, "name") == 0);
  assert(strcmp(request_obj->querystring->value, "leonardo") == 0);
  assert(request_obj->querystring->next == NULL);

  assert(strcmp(request_obj->http_version, "HTTP/1.1") == 0);

  assert(strcmp(request_obj->general_header.connection, "keep-alive"));
  assert(strcmp(request_obj->request_header.host, "localhost:9500"));
  assert(strcmp(request_obj->request_header.user_agent,
                "Mozilla/5.0 (X11; Linux x86_64; rv:128.0) Gecko/20100101 "
                "Firefox/128.0"));
  assert(strcmp(request_obj->request_header.accept,
                "text/html,application/xhtml+xml,application/xml;q=0.9,image/"
                "avif,image/webp,image/png,image/svg+xml,*/*;q=0.8"));
  assert(strcmp(request_obj->request_header.accept_language, "en-US,en;q=0.5"));
  assert(strcmp(request_obj->request_header.accept_encoding,
                "gzip, deflate, br, zstd"));
  free(b);
}
