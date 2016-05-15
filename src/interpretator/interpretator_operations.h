#ifndef INTERPRETATOR_OPERATIONS_H_INCLUDED
#define INTERPRETATOR_OPERATIONS_H_INCLUDED

#include "../lexer/lexer_operations.h"
#include "../types.h"
#include "../List.h"
#include "interpretator.h"

#define LOOP_NORMAL   0
#define LOOP_BREAK    1
#define LOOP_CONTINUE 2
//#define FUNCTION_RETURN 3

typedef struct
{
    unsigned int  count;
    char         *data;
}
GCVariable;

Variable* interpretator_add_var(Interpretator *interpretator, Variable *var);

extern Variable *undefined;

void interpretator_table_init();

char interpretator_loop(List *loop_body, Interpretator *interpretator);
char interpretator_while(While *while_data, Interpretator *intepretator);
char interpretator_do(Do *do_data, Interpretator *intepretator);
char interpretator_break();
char interpretator_continue();
char interpretator_print(Print *print, Interpretator *interpretator);
char interpretator_if(If *if_data, Interpretator *interpretator);
char interpretator_call(Array *call_data, Interpretator *interpretator);
char interpretator_assignment(Assignment *assignment_data, Interpretator *interpretator);
char interpretator_return(List *return_expression, Interpretator *interpretator);
char interpretator_push(Push *push_data, Interpretator *interpretator);

#endif // INTERPRETATOR_OPERATIONS_H_INCLUDED
