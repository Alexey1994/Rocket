#include "Map.h"
#include "extends.h"

Map* map_init()
{
    Map *map=new(Map);

    map->tree=tree_init();

    return map;
}

static void map_node_free(MapNode *map_node)
{
    free(map_node);
}

void map_free(Map *map)
{
    tree_free(map->tree, map_node_free);
    free(map);
}

static int map_node_comparision(MapNode *map_node1, MapNode *map_node2)
{
    return str_comparision(map_node1->key, map_node2->key);
}

void map_add(Map *map, String *key, Variable *value)
{
    MapNode *map_node=new(MapNode);

    map_node->key=key;
    map_node->value=value;

    tree_add(map->tree, map_node, map_node_comparision);
}

static int str_map_node_comparision(String *key, MapNode *map_node)
{
    return str_comparision(key, map_node->key);
}

Variable* map_get(Map *map, String *key)
{
    MapNode *map_node=tree_find(map->tree, key, str_map_node_comparision);
    if(!map_node)
        return 0;
    return map_node->value;
}
