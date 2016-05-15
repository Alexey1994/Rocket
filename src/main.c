#include <stdio.h>
#include <stdlib.h>

#include "lexer/lexer.h"

#include <locale.h>

int main()
{
    /*
    FILE *t=fopen("a.txt", "rb");
    char head=fgetc(t);

    if(is_true_word(t, &head, "fun"))
    {
        printf(":)");
    }

    fclose(t);
    return 0;*/

    setlocale(LC_ALL, "rus");

    FILE *f=fopen("test.txt", "rb");
    char *main;

    lexer_table_init();
    interpretator_table_init();
    interpretator_operation_table_init();

    main=lexer(f);

    if(main)
    {
        //printf("\ninterpretator\n");
        interpretator(main);
    }

    return 0;
}
