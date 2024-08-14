#pragma once
#include <stdlib.h>

#include "../alloc/arena.h"

typedef struct nodo_segmento_url {
  char *segmento;
  struct nodo_segmento_url *proximo;
} SegmentoUrl;

typedef struct {
  char *method;
  SegmentoUrl *url;
  char *http_version;
} Request;

typedef struct {
  char status[4];
  char *descricao;
} ErroRequest;

char *HTTP_ReceberRequest(int accept_fd, size_t *total_bytes_recebidos);

int HTTP_AnaliseRequest(char *buf_request_recebida,
                        size_t tamanho_request_recebida, Request **request_obj,
                        ErroRequest **err_request, ArenaSimples *as);