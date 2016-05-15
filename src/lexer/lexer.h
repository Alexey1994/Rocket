#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

#include <stdio.h>
#include "../tree.h"

Tree* lexer(FILE *f);
void lexer_table_init();

#endif // LEXER_H_INCLUDED
