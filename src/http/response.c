#include <stdio.h>
#include <string.h>

#include "../alloc/arena.h"
#include "request.h"
#include "response.h"

ResponseOutput *Home(Request *request, ArenaSimples *arena) {
  ResponseOutput *rout =
      (ResponseOutput *)ArenaS_Alocar(arena, sizeof(ResponseOutput));

  char start_line[] = "HTTP/1.1 200 OK\r\n";
  char end_of_header_separator[] = "\r\n";

  char message_body[] = "<html><h1>Servidor On-line!</h1></html>";
  size_t content_length = strlen(message_body);
  char headers[100];
  snprintf(headers, 100,
           "Content-Type: text/html; charset=UTF-8\r\nContent-Length: %zu\r\n",
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
