#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/alloc/arena.h"
#include "../src/http/request.h"

int main(int argc, char *argv[]) {
  char *malformed_requests[] = {"GETTTT",
                                "GET /yyyyy",
                                "GET /yyyyy HTTP",
                                "GET / HTTP1.1",
                                "GET / HTTP1.1\r\nHost:",
                                "GET / HTTP1.1\r\nHost: localhost:9500",
                                "GET / HTTP1.1\r\nHost: localhost:9500\r\n",
                                "GET / HTTP1.1\r\n",
                                NULL};
  int i = 0;
  while (malformed_requests[i] != NULL) {
    char *received_request = malformed_requests[i];
    Request *request_obj = NULL;
    ErrorRequest *err_request = NULL;
    ArenaSimples as = {.posicao = 0, .capacidade = 2048};
    char *b = malloc(2048);
    as.buf = b;

    HTTP_ParseRequest(received_request, strlen(received_request), &request_obj,
                      &err_request, &as);

    assert(err_request != NULL);
    assert(strcmp(err_request->status, "400") == 0);

    free(b);

    i++;
  }
}
