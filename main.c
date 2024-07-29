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

void crash(const char *e) {
  perror(e);
  exit(EXIT_FAILURE);
}

void print_string_parcial(char *s, int inicio, int fim) {
  for (int i = inicio; i <= fim; i++) {
    printf("%c", s[i]);
  }
  printf("\n");
}

char *HTTP_ReceberMensagem(int accept_fd) {
  size_t tamanho_buf = 1024;
  char *buf_recv = (char *)malloc(tamanho_buf);
  ssize_t total_bytes_recebidos = 0;
  while (1) {
    ssize_t bytes_recebidos =
        recv(accept_fd, (void *)&buf_recv[total_bytes_recebidos], 1024, 0);
    if (bytes_recebidos == -1) {
      crash("recv");
    } else if (bytes_recebidos == 0) {
      break;
    } else if (bytes_recebidos < tamanho_buf) {
      total_bytes_recebidos += bytes_recebidos;
      break;
    } else {
      total_bytes_recebidos += bytes_recebidos;
      if (total_bytes_recebidos >= tamanho_buf) {
        tamanho_buf += tamanho_buf;
        char *novo_buf = (char *)realloc(buf_recv, tamanho_buf);
        buf_recv = novo_buf;
      }
    }
  }

  // request-line
  int inicio = 0;
  int atual = 0;
  while (true) {
    if (buf_recv[inicio] == '\r' || buf_recv[inicio] == '\n') {
      inicio++;
      atual = inicio;
    } else {
      while (true) {
        if (buf_recv[atual] == ' ') {
          break;
        }

        atual++;
      }

      print_string_parcial(buf_recv, inicio, atual);
      break;
    }
  }
  // headers
  /***
  for (int i = 0; i < total_bytes_recebidos; i++) {
    printf("%c", buf_recv[i]);
  }
  ***/

  return buf_recv;
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

  while (1) {
    struct sockaddr_storage endereco_cliente;
    socklen_t t = sizeof(endereco_cliente);
    int accept_fd = accept(sock_fd, (struct sockaddr *)&endereco_cliente, &t);
    if (accept_fd == -1) {
      perror("accept");
      continue;
    }

    char *buf_recv = HTTP_ReceberMensagem(accept_fd);
    free((void *)buf_recv);

    char resposta[] = "HTTP/1.1 200 OK\r\nContent-Type: "
                      "text/html\r\n\r\n<h1>Servidor HTTP, ao seu dispor</h1>";
    ssize_t bytes_enviados = send(accept_fd, resposta, strlen(resposta), 0);
    if (bytes_enviados == -1) {
      crash("send");
    }

    close(accept_fd);
  }

  freeaddrinfo(r);

  return 0;
}
