#include "Array.h"
#include <stdlib.h>

Array* array_init()
{
    Array *array=malloc(sizeof(Array));

    array->timer_to_realloc=ARRAY_BLOCK_SIZE;
    array->data=0;
    array->length=0;

    return array;
}

void array_push(Array *array, char *data)
{
    if(array->timer_to_realloc>>ARRAY_TWO_POW_LENGTH)
    {
        array->data=realloc(array->data, sizeof(char*)*(array->length+ARRAY_BLOCK_SIZE));
        array->timer_to_realloc=1;
    }
    else
        array->timer_to_realloc++;

    array->data[array->length]=data;
    array->length++;
}

char* array_pop(Array *array)
{

}

char* array_get(Array *array, int index)
{
    return array->data[index];
}

void array_free(Array *array)
{
    free(array->data);
    free(array);
}
