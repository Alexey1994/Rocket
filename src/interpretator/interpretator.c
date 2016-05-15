#include "interpretator.h"
#include "../lexer/lexer_operations.h"
#include "interpretator_operations.h"

Variable   *undefined;

static char (*interpretator_table[256])(char *data, Interpretator *interpretator);

void interpretator_table_init()
{
    interpretator_table[PRINT]=interpretator_print;
    interpretator_table[IF]=interpretator_if;
    interpretator_table[LOOP]=interpretator_loop;
    interpretator_table[WHILE]=interpretator_while;
    interpretator_table[DO]=interpretator_do;
    interpretator_table[BREAK]=interpretator_break;
    interpretator_table[CONTINUE]=interpretator_continue;
    interpretator_table[CALL]=interpretator_call;
    interpretator_table[ASSIGNMENT]=interpretator_assignment;
    interpretator_table[RETURN]=interpretator_return;
    interpretator_table[PUSH]=interpretator_push;
}

int u=0;

char execute(Interpretator *interpretator, List *body)
{
    struct ListNode *i=body->begin;
    Data  *data;
    char   loop_state;


    if(u==1000000)
    {
        printf("%d\n", u);
        u=0;
    }
    u++;

    while(i)
    {
        data=i->data;
        loop_state=interpretator_table[data->type](data->data, interpretator);

        i=i->next;

        if(loop_state)
        {
            switch(loop_state)
            {
                case LOOP_BREAK    : return loop_state; break;
                case LOOP_CONTINUE : i=body->begin; break;
                case RETURN        : return RETURN; break;
            }
        }
    }

    return LOOP_NORMAL;
}

Variable* interpretator_var_alloc(Interpretator *interpretator)
{
    Variable *alloc_var=interpretator->stack_head;
    interpretator->stack_head++;
    return alloc_var;
}

Variable* interpretator_pop_var(Interpretator *interpretator)
{
    interpretator->stack_head--;
    return interpretator->stack_head;
}

Variable* eval(Interpretator *interpretator, struct ListNode *i);

static void eval_args(Interpretator *interpretator, Array *args, Array *function_args)
{
    int       i;
    Variable *variable1,
             *variable2;
    List     *arg_expression;

    for(i=0; i<args->length && i<function_args->length; i++)
    {

        variable1=function_args->data[i];
        arg_expression=args->data[i];
        variable2=eval(interpretator, arg_expression->begin);

        switch(variable2->type)
        {
        case CONST_INTEGER:
        case STRING:
        //case ARRAY:
            ((int*)interpretator->stack_head)[variable1->shift]=variable2->shift;
            variable1->type=variable2->type;
            break;

        case MAP:
            //((int*)interpretator->stack_head)[variable1->shift]=variable2->shift;
            variable1->shift=variable2->shift;
            variable1->type=variable2->type;
            break;

        case ARRAY:
            //((int*)interpretator->stack_head)[variable1->shift]=variable2->shift;
            variable1->shift=variable2->shift;
            variable1->type=variable2->type;
            break;

        case INTEGER:
            ((int*)interpretator->stack_head)[variable1->shift]=interpretator->stack_base[variable2->shift];
            variable1->type=INTEGER;
            break;

        default:
            printf("\nне определён тип в аллокаторе стека аргументов");
            break;
        }
    }
}

Variable* run_function(Interpretator *interpretator, Function *f, Array *args)
{
    //Variable *return_var=undefined;
    int       i;
    int      *prev_base;

    //if(f->return_var)
        //return_var=interpretator_add_var(interpretator, f->return_var);

    if(f->return_var)
        interpretator->stack_head++;

    if(args)
        eval_args(interpretator, args, f->args);

    prev_base=interpretator->stack_base;
    interpretator->stack_base=(int*)interpretator->stack_head;
    *interpretator->stack_base=prev_base;
    interpretator->stack_head=(int*)interpretator->stack_head+f->variables->length+1;

    execute(interpretator, f->body);

    interpretator->stack_base=*(int*)interpretator->stack_base;

    if(f->return_var)
    {
        printf("\n<function ");
        str_print(f->name);
        printf(" return ");
        str_print(f->return_var->name);
        printf(">\n");
    }

    return f->return_var;
    //return return_var;
}

static int tree_str_function_cmp(String *s, Function *f)
{
    return str_comparision(s, f->name);
}

void interpretator(Function *main)
{
    undefined=new(Variable);
    undefined->name=str_init("undefined");
    undefined->type=UNDEFINED;

    Interpretator *interpretator=new(Interpretator);

    interpretator->stack=malloc(1000);
    interpretator->stack_head=interpretator->stack;

    run_function(interpretator, main, 0);
}
