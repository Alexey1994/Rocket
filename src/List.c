#include "List.h"
#include <stdlib.h>

List* list_init()
{
    List *list=malloc(sizeof(List));

    list->begin=0;
    list->end=0;
    list->length=0;

    return list;
}

void list_push(List *list, char *data)
{
    struct ListNode *ins=malloc(sizeof(struct ListNode));

    ins->data=data;
    ins->next=0;

    if(list->end)
        list->end->next=ins;
    else
        list->begin=ins;

    list->end=ins;
    list->length++;
}

void list_free(List *list, void (*free_data)(char *data))
{
    struct ListNode *i=list->begin, *del;

    while(i)
    {
        del=i;
        i=i->next;
        free_data(del->data);
        free(del);
    }

    free(list);
}
