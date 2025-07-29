#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>

#ifndef DYNVEC_INIT_CAPACITY
#define DYNVEC_INIT_CAPACITY 4
#endif

typedef struct Dynvec {
   void * data;      // apontador para a sequencia dos elementos
   size_t elem_size; // tamanho dos elementos de datat (em bytes)
   size_t capacity;  // capacidade total da sequência data
   size_t length;    // quantidade atual dos elementos arquivados
}
dynvec;

// Cria um vetor dinâmico genérico
dynvec * dynvec_create(size_t elem_size) {
   dynvec * v = malloc(sizeof(dynvec));
   if (!v) {
      errno = ENOMEM;
      #ifdef DEBUG_ON
      perror("Create. Erro ao alocar memória para estrutura");
      #endif
      return NULL;
   }

   v -> elem_size = elem_size;
   v -> capacity = DYNVEC_INIT_CAPACITY;
   v -> length = 0;
   v -> data = malloc(v -> capacity * elem_size);

   if (!v -> data) {
      errno = ENOMEM;
      #ifdef DEBUG_ON
      perror("Create. Erro ao alocar memória para dados");
      #endif
      free(v);
      return NULL;
   }
   return v;
}

// Adiciona um elemento ao vetor
void dynvec_push(dynvec * v,
   const void * elem) {
   if (v -> length >= v -> capacity) {
      v -> capacity *= 2;
      v -> data = realloc(v -> data, v -> capacity * v -> elem_size);
   }
   memcpy((char * ) v -> data + (v -> length * v -> elem_size), elem, v -> elem_size);
   v -> length++;
}

// Libera a memória do vetor
void dynvec_free(dynvec * v) {
   if (v) {
      free(v -> data);
      free(v);
   }
}

// Obtém um elemento do vetor
void * dynvec_get(const dynvec * v, size_t index) {
   if (index >= v -> length) {
      return NULL;
   }
   return (char * ) v -> data + (index * v -> elem_size);
}

// Define um elemento no vetor
void dynvec_set(dynvec * v, size_t index,
   const void * elem) {
   if (index < v -> length)
      memcpy((char * ) v -> data + (index * v -> elem_size), elem, v -> elem_size);
}

// Retorna o número de elementos
size_t dynvec_length(const dynvec * v) {
   return v -> length;
}

// Retorna os elementos do vetor
void dynvec_map(dynvec * v, void( * processo)(void * )) {
   if (!v || !processo) return;
   for (size_t i = 0; i < v -> length; i++) {
      processo((char * ) v -> data + (i * v -> elem_size));
   }
}

// Verifica se um elemento existe no vetor
bool dynvec_contains(const dynvec * v,
   const void * elem, int( * cmp)(const void * ,
      const void * )) {
   for (size_t i = 0; i < v -> length; i++) {
      const void * current = (char * ) v -> data + (i * v -> elem_size);
      if (cmp(current, elem) == 0) return true;
   }
   return false;
}

// Retorna o índice do elemento (ou -1 se não encontrado)
size_t dynvec_index(const dynvec * v,
   const void * elem, int( * cmp)(const void * ,
      const void * )) {
   for (size_t i = 0; i < v -> length; i++) {
      const void * current = (char * ) v -> data + (i * v -> elem_size);
      if (cmp(current, elem) == 0) return i;
   }
   return (size_t) - 1;
}

// Aplica uma função acumuladora a todos os elementos
void dynvec_fold_left(const dynvec * v, void * acc, void( * func)(void * acc,
   const void * elem)) {
   for (size_t i = 0; i < v -> length; i++) {
      const void * elem = (char * ) v -> data + (i * v -> elem_size);
      func(acc, elem);
   }
}

// Verifica se todos os elementos satisfazem um predicado
bool dynvec_forall(const dynvec * v, bool( * predicate)(const void * elem)) {
   for (size_t i = 0; i < v -> length; i++) {
      const void * elem = (char * ) v -> data + (i * v -> elem_size);
      if (!predicate(elem)) return false;
   }
   return true;
}

// Verifica se pelo menos um elemento satisfaz um predicado
bool dynvec_exists(const dynvec * v, bool( * predicate)(const void * elem)) {
   for (size_t i = 0; i < v -> length; i++) {
      const void * elem = (char * ) v -> data + (i * v -> elem_size);
      if (predicate(elem)) return true;
   }
   return false;
}

// Retorna o índice do primeiro elemento que satisfaz o predicado
size_t dynvec_exists_index(const dynvec * v, bool( * predicate)(const void * elem)) {
   for (size_t i = 0; i < v -> length; i++) {
      const void * elem = (char * ) v -> data + (i * v -> elem_size);
      if (predicate(elem)) return i;
   }
   return (size_t) - 1;
}

// Filtra elementos com base em um predicado
dynvec * dynvec_filter(const dynvec * v, bool( * predicate)(const void * elem)) {
   dynvec * filtered = dynvec_create(v -> elem_size);
   for (size_t i = 0; i < v -> length; i++) {
      const void * elem = (char * ) v -> data + (i * v -> elem_size);
      if (predicate(elem)) {
         dynvec_push(filtered, elem);
      }
   }
   return filtered;
}

// ===================== TD3 =====================
#define MAXTXT 4096
#define MAXCHOICE 10

typedef enum {
   NORMAL,
   WON,
   FAILED
}
TipoCena;

typedef struct {
   char * descritivo;
   TipoCena tipo;
   int nopcoes;
   char ** vopcoes;
}
Cena;

typedef struct no {
   Cena * cena;
   struct no ** vizinhos;
   int nvizinhos;
}
No;

typedef struct historia {
   No * cena_inicial;
   No * cena_ativa;
   dynvec * cenas; // dynvec de ponteiros (Cena*)
   dynvec * nos; // dynvec de ponteiros (No*)
}
historia;

// ===================== Funções Cena =====================

Cena * criaCena(char * desc, TipoCena tipo, int nopcoes, char ** vopcoes) {
   if ((tipo == NORMAL && (nopcoes <= 0 || !vopcoes)) || (tipo != NORMAL && nopcoes != 0)) {
      #ifdef DEBUG_ON
      errno = EINVAL;
      fprintf(stderr, "criaCena. Erro de argumentos: tipo=%d, nopcoes=%d, vopcoes=%p\n", tipo, nopcoes, vopcoes);
      #endif
      return NULL;
   }

   Cena * c = malloc(sizeof(Cena));
   if (!c) {
      errno = ENOMEM;
      #ifdef DEBUG_ON
      perror("criaCena. Erro ao alocar memória para Cena");
      #endif
      return NULL;
   }

   c -> descritivo = strdup(desc);
   c -> tipo = tipo;
   c -> nopcoes = nopcoes;
   c -> vopcoes = malloc(nopcoes * sizeof(char * ));

   if (!c -> descritivo || (nopcoes && !c -> vopcoes)) {
      #ifdef DEBUG_ON
      errno = ENOMEM;
      perror("criaCena. Erro ao alocar memória para descritivo ou vopcoes");
      #endif
      free(c -> descritivo);
      free(c);
      return NULL;
   }

   for (int i = 0; i < nopcoes; i++) {
      c -> vopcoes[i] = strdup(vopcoes[i]);
      if (!c -> vopcoes[i]) {
         #ifdef DEBUG_ON
         errno = ENOMEM;
         fprintf(stderr, "criaCena. Erro ao alocar memória para vopcoes[%d]\n", i);
         #endif
         while (i--) {
            free(c -> vopcoes[i]);
         }
         free(c -> vopcoes);
         free(c -> descritivo);
         free(c);
         return NULL;
      }
   }
   return c;
}

void mostrarCena(Cena * c) {
   if (!c) {
      #ifdef DEBUG_ON
      errno = EINVAL;
      fprintf(stderr, "mostrarCena. Erro: Cena é NULL\n");
      #endif
      return;
   }

   printf("<<<\n%s\n>>>", c -> descritivo);
   if (c -> tipo == NORMAL) {
      printf("<%d>\n***\n", c -> nopcoes);
      for (int i = 0; i < c -> nopcoes; i++)
         printf("+ %c. %s\n", 'a' + i, c -> vopcoes[i]);
      printf("***\n");
   } else {
      printf("<%s>\n***\n", (c -> tipo == WON) ? "WON" : "FAILED");
   }
}

void libertaCena(Cena * c) {
   if (!c) {
      #ifdef DEBUG_ON
      errno = EINVAL;
      fprintf(stderr, "libertaCena. Erro: Cena é NULL\n");
      #endif
      return;
   }
   free(c -> descritivo);
   for (int i = 0; i < c -> nopcoes; i++) free(c -> vopcoes[i]);
   free(c -> vopcoes);
   free(c);
}

// ===================== Funções Nó =====================

No * criaNo(Cena * c, int nvizinhos) {
   if (!c || (c -> tipo == NORMAL && nvizinhos != c -> nopcoes)) {
      #ifdef DEBUG_ON
      errno = EINVAL;
      fprintf(stderr, "criaNo. Erro de argumentos: Cena=%p, nvizinhos=%d, nopcoes=%d\n", c, nvizinhos, c ? c -> nopcoes : -1);
      #endif
      return NULL;
   }
   if (nvizinhos < 0 || nvizinhos > MAXCHOICE) {
      #ifdef DEBUG_ON
      errno = EINVAL;
      fprintf(stderr, "criaNo. Erro: nvizinhos=%d fora do intervalo válido [0, %d]\n", nvizinhos, MAXCHOICE);
      #endif
      return NULL;
   }
   No * no = malloc(sizeof(No));
   if (!no) {
      #ifdef DEBUG_ON
      errno = ENOMEM;
      perror("criaNo. Erro ao alocar memória para No");
      #endif
      return NULL;
   }

   no -> cena = c;
   no -> nvizinhos = nvizinhos;
   no -> vizinhos = calloc(nvizinhos, sizeof(No * ));

   if (!no -> vizinhos) {
      #ifdef DEBUG_ON
      errno = ENOMEM;
      perror("criaNo. Erro ao alocar memória para vizinhos");
      #endif
      free(no -> cena);
      free(no);
      return NULL;
   }

   return no;
}

void juntaVizinhoNo(No * no, int pos, No * v) {
   if (!no || pos < 0 || pos >= no -> nvizinhos) {
      #ifdef DEBUG_ON
      errno = EINVAL;
      fprintf(stderr, "juntaVizinhoNo. Erro: No=%p, pos=%d, nvizinhos=%d\n", no, pos, no ? no -> nvizinhos : -1);
      #endif
      return;
   }
   no -> vizinhos[pos] = v;
}

No * proximoNo(No * no, int escolha) {
   if (!no || escolha < 0 || escolha >= no -> nvizinhos) {
      #ifdef DEBUG_ON
      errno = EINVAL;
      fprintf(stderr, "proximoNo. Erro: No=%p, escolha=%d, nvizinhos=%d\n", no, escolha, no ? no -> nvizinhos : -1);
      #endif
      return NULL;
   }
   return no -> vizinhos[escolha];
}

void mostraCenaNo(No * no) {
   if (no) mostrarCena(no -> cena);
}

TipoCena estadoCenaNo(No * no) {
   return (no && no -> cena) ? no -> cena -> tipo : FAILED;
}

int escolheCenaNo(No * no) {
   if (!no) return -1;
   int escolha;
   printf("Escolha (0-%d): ", no -> cena -> nopcoes - 1);
   if (scanf("%d", & escolha) != 1 || escolha < 0 || escolha >= no -> cena -> nopcoes)
      return -1;
   return escolha;
}

// ===================== Auxiliar de leitura =====================

static char * leBloco(FILE * in, char * buf) {
   char line[512];
   buf[0] = '\0';
   while (fgets(line, sizeof line, in)) {
      char * p = strstr(line, ">>>");
      if (p) {
         * p = '\0';
         strncat(buf, line, MAXTXT - strlen(buf) - 1);
         p += 3;
         while ( * p == ' ' || * p == '\t') p++;
         return ( * p == '<') ? strdup(p) : NULL;
      }
      strncat(buf, line, MAXTXT - strlen(buf) - 1);
   }
   return NULL;
}

// ===================== Criação da história =====================

historia * criaHistoria(void) {
   int n;
   if (scanf("%d\n", & n) != 1 || n <= 0) {
      errno = EINVAL;
      return NULL;
   }

   // cria dois dynvecs para guardar cenas e nós
   dynvec * cenas_vec = dynvec_create(sizeof(Cena * ));
   dynvec * nos_vec = dynvec_create(sizeof(No * ));
   if (!cenas_vec || !nos_vec) return NULL;

   // temporários para ligações e descrições (idem à sua versão original)
   int targets[n][MAXCHOICE];
   char * vtxt[n][MAXCHOICE];
   for (int i = 0; i < n; i++) {
    char tmp[64], *typeLine;
    char descript[MAXTXT];

    /* ———— lê "[i]" e o bloco <<< … >>> ———— */
    /* (o seu código de fgets em tmp e depois:) */
    typeLine = leBloco(stdin, descript);
    if (!typeLine) {
        typeLine = malloc(64);
        if (!fgets(typeLine, 64, stdin)) {
            free(typeLine);
            goto fail;
        }
    }

    /* ———— DECLARAÇÃO E PARSING de tipo e nopc ———— */
    TipoCena tipo;
    int nopc = 0;
    if (strstr(typeLine, "WON")) {
        tipo = WON;
    } else if (strstr(typeLine, "FAILED")) {
        tipo = FAILED;
    } else if (sscanf(typeLine, "<%d>", &nopc) == 1) {
        tipo = NORMAL;
    } else {
        free(typeLine);
        errno = EINVAL;
        goto fail;
    }
    free(typeLine);
    /* ————————— FIM do PARSING ——————————— */

    if (tipo == NORMAL) {
        fgets(tmp, sizeof tmp, stdin); // "***"
        for (int j = 0; j < nopc; j++) {
            char line[512];
            fgets(line, sizeof line, stdin);
            sscanf(line + 2, "%d.", &targets[i][j]);
            vtxt[i][j] = strdup(line + 2);
        }
        fgets(tmp, sizeof tmp, stdin); // "***"
    }


      // cria Cena* e No*
      Cena * c = criaCena(descript, tipo, nopc,
         (tipo == NORMAL) ? vtxt[i] : NULL);
      No * no = criaNo(c, nopc);
      if (!c || !no) {
         libertaCena(c);
         goto fail;
      }

      // armazena nos dynvecs
      dynvec_push(cenas_vec, & c);
      dynvec_push(nos_vec, & no);
   }

   // liga vizinhos (idem ao seu original):
   for (size_t i = 0; i < dynvec_length(nos_vec); i++) {
      No * no = * (No ** ) dynvec_get(nos_vec, i);
      for (int j = 0; j < no -> nvizinhos; j++) {
         No * dest = * (No ** ) dynvec_get(nos_vec, targets[i][j]);
         juntaVizinhoNo(no, j, dest);
      }
   }

   // monta a estrutura historia
   historia * H = malloc(sizeof * H);
   H -> cena_inicial = * (No ** ) dynvec_get(nos_vec, 0);
   H -> cena_ativa = H -> cena_inicial;
   H -> cenas = cenas_vec;
   H -> nos = nos_vec;
   return H;

   fail:
      // em caso de erro, limpa tudo
      for (size_t k = 0; k < dynvec_length(cenas_vec); k++)
         libertaCena( * (Cena ** ) dynvec_get(cenas_vec, k));
   dynvec_free(cenas_vec);
   dynvec_free(nos_vec);
   return NULL;
}
// ===================== Main =====================

int main(void) {
   historia * H = criaHistoria();
   if (!H) {
      #ifdef DEBUG_ON
      perror("criaHistoria");
      #endif
      return 1;
   }

   No * atual = H -> cena_ativa;
   while (estadoCenaNo(atual) == NORMAL) {
      int id;
      if (scanf("%d", & id) != 1) {
         printf("WAITING\n");
         goto cleanup;
      }

      int idx = -1;
      for (int i = 0; i < atual -> nvizinhos; i++) {
         int tgt;
         if (sscanf(atual -> cena -> vopcoes[i], "%d.", & tgt) == 1 && tgt == id) {
            idx = i;
            break;
         }
      }
      if (idx < 0) {
         printf("WAITING\n");
         goto cleanup;
      }

      atual = proximoNo(atual, idx);
   }

   printf("%s\n", (estadoCenaNo(atual) == WON) ? "WON" : "FAILED");

   cleanup:
      // libera a memória alocada
      for (size_t i = 0; i < dynvec_length(H -> cenas); i++)
         libertaCena( * (Cena ** ) dynvec_get(H -> cenas, i));
      for (size_t i = 0; i < dynvec_length(H -> nos); i++) {
         No * no = * (No ** ) dynvec_get(H -> nos, i);
         free(no -> vizinhos);
         free(no);
      }
      dynvec_free(H -> cenas);
      dynvec_free(H -> nos);
      free(H);
   return 0;
}