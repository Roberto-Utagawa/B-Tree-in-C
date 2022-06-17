#ifndef REGISTRY_H
#define REGISTRY_H

#include "../util/util.h"

typedef struct registry_st REGISTRY;

#define DATAFILE "data/dataFile.data"
#define REGSIZE sizeof(REGISTRY) + 1 /* Delimitadores e o \n\0 */

REGISTRY* registry_create(const int nUsp,  const char name[], const char surname[], 
                                           const char course[], const float grade);
bool registry_memory_delete(REGISTRY **reg);
void registry_print(REGISTRY *reg);
long registry_save_file(REGISTRY *reg);
REGISTRY* registry_search_file(int RRN);
int registry_nUps(REGISTRY *reg);

#endif //REGISTRY_H