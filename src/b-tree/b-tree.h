#ifndef B_TREE_H
#define B_TREE_H

#include "../util/util.h"
#include "../registros/registry.h"

#define NODE_SIZE 4092 //(int)(4091 + (sizeof(char))) // \n  //92
#define ORDER 205  //5

#define MAX_KEYS (ORDER - 2) // 203
#define MAX_CHILDRENS (MAX_KEYS + 1) // 204

#define MAX_KEYS_WITH_OVERFLOW (MAX_KEYS + 1) // 204
#define MAX_CHILDRENS_WITH_OVERFLOW (MAX_KEYS_WITH_OVERFLOW + 1) // 205
#define NOTHING -1
#define HEADER (sizeof(long) + 1)

#define BTREE "data/btree.data"

typedef struct key_st KEY;
typedef struct node_st NODE;

/* Busca o registro pelo nUsp na árvore */
KEY* btree_search(int nUsp);


/* Se possivel, insere o registro na árvore */
bool btree_insertion(REGISTRY *reg);


bool btree_update(REGISTRY *reg);
void node_print(NODE *node);
void key_print(KEY *key);

void key_memory_clear(KEY **key);

/* Cria um nó */
NODE* node_create(const int nUsp[], const long RRN[], const long childs[], const short keysNumber, const bool isLeaf);


/* Escreve o nó recebido no local que o ponteiro do arquivo aponta e retorna o RRN dele */
long node_write_file(NODE *node, FILE *btree);


/* Cria um nó vazio e insere as informações do arquivo nele a partir do local que o arquivo aponta */
NODE* node_read_file(FILE *btree);


/* Apaga o nó da memória */
void node_memory_clear(NODE **node);


/* Retorna o nUsp de uma chave */
int key_nusp(KEY *key);


/* Retorna o RRN de uma chave */
long key_RRN(KEY *key);


/* Retorna um filho do nó */
long node_child(NODE *node, int child_index);


/* Verifica se o nó e folha */
bool node_isLeaf(NODE *node);


/* Retorna a quantidade de chaves no nó */
short node_keysNumber(NODE *node);


/* Retorna uma chave do nó */
KEY* node_key(NODE *node, int KeyIndex);


void btree_print(FILE *a);

#endif //B_TREE_H