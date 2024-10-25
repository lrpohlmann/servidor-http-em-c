#include <assert.h>
#include <stdlib.h>

#include "../src/routing/routing.h"

ResponseOutput *test_view(Request *request, ArenaSimples *arena) {
  ResponseOutput *a =
      (ResponseOutput *)ArenaS_Alocar(arena, sizeof(ResponseOutput));
  return a;
}

ResponseOutput *test_home_view(Request *request, ArenaSimples *arena) {
  ResponseOutput *a =
      (ResponseOutput *)ArenaS_Alocar(arena, sizeof(ResponseOutput));
  return a;
}

ResponseOutput *test_faq_view(Request *request, ArenaSimples *arena) {
  ResponseOutput *a =
      (ResponseOutput *)ArenaS_Alocar(arena, sizeof(ResponseOutput));
  return a;
}

ResponseOutput *test_question_one_view(Request *request, ArenaSimples *arena) {
  ResponseOutput *a =
      (ResponseOutput *)ArenaS_Alocar(arena, sizeof(ResponseOutput));
  return a;
}

int main(void) {
  RouteNode root = {.url = "/",
                    .view = test_view,
                    .number_of_child_url_segments = 2,
                    .child_url_segments = (struct route_node *)malloc(
                        sizeof(struct route_node) * 2)};

  RouteNode home_route = {
      .url = "home", .view = test_home_view, .number_of_child_url_segments = 0};

  RouteNode faq_route = {.url = "faq",
                         .view = test_faq_view,
                         .number_of_child_url_segments = 1,
                         .child_url_segments = (struct route_node *)malloc(
                             sizeof(struct route_node) * 1)};

  RouteNode question_one_route = {.url = "question_one",
                                  .view = test_question_one_view,
                                  .number_of_child_url_segments = 0};

  faq_route.child_url_segments[0] = question_one_route;
  root.child_url_segments[0] = home_route;
  root.child_url_segments[1] = faq_route;

  UrlSegment question_url = {.segment = "question_one", .next = NULL};
  UrlSegment faq_url = {.segment = "faq", .next = &question_url};
  UrlSegment root_url = {.segment = "/", .next = &faq_url};
  Request request = {.url = &root_url};

  View view = Routing_GetRoute(&request, &root);
  assert(view != NULL);
  assert(view == test_question_one_view);

  faq_url.next = NULL;
  view = Routing_GetRoute(&request, &root);

  assert(view != NULL);
  assert(view == test_faq_view);

  root_url.next = NULL;
  view = Routing_GetRoute(&request, &root);

  assert(view != NULL);
  assert(view == test_view);

  UrlSegment wrong_url = {.segment = "blog", .next = NULL};
  root_url.next = &wrong_url;

  view = Routing_GetRoute(&request, &root);
  assert(view == NULL);

  return EXIT_SUCCESS;
}
