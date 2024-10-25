#include <string.h>

#include "routing.h"

static View match_url(UrlSegment *url, struct route_node *route) {
  if (strcmp(url->segment, route->url) != 0) {
    return NULL;
  }

  if (url->next == NULL) {
    return route->view;
  }

  url = url->next;
  for (int i = 0; i < route->number_of_child_url_segments; i++) {
    View matched = match_url(url, &route->child_url_segments[i]);
    if (matched != NULL) {
      return matched;
    }
  }
  return NULL;
}

View Routing_GetRoute(Request *request, RouteNode *routes) {
  UrlSegment *current_url = request->url;
  if (current_url == NULL) {
    return NULL;
  }

  return match_url(request->url, routes);
}
