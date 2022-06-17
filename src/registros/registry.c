#include "registry.h"

static FILE* open_verify(char *mode);

struct registry_st
{
    int nUsp;
    char name[NAME_SZ];
    char surname[SURNAME_SZ];
    char course[COURSE_SZ];
    float grade;
};

/* Cria um registro */
REGISTRY* registry_create(const int nUsp,  const char name[], const char surname[], 
                                            const char course[], const float grade)
{
    REGISTRY* registry = (REGISTRY *) calloc(1, sizeof(REGISTRY));
    if(registry == NULL)
    {
        exit(-1);
    }
    registry->nUsp = nUsp;
    strcpy(registry->name, name);
    strcpy(registry->surname, surname);
    strcpy(registry->course, course);
    registry->grade = grade;

    return registry;
}
                                             
/* Apaga o registro */
bool registry_memory_delete(REGISTRY **reg)
{
    if(reg != NULL)
    {
        free(*reg);
        *reg = NULL;
        return true;
    }

    return false;
}
                         
/* Imprime o registro */
void registry_print(REGISTRY *reg)
{
    printf("-------------------------------\n");
    printf("nUSP: %d\n", reg->nUsp);
    printf("Nome: %s\n", reg->name);
    printf("Sobrenome: %s\n", reg->surname);
    printf("Curso: %s\n", reg->course);
    printf("Nota: %.2f\n", reg->grade);
    printf("-------------------------------\n");
}

/* Salva o registro no arquiquivo e retorna o RRN */
long registry_save_file(REGISTRY *reg)
{
    long RRN;
    int size;
    /* Abre o arquivo e atualiza a quantidade de registros no Header */
    FILE* dataFile = open_verify("rb+");
    fread(&size, sizeof(int), 1, dataFile);
    size++;

    fseek(dataFile, 0, SEEK_SET);
    fwrite(&size, sizeof(int), 1, dataFile);
    fwrite("\n", sizeof(char), 1, dataFile);

    fseek(dataFile, 0, SEEK_END);

    /* Transforma os dados do registro para a inserção no Data File */
    fwrite(&(reg->nUsp), sizeof(int), 1, dataFile);
    fwrite(reg->name, sizeof(char), NAME_SZ, dataFile);
    fwrite(reg->surname, sizeof(char), SURNAME_SZ, dataFile);
    fwrite(reg->course, sizeof(char), COURSE_SZ, dataFile);
    fwrite(&(reg->grade), sizeof(float), 1, dataFile);
    fwrite("\n", sizeof(char), 1, dataFile);

    fclose(dataFile);
    RRN = size;
    return RRN;
}

/* Busca e retorna o registro no arquivo */
REGISTRY* registry_search_file(int RRN)
{
    /* Declaraçõ das variaveis */
    int nUsp;
    char name[NAME_SZ];
    char surname[SURNAME_SZ];
    char course[COURSE_SZ];
    float grade;

    /* Abre o arquivo e posiciona o ponteiro no lugar correto */
    FILE* dataFile = open_verify("rb");
    fseek(dataFile, (RRN * (REGSIZE)) + sizeof(int) + sizeof(char), SEEK_SET);
    
    /* Pega as informações */
    fread(&nUsp, sizeof(int), 1, dataFile);
    fread(name, sizeof(char), NAME_SZ, dataFile);
    fread(surname, sizeof(char), SURNAME_SZ, dataFile);
    fread(course, sizeof(char), COURSE_SZ, dataFile);
    fread(&grade, sizeof(float), 1, dataFile);
    
    REGISTRY* reg = registry_create(nUsp, name, surname, course, grade);

    fclose(dataFile);
    return reg;
}

static FILE* open_verify(char *mode)
{
    FILE* dataFile = fopen(DATAFILE, mode);
    /* Verifica se o arquivo não existe, se não existe, cria um */
    if(dataFile == NULL)
    {
        int value = -1;
        dataFile = fopen(DATAFILE, "wb+");

        fwrite(&value, sizeof(int), 1, dataFile);
        fwrite("\n", sizeof(char), 1, dataFile);

        fseek(dataFile, 0, SEEK_SET);
    }
    
    return dataFile;
}

/* Retorna o nUsp do registro */
int registry_nUps(REGISTRY *reg)
{
    if(reg != NULL)
    {
        return reg->nUsp;
    }
    return -1;
}
