#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#include "../alloc/arena.h"
#include "request.h"

void print_string_parcial(char *s, int inicio, int fim) {
  for (int i = inicio; i <= fim; i++) {
    printf("%c", s[i]);
  }
  printf("\n");
}

char *HTTP_ReceberMensagem(int accept_fd, size_t *total_bytes_recebidos) {
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
                        size_t tamanho_request_recebida, ArenaSimples *as) {
  for (int i = 0; i < tamanho_request_recebida; i++) {
    printf("%c", buf_request_recebida[i]);
  }
  printf("\n");

  // request-line
  int inicio = 0;
  int atual = 0;
  while (buf_request_recebida[atual] != ' ') {
    atual++;
  }
  char *method = ArenaS_AlocarSetTexto(as, &buf_request_recebida[inicio],
                                       atual - inicio + 1);
  method[atual - inicio + 1] = '\0';
  printf("%s\n", method);
  atual++;
  inicio = atual;

  while (buf_request_recebida[atual] == ' ') {
    atual++;
  }
  inicio = atual;

  while (buf_request_recebida[atual] != ' ') {
    atual++;
  }
  print_string_parcial(buf_request_recebida, inicio, atual - 1);
  atual++;
  inicio = atual;

  while (buf_request_recebida[atual] == ' ') {
    atual++;
  }
  inicio = atual;

  while (buf_request_recebida[atual] != ' ' &&
         buf_request_recebida[atual] != '\r' &&
         buf_request_recebida[atual] != '\n') {
    atual++;
  }
  print_string_parcial(buf_request_recebida, inicio, atual - 1);
  atual++;
  inicio = atual;
  return 0;
}
