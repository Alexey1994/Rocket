#ifndef EXTENDS_H_INCLUDED
#define EXTENDS_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include "String.h"

#define new(data_type) malloc(sizeof(data_type))
#define arr_alloc(data_type, sz) malloc(sizeof(#data_type)*(#sz))

#define loop for(;;)

#define OPERATION        1
#define OPERAND          2
#define COMPOUND_OPERAND 3
#define EXPRESSION       4

typedef struct
{
    char *data;
    char  type;
}
Data;

char is_hex_number(char c);
char is_number(char c);

Data* new_data(char *data, char type);
String* get_token_data(FILE *f, char *head);
char is_true_word(FILE *f, char *head, char *word);

#endif // EXTENDS_H_INCLUDED
