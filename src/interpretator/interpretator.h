#ifndef INTERPRETATOR_H_INCLUDED
#define INTERPRETATOR_H_INCLUDED

#include "../tree.h"
#include "../List.h"
#include "../types.h"

typedef struct
{
    Variable *stack;
    Variable *stack_head;
    int      *stack_base;
}
Interpretator;

Variable* run_function(Interpretator *interpretator, Function *f, Array *args);
char execute(Interpretator *interpretator, List *body);
void interpretator(Function *main);

Variable* interpretator_var_alloc(Interpretator *interpretator);
Variable* interpretator_pop_var(Interpretator *interpretator);

void interpretator_operation_table_init();

#endif // INTERPRETATOR_H_INCLUDED
