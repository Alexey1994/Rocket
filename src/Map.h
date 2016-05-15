#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "Tree.h"
#include "String.h"
#include "types.h"

typedef struct
{
    String   *key;
    Variable *value;
}
MapNode;

typedef struct
{
    Tree *tree;
}
Map;

Map* map_init();
void map_free(Map *map);

void map_add(Map *map, String *key, Variable *value);
Variable* map_get(Map *map, String *key);

#endif // MAP_H_INCLUDED
