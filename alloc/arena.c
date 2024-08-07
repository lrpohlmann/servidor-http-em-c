#include <assert.h>
#include <string.h>

#include "arena.h"

char *ArenaS_Alocar(ArenaSimples *as, size_t tamanho_alloc) {
  assert(as->capacidade >= as->posicao + tamanho_alloc);

  char *ptr = &as->buf[as->posicao];
  memset(ptr, 0, tamanho_alloc);
  as->posicao += tamanho_alloc;
  return ptr;
}
