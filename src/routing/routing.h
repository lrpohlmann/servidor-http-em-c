#pragma once

#include "../alloc/arena.h"
#include "../http/request.h"
#include "../http/response.h"

typedef ResponseOutput *(*View)(Request *, ArenaSimples *);

typedef struct route_node {
  char *url;
  View view;
  int number_of_child_url_segments;
  struct route_node *child_url_segments;
} RouteNode;

View Routing_GetRoute(Request *request, RouteNode *routes);
