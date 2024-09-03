#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/alloc/arena.h"
#include "../src/http/request.h"

int main(int argc, char *argv[]) {
  char received_request[] = "GETTTT";
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
}
