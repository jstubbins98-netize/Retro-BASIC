#ifndef EXPR_H
#define EXPR_H

int eval_expr(char **p);
int parse_string_expr(char **p, char *dst, int max);
void skip_spaces(char **p);
int parse_number(char **p);
int match_keyword(char **p, const char *kw);

#endif
