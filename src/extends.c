#include "extends.h"
#include "String.h"
#include <stdio.h>

Data* new_data(char *data, char type)
{
    Data *data_alloc=new(Data);

    data_alloc->data=data;
    data_alloc->type=type;

    return data_alloc;
}

char is_number(char c)
{
    if(c>='0' && c<='9')
        return 1;
    return 0;
}

char is_hex_number(char c)
{
    if((c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F'))
        return 1;
    return 0;
}

char is_letter(char c)
{
    if((c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_')
        return 1;
    return 0;
}

char is_space(char c)
{
    if(c==' ' || c=='\n' || c=='\r' || c=='\t')
        return 1;
    return 0;
}

void skip(FILE *f, char *head)
{
    while(is_space(*head) && !feof(f))
        *head=fgetc(f);
}

String* get_word(FILE *f, char *head, String *token)
{
    while(is_letter(*head) && !feof(f))
    {
        str_push(token, *head);
        *head=fgetc(f);
    }

    return token;
}

String* get_token_data(FILE *f, char *head)
{
    String *token=0;

    skip(f, head);
    if(is_letter(*head))
    {
        token=str_init("");
        get_word(f, head, token);
    }

    return token;
}

void get_token(FILE *f, char *head, String *token)
{
    str_clear(token);
    skip(f, head);
    get_word(f, head, token);
}

char is_true_word(FILE *f, char *head, char *word)
{
    long int pos=ftell(f);
    char     tmp_head=*head;

    for(; *word; word++)
    {
        if(*word!=*head)
        {
            fseek(f, pos, SEEK_SET);
            *head=tmp_head;
            return 0;
        }

        *head=fgetc(f);
    }

    return 1;
}
