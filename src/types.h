#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include "String.h"
#include "tree.h"
#include "stack.h"
#include "List.h"
#include "Array.h"

#define UNDEFINED     0x00
#define STRING        0x01
#define CONST_INTEGER 0x02
#define INTEGER       0x03
#define GC_INTEGER    0x04
#define ARRAY         0x05
#define MAP           0x06
#define REAL          0x07
#define CONST_REAL    0x08
#define BOOLEAN       0x09
#define FUNCTION      0x0a

#define END           0x01
#define PRINT         0x02
#define IF            0x03
#define WHILE         0x04
#define LOOP          0x05
#define BREAK         0x06
#define CONTINUE      0x07
#define ELSE          0x09
#define DO            0x0b
#define CALL          0x0c
#define ASSIGNMENT    0x0d
#define RETURN        0x0e
#define PUSH          0x0f

typedef struct
{
    char    is_closed;
    char    type;
    String *name;
    int     shift;
}
Variable;

typedef struct
{
    String    *name;
    Tree      *functions;
    List      *body;
    Array     *args;
    Variable  *return_var;
    Tree      *variables;
    int        count_vars;
}
Function;


typedef struct
{
    List  *cond;
    List  *body;
    List  *else_body;
}
If;

typedef struct
{
    List *cond,
         *body;
}
While;

typedef struct
{
    List *cond,
         *body;
}
Do;

typedef struct
{
    List *expression;
}
Print;

typedef struct
{
    Variable *function;
    Array    *args;
}
Call;

typedef struct
{
    Array *left_operand;
    List  *right_expression;
}
Assignment;

typedef struct
{
    List *array,
         *expression;
}
Push;

extern Variable *undefined;

#endif // TYPES_H_INCLUDED
