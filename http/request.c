#include <arpa/inet.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../alloc/arena.h"
#include "request.h"

static void init_request_obj(Request *request) {
  request->url = NULL;
  request->method = NULL;
  request->http_version = NULL;
}

static SegmentoUrl *adicionar_segmento_url(Request *request, char *url,
                                           ArenaSimples *as) {
  SegmentoUrl *novo_segmento =
      (SegmentoUrl *)ArenaS_Alocar(as, sizeof(SegmentoUrl));
  novo_segmento->segmento = url;
  if (request->url == NULL) {
    request->url = novo_segmento;
    return novo_segmento;
  }

  SegmentoUrl *atual = request->url;
  while (atual->proximo != NULL) {
    atual = atual->proximo;
  }
  atual->proximo = novo_segmento;
  return novo_segmento;
}

static int analizar_url(char *url, int tamanho, Request *request_obj,
                        ArenaSimples *as) {
  if (tamanho == 1) {
    char *segmento = (char *)ArenaS_Alocar(as, 2);
    strncpy(segmento, "/", 2);
    adicionar_segmento_url(request_obj, segmento, as);
    return 0;
  }

  int inicio = 0;
  int fim = 0;
  while (inicio < tamanho) {
    if (url[inicio] == '/') {
      char *segmento = (char *)ArenaS_Alocar(as, 2);
      strncpy(segmento, "/", 2);
      adicionar_segmento_url(request_obj, segmento, as);
      inicio++;
      if (url[inicio] == '?') {
        return 0;
      }
      continue;
    }

    fim = inicio;
    while (true) {
      if (url[fim] == '/') {
        char *segmento = (char *)ArenaS_Alocar(as, fim - inicio + 1);
        strncpy(segmento, &url[inicio], fim - inicio);
        segmento[fim - inicio] = '\0';
        adicionar_segmento_url(request_obj, segmento, as);
        inicio = fim;
        break;
      } else if (url[fim] == '?' || fim >= tamanho) {
        char *segmento = (char *)ArenaS_Alocar(as, fim - inicio + 1);
        strncpy(segmento, &url[inicio], fim - inicio);
        segmento[fim - inicio] = '\0';
        adicionar_segmento_url(request_obj, segmento, as);
        return 0;
      }

      fim++;
    }
  }

  return 0;
}

char *HTTP_ReceberRequest(int accept_fd, size_t *total_bytes_recebidos) {
  size_t tamanho_buf = 1024;
  char *buf_recv = (char *)malloc(tamanho_buf);
  *total_bytes_recebidos = 0;
  while (1) {
    ssize_t bytes_recebidos =
        recv(accept_fd, (void *)&buf_recv[*total_bytes_recebidos], 1024, 0);
    if (bytes_recebidos == -1) {
      perror("recv");
      exit(EXIT_FAILURE);
    } else if (bytes_recebidos == 0) {
      break;
    } else if (bytes_recebidos < tamanho_buf) {
      *total_bytes_recebidos += bytes_recebidos;
      break;
    } else {
      *total_bytes_recebidos += bytes_recebidos;
      if (*total_bytes_recebidos >= tamanho_buf) {
        tamanho_buf += tamanho_buf;
        char *novo_buf = (char *)realloc(buf_recv, tamanho_buf);
        buf_recv = novo_buf;
      }
    }
  }

  return buf_recv;
}

int HTTP_AnaliseRequest(char *buf_request_recebida,
                        size_t tamanho_request_recebida, Request **request_obj,
                        ErroRequest **err_request, ArenaSimples *as) {
  for (int i = 0; i < tamanho_request_recebida; i++) {
    printf("%c", buf_request_recebida[i]);
  }
  printf("\n");

  *request_obj = (Request *)ArenaS_Alocar(as, sizeof(Request));
  init_request_obj(*request_obj);
  *err_request = NULL;

  // request-line
  int inicio = 0;
  int atual = 0;

  /*
   * method
   */
  {
    while (buf_request_recebida[atual] != ' ') {
      atual++;
    }
    char *method = ArenaS_Alocar(as, atual - inicio + 1);
    strncpy(method, &buf_request_recebida[inicio], atual - inicio);
    method[atual - inicio] = '\0';

    switch (strcmp(method, "GET")) {
    case 0:
      (*request_obj)->method = method;
      break;
    default:
      *request_obj = NULL;
      *err_request = (ErroRequest *)ArenaS_Alocar(as, sizeof(ErroRequest));
      strncpy((*err_request)->status, "501", 4);
      char err_msg[] = "Método não suportado/conhecido.";
      (*err_request)->descricao = (char *)ArenaS_Alocar(as, strlen(err_msg));
      strncpy((*err_request)->descricao, err_msg, strlen(err_msg));
      return -1;
    }

    atual++;
    inicio = atual;
  }

  /*
   * url
   */
  {
    while (buf_request_recebida[atual] == ' ') {
      atual++;
    }
    inicio = atual;

    while (buf_request_recebida[atual] != ' ') {
      atual++;
    }
    char *url = (char *)ArenaS_Alocar(as, atual - inicio + 1);
    strncpy(url, &buf_request_recebida[inicio], atual - inicio);
    url[atual - inicio] = '\0';
    analizar_url(url, atual - inicio, *request_obj, as);

    atual++;
    inicio = atual;
  }

  /*
   * version
   */
  {
    while (buf_request_recebida[atual] == ' ') {
      atual++;
    }
    inicio = atual;

    while (buf_request_recebida[atual] != ' ' &&
           buf_request_recebida[atual] != '\r' &&
           buf_request_recebida[atual] != '\n') {
      atual++;
    }
    char *http_version = ArenaS_Alocar(as, atual - inicio + 1);
    strncpy(http_version, &buf_request_recebida[inicio], atual - inicio);
    http_version[atual - inicio] = '\0';
    (*request_obj)->http_version = http_version;

    atual++;
    inicio = atual;
  }

  assert((*request_obj)->method != NULL);
  assert((*request_obj)->url != NULL);
  assert((*request_obj)->http_version != NULL);

  return 0;
}
