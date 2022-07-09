#include "b-tree.h"
#include "b-tree-aux.h"

static void btree_print_r(NODE *node, FILE *btree);

/* Ordena um nó pelo nUsp, utilizando o Insertion Sort */
static void node_sort(NODE *node, long child_left);

static KEY* btree_search_r(int nUsp, NODE *node, FILE *btree);

static void btree_update_r(FILE* btree, NODE *node, int nUsp, long RRN);

/* Insere 1 elemento em um nó */
static void node_insert_1_element(NODE *node, int nUsp, long RRN, long child_1, long child_2);

static void btree_insert_r(FILE* btree, NODE *node, int nUsp, long RRN, NODE *father_node, 
                           long father_node_RRN);

/* Realiza a função split no root */
static void split_root(FILE *btree, NODE *split_node, NODE *father_node, long split_node_RRN, 
                  long father_node_RRN, long new_node_RRN, int median_index, NODE *new_node);

/* Realiza a função split no node atual */
static void split_actual_node(FILE *btree, NODE *split_node, NODE *father_node, long split_node_RRN, 
                         long father_node_RRN, long new_node_RRN, int median_index, NODE *new_node);

struct key_st
{
    int nUsp;
    long RRN;
};

struct node_st
{
    KEY *keys;
    long *childs;
    short keysNumber;
    bool isLeaf;
};


static void split_root(FILE *btree, NODE *split_node, NODE *father_node, long split_node_RRN, 
                   long father_node_RRN, long new_node_RRN, int median_index, NODE *new_node)
{
    long new_root_RRN;

    /* Cria novo Node Root */
    NODE* new_root = node_create_null();

    /* Definir novo node root como não folha */
    new_root->isLeaf = false;

    /* Inserir elementos a direita do nó dividido no novo nó e apaga do nó antigo */
    short median_nUsp = split_node->keys[median_index].nUsp;
    long median_RRN = split_node->keys[median_index].RRN;

    split_node->keys[median_index].nUsp = NOTHING;
    split_node->keys[median_index].RRN = NOTHING;
    for(int i = median_index + 1; i < split_node->keysNumber; i++)
    {
        node_insert_1_element(new_node, split_node->keys[i].nUsp, split_node->keys[i].RRN, 
                                        split_node->childs[i], split_node->childs[i + 1]);

        split_node->keys[i].nUsp = NOTHING;
        split_node->keys[i].RRN = NOTHING;
        
        split_node->childs[i] = NOTHING;
        split_node->childs[i+1] = NOTHING;
    }
    split_node->keysNumber = split_node->keysNumber - median_index;


    /* Escreve no arquivo o nó criado e retorna o RRN dele */
    fseek(btree, 0, SEEK_END);
    new_node_RRN = node_write_file(new_node, btree);        

    /* Atualiza o nó dividido */
    fseek(btree, split_node_RRN, SEEK_SET);
    split_node_RRN = node_write_file(split_node, btree);

    /* Inserir o valor central no novo root e aponta para os novos nós criados */
    node_insert_1_element(new_root, median_nUsp, median_RRN, split_node_RRN, new_node_RRN);

    /* Escreve o novo root no arquivo */
    fseek(btree, 0, SEEK_END);
    new_root_RRN = node_write_file(new_root, btree);

    /* Atualizar Header para o RRN do novo node ROOT */
    fseek(btree, 0, SEEK_SET);
    fwrite(&new_root_RRN, sizeof(long), 1, btree);
    fseek(btree, 0, SEEK_SET);
    fread(&new_root_RRN, sizeof(long), 1, btree);
    
    node_memory_clear(&new_node);
}


static void split_actual_node(FILE *btree, NODE *split_node, NODE *father_node, long split_node_RRN, 
                          long father_node_RRN, long new_node_RRN, int median_index, NODE *new_node)
{
    /* Valor central do nó a ser dividido, passar para o nó-pai */
    father_node->keys[father_node->keysNumber].nUsp = split_node->keys[median_index].nUsp;
    father_node->keys[father_node->keysNumber].RRN = split_node->keys[median_index].RRN;
    father_node->childs[father_node->keysNumber + 1] = NOTHING;
    father_node->keysNumber++;

    /* Inserir elementos a direita do nó dividido no novo nó e apaga do nó antigo */
    split_node->keys[median_index].nUsp = NOTHING;
    split_node->keys[median_index].RRN = NOTHING;
    for(int i = median_index + 1; i < split_node->keysNumber; i++)
    {
        node_insert_1_element(new_node, split_node->keys[i].nUsp, split_node->keys[i].RRN, 
                                        split_node->childs[i], split_node->childs[i + 1]);

        split_node->keys[i].nUsp = NOTHING;
        split_node->keys[i].RRN = NOTHING;

        split_node->childs[i] = NOTHING;
        split_node->childs[i+1] = NOTHING;
    }
    split_node->keysNumber = split_node->keysNumber - median_index;

    /* Escreve o novo nó no arquivo */
    fseek(btree, 0, SEEK_END);
    new_node_RRN = node_write_file(new_node, btree);

    /* Atualiza o nó que sofreu o split no arquivo */
    fseek(btree, split_node_RRN, SEEK_SET);
    node_write_file(split_node, btree);

    /* Registro novo do pai aponta para os novos nó criado */
    father_node->childs[father_node->keysNumber] = new_node_RRN;

    node_sort(father_node, split_node_RRN);

    /* Atualiza o nó pai */
    fseek(btree, father_node_RRN, SEEK_SET);
    node_write_file(father_node, btree);
}


static void split(FILE *btree, NODE *split_node, bool inInsetion, NODE *father_node, 
                                          long split_node_RRN, long father_node_RRN)
{    
    long new_node_RRN = NOTHING;

    int median_index = node_median_index(split_node);

    NODE* new_node = node_create_null();// Criar o novo nó

    if(inInsetion)// Verificar se o split estiver acontecendo na inserção
    {
        new_node->isLeaf = true; // Definir o novo nó como folha 
    }
    else
    {
        new_node->isLeaf = false; // Definir o novo nó como não sendo folha 
    }
    
    if(father_node == NULL) // Verifica se é um nó raiz 
    {
        split_root(btree, split_node, father_node, split_node_RRN, father_node_RRN, 
                                             new_node_RRN, median_index, new_node);
    }
    else
    {
        split_actual_node(btree, split_node, father_node, split_node_RRN, father_node_RRN, 
                                                    new_node_RRN, median_index, new_node);
    }
}


static void node_insert_1_element(NODE *node, int nUsp, long RRN, long child_1, long child_2)
{
    if(node != NULL)
    {
        /* Verifica se é um nó vazio e o torna apto */
        if(node->keysNumber == NOTHING)
        {
            node->keysNumber++;
        }

        node->keys[node->keysNumber].nUsp = nUsp;
        node->keys[node->keysNumber].RRN = RRN;

        node->childs[node->keysNumber] = child_1;
        node->childs[node->keysNumber + 1] = child_2;

        node->keysNumber++;
        return;
    }

    exit(-1);
}


bool btree_update(REGISTRY *reg)
{
    /* Verificar se o registro já está na árvore */
    if(btree_search(registry_nUps(reg)) != NULL)
    {
        long* RRN = (long *) calloc(1, sizeof(long));

        /* Pega a raiz da árvore */
        NODE* root = node_take_root(RRN);

        /* Abre o arquivo da árvore */
        FILE* btree = fopen(BTREE, "rb+");
        if(btree == NULL)
        {
            exit(-1);
        }

        /* Aponta para o primeiro nó */
        fseek(btree, *RRN, SEEK_CUR);

        /* Salva o registro no dataFile */
        long RRN_ = registry_save_file(reg);

        /* Busca a folha recursivamente */
        btree_update_r(btree, root, registry_nUps(reg), RRN_);

        /* Fecha a árvore */
        fclose(btree);
        node_memory_clear(&root);
        free(RRN);

        return true;

    }
    else
    {
        printf("Registro nao encontrado!\n");
        return false;
    }
}

static void btree_update_r(FILE* btree, NODE *node, int nUsp, long RRN)
{
    /* Verifica se o registro está nesse nó */
    for(int i = 0; i <= node->keysNumber; i++)
    {
            /* Caso o registro seja o buscado */
            if(node->keys[i].nUsp == nUsp)
            {
                node->keys[i].RRN = RRN;

                fseek(btree, -NODE_SIZE, SEEK_CUR);
                node_write_file(node, btree);

                return;

            }
            /* Caso o filho buscado seja o primeiro filho do nó */
            else if(i == 0 && node->keys[i].nUsp > nUsp)
            {
                fseek(btree, node->childs[i], SEEK_SET);
                NODE* temp = node_read_file(btree);
                btree_update_r(btree, temp, nUsp, RRN);
    
                return;
            }
            /* Caso o filho buscado seja o filho direito do ultimo registro */
            else if(i == node->keysNumber)
            {
                fseek(btree, node->childs[i], SEEK_SET);
                NODE* temp = node_read_file(btree);
                btree_update_r(btree, temp, nUsp, RRN);
    
                return;
            }
            /* Caso o filho buscado está entre os dois registros buscados */
            else if(nUsp > node->keys[i].nUsp && nUsp < node->keys[i+1].nUsp)
            {
                fseek(btree, node->childs[i + 1], SEEK_SET);
                NODE* temp = node_read_file(btree);
                btree_update_r(btree, temp, nUsp, RRN);
    
                return;
            }
    }
}


bool btree_insertion(REGISTRY *reg)
{
    if(!registry_exist(reg))
    {
        long* RRN = (long *) calloc(1, sizeof(long));
    
        /* Pega a raiz da árvore */
        NODE* root = node_take_root(RRN);
        
        /* Abre o arquivo da árvore */
        FILE* btree = fopen(BTREE, "rb+");
        if(btree == NULL)
        {
            exit(-1);
        }

        /* Aponta para o nó root */
        fseek(btree, *RRN, SEEK_SET);

        /* Salva o registro no dataFile */
        long RRN_data = registry_save_file(reg);

        /* Busca a folha recursivamente */
        btree_insert_r(btree, root, registry_nUps(reg), RRN_data, NULL, NOTHING);
        root = node_take_root(RRN);
    
        if(node_overflow(root))
        {
            split(btree, root, false, NULL, *RRN, NOTHING);
        }
        
        /* Fecha a árvore */
        fclose(btree);
        node_memory_clear(&root);
        free(RRN); 
    
        return true;
    }


    printf("O Registro ja existe!\n");
    return false;
}

static void btree_insert_r(FILE* btree, NODE *node, int nUsp, long RRN, NODE *father_node, long father_node_RRN)
{
    /* Verifica se é um nó folha */
    if(node->isLeaf)
    {
        /* Insere as informações na chave */
        if(node->keysNumber == NOTHING)
        {
            node->keysNumber++;
        }
        node->keys[node->keysNumber].nUsp = nUsp;
        node->keys[node->keysNumber].RRN = RRN;
        node->keysNumber++;
        /* Ordena a chave */
        node_sort(node, -1);

        /* Verifica se o nó sofreu overflow */
        if(node_overflow(node))
        {
            //DEBUG
            split(btree, node, true, father_node, ftell(btree) - (NODE_SIZE), father_node_RRN); 
            //DEBUGB
            //btree_print(btree);
        }
        else
        {
            /* Escreve a chave */
            fseek(btree, -(NODE_SIZE), SEEK_CUR);
            node_write_file(node, btree);
        }
        return;
    }
    else
    {
        /* Se não for um nó folha, buscar até um nó folha */
        for(int i = 0; i <= node->keysNumber; i++)
        {
            /* Caso o filho buscado seja o filho direito do ultimo registro */
            if(i == node->keysNumber)
            {
                long node_rrn = ftell(btree); // RRN do nó atual
                fseek(btree, node->childs[i], SEEK_SET); //Posiciona o ponteiro nó nó
                long child_rrn = node->childs[i];

                NODE* temp = node_read_file(btree);

                btree_insert_r(btree, temp, nUsp, RRN, node, node_rrn);
                
                node_memory_clear(&temp);

                fseek(btree, child_rrn, SEEK_SET);
                temp = node_read_file(btree);

                if(node_overflow(temp))
                {
                    split(btree, temp, false, father_node, child_rrn, father_node_RRN);
                }

                node_memory_clear(&temp);

                break;
            }
            /* Caso o filho buscado seja o primeiro filho do nó */
            else if(i == 0 && node->keys[i].nUsp > nUsp)
            {

                long node_rrn = ftell(btree); // RRN do nó atual
                fseek(btree, node->childs[i], SEEK_SET); // Ler filho da esquerda
                long child_rrn = node->childs[i];

                NODE* temp = node_read_file(btree); // Pega o nó filho desejado

                btree_insert_r(btree, temp, nUsp, RRN, node, node_rrn);

                node_memory_clear(&temp); // Libera o nó filho antigo

                /* Lê o nó filho atualizado */
                fseek(btree, child_rrn, SEEK_SET);
                temp = node_read_file(btree);

                if(node_overflow(temp))
                {
                    split(btree, temp, false, father_node, child_rrn, father_node_RRN);
                }

                node_memory_clear(&temp);

                return;
            }
            /* Caso o filho buscado está entre os dois registros buscados */
            else if(nUsp > node->keys[i].nUsp && nUsp < node->keys[i+1].nUsp)
            {
                long node_rrn = ftell(btree); // RRN do nó atual

                fseek(btree, node->childs[i + 1], SEEK_SET); // Posiciona o ponteiro no filho da direita
                long child_rrn = ftell(btree);

                NODE* temp = node_read_file(btree);
    
                btree_insert_r(btree, temp, nUsp, RRN, node, node_rrn);

                node_memory_clear(&temp);

                fseek(btree, child_rrn, SEEK_SET);
                temp = node_read_file(btree);

                if(node_overflow(temp))
                {
                    split(btree, temp, false, father_node, child_rrn, father_node_RRN);
                }

                node_memory_clear(&temp);

                break;
            }
        }
    }
}


KEY* btree_search(int nUsp)
{
    long temp;

    NODE* root = node_take_root(&temp);
    
    FILE* btree = fopen(BTREE, "rb");
    
    KEY* key = btree_search_r(nUsp, root, btree);

    fclose(btree);
    node_memory_clear(&root);

    return key;
}

static KEY* btree_search_r(int nUsp, NODE *node, FILE *btree)
{
    if(node->keysNumber != NOTHING) // Verifica se o nó existe
    {
        for(int i = 0; i < node->keysNumber; i++)
        {
            /* Caso a chave atual do nó for a procurada, retorna o registro */
            if(node->keys[i].nUsp == nUsp)
            {
                KEY* key = (KEY *) calloc(1, sizeof(KEY));

                key->nUsp = node->keys[i].nUsp;
                key->RRN = node->keys[i].RRN;
    
                return key;
            }
            
            /* Caso seja o ultimo registro do nó, e ainda assim o valor do registro buscado  */
            /* seje maior que esse ultimo registro, ir para o filho da direita do ultimo reg */
            else if(i + 1 == node->keysNumber && nUsp > node->keys[i].nUsp && node->childs[i+1] != NOTHING)
            {
                fseek(btree, node->childs[i+1], SEEK_SET);
                NODE *child = node_read_file(btree);

                KEY* key = btree_search_r(nUsp, child, btree);
                node_memory_clear(&child);
                return key;
            }

            /* Caso o registro atual seja maior o registro buscado, ir para o filho da esquerda */
            else if(node->keys[i].nUsp > nUsp && node->childs[i] != NOTHING)
            {
                fseek(btree, node->childs[i], SEEK_SET);
                NODE *child = node_read_file(btree);
                
                KEY* key = btree_search_r(nUsp, child, btree);
                node_memory_clear(&child);
                return key;

            }
        }
    }
    return NULL;
}


static void node_sort(NODE *node, long child_left)
{
    int temp_nUsp = node->keys[node->keysNumber - 1].nUsp;
    long temp_rrn = node->keys[node->keysNumber - 1].RRN;
    long temp_child_right = node->childs[node->keysNumber];

    int i = node->keysNumber - 2;
    bool change = false;

    while(node->keys[i].nUsp > temp_nUsp && i >= 0 && node->keysNumber > 1)
    {
        change = true;
        node->keys[i+1].nUsp =  node->keys[i].nUsp;
        node->keys[i+1].RRN = node->keys[i].RRN;
        node->childs[i+2] = node->childs[i+1];

        i--;
    }
    if(change)
    {
        i++;
        node->keys[i].nUsp = temp_nUsp;
        node->keys[i].RRN = temp_rrn;
        if(i == MAX_KEYS_WITH_OVERFLOW - 2) // Se acontece no final
        {
            node->childs[i] = child_left;
            node->childs[i+1] = temp_child_right;
        }
        else if(i != 0)
        {
            node->childs[i] = child_left;
        }
        else // Se acontece no inicio
        {
            node->childs[i + 1] = temp_child_right;
        }
    }
}

NODE* node_create(const int nUsp[], const long RRN[], const long childs[], 
                               const short keysNumber, const bool isLeaf)
{
    NODE *node = (NODE *)calloc(1, sizeof(NODE));
    if (node == NULL)
    {
        exit(-1);
    }

    node->keys = (KEY *) calloc((MAX_KEYS_WITH_OVERFLOW), sizeof(KEY));
    node->childs = (long *) calloc((MAX_CHILDRENS_WITH_OVERFLOW), sizeof(long));
    
    /* Insere as informações dentro do node */
    for(int i = 0; i < MAX_CHILDRENS_WITH_OVERFLOW; i++)
    {
        if(i < MAX_KEYS_WITH_OVERFLOW)
        {
            node->keys[i].nUsp = nUsp[i];
            node->keys[i].RRN = RRN[i];
        }
        node->childs[i] = childs[i];
    }
    node->keysNumber = keysNumber;
    node->isLeaf = isLeaf;

    return node;
}


long node_write_file(NODE *node, FILE *btree)
{
    long RRN = ftell(btree);

    /* Escreve os nUsp */
    for(int i = 0; i < MAX_KEYS_WITH_OVERFLOW; i++)
    {
        int temp = node->keys[i].nUsp;
        fwrite(&temp, sizeof(int), 1, btree);
    }

    /* Escreve os RRNS dos nUsp*/
    for(int i = 0; i < MAX_KEYS_WITH_OVERFLOW; i++)
    {
        long temp = node->keys[i].RRN;
        fwrite(&temp, sizeof(long), 1, btree);
    }

    /* Escreve os RRN'S dos filhos na B-Tree*/
    for(int i = 0; i < MAX_CHILDRENS_WITH_OVERFLOW; i++)
    {
        long temp = node->childs[i];
        fwrite(&temp, sizeof(long), 1, btree);
    }

    /* Escreve a quantidade de registros que há no nó*/
    fwrite(&(node->keysNumber), sizeof(short), 1, btree);

    /* Escreve se o nó é folha ou não */
    fwrite(&(node->isLeaf), sizeof(bool), 1, btree);

    fwrite("\n", sizeof(char), 1, btree);

    return RRN;
}


NODE* node_read_file(FILE *btree)
{
    NODE* node = node_create_null();

    /* Lê os nUsp */
    for(int i = 0; i < MAX_KEYS_WITH_OVERFLOW; i++)
    {
        fread(&(node->keys[i].nUsp), sizeof(int), 1, btree);
    }

    /* Lê os RRN'S */
    for(int i = 0; i < MAX_KEYS_WITH_OVERFLOW; i++)
    {
        fread(&(node->keys[i].RRN), sizeof(long), 1, btree);
    }

    /* Lê os RRN'S dos filhos */
    for(int i = 0; i < MAX_CHILDRENS_WITH_OVERFLOW; i++)
    {
        fread(&(node->childs[i]), sizeof(long), 1, btree);
    }

    /* Lê a quantidade de registros tem no nó*/
    fread(&(node->keysNumber), sizeof(short), 1, btree);

    /* Lê se o nó é folha ou não */
    fread(&(node->isLeaf), sizeof(bool), 1, btree);

    /* Pula o \n no fim do arquivo */
    fseek(btree, 1, SEEK_CUR);

    return node;
}


void node_memory_clear(NODE **node)
{
    free((*node)->keys);
    ((*node)->keys) = NULL;
    free((*node)->childs);
    ((*node)->childs) = NULL;
    free(*node);
    (*node) = NULL;
}


int key_nusp(KEY *key)
{
    return key->nUsp;
}


long key_RRN(KEY *key)
{
    return key->RRN;
}


long node_child(NODE *node, int child_index)
{
    return node->childs[child_index];
}


bool node_isLeaf(NODE *node)
{
    return node->isLeaf;
}


short node_keysNumber(NODE *node)
{
    return node->keysNumber;
}


KEY* node_key(NODE *node, int KeyIndex)
{
    return &(node->keys[KeyIndex]);
}


void key_memory_clear(KEY **key)
{
    (*key)->nUsp = NOTHING;
    (*key)->RRN = NOTHING;
    free(*key);
    (*key) = NULL;
}


void btree_print(FILE *a)
{
    if(a == NULL)
    {
        a = fopen(BTREE, "rb");
    }
        fseek(a, 0, SEEK_SET);
        long header;
        fread(&header, sizeof(long), 1, a);

        printf("Root RRN: %ld\n\n", header);

        fseek(a, header, SEEK_SET); //POSICIONA O ROOT
        long b;
        NODE* root = node_take_root(&b);

        btree_print_r(root, a);

        node_memory_clear(&root);
}

static void btree_print_r(NODE *node, FILE *btree)
{
    for(int i = 0; i <= node->keysNumber; i++)
    {
        if(node->childs[i] != NOTHING)
        {
            fseek(btree, node->childs[i], SEEK_SET);
            NODE* temp = node_read_file(btree);

            btree_print_r(temp, btree);

            node_memory_clear(&temp);
        }
    }

    node_print(node);
}