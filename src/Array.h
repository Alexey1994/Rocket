#ifndef ARRAY_H_INCLUDED
#define ARRAY_H_INCLUDED

#define ARRAY_BLOCK_SIZE     8
#define ARRAY_TWO_POW_LENGTH 3

typedef struct
{
    char **data;
    int    timer_to_realloc,
           length;
}
Array;

Array* array_init();
void array_free(Array *array);

void array_push(Array *array, char *data);
char* array_pop(Array *array);

#endif // ARRAY_H_INCLUDED
