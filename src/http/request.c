#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../alloc/arena.h"
#include "request.h"

/*
 * internals
 */

static void init_request_obj(Request *request) {
  request->url = NULL;
  request->method = NULL;
  request->http_version = NULL;
  request->querystring = NULL;
  request->request_header = (RequestHeader){
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  request->general_header =
      (GeneralHeader){NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
}

static UrlSegment *add_url_segment(Request *request, char *url,
                                   ArenaSimples *as) {
  UrlSegment *new_segment = (UrlSegment *)ArenaS_Alocar(as, sizeof(UrlSegment));
  new_segment->segment = url;
  if (request->url == NULL) {
    request->url = new_segment;
    return new_segment;
  }

  UrlSegment *actual = request->url;
  while (actual->next != NULL) {
    actual = actual->next;
  }
  actual->next = new_segment;
  return new_segment;
}

static int parse_url(char *url, int size, Request *request_obj,
                     ArenaSimples *as) {
  if (size == 1) {
    char *segment = (char *)ArenaS_Alocar(as, 2);
    strncpy(segment, "/", 2);
    add_url_segment(request_obj, segment, as);
    return 0;
  }

  int start = 0;
  int end = 0;
  while (start < size) {
    if (url[start] == '/') {
      char *segment = (char *)ArenaS_Alocar(as, 2);
      strncpy(segment, "/", 2);
      add_url_segment(request_obj, segment, as);
      start++;
      end = start;
    } else if (url[start] == '?') {
      start++;
      end = start;
      while (true) {
        if (start >= size) {
          return 0;
        }

        while (url[end] != '=') {
          end++;
          if (end >= size) {
            return -1;
          }
        }

        char param_name[end - start + 1];
        strncpy(param_name, &url[start], end - start);
        param_name[end - start] = '\0';

        end++;
        start = end;
        if (end >= size) {
          return -1;
        }

        while (url[end] != '&' && end < size) {
          end++;
        }

        char param_value[end - start + 1];
        strncpy(param_value, &url[start], end - start);
        param_value[end - start] = '\0';

        end++;
        start = end;

        QueryString_Set(request_obj, param_name, param_value, as);
      }
    } else {
      while (url[end] != '/' && url[end] != '?' && end < size) {
        end++;
      }
      char *segment = ArenaS_Alocar(as, end - start + 1);
      strncpy(segment, &url[start], end - start);
      segment[end - start] = '\0';
      add_url_segment(request_obj, segment, as);

      start = end;
    }
    continue;
  }

  end = start;
  while (true) {
    if (url[end] == '/') {
      char *segment = (char *)ArenaS_Alocar(as, end - start + 1);
      strncpy(segment, &url[start], end - start);
      segment[end - start] = '\0';
      add_url_segment(request_obj, segment, as);
      start = end;
      break;
    } else if (url[end] == '?' || end >= size) {
      char *segment = (char *)ArenaS_Alocar(as, end - start + 1);
      strncpy(segment, &url[start], end - start);
      segment[end - start] = '\0';
      add_url_segment(request_obj, segment, as);
      return 0;
    }

    end++;
  }
  return 0;
}

static void validate_and_set_headers(Request *request_obj, char *field_name,
                                     char *field_value, ArenaSimples *as) {
  unsigned long field_name_size = strlen(field_name);
  char *lower_field_name = ArenaS_Alocar(as, field_name_size);

  int i = 0;
  while (field_name[i] != '\0') {
    lower_field_name[i] = tolower(field_name[i]);
    i++;
  }
  lower_field_name[field_name_size] = '\0';

  /* general header */
  if (strcmp(lower_field_name, "cache-control") == 0) {
    request_obj->general_header.cache_control = lower_field_name;
  } else if (strcmp(lower_field_name, "connection") == 0) {
    request_obj->general_header.connection = lower_field_name;
  } else if (strcmp(lower_field_name, "date") == 0) {
    request_obj->general_header.date = lower_field_name;
  } else if (strcmp(lower_field_name, "pragma") == 0) {
    request_obj->general_header.pragma = lower_field_name;
  } else if (strcmp(lower_field_name, "trailer") == 0) {
    request_obj->general_header.trailer = lower_field_name;
  } else if (strcmp(lower_field_name, "transfer-encoding") == 0) {
    request_obj->general_header.transfer_encoding = lower_field_name;
  } else if (strcmp(lower_field_name, "upgrade") == 0) {
    request_obj->general_header.upgrade = lower_field_name;
  } else if (strcmp(lower_field_name, "via") == 0) {
    request_obj->general_header.via = lower_field_name;
  } else if (strcmp(lower_field_name, "warning") == 0) {
    request_obj->general_header.warning = lower_field_name;
  }

  /* request header */
  else if (strcmp(lower_field_name, "accept") == 0) {
    request_obj->request_header.accept = lower_field_name;
  } else if (strcmp(lower_field_name, "accept-charset") == 0) {
    request_obj->request_header.accept_charset = lower_field_name;
  } else if (strcmp(lower_field_name, "accept-encoding") == 0) {
    request_obj->request_header.accept_encoding = lower_field_name;
  } else if (strcmp(lower_field_name, "accept-language") == 0) {
    request_obj->request_header.accept_language = lower_field_name;
  } else if (strcmp(lower_field_name, "authorization") == 0) {
    request_obj->request_header.authorization = lower_field_name;
  } else if (strcmp(lower_field_name, "expect") == 0) {
    request_obj->request_header.expect = lower_field_name;
  } else if (strcmp(lower_field_name, "from") == 0) {
    request_obj->request_header.from = lower_field_name;
  } else if (strcmp(lower_field_name, "host") == 0) {
    request_obj->request_header.host = lower_field_name;
  } else if (strcmp(lower_field_name, "if-match") == 0) {
    request_obj->request_header.if_match = lower_field_name;
  } else if (strcmp(lower_field_name, "if-modified-since") == 0) {
    request_obj->request_header.if_modified_since = lower_field_name;
  } else if (strcmp(lower_field_name, "if-none-match") == 0) {
    request_obj->request_header.if_none_match = lower_field_name;
  } else if (strcmp(lower_field_name, "if-range") == 0) {
    request_obj->request_header.if_range = lower_field_name;
  } else if (strcmp(lower_field_name, "if-unmodified-since") == 0) {
    request_obj->request_header.if_unmodified_since = lower_field_name;
  } else if (strcmp(lower_field_name, "max-forwards") == 0) {
    request_obj->request_header.max_forwards = lower_field_name;
  } else if (strcmp(lower_field_name, "proxy-authorization") == 0) {
    request_obj->request_header.proxy_authorization = lower_field_name;
  } else if (strcmp(lower_field_name, "range") == 0) {
    request_obj->request_header.range = lower_field_name;
  } else if (strcmp(lower_field_name, "referer") == 0) {
    request_obj->request_header.referer = lower_field_name;
  } else if (strcmp(lower_field_name, "te") == 0) {
    request_obj->request_header.te = lower_field_name;
  } else if (strcmp(lower_field_name, "user-agent") == 0) {
    request_obj->request_header.user_agent = lower_field_name;
  }
}

static void set_request_error(Request **request_obj, ErrorRequest **err_request,
                              char *http_status_code, char *error_message,
                              ArenaSimples *as) {
  *request_obj = NULL;
  *err_request = (ErrorRequest *)ArenaS_Alocar(as, sizeof(ErrorRequest));
  assert(strlen(http_status_code) == 3);

  unsigned long len_error_message = strlen(error_message);
  char *copy_error_message = ArenaS_Alocar(as, len_error_message);

  strncpy((*err_request)->status, http_status_code, 4);
  strncpy(copy_error_message, error_message, len_error_message);

  (*err_request)->descricao = copy_error_message;
}

/*
 * public interface
 */

int QueryString_Get(Request *request, char *key, char **ptr_found_value) {
  *ptr_found_value = NULL;
  QuerystringKeyValue *current = request->querystring;
  while (current != NULL) {
    if (strcmp(key, current->key) == 0) {
      *ptr_found_value = current->key;
      return 0;
    }

    current = current->next;
  }

  return -1;
}

void QueryString_Set(Request *request, char *key, char *value,
                     ArenaSimples *as) {
  assert(key != NULL);
  assert(value != NULL);

  int len_key = strlen(key);
  int len_value = strlen(value);

  if (request->querystring == NULL) {
    QuerystringKeyValue *new_key_value =
        (QuerystringKeyValue *)ArenaS_Alocar(as, sizeof(QuerystringKeyValue));
    char *new_key = ArenaS_Alocar(as, len_key + 1);
    char *new_value = ArenaS_Alocar(as, len_value + 1);
    strncpy(new_key, key, len_key + 1);
    new_key[len_key] = '\0';
    strncpy(new_value, value, len_value + 1);
    new_value[len_value] = '\0';
    new_key_value->key = new_key;
    new_key_value->value = new_value;

    request->querystring = new_key_value;
    return;
  }

  QuerystringKeyValue *current = request->querystring;
  while (current != NULL) {
    if (strcmp(key, current->key) == 0) {
      char *new_value = ArenaS_Alocar(as, len_value);
      strncpy(new_value, value, len_value);
      current->value = new_value;
      return;
    } else if (current->next == NULL) {
      QuerystringKeyValue *new_key_value =
          (QuerystringKeyValue *)ArenaS_Alocar(as, sizeof(QuerystringKeyValue));
      char *new_key = ArenaS_Alocar(as, len_key);
      char *new_value = ArenaS_Alocar(as, len_value);
      strncpy(new_key, key, len_key);
      strncpy(new_value, value, len_value);
      new_key_value->key = new_key;
      new_key_value->value = new_value;
      current->next = new_key_value;
      return;
    }

    current = current->next;
  }
}

char *HTTP_ReceiveRequest(int accept_fd, size_t *total_recv_received_bytes) {
  size_t buffer_size = REQUEST_BUFFER_BASE_SIZE;
  char *buffer = (char *)malloc(buffer_size);

  *total_recv_received_bytes = 0;
  while (1) {

    ssize_t received_bytes =
        recv(accept_fd, (void *)&buffer[*total_recv_received_bytes],
             REQUEST_BUFFER_BASE_SIZE, 0);
    if (received_bytes == -1) {
      perror("recv");
      exit(EXIT_FAILURE);
    }

    *total_recv_received_bytes += received_bytes;

    // receiving less than the expected means no more data expected
    // (currently... ?)
    if (received_bytes < REQUEST_BUFFER_BASE_SIZE) {
      return buffer;
    }

    if (received_bytes == REQUEST_BUFFER_BASE_SIZE) {
      buffer_size += REQUEST_BUFFER_BASE_SIZE;
      if (buffer_size > MAX_REQUEST_SIZE) {
        free(buffer);
        return NULL;
      }

      char *resized_buf = realloc(buffer, buffer_size);
      buffer = resized_buf;
      continue;
    }

    assert(received_bytes <= REQUEST_BUFFER_BASE_SIZE);
  }

  return buffer;
}

int HTTP_ParseRequest(char *buffer_received_request,
                      size_t size_received_request, Request **request_obj,
                      ErrorRequest **err_request, ArenaSimples *as) {

  *request_obj = (Request *)ArenaS_Alocar(as, sizeof(Request));
  init_request_obj(*request_obj);
  *err_request = NULL;

  // request-line
  int start = 0;
  int current = 0;

  /*
   * method
   */
  {
    while (buffer_received_request[current] != ' ') {
      current++;
      if (current >= size_received_request) {
        set_request_error(request_obj, err_request, "400",
                          "Requisição malformada.", as);
        return -1;
      }
    }
    char *method = ArenaS_Alocar(as, current - start + 1);
    strncpy(method, &buffer_received_request[start], current - start);
    method[current - start] = '\0';

    switch (strcmp(method, "GET")) {
    case 0:
      (*request_obj)->method = method;
      break;
    default:
      *request_obj = NULL;
      *err_request = (ErrorRequest *)ArenaS_Alocar(as, sizeof(ErrorRequest));
      strncpy((*err_request)->status, "501", 4);
      char err_msg[] = "Método não suportado/conhecido.";
      (*err_request)->descricao = (char *)ArenaS_Alocar(as, strlen(err_msg));
      strncpy((*err_request)->descricao, err_msg, strlen(err_msg));
      return -1;
    }

    current++;
    start = current;
  }

  /*
   * url
   */
  {
    while (buffer_received_request[current] == ' ') {
      current++;
      if (current >= size_received_request) {
        set_request_error(request_obj, err_request, "400",
                          "Requisição malformada.", as);
        return -1;
      }
    }
    start = current;

    while (buffer_received_request[current] != ' ') {
      current++;
      if (current >= size_received_request) {
        set_request_error(request_obj, err_request, "400",
                          "Requisição malformada.", as);
        return -1;
      }
    }
    char *url = (char *)ArenaS_Alocar(as, current - start + 1);
    strncpy(url, &buffer_received_request[start], current - start);
    url[current - start] = '\0';
    parse_url(url, current - start, *request_obj, as);

    current++;
    start = current;
  }

  /*
   * version
   */
  {
    while (buffer_received_request[current] == ' ') {
      current++;
      if (current >= size_received_request) {
        set_request_error(request_obj, err_request, "400",
                          "Requisição malformada.", as);
        return -1;
      }
    }
    start = current;

    while (buffer_received_request[current] != ' ' &&
           buffer_received_request[current] != '\r' &&
           buffer_received_request[current] != '\n') {
      current++;
      if (current >= size_received_request) {
        set_request_error(request_obj, err_request, "400",
                          "Requisição malformada.", as);
        return -1;
      }
    }
    char *http_version = ArenaS_Alocar(as, current - start + 1);
    strncpy(http_version, &buffer_received_request[start], current - start);
    http_version[current - start] = '\0';
    (*request_obj)->http_version = http_version;

    start = current;
    if (buffer_received_request[current] != '\r' ||
        buffer_received_request[current + 1] == '\n') {
      current += 2;
      start = current;
    } else {
      assert(0);
    }
  }

  assert((*request_obj)->method != NULL);
  assert((*request_obj)->url != NULL);
  assert((*request_obj)->http_version != NULL);

  /*
   * headers
   */
  if (current + 1 >= size_received_request) {
    set_request_error(request_obj, err_request, "400", "Requisição malformada.",
                      as);
    return -1;
  }

  if (buffer_received_request[current] != '\r' &&
      buffer_received_request[current + 1] != '\n') {
    while (true) {
      while (buffer_received_request[current] != ':') {
        current++;
        if (current >= size_received_request) {
          set_request_error(request_obj, err_request, "400",
                            "Requisição malformada.", as);
          return -1;
        }
      }
      char field_name[current - start + 1];
      strncpy(field_name, &buffer_received_request[start], current - start);
      field_name[current - start] = '\0';
      current++;
      start = current;

      while (buffer_received_request[current] == ' ') {
        current++;
        if (current >= size_received_request) {
          set_request_error(request_obj, err_request, "400",
                            "Requisição malformada.", as);
          return -1;
        }
      }
      start = current;

      while (buffer_received_request[current] != '\r' &&
             buffer_received_request[current] != '\n') {
        current++;
        if (current >= size_received_request) {
          set_request_error(request_obj, err_request, "400",
                            "Requisição malformada.", as);
          return -1;
        }
      }
      char field_value[current - start + 1];
      strncpy(field_value, &buffer_received_request[start], current - start);
      field_value[current - start] = '\0';
      start = current;

      validate_and_set_headers(*request_obj, field_name, field_value, as);

      if (strncmp(&buffer_received_request[start], "\r\n", 2) == 0) {
        start += 2;
        current = start;
        if (strncmp(&buffer_received_request[start], "\r\n", 2) == 0) {
          start += 2;
          current = start;
          break;
        }
      }
    }
  }

  return 0;
}
