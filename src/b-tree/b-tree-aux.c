#include "b-tree-aux.h"

void key_print(KEY *key)
{
    if(key != NULL)
    {
        printf("nUsp: %d RRN: %ld", key_nusp(key), key_RRN(key));
    }
}

void node_print(NODE *node)
{
    if(node == NULL)
    {
        printf("Nada para printar\n\n");
        return;
    }
    for(int i = 0; i <= MAX_CHILDRENS_WITH_OVERFLOW; i++)
    {
        if(i <= MAX_KEYS_WITH_OVERFLOW)
        {
            printf("Key: %d - ", i);
            key_print(node_key(node, i));
        }
        printf(" - CHILDREN RRN: %ld\n", node_child(node, i));
        if(key_nusp(node_key(node, i)) == NOTHING)
        {
            break;
        }
    }

    printf("isLeaf: %d\n", node_isLeaf(node));
    printf("keys number: %d\n\n", node_keysNumber(node));
}

bool registry_exist(REGISTRY *reg)
{
    KEY *temp;
    temp = btree_search(registry_nUps(reg));
    if(temp == NULL)
    {
        return false;
    }
    key_memory_clear(&temp);
    return true;
}

bool node_overflow(NODE *node)
{
    if(node_keysNumber(node) == MAX_KEYS_WITH_OVERFLOW)
    {
        return true;
    }
    return false;
}

NODE* node_create_null()
{
    int nUsp[MAX_KEYS_WITH_OVERFLOW] = {[0 ... (MAX_KEYS_WITH_OVERFLOW - 1)] = NOTHING};
    long RRN[MAX_KEYS_WITH_OVERFLOW] = {[0 ... (MAX_KEYS_WITH_OVERFLOW - 1)] = NOTHING};
    long childs[MAX_CHILDRENS_WITH_OVERFLOW] = {[0 ... (MAX_CHILDRENS_WITH_OVERFLOW - 1)] = NOTHING};

    return node_create(nUsp, RRN, childs, NOTHING, true);
}

FILE* btree_create()
{
    /* Cria um da B-Tree */
    FILE* btree = fopen(BTREE, "wb+");
    
    /* Cria um Header e insere no arquivo */
    long RRN = HEADER;
    fwrite(&RRN, sizeof(long), 1, btree);
    fwrite("\n", sizeof(char), 1, btree);

    /* Cria o node Root vazio e salva no arquivo */
    NODE* root = node_create_null();
    fseek(btree, 0, SEEK_END);
    node_write_file(root, btree);

    node_memory_clear(&root);
    fseek(btree, 0, SEEK_SET);

    return btree;
}

NODE* node_take_root(long *RRN)
{
    /* Abre a B-Tree, se não existir, cria uma */
    FILE* b_tree = fopen(BTREE, "rb+");
    if(b_tree == NULL)
    {
        b_tree = btree_create();
    }

    /* Faz a leitura do RRN para Root */
    fread(RRN, sizeof(long), 1, b_tree);
    
    /* Lê o node Root */
    fseek(b_tree, *RRN, SEEK_SET);
    NODE* root = node_read_file(b_tree);
    
    fclose(b_tree);

    return root;
}

int node_median_index(NODE *node)
{
    if(node == NULL)
    {
        exit(-1);
    }

    return (node_keysNumber(node))/2;
}
