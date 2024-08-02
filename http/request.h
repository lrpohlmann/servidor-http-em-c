#pragma once
#include <stdlib.h>

#include "../alloc/arena.h"

typedef struct {
  char *method;
  char *url;
  char *http_version;
} Request;

char *HTTP_ReceberMensagem(int accept_fd, size_t *total_bytes_recebidos);

int HTTP_AnaliseRequest(char *buf_request_recebida,
                        size_t tamanho_request_recebida, ArenaSimples *as);
