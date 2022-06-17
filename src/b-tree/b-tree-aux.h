#ifndef B_BTREE_AUX_H
#define B_BTREE_AUX_H

#include "b-tree.h"

/* 
 * @brief Printa os elementos de uma Key
 *
 * @param key A chave a ser printada
*/
void key_print(KEY *key);

/*
 * @brief Printa os elementos de um nó
 *
 * @param node Nó a ser imprimido
*/
void node_print(NODE *node);

/*
 * @brief Verifica se o registro existe
 *
 * @param reg Registro a ser verificado
*/
bool registry_exist(REGISTRY *reg);

/*
 * @brief Verifica se o nó esta com Overflow
 *
 * @param node Nó a ser verificado
*/
bool node_overflow(NODE *node);

/* Cria um nó vazio e o retorna */
NODE* node_create_null();

/* Cria um B-Tree com o Root vazio e retorna um ponteiro para o inicio do arquivo */
FILE* btree_create();

/* 
 * @brief Abre a B-Tree e retorna o nó raiz
 *
 * @param RRN Atualiza o RRN para o RRN do nó raiz
 */
NODE* node_take_root(long *RRN);


#endif //B_BTREE_AUX_H