#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../alloc/arena.h"
#include "request.h"
#include "response.h"

static char *read_html(char *html_filepath, ArenaSimples *arena,
                       ssize_t *html_size) {
  int fd_html = open(html_filepath, 0, O_RDONLY);
  if (fd_html == -1) {
    perror("open");
    exit(1);
  }

  int block_size = 100;
  ssize_t total_bytes_read = 0;
  char *ptr_html = ArenaS_Alocar(arena, block_size);
  char *current_ptr_html = ptr_html;
  while (1) {
    ssize_t bytes_read = read(fd_html, current_ptr_html, block_size);
    if (bytes_read == -1) {
      perror("read");
      exit(1);
    } else if (bytes_read >= 0 && bytes_read < block_size) {
      total_bytes_read += bytes_read;
      break;
    } else {
      total_bytes_read += bytes_read;
      current_ptr_html = ArenaS_Alocar(arena, block_size);
    }
  }

  *html_size = total_bytes_read;
  return ptr_html;
}

ResponseOutput *Home(Request *request, ArenaSimples *arena) {
  ResponseOutput *rout =
      (ResponseOutput *)ArenaS_Alocar(arena, sizeof(ResponseOutput));

  char start_line[] = "HTTP/1.1 200 OK\r\n";
  char end_of_header_separator[] = "\r\n";

  ssize_t html_size = 0;
  char *ptr_html = read_html("./html/index.html", arena, &html_size);

  char *message_body = ptr_html;
  size_t content_length = html_size;
  char headers[300];
  snprintf(headers, 300,
           "Server: MulaServer/0.1\r\nContent-Type: text/html; "
           "charset=UTF-8\r\nContent-Length: %zu\r\n",
           content_length);

  size_t start_line_length = strlen(start_line);
  size_t headers_length = strlen(headers);
  size_t end_of_header_length = 2;
  size_t response_length = start_line_length + headers_length +
                           end_of_header_length + content_length;

  char *http = ArenaS_Alocar(arena, response_length + 1);
  int http_ptr_position = 0;
  strncpy(&http[http_ptr_position], start_line, start_line_length);
  http_ptr_position += start_line_length;

  strncpy(&http[http_ptr_position], headers, headers_length);
  http_ptr_position += headers_length;
  strncpy(&http[http_ptr_position], end_of_header_separator,
          end_of_header_length);
  http_ptr_position += end_of_header_length;
  strncpy(&http[http_ptr_position], message_body, content_length);
  http[response_length] = '\0';

  rout->message = http;
  rout->b_size = response_length;
  return rout;
}

ResponseOutput *NotFound404View(Request *request, ArenaSimples *arena) {
  ResponseOutput *output =
      (ResponseOutput *)ArenaS_Alocar(arena, sizeof(ResponseOutput));
  char start_line[] =
      "HTTP/1.1 404 Not Found\r\nServer: MulaServer/0.1\r\nContent-Type: "
      "text/html; charset=UTF-8\r\n\r\n<html><head></head><body><h1>404 Não "
      "Encontrado</h1></body></html>";
  size_t start_line_length = strlen(start_line);

  char *http = ArenaS_Alocar(arena, start_line_length);
  strncpy(http, start_line, start_line_length);

  output->b_size = start_line_length;
  output->message = http;

  return output;
}
