#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

struct ListNode
{
  char *data;
  struct ListNode *next;
};

typedef struct
{
    struct ListNode *begin, *end;
    int length;
}List;

List* list_init();
void list_free(List *list, void (*free_data)(char *data));

void list_push(List *list, char *data);

#endif
