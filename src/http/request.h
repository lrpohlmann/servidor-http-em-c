#pragma once
#include <stdlib.h>

#include "../alloc/arena.h"

typedef struct nodo_segmento_url {
  char *segmento;
  struct nodo_segmento_url *proximo;
} SegmentoUrl;

typedef struct {
  char *cache_control;
  char *connection;
  char *date;
  char *pragma;
  char *trailer;
  char *transfer_encoding;
  char *upgrade;
  char *via;
  char *warning;
} GeneralHeader;

typedef struct {
  char *accept;
  char *accept_charset;
  char *accept_encoding;
  char *accept_language;
  char *authorization;
  char *expect;
  char *from;
  char *host;
  char *if_match;
  char *if_modified_since;
  char *if_none_match;
  char *if_range;
  char *if_unmodified_since;
  char *max_forwards;
  char *proxy_authorization;
  char *range;
  char *referer;
  char *te;
  char *user_agent;
} RequestHeader;

typedef struct {
  char *method;
  SegmentoUrl *url;
  char *http_version;
  GeneralHeader general_header;
  RequestHeader request_header;
} Request;

typedef struct {
  char status[4];
  char *descricao;
} ErroRequest;

char *HTTP_ReceberRequest(int accept_fd, size_t *total_bytes_recebidos);

int HTTP_AnaliseRequest(char *buf_request_recebida,
                        size_t tamanho_request_recebida, Request **request_obj,
                        ErroRequest **err_request, ArenaSimples *as);
