#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "src/alloc/arena.h"
#include "src/http/request.h"
#include "src/http/response.h"

void crash(const char *e) {
  perror(e);
  exit(EXIT_FAILURE);
}

int main() {
  struct addrinfo base;
  memset(&base, 0, sizeof(base));
  base.ai_family = AF_INET;       // IPv4
  base.ai_socktype = SOCK_STREAM; // TCP
  base.ai_flags = AI_PASSIVE;

  struct addrinfo *r;

  int getaddrinfo_status = 0;
  getaddrinfo_status = getaddrinfo(NULL, "9500", &base, &r);
  if (getaddrinfo_status != 0) {
    crash("getaddrinfo");
  }

  struct addrinfo *n;
  n = r;
  while (n != NULL) {
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)n->ai_addr;
    char addr[INET_ADDRSTRLEN];
    inet_ntop(n->ai_family, &ipv4->sin_addr, addr, INET_ADDRSTRLEN);
    printf("%s\n", addr);
    n = n->ai_next;
  }

  int sock_fd = socket(base.ai_family, base.ai_socktype, base.ai_protocol);
  printf("%d\n", sock_fd);

  int bind_status = 0;
  bind_status = bind(sock_fd, r->ai_addr, r->ai_addrlen);
  if (bind_status != 0) {
    crash("bind");
  }

  int listen_status = 0;
  listen_status = listen(sock_fd, 5);
  if (listen_status != 0) {
    crash("listen");
  }

  char *buf = (char *)malloc(2048);
  ArenaSimples arena = {.buf = buf, .posicao = 0, .capacidade = 2048};
  while (1) {
    struct sockaddr_storage endereco_cliente;
    socklen_t t = sizeof(endereco_cliente);
    int accept_fd = accept(sock_fd, (struct sockaddr *)&endereco_cliente, &t);
    if (accept_fd == -1) {
      perror("accept");
      continue;
    }

    size_t bytes_recebidos = 0;
    char *buf_recv = HTTP_ReceiveRequest(accept_fd, &bytes_recebidos);
    if (buf_recv == NULL) {
      // browser doesn't show this. Says that the connection was reset
      char resposta[] = "HTTP/1.1 413 Payload Too Large\r\nContent-Type: "
                        "text/html\r\n\r\n<html><body><h1>HTTP 413 - Payload "
                        "Too Large</h1></body></html>";
      ssize_t bytes_enviados = send(accept_fd, resposta, strlen(resposta), 0);
      if (bytes_enviados == -1) {
        crash("send");
      }
      close(accept_fd);
      arena.posicao = 0;
      continue;
    }

    ErrorRequest *err_request;
    Request *request_obj;
    int status_analise_request = HTTP_ParseRequest(
        buf_recv, bytes_recebidos, &request_obj, &err_request, &arena);
    if (status_analise_request != 0) {
      printf("ERRO\n");
    }

    free((void *)buf_recv);

    ArenaSimples as_response = {
        .buf = malloc(500), .posicao = 0, .capacidade = 500};
    ResponseOutput *respose = Home(request_obj, &as_response);

    ssize_t bytes_enviados =
        send(accept_fd, respose->message, respose->b_size, 0);
    if (bytes_enviados == -1) {
      crash("send");
    }

    free(as_response.buf);

    close(accept_fd);
    arena.posicao = 0;
  }

  free((void *)buf);
  freeaddrinfo(r);

  return 0;
}
