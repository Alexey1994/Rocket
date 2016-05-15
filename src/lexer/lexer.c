#include "lexer.h"
#include "../String.h"
#include "../extends.h"
#include "lexer_operations.h"

typedef struct
{
    String *token;
    char (*func)(Lexer *lexer_data);
}
LexerTableData;

static Tree *lexer_table;

static LexerTableData* new_lexer_table_data(String *token, char (*func)(Lexer *lexer_data))
{
    LexerTableData *data=new(LexerTableData);

    data->token=token;
    data->func=func;

    return data;
}

static int lexer_data_cmp(LexerTableData *data1, LexerTableData *data2)
{
    return str_comparision(data1->token, data2->token);
}

static int lexer_token_cmp(String *token, LexerTableData *data)
{
    return str_comparision(token, data->token);
}

void lexer_table_init()
{
    lexer_table=tree_init();

    tree_add(lexer_table, new_lexer_table_data(str_init("print"   ), lexer_print   ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("if"      ), lexer_if      ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("else"    ), lexer_else    ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("var"     ), lexer_var     ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("end"     ), lexer_end     ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("loop"    ), lexer_loop    ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("while"   ), lexer_while   ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("do"      ), lexer_do      ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("break"   ), lexer_break   ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("continue"), lexer_continue), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("function"), lexer_function), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("return"  ), lexer_return  ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("push"    ), lexer_push    ), lexer_data_cmp);
}

char get_function_body(Lexer *lexer)
{
    LexerTableData *lexer_data;

    lexer->is_end_function=0;

    while(!lexer->is_end_function && !feof(lexer->f))
    {
        str_clear(lexer->token);
        get_token(lexer->f, lexer->head, lexer->token);

        lexer_data=tree_find(lexer_table, lexer->token, lexer_token_cmp);
        if(lexer_data)
        {
            if(!lexer_data->func(lexer))
                return 0;
        }
        else
        {
            if(*lexer->head=='#')
            {
                do
                    *lexer->head=fgetc(lexer->f);
                while(!feof(lexer->f) && *lexer->head!='\n');
            }
            else if(!get_call_or_assignment(lexer))
            {
                printf(" or undefined token '%c'", *lexer->head);
                return 0;
            }
        }

        //skip(lexer->f, lexer->head);
    }

    lexer->is_end_function=0;

    return 1;
}

Tree* lexer(FILE *f)
{
    String   *main_name=str_init("");
    char      head=fgetc(f);
    Lexer    *lexer_alloc=new(Lexer);
    Function *main=new_function(main_name, 0);

    lexer_alloc->token=str_init("");
    lexer_alloc->f=f;
    lexer_alloc->head=&head;

    lexer_alloc->blocks_pos=stack_init();
    lexer_alloc->scopes=stack_init();

    lexer_alloc->id=id_init(128, 255);
    lexer_alloc->expr_token=str_init("");

    lexer_alloc->cur_function=main;
    lexer_alloc->cur_body=main->body;
    lexer_alloc->cur_block=new_data(main, FUNCTION);
    lexer_alloc->cur_scope=lexer_alloc->cur_function->variables;

    if(!get_function_body(lexer_alloc))
        return 0;

    if(lexer_alloc->blocks_pos->begin)
    {
        printf("error: expected end\n");
    }

    return main;
}
