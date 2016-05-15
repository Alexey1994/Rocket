#ifndef STRINSSS_H_INCLUDED
#define STRINSSS_H_INCLUDED

#ifndef STRING_BUF_RESERVE
    #define STRING_BUF_RESERVE 100
#endif

//#define str_comparision(s1, s2) strcmp((s1->begin), (s2->begin))

typedef struct
{
    char *begin, *end;
    int length, length_begin;
    int reserve_count_end, reserve_count_begin;
}String;

int str_comparision(String *s1, String *s2);
void str_concat(String *s1, String *s2);
void str_push(String *s, char data);
char str_pop(String *s);
void str_copy(String *str1, String *str2);
void str_clear(String *s);
String *str_init(const char *c);
void str_free(String *s);

void str_print(String *s);

int str_to_int(String *s);
float str_to_float(String *s);
int str_hex_to_int(String *s);

#endif // STRINSSS_H_INCLUDED
