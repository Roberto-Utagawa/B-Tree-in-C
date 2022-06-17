#include "util.h"
#include "../registros/registry.h"
#include "../b-tree/b-tree.h"
#include "../b-tree/b-tree-aux.h"

/* Formata os locais com cada elemento correto dado pela string input */
void strings_formatation(char *input, int *nUsp, char *name, char *surname, char *course, float *grade)
{
    char temp[COURSE_SZ];
    int i = 1, j = 0;

    /* Localizar o primeiro elemento(nUsp) */
    while(input[i-1] != ' ')
    {
        i++;
    }
    
    /* Pega o nUsp */
    while(input[i-1] != '"')
    {
        if(input[i] != ',' && input[i] != '"')
        {
            temp[j] = input[i];
            j++;
        }
        i++;
    }
    temp[j] = '\0';
    *nUsp = atoi(temp);

    /* Pega o nome */
    j = 0;
    do
    {
        if(input[i] != ',' && input[i] != '"')
        {
            temp[j] = input[i];
            j++;
        }
        i++;
    }while(input[i-1] != '"' || input[i-2] != ',');

    temp[j] = '\0';
    strcpy(name, temp);

    /* Pega o sobrenome */
    j = 0;
    do
    {
        if(input[i] != ',' && input[i] != '"')
        {
            temp[j] = input[i];
            j++;
        }
        i++;
    }while(input[i-1] != '"' || input[i-2] != ',');

    temp[j] = '\0';
    strcpy(surname, temp);

    /* Pega o curso */
    j = 0;
    do
    {
        if(input[i] != ',' && input[i] != '"')
        {
            temp[j] = input[i];
            j++;
        }
        i++;
    }while(input[i-1] != ',' || input[i-2] != '"');

    temp[j] = '\0';
    strcpy(course, temp);

    /* Pega a nota */
    j = 0;
    while(input[i] != '\n')
    {
        temp[j] = input[i];
        i++;
        j++;
    }
    temp[j] = '\0';
    *grade = atof(temp);
}

/* Opções de ações possíveis */
bool menu(char *input)
{
    int nUsp;
    char name[NAME_SZ];
    char surname[SURNAME_SZ];
    char course[COURSE_SZ];
    float grade;
    REGISTRY *reg;

    switch (input[0])
    {
        /* Inserção */
        case 'i':
            strings_formatation(input, &nUsp, name, surname, course, &grade);
            reg = registry_create(nUsp, name, surname, course, grade);
            
            btree_insertion(reg);

            registry_memory_delete(&reg);

            return true;

        /* Busca */
        case 's':
            nUsp = take_nUsp(input);

            KEY* key = btree_search(nUsp);
            
            if(key != NULL)
            {
                REGISTRY* reg = registry_search_file(key_RRN(key));
                registry_print(reg);
                registry_memory_delete(&reg);
                key_memory_clear(&key);
            }
            else
            {
                printf("Registro nao encontrado!\n");
            }


            return true;

        /* Atualização */
        case 'u':
            strings_formatation(input, &nUsp, name, surname, course, &grade);
            reg = registry_create(nUsp, name, surname, course, grade);
            btree_update(reg);
            registry_memory_delete(&reg);
            return true;

        case 'p':
            btree_print(NULL);

        /* Saida */
        default:
            return false;
        }   
}

/* Retorna o nUsp da string */
int take_nUsp(char *input)
{
    int j = 0;
    for(int i = 7; input[i] != '\0'; i++)
    {
        input[j] = input[i];
        j++;
    }
    input[j] = '\0';
    int nUsp = atoi(input);
    return nUsp;
}