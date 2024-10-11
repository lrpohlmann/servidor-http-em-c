#include <string.h>

#include "../alloc/arena.h"
#include "request.h"
#include "response.h"

ResponseOutput *Home(Request *request, ArenaSimples *arena) {
  ResponseOutput *rout =
      (ResponseOutput *)ArenaS_Alocar(arena, sizeof(ResponseOutput));

  char *res = "HTTP/1.1 200 OK \r\n\r\n<html><h1>Servidor On-line!</h1></html>";
  unsigned long len_res = strlen(res);
  char *http = ArenaS_Alocar(arena, len_res);
  strncpy(http, res, len_res);

  rout->message = http;
  rout->b_size = len_res;
  return rout;
}
