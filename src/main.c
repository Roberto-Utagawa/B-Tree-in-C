#include "./util/util.h"
#include "./registros/registry.h"

/*
    Saidas possíveis do programa:
        - exit(-1) para erro de alocação na memória
*/

int main()
{
    char* input = (char *) calloc(NAME_SZ + SURNAME_SZ + COURSE_SZ + 10, sizeof(char));
    if(input == NULL)
    {
        exit(-1);
    }

    /* Lê os inputs e analisa as opções possíveis para cada entrada */
    do
    {
        fgets(input, NAME_SZ + SURNAME_SZ + COURSE_SZ + 10, stdin);
    }while(menu(input));

    free(input);
    return 0;
}