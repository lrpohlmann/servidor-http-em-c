#include <assert.h>
#include <string.h>

#include "arena.h"

char *ArenaS_Alocar(ArenaSimples *as, size_t tamanho_alloc) {
  assert(as->capacidade >= as->posicao + tamanho_alloc);

  char *ptr = &as->buf[as->posicao];
  as->posicao += tamanho_alloc;
  return ptr;
}

char *ArenaS_AlocarSetTexto(ArenaSimples *as, char *texto,
                            size_t tamanho_texto) {
  char *ptr = (char *)ArenaS_Alocar(as, tamanho_texto);
  strncpy(ptr, texto, tamanho_texto);
  return ptr;
}
