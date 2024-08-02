#pragma once
#include <stdio.h>

typedef struct {
  size_t capacidade;
  size_t posicao;
  char *buf;
} ArenaSimples;

char *ArenaS_Alocar(ArenaSimples *as, size_t tamanho_alloc);

char *ArenaS_AlocarSetTexto(ArenaSimples *as, char *texto,
                            size_t tamanho_texto);
