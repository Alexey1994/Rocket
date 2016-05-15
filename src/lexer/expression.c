#include "expression.h"
#include "../Map.h"

int prior(char c)
{
    switch(c)
    {
        case '(': return 0;
        case '|': return 1;
        case '^': return 2;
        case '&': return 3;

        case '=':           // ==
        case '!': return 4; // !=

        case '<':           // <
        case '>':           // >
        case '1':           // <=
        case '2': return 5; // >=

        case '+':
        case '-': return 6;
        case '%':
        case '*':
        case '/': return 7;

        case '~': return 8;
        case '_': return 9; //_ - унарный -
    }
}

static Variable* get_anonim_function(Lexer *lexer)
{
    String   *unique_function_name=str_init(generate_id(lexer->id));
    Function *function;

    function=get_function(lexer, unique_function_name);
    if(!function)
        return 0;

    skip(lexer->f, lexer->head);

    return new_static_variable(lexer, str_init(generate_id(lexer->id)), FUNCTION, function);
}

static Variable* get_map(Lexer *lexer)
{
    String     *key;
    Variable   *map_variable;
    Map        *map=map_init();
    Assignment *assignment;

    map_variable=new_static_variable(lexer, str_init(generate_id(lexer->id)), MAP, map);

    *lexer->head=fgetc(lexer->f);

    while(*lexer->head!='}')
    {
        key=get_token_data(lexer->f, lexer->head);
        if(!key)
        {
            printf("invalid key\n");
            return 0;
        }

        skip(lexer->f, lexer->head);
        if(*lexer->head!=':')
        {
            printf("expected :\n");
            return 0;
        }
        *lexer->head=fgetc(lexer->f);

        assignment=new(Assignment);
        assignment->left_operand=array_init();

        array_push(assignment->left_operand, new_data(map_variable, 'v'));
        array_push(assignment->left_operand, new_data(key, '.'));
        assignment->right_expression=lexer_get_expression(lexer);

        if(!assignment->right_expression)
        {
            return 0;
        }

        list_push(lexer->cur_body, new_data(assignment, ASSIGNMENT));

        skip(lexer->f, lexer->head);
    }

    *lexer->head=fgetc(lexer->f);

    return map_variable;
}

static Variable* get_array(Lexer *lexer)
{
    Variable     *array_var;
    Array        *array;
    Variable     *array_index;
    List         *array_element;
    Push         *push_data;

    array=array_init();

    array_var=new_static_variable(lexer, str_init(generate_id(lexer->id)), ARRAY, array);

    *lexer->head=fgetc(lexer->f);
    skip(lexer->f, lexer->head);

    while(*lexer->head!=']')
    {
        push_data=new(Push);

        array_element=lexer_get_expression(lexer);

        if(!array_element)
            goto error;

        push_data->array=array_init();
        array_push(push_data->array, new_data(array_var, 'v'));
        push_data->expression=array_element;

        list_push(lexer->cur_body, new_data(push_data, PUSH));

        if(*lexer->head==',')
            *lexer->head=fgetc(lexer->f);

        skip(lexer->f, lexer->head);
    }

    *lexer->head=fgetc(lexer->f);
    skip(lexer->f, lexer->head);

    return array_var;

error:
    //array_free(array);
    return 0;
}

void pp(unsigned int *n)
{
    printf("\n%f\n", *n);
}

Variable* get_number(Lexer *lexer)
{
    Variable *number;
    char      is_real=0;
    float     real_value;

    str_clear(lexer->token);

    if(*lexer->head=='0')
    {
        *lexer->head=fgetc(lexer->f);
        if(*lexer->head=='x' || *lexer->head=='X')
        {
            loop
            {
                *lexer->head=fgetc(lexer->f);
                if(!is_hex_number(*lexer->head))
                    break;
                str_push(lexer->token, *lexer->head);
            }

            number=new_static_variable(lexer, str_init(generate_id(lexer->id)), CONST_INTEGER, str_hex_to_int(lexer->token));

            return number;
        }
        else
        {
            fseek(lexer->f, -2, SEEK_CUR);
            *lexer->head=fgetc(lexer->f);
        }
    }

    do
    {
        str_push(lexer->token, *lexer->head);
        *lexer->head=fgetc(lexer->f);
        skip(lexer->f, lexer->head);

        if(*lexer->head=='.')
        {
            str_push(lexer->token, ',');
            *lexer->head=fgetc(lexer->f);
            skip(lexer->f, lexer->head);

            if(is_real)
            {
                printf("\nнеправильная запись вещественного числа-две точки");
                return 0;
            }

            is_real=1;
        }
    }
    while(is_number(*lexer->head) || *lexer->head=='.');

    if(is_real)
    {
        real_value=str_to_float(lexer->token);
        return new_static_variable(lexer, str_init(generate_id(lexer->id)), CONST_REAL, *(int*)&real_value);
    }
    else
        return new_static_variable(lexer, str_init(generate_id(lexer->id)), CONST_INTEGER, str_to_int(lexer->token));
}

static Variable* get_const_string(FILE *f, char *head, char format_quotes)
{
    String   *const_string=str_init("");

    *head=fgetc(f);
    while(*head!=format_quotes)
    {
        if(feof(f))
        {
            str_free(const_string);
            return 0;
        }

        if(*head=='\\')
        {
            *head=fgetc(f);
            switch(*head)
            {
                case 'n': str_push(const_string, '\n'); break;
                case 't': str_push(const_string, '\t'); break;
                case 'b': str_push(const_string, '\b'); break;
                case 'r': str_push(const_string, '\r'); break;
                case '\\': str_push(const_string, '\\'); break;
            }
        }
        else
            str_push(const_string, *head);
        *head=fgetc(f);
    }
    *head=fgetc(f);

    return const_string;
}

static Variable* get_anonim_string(Lexer *lexer)
{
    String   *const_string;
    Variable *const_string_variable;

    const_string=get_const_string(lexer->f, lexer->head, *lexer->head);
    if(!const_string)
        return 0;

    const_string_variable=new_static_variable(lexer, str_init(generate_id(lexer->id)), STRING, const_string);

    return const_string_variable;
}

static char get_operand(Lexer *lexer, List *expression)
{
    Variable *var;
    Array    *arr;

    skip(lexer->f, lexer->head);

    if(is_true_word(lexer->f, lexer->head, "function"))
        var=get_anonim_function(lexer);
    else if(is_letter(*lexer->head))
    {
        get_token(lexer->f, lexer->head, lexer->token);
        arr=get_defined_operand(lexer);

        if(!arr)
            return 0;

        list_push(expression, new_data(arr, COMPOUND_OPERAND));
        return 1;
    }

    else if(is_number(*lexer->head))
        var=get_number(lexer);

    else if(*lexer->head=='[')
        var=get_array(lexer);

    else if(*lexer->head=='{')
        var=get_map(lexer);

    else if(*lexer->head=='\'' || *lexer->head=='"')
        var=get_anonim_string(lexer);

    else
    {
        printf("undefined operand\n");
        return 0;
    }

    if(!var)
        return 0;

    list_push(expression, new_data(var, OPERAND));

    return 1;
}

static char stack_contains_open_gap(NodeStack *i)
{
    for(; i; i=i->previouse)
        if(i->data=='(')
            return 1;
    return 0;
}

static void get_operation(List *expression, Stack *s, char operation, int (*prior)(char op))
{
    if(!s->begin || prior(s->begin->data)<prior(operation))
        push(s, operation);
    else
    {
        while(s->begin && prior(s->begin->data) >= prior(operation))
            list_push(expression, new_data(pop(s), OPERATION));

        push(s, operation);
    }
}

static char get_close_brace(Stack *s, List *expression)
{
    if(s->begin->data=='(')
    {
        printf("\nскобки без выражения излишни");
        return 0;
    }

    if(!stack_contains_open_gap(s->begin))
    {
        printf("\nтсутствует (");
        return 0;
    }

    do
    {
        if(!s->begin)
        {
            printf("\nотсутствует (");
            return 0;
        }

        list_push(expression, new_data(pop(s), OPERATION));
    }
    while(s->begin->data!='(');

    pop(s);

    return 1;
}

static char set_operation_state(
                                List *expression,
                                Stack *stack,
                                char *is_operation,
                                char *is_close_gap,
                                char(*prior)(char operation),
                                char operation
                               )
{
    if(*is_operation)
    {
        printf("2 operations\n");
        return 0;
    }

    get_operation(expression, stack, operation, prior);

    *is_operation=1;
    *is_close_gap=0;

    return 1;
}

List* lexer_get_expression(Lexer *lexer)
{
    Variable *operand;
    Stack    *s=stack_init();
    List     *expression=list_init();

    char      is_expression=1,
              is_operation=0,
              is_close_gap=0,
              op,
              is_compound_operator;

    while(!feof(lexer->f) && is_expression)
    {
        while(*lexer->head=='-' || *lexer->head=='~' || (*lexer->head=='!' && !is_true_word(lexer->f, lexer->head, "!=")))
        {
            *lexer->head=fgetc(lexer->f);
            skip(lexer->f, lexer->head);

            if(*lexer->head=='-')
                get_operation(expression, s, '_', prior);
            else
                get_operation(expression, s, *lexer->head, prior);
        }

        skip(lexer->f, lexer->head);

        if(*lexer->head!='(' && !is_close_gap)
        {
            if(!get_operand(lexer, expression))
                return 0;

            if(lexer->expr_token->length)
                break;

            is_operation=0;
        }

        is_compound_operator=0;

        switch(*lexer->head)
        {
        case '(':
            push(s, '(');
            is_operation=1;
            is_close_gap=0;
            break;

        case ')':
            get_close_brace(s, expression);
            is_operation=0;
            is_close_gap=1;
            break;

        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '^':
        case '=':

            if(!set_operation_state(expression, s, &is_operation, &is_close_gap, prior, *lexer->head))
                return 0;

            break;

        case '>':
            if(is_true_word(lexer->f, lexer->head, ">="))
            {
                if(!set_operation_state(expression, s, &is_operation, &is_close_gap, prior, '2'))
                    return 0;

                is_compound_operator=1;
            }
            else
            {
                if(!set_operation_state(expression, s, &is_operation, &is_close_gap, prior, '>'))
                    return 0;
            }
            break;

        case '<':
            if(is_true_word(lexer->f, lexer->head, "<="))
            {
                if(!set_operation_state(expression, s, &is_operation, &is_close_gap, prior, '1'))
                    return 0;

                is_compound_operator=1;
            }
            else
            {
                if(!set_operation_state(expression, s, &is_operation, &is_close_gap, prior, '<'))
                    return 0;
            }
            break;

        case '!':
            if(is_true_word(lexer->f, lexer->head, "!="))
            {
                if(!set_operation_state(expression, s, &is_operation, &is_close_gap, prior, '!'))
                    return 0;

                is_compound_operator=1;
            }
            else
            {
                printf("\nоператор отрицания '!' не на своём месте");
                return 0;
            }
            break;

        case '&':
            if(is_true_word(lexer->f, lexer->head, "&&"))
            {
                fseek(lexer->f, -3, SEEK_CUR);
                *lexer->head=fgetc(lexer->f);

                is_expression=0;
            }
            else
            {
                if(!set_operation_state(expression, s, &is_operation, &is_close_gap, prior, '&'))
                    return 0;
            }
            break;

        case '|':
            if(is_true_word(lexer->f, lexer->head, "||"))
            {
                fseek(lexer->f, -3, SEEK_CUR);
                *lexer->head=fgetc(lexer->f);

                is_expression=0;
            }
            else
            {
                if(!set_operation_state(expression, s, &is_operation, &is_close_gap, prior, '|'))
                    return 0;
            }
            break;

        default:
            is_expression=0;
        }

        if(is_expression && !is_compound_operator)
        {
            *lexer->head=fgetc(lexer->f);
            skip(lexer->f, lexer->head);
        }

        str_clear(lexer->expr_token);
    }

    while(s->begin)
    {
        op=pop(s);

        if(op=='(')
        {
            printf("expected )\n");
            return 0;
        }

        list_push(expression, new_data(op, OPERATION));
    }

    return expression;
}

static int condition_prior(char c)
{
    switch(c)
    {
        case '(': return 0;
        case '|': return 1;
        case '!':
        case '&': return 2;
    }
}

List* lexer_get_condition(Lexer *lexer)
{
    Variable *operand;
    Stack    *s=stack_init();
    List     *condition=list_init(),
             *arithmetic_expression;

    char      is_condition=1,
              is_operation=0,
              is_close_gap=0,
              op;

    while(!feof(lexer->f) && is_condition)
    {
        while(*lexer->head=='!')
        {
            *lexer->head=fgetc(lexer->f);
            skip(lexer->f, lexer->head);

            get_operation(condition, s, '!', condition_prior);
        }

        skip(lexer->f, lexer->head);

        if(*lexer->head!='(' && !is_close_gap)
        {
            arithmetic_expression=lexer_get_expression(lexer);

            if(!arithmetic_expression)
                return 0;

            list_push(condition, arithmetic_expression);

            is_operation=0;
        }

        switch(*lexer->head)
        {
        case '(':
            push(s, '(');
            is_operation=1;
            is_close_gap=0;
            break;

        case ')':
            get_close_brace(s, condition);
            is_operation=0;
            is_close_gap=1;
            break;

        case '&':
            *lexer->head=fgetc(lexer->f);
            if(*lexer->head!='&')
            {
                printf("\nundefined operation &%c", *lexer->head);
                return 0;
            }

            if(is_operation)
            {
                printf("2 operations\n");
                return 0;
            }

            get_operation(condition, s, *lexer->head, condition_prior);

            is_operation=1;
            is_close_gap=0;
            break;

        case '|':
            *lexer->head=fgetc(lexer->f);
            if(*lexer->head!='|')
            {
                printf("\nundefined operation &%c", *lexer->head);
                return 0;
            }

            if(is_operation)
            {
                printf("2 operations\n");
                return 0;
            }

            get_operation(condition, s, *lexer->head, condition_prior);

            is_operation=1;
            is_close_gap=0;
            break;

        default:
            is_condition=0;
        }

        if(is_condition)
        {
            *lexer->head=fgetc(lexer->f);
            skip(lexer->f, lexer->head);
        }

        str_clear(lexer->expr_token);
    }

    while(s->begin)
    {
        op=pop(s);

        if(op=='(')
        {
            printf("expected )\n");
            return 0;
        }

        list_push(condition, new_data(op, OPERATION));
    }

    return condition;
}
