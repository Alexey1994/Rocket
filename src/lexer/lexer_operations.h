#ifndef LEXER_OPERATIONS_H_INCLUDED
#define LEXER_OPERATIONS_H_INCLUDED

#include <stdio.h>
#include "../types.h"
#include "../extends.h"
#include "../tree.h"
#include "../stack.h"
#include "../List.h"
#include "../Id.h"

typedef struct
{
    FILE     *f;
    char     *head;
    Stack    *blocks_pos;
    Stack    *scopes;
    Tree     *cur_scope;
    List     *cur_body;
    Data     *cur_block;
    Function *cur_function;
    Id       *id;
    String   *expr_token;
    String   *token;
    char      is_end_function;
}
Lexer;

Function* get_function(Lexer *lexer, String *function_name);
char get_function_body(Lexer *lexer);

char lexer_print(Lexer *lexer);
char lexer_if(Lexer *lexer);
char lexer_else(Lexer *lexer);
char lexer_var(Lexer *lexer);
char lexer_end(Lexer *lexer);
char lexer_loop(Lexer *lexer);
char lexer_while(Lexer *lexer);
char lexer_do(Lexer *lexer);
char lexer_function(Lexer *lexer);
char lexer_break(Lexer *lexer);
char lexer_continue(Lexer *lexer);
char lexer_return(Lexer *lexer);
char lexer_push(Lexer *lexer);

Variable* find_local_var(Tree *variables, String *name);
Variable* new_static_variable(Lexer *lexer, String *name, char type, int *data);
Variable* new_variable(Lexer *lexer, String *name, char type);

Array* get_defined_operand(Lexer *lexer);
char get_call_or_assignment(Lexer *lexer);

#endif // LEXER_OPERATIONS_H_INCLUDED
