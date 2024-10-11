#include <assert.h>
#include <stdlib.h>

#include "../src/alloc/arena.h"
#include "../src/http/request.h"
#include "../src/http/response.h"

int main(void) {
  Request *request;
  ArenaSimples as;
  as.buf = malloc(2000);
  as.posicao = 0;
  as.capacidade = 2000;
  ResponseOutput *output = Home(request, &as);
  assert(output != NULL);

  free(as.buf);
  return 0;
}
