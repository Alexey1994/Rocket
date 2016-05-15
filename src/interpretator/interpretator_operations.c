#include "interpretator_operations.h"
#include "../Array.h"
#include "../Map.h"

Variable* eval(Interpretator *interpretator, struct ListNode *i);
static Variable* eval_left_operand(Array *operands, Interpretator *interpretator, char new_type, Variable *right_variable);

static Variable* interpretator_heap_var_alloc(Interpretator *interpretator, Variable *variable)
{
    Variable *new_variable=new(Variable);

    new_variable->is_closed=variable->is_closed;
    new_variable->type=variable->type;
    new_variable->name=variable->name;
    new_variable->shift=variable->shift;

    return new_variable;
}

Variable* interpretator_add_var(Interpretator *interpretator, Variable *var)
{
    Variable* new_var=interpretator_var_alloc(interpretator);

    new_var->name=var->name;

    switch(var->type)
    {
    case UNDEFINED:
        ///////////////////////
        //new_var->type=UNDEFINED;
        //new_var->shift=var->shift;
        //printf("\n:)");
        break;

    case REAL:
        new_var->type=CONST_REAL;
        new_var->shift=interpretator->stack_base[var->shift];
        break;

    case CONST_REAL:
        new_var->type=CONST_REAL;
        new_var->shift=var->shift;
        break;

    case INTEGER:
        new_var->type=CONST_INTEGER;
        new_var->shift=interpretator->stack_base[var->shift];
        break;

    case CONST_INTEGER:
        new_var->type=CONST_INTEGER;
        new_var->shift=var->shift;
        break;

    case GC_INTEGER:
        new_var->type=CONST_INTEGER;
        new_var->shift=*(int*)var->shift;
        break;

    case STRING:
        new_var->type=STRING;
        new_var->shift=var->shift;
        break;

    case MAP:
        new_var->type=MAP;
        new_var->shift=var->shift;
        break;

    case ARRAY:
        new_var->type=ARRAY;
        new_var->shift=var->shift;
        break;

    default:
        printf("\nне определён тип в аллокаторе стека");
        break;
    }

    return new_var;
}

GCVariable* alloc_gc_variable(Variable *variable, Interpretator *interpretator)
{
    GCVariable *gc_variable;

    switch(variable->type)
    {
    case INTEGER:
        gc_variable=new(GCVariable);
        gc_variable->data=*((int*)interpretator->stack_base+variable->shift);
        break;

    case CONST_INTEGER:
        gc_variable=new(GCVariable);
        gc_variable->data=variable->shift;
        break;

    case GC_INTEGER:
        ((GCVariable*)variable->shift)->count++;
        return variable->shift;
        break;

    default:
        printf("\nне определён тип в аллокаторе кучи");
        break;
    }

    gc_variable->count=1;

    return gc_variable;
}

void free_gc_variable(GCVariable *gc_variable)
{
    gc_variable->count--;

    if(!gc_variable->count)
    {
        free(gc_variable->data);
        free(gc_variable);
    }
}

char interpretator_if(If *if_data, Interpretator *interpretator)
{
    Variable *cond=eval(interpretator, if_data->cond->begin);

    printf("\n<if");

    switch(cond->type)
    {
    case INTEGER:
        printf(" INTEGER>");

        if(interpretator->stack_base[cond->shift])
            return execute(interpretator, if_data->body);
        else
            return execute(interpretator, if_data->else_body);

        break;

    case CONST_INTEGER:
        printf(" INTEGER>");

        if(cond->shift)
            return execute(interpretator, if_data->body);
        else
            return execute(interpretator, if_data->else_body);

        break;
    }

    return LOOP_NORMAL;
}

char interpretator_loop(List *loop_body, Interpretator *interpretator)
{
    while(execute(interpretator, loop_body)!=LOOP_BREAK);

    return LOOP_NORMAL;
}

char interpretator_while(While *while_data, Interpretator *intepretator)
{
    while(while_data->cond && execute(interpretator, while_data->body)!=LOOP_BREAK);

    return LOOP_NORMAL;
}

char interpretator_do(Do *do_data, Interpretator *intepretator)
{
    if(execute(interpretator, do_data->body)!=LOOP_BREAK)
        while(do_data->cond && execute(interpretator, do_data->body)!=LOOP_BREAK);

    return LOOP_NORMAL;
}

char interpretator_break()
{
    return LOOP_BREAK;
}

char interpretator_continue()
{
    return LOOP_CONTINUE;
}

static char* get_left_variable_data(Interpretator *interpretator, Variable *variable, Variable *variable2)
{
    switch(variable->type)
    {
    case CONST_REAL:
        return &variable->shift;
        break;

    case CONST_INTEGER:
        return &variable->shift;

    default:
        printf("\nТЕСТ не определен тип при вычислении левой переменной номер типа %d", variable->type);
        break;
    }
}

static void add(Interpretator *interpretator)
{
    Variable *op2=interpretator_pop_var(interpretator),
             *op1=interpretator->stack_head-1;

    switch(op1->type)
    {
    case CONST_INTEGER:
        switch(op2->type)
        {
        case CONST_INTEGER:
            op1->shift+=op2->shift;
            break;

        case CONST_REAL:
            op1->type=CONST_REAL;
            *(float*)&op1->shift=(float)op1->shift;

            *(float*)&op1->shift+=*(float*)&op2->shift;
            break;
        }
        break;

    case CONST_REAL:
        switch(op2->type)
        {
        case CONST_INTEGER:
            op2->type=CONST_REAL;
            *(float*)&op2->shift=(float)op2->shift;

            *(float*)&op1->shift+=*(float*)&op2->shift;
            break;

        case CONST_REAL:
            *(float*)&op1->shift+=*(float*)&op2->shift;
            break;
        }
        break;
    }
}

static void sub(Interpretator *interpretator)
{
    Variable *op2=interpretator_pop_var(interpretator),
             *op1=interpretator->stack_head-1;
    char     *left_variable_data=get_left_variable_data(interpretator, interpretator->stack_head-1, op2);

    switch(op2->type)
    {
    case CONST_INTEGER:
        *(int*)left_variable_data-=op2->shift;
        break;

    default:
        printf("\nТЕСТ не определен тип при вычислении правой переменной в вычитании");
        break;
    }
}

static void mul(Interpretator *interpretator)
{
    Variable *op2=interpretator_pop_var(interpretator),
             *op1=interpretator->stack_head-1;
    char     *left_variable_data=get_left_variable_data(interpretator, interpretator->stack_head-1, op2);

    switch(op2->type)
    {
    case CONST_INTEGER:
        *(int*)left_variable_data*=op2->shift;
        break;

    default:
        printf("\nТЕСТ не определен тип при вычислении правой переменной в умножении");
        break;
    }
}

static void division(Interpretator *interpretator)
{
    Variable *op2=interpretator_pop_var(interpretator),
             *op1=interpretator->stack_head-1;
    char     *left_variable_data=get_left_variable_data(interpretator, interpretator->stack_head-1, op2);

    switch(op2->type)
    {
    case CONST_INTEGER:
        *(int*)left_variable_data/=op2->shift;
        break;

    default:
        printf("\nТЕСТ не определен тип при вычислении правой переменной в делении");
        break;
    }
}

static void lesser(Interpretator *interpretator)
{
    Variable *op2=interpretator_pop_var(interpretator);
    char     *left_variable_data=get_left_variable_data(interpretator, interpretator->stack_head-1, op2);

    switch(op2->type)
    {
    case CONST_INTEGER:
        *(int*)left_variable_data=*(int*)left_variable_data<op2->shift;
        break;

    default:
        printf("\nТЕСТ не определен тип при вычислении правой переменной в меньше");
        break;
    }
}

static char* (*op[256])(Interpretator *interpretator);

void interpretator_operation_table_init()
{
    op['+']=add;
    op['-']=sub;
    op['*']=mul;
    op['/']=division;

    op['<']=lesser;
}


static Variable* eval_operand(Array *arr, Interpretator *interpretator)
{
    Data     *data;

    Variable *prev_var,
             *cur_var;
    int       i;
    List     *index;

    Array    *array;
    Map      *map;
    List     *expression;

    for(i=0; i<arr->length; i++)
    {
        data=arr->data[i];
        switch(data->type)
        {
        case 'v':
            cur_var=data->data;
            break;

        case '[':
            index=data->data;

            if(cur_var->type!=ARRAY)
            {
                printf("\nvariable '");
                str_print(cur_var->name);
                printf("' is not array");

                return undefined;
            }

            array=cur_var->shift;

            cur_var=eval(interpretator, index->begin);

            switch(cur_var->type)
            {
            case CONST_INTEGER:
                if(cur_var->shift>=array->length)
                {
                    printf("\nout of range in array");
                    return cur_var;
                }

                cur_var=array->data[cur_var->shift];
                break;
            }
            break;

        case '(':
            if(cur_var->type!=FUNCTION)
            {
                printf("\nvariable ");
                str_print(cur_var->name);
                printf(" is not function");

                return 0;
            }

            cur_var=run_function(interpretator, cur_var->shift, 0);
            break;

        case '.':
            if(cur_var->type!=MAP)
            {
                printf("\nvariable ");
                str_print(cur_var->name);
                printf(" is not map");

                return 0;
            }

            map=cur_var->shift;

            cur_var=map_get(map, data->data);
            if(!cur_var)
            {
                printf("\nkey ");
                str_print(data->data);
                printf(" not found");

                return undefined;
            }
            break;
        }
    }

    return cur_var;
}

Variable* eval(Interpretator *interpretator, struct ListNode *i)
{
    Variable *rez,
             *var,
             *op1,
             *op2;
    Data     *data;
    int      *op1_data,
             *op2_data;

    for(; i; i=i->next)
    {
        data=i->data;

        switch(data->type)
        {
            case OPERAND:
                interpretator_add_var(interpretator, data->data);
                break;

            case COMPOUND_OPERAND:
                var=eval_operand(data->data, interpretator);

                if(!var)
                    return undefined;

                interpretator_add_var(interpretator, var);
                break;

            case OPERATION:
                op[(char)data->data](interpretator);

                break;
        }
    }

    //return interpretator_pop_var(interpretator);
    return interpretator->stack_head-1;
}

char interpretator_call(Array *call_data, Interpretator *interpretator)
{
    char     *result;

    result=eval_left_operand(call_data, interpretator, FUNCTION, 0);

    return LOOP_NORMAL;
}


static Variable* eval_left_operand(Array *operands, Interpretator *interpretator, char new_type, Variable *right_variable)
{
    Data     *data;

    Variable *prev_var,
             *cur_var,
             *variable;
    int       i,
              index;
    List     *index_expression;

    Array    *array;
    Map      *map;
    List     *expression;
    Array    *args;
    int       j;
    Function *function;


    for(i=0; i<operands->length; i++)
    {
        data=operands->data[i];

        switch(data->type)
        {
        case 'v':
            cur_var=data->data;
            break;

        case '[':
            index_expression=data->data;

            if(cur_var->type!=ARRAY)
            {
                printf("\nпеременная '");
                str_print(cur_var->name);
                printf("' не является массивом");

                return 0;
            }

            array=cur_var->shift;

            cur_var=eval(interpretator, index_expression->begin);

            switch(cur_var->type)
            {
            case CONST_INTEGER:
                if(cur_var->shift>=array->length)
                {
                    printf("\nout of range in array");
                    return cur_var;
                }

                cur_var=array->data[cur_var->shift];

                break;
            }

            break;

        case '(':
            if(cur_var->type!=FUNCTION)
            {
                printf("\nvariable ");
                str_print(cur_var->name);
                printf(" is not function");

                return 0;
            }

            args=data->data;

            cur_var=run_function(interpretator, cur_var->shift, data->data);
            break;

        case '.':

            if(cur_var->type!=MAP)
            {
                printf("\nvariable ");
                str_print(cur_var->name);
                printf(" is not map");

                return undefined;
            }

            map=cur_var->shift;

            cur_var=map_get(map, data->data);
            if(!cur_var)
            {
                map_add(map, data->data, undefined);
                cur_var=map_get(map, data->data);
            }

            if(i==operands->length-1)
            {
                cur_var->type=new_type;
                return &cur_var->shift;
            }

            break;
        }
    }

    switch(new_type)
    {
        case INTEGER: case CONST_INTEGER:
            cur_var->type=INTEGER;
            break;

        case STRING:
            cur_var->type=STRING;
            return &cur_var->shift;
            break;

        case ARRAY:
            cur_var->type=ARRAY;
            return &cur_var->shift;
            break;

        case FUNCTION:
            cur_var->type=FUNCTION;
            return &cur_var->shift;
            break;

        case MAP:
            cur_var->type=MAP;
            return &cur_var->shift;
            break;
    }

    if(cur_var->is_closed)
    {

    }
    else
        return (int*)interpretator->stack_base+cur_var->shift;
}

char interpretator_assignment(Assignment *assignment_data, Interpretator *interpretator)
{
    Variable   *result=eval(interpretator, assignment_data->right_expression->begin);
    char       *left_variable=eval_left_operand(assignment_data->left_operand, interpretator, result->type, result);

    switch(result->type)
    {
    case CONST_REAL:
        *(float*)left_variable=*(float*)&result->shift;
        break;

    case CONST_INTEGER:
        *(int*)left_variable=result->shift;
        break;

    case GC_INTEGER:
        printf("ASSIGNMENT: OPA");
        break;

    case ARRAY:
        *(int*)left_variable=result->shift;
        break;

    case FUNCTION:
        *(int*)left_variable=result->shift;
        break;

    case MAP:
        *(int*)left_variable=result->shift;
        break;

    case STRING:
        *(int*)left_variable=result->shift;
        break;

    default:
        printf("ASSIGNMENT: OPA %d", result->type);
        break;
    }

    return LOOP_NORMAL;
}

static void print_map_node(MapNode *map_node)
{
    printf("\n");
    str_print(map_node->key);
}

static void print_variable(Interpretator *interpretator, Variable *result)
{
    GCVariable *gc_variable;
    int         i;
    Array      *array;
    List       *expression;
    Map        *map;

    if(!result)
    {
        printf("\nошибка при печати-переменная не проинициализирована");
        return;
    }

    switch(result->type)
    {
    case UNDEFINED:
        printf("undefined");
        break;

    case STRING:
        str_print(result->shift);
        break;

    case INTEGER:
        printf("%d", *((int*)interpretator->stack_base+result->shift));
        break;

    case CONST_INTEGER:
        printf("%d", result->shift);
        break;

    case GC_INTEGER:
        gc_variable=result->shift;
        printf("%d", gc_variable->data);
        break;

    case ARRAY:
        array=result->shift;

        printf("[");
        for(i=0; i<array->length-1; i++)
        {
            print_variable(interpretator, array->data[i]);
            printf(", ");
        }

        if(array->length)
            print_variable(interpretator, array->data[array->length-1]);

        printf("]");
        break;

    case FUNCTION:
        printf("\nfunction ");
        str_print(result->name);
        printf(" 0x%08x", result->shift);
        break;

    case MAP:
        map=result->shift;

        printf("\n{");
        tree_print(map->tree, print_map_node);
        printf("\n}");
        break;

    case CONST_REAL:
        printf("%f", *(float*)&result->shift);
        break;
    }
}

char interpretator_print(Print *print, Interpretator *interpretator)
{
    print_variable(interpretator, eval(interpretator, print->expression->begin));

    return LOOP_NORMAL;
}

char interpretator_return(List *return_expression, Interpretator *interpretator)
{
    Variable *return_variable=eval(interpretator, return_expression->begin),
             *stack_variable=(Variable*)interpretator->stack_base-1;

    stack_variable->is_closed=return_variable->is_closed;
    stack_variable->name=return_variable->name;
    stack_variable->shift=return_variable->shift;
    stack_variable->type=return_variable->type;

    return RETURN;
}

char interpretator_push(Push *push_data, Interpretator *interpretator)
{
    Variable *array_variable=eval_operand(push_data->array, interpretator),
             *array_element;
    Array    *array;

    if(array_variable->type!=ARRAY)
    {
        printf("\npush работает только с массивами");
        return LOOP_NORMAL;
    }

    array=array_variable->shift;
    array_element=interpretator_heap_var_alloc(interpretator, eval(interpretator, push_data->expression->begin));

    array_push(array, array_element);

    return LOOP_NORMAL;
}
