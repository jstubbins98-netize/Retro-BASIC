#include "config.h"
#include "expr.h"
#include "vars.h"

static unsigned int rng_state = 12345;

void skip_spaces(char **p)
{
    while (**p == ' ') (*p)++;
}

int match_keyword(char **p, const char *kw)
{
    char *start = *p;
    skip_spaces(p);
    while (*kw) {
        if (**p != *kw) {
            *p = start;
            return 0;
        }
        (*p)++;
        kw++;
    }
    return 1;
}

int parse_number(char **p)
{
    int v = 0;
    int neg = 0;

    skip_spaces(p);

    if (**p == '-') {
        neg = 1;
        (*p)++;
    }

    while (**p >= '0' && **p <= '9') {
        v = (v * 10) + (**p - '0');
        (*p)++;
    }

    return neg ? -v : v;
}

static int rnd(int n)
{
    rng_state = rng_state * 25173 + 13849;
    if (n <= 0) return (int)(rng_state & 0x7FFF);
    return (int)((rng_state & 0x7FFF) % (unsigned int)n);
}

static int int_abs(int n)
{
    return n < 0 ? -n : n;
}

static int int_sgn(int n)
{
    if (n > 0) return 1;
    if (n < 0) return -1;
    return 0;
}

static int int_sqrt(int n)
{
    int r = 0;
    int t;
    if (n <= 0) return 0;
    t = n;
    while (t > 0) {
        r++;
        t -= (2 * r - 1);
    }
    if (r * r > n) r--;
    return r;
}

static int int_min(int a, int b)
{
    return a < b ? a : b;
}

static int int_max(int a, int b)
{
    return a > b ? a : b;
}

static int parse_primary(char **p);
static int parse_unary(char **p);
static int parse_power(char **p);
static int parse_mul(char **p);
static int parse_add(char **p);
static int parse_compare(char **p);
static int parse_logic(char **p);

static int parse_primary(char **p)
{
    int v = 0;
    int idx, i1, i2;

    skip_spaces(p);

    if (**p == '(') {
        (*p)++;
        v = parse_logic(p);
        skip_spaces(p);
        if (**p == ')') (*p)++;
        return v;
    }

    if (**p == 'A' && *(*p+1) == 'B' && *(*p+2) == 'S') {
        *p += 3;
        skip_spaces(p);
        if (**p == '(') (*p)++;
        v = parse_logic(p);
        skip_spaces(p);
        if (**p == ')') (*p)++;
        return int_abs(v);
    }

    if (**p == 'S' && *(*p+1) == 'G' && *(*p+2) == 'N') {
        *p += 3;
        skip_spaces(p);
        if (**p == '(') (*p)++;
        v = parse_logic(p);
        skip_spaces(p);
        if (**p == ')') (*p)++;
        return int_sgn(v);
    }

    if (**p == 'S' && *(*p+1) == 'Q' && *(*p+2) == 'R') {
        *p += 3;
        skip_spaces(p);
        if (**p == '(') (*p)++;
        v = parse_logic(p);
        skip_spaces(p);
        if (**p == ')') (*p)++;
        return int_sqrt(v);
    }

    if (**p == 'R' && *(*p+1) == 'N' && *(*p+2) == 'D') {
        *p += 3;
        skip_spaces(p);
        if (**p == '(') (*p)++;
        v = parse_logic(p);
        skip_spaces(p);
        if (**p == ')') (*p)++;
        return rnd(v);
    }

    if (**p == 'M' && *(*p+1) == 'I' && *(*p+2) == 'N') {
        int a, b;
        *p += 3;
        skip_spaces(p);
        if (**p == '(') (*p)++;
        a = parse_logic(p);
        skip_spaces(p);
        if (**p == ',') (*p)++;
        b = parse_logic(p);
        skip_spaces(p);
        if (**p == ')') (*p)++;
        return int_min(a, b);
    }

    if (**p == 'M' && *(*p+1) == 'A' && *(*p+2) == 'X') {
        int a, b;
        *p += 3;
        skip_spaces(p);
        if (**p == '(') (*p)++;
        a = parse_logic(p);
        skip_spaces(p);
        if (**p == ',') (*p)++;
        b = parse_logic(p);
        skip_spaces(p);
        if (**p == ')') (*p)++;
        return int_max(a, b);
    }

    if (**p == 'P' && *(*p+1) == 'E' && *(*p+2) == 'E' && *(*p+3) == 'K') {
        *p += 4;
        skip_spaces(p);
        if (**p == '(') (*p)++;
        v = parse_logic(p);
        skip_spaces(p);
        if (**p == ')') (*p)++;
        if (v < 0 || v >= SIM_MEM_SIZE) return 0;
        return (int)sim_mem[v];
    }

    if (**p == 'L' && *(*p+1) == 'E' && *(*p+2) == 'N') {
        *p += 3;
        skip_spaces(p);
        if (**p == '(') (*p)++;
        skip_spaces(p);
        if (**p >= 'A' && **p <= 'Z' && *(*p+1) == '$') {
            idx = **p - 'A';
            *p += 2;
            skip_spaces(p);
            if (**p == ')') (*p)++;
            return str_len(str_vars[idx]);
        }
        skip_spaces(p);
        if (**p == ')') (*p)++;
        return 0;
    }

    if (**p >= 'A' && **p <= 'Z') {
        char var = **p;

        if (*(*p+1) == '(') {
            idx = var - 'A';
            *p += 2;
            i1 = parse_logic(p);
            i2 = 0;
            skip_spaces(p);
            if (**p == ',') {
                (*p)++;
                i2 = parse_logic(p);
            }
            skip_spaces(p);
            if (**p == ')') (*p)++;
            return array_get(idx, i1, i2);
        }

        if (*(*p+1) != '$' &&
            !(*(*p+1) >= 'A' && *(*p+1) <= 'Z')) {
            v = vars[var - 'A'];
            (*p)++;
            return v;
        }
    }

    while (**p >= '0' && **p <= '9') {
        v = (v * 10) + (**p - '0');
        (*p)++;
    }

    return v;
}

static int parse_unary(char **p)
{
    skip_spaces(p);

    if (**p == '-') {
        (*p)++;
        return -parse_unary(p);
    }

    if (**p == 'N' && *(*p+1) == 'O' && *(*p+2) == 'T') {
        *p += 3;
        return ~parse_unary(p);
    }

    return parse_primary(p);
}

static int parse_power(char **p)
{
    int base = parse_unary(p);
    int exp, result, i;

    skip_spaces(p);
    if (**p == '^') {
        (*p)++;
        exp = parse_unary(p);
        result = 1;
        for (i = 0; i < exp; i++)
            result *= base;
        return result;
    }
    return base;
}

static int parse_mul(char **p)
{
    int v = parse_power(p);
    int t;

    while (1) {
        skip_spaces(p);

        if (**p == '*') {
            (*p)++;
            t = parse_power(p);
            v *= t;
        }
        else if (**p == '/') {
            (*p)++;
            t = parse_power(p);
            if (t != 0) v /= t;
        }
        else if (**p == 'M' && *(*p+1) == 'O' && *(*p+2) == 'D') {
            *p += 3;
            t = parse_power(p);
            if (t != 0) v %= t;
        }
        else
            break;
    }
    return v;
}

static int parse_add(char **p)
{
    int v = parse_mul(p);
    int t;

    while (1) {
        skip_spaces(p);

        if (**p == '+') {
            (*p)++;
            t = parse_mul(p);
            v += t;
        }
        else if (**p == '-') {
            (*p)++;
            t = parse_mul(p);
            v -= t;
        }
        else
            break;
    }
    return v;
}

static int parse_compare(char **p)
{
    int v = parse_add(p);
    int t;

    skip_spaces(p);

    if (**p == '<' && *(*p+1) == '>') {
        *p += 2;
        t = parse_add(p);
        return (v != t) ? -1 : 0;
    }
    if (**p == '<' && *(*p+1) == '=') {
        *p += 2;
        t = parse_add(p);
        return (v <= t) ? -1 : 0;
    }
    if (**p == '>' && *(*p+1) == '=') {
        *p += 2;
        t = parse_add(p);
        return (v >= t) ? -1 : 0;
    }
    if (**p == '<') {
        (*p)++;
        t = parse_add(p);
        return (v < t) ? -1 : 0;
    }
    if (**p == '>') {
        (*p)++;
        t = parse_add(p);
        return (v > t) ? -1 : 0;
    }
    if (**p == '=' && *(*p+1) != '=') {
        (*p)++;
        t = parse_add(p);
        return (v == t) ? -1 : 0;
    }

    return v;
}

static int parse_logic(char **p)
{
    int v = parse_compare(p);
    int t;

    while (1) {
        skip_spaces(p);

        if (**p == 'A' && *(*p+1) == 'N' && *(*p+2) == 'D') {
            *p += 3;
            t = parse_compare(p);
            v = v & t;
        }
        else if (**p == 'O' && *(*p+1) == 'R') {
            *p += 2;
            t = parse_compare(p);
            v = v | t;
        }
        else
            break;
    }
    return v;
}

int eval_expr(char **p)
{
    return parse_logic(p);
}

int parse_string_expr(char **p, char *dst, int max)
{
    int i = 0;
    int idx;
    char tmp[MAX_STR_LEN];

    skip_spaces(p);

    dst[0] = 0;

    if (**p == '"') {
        (*p)++;
        while (**p && **p != '"' && i < max - 1) {
            dst[i++] = **p;
            (*p)++;
        }
        if (**p == '"') (*p)++;
        dst[i] = 0;
    }
    else if (**p >= 'A' && **p <= 'Z' && *(*p+1) == '$') {
        idx = **p - 'A';
        *p += 2;
        str_copy(dst, str_vars[idx], max);
    }
    else if (**p == 'L' && *(*p+1) == 'E' && *(*p+2) == 'F'
          && *(*p+3) == 'T' && *(*p+4) == '$') {
        *p += 5;
        skip_spaces(p);
        if (**p == '(') (*p)++;
        parse_string_expr(p, tmp, MAX_STR_LEN);
        skip_spaces(p);
        if (**p == ',') (*p)++;
        i = eval_expr(p);
        skip_spaces(p);
        if (**p == ')') (*p)++;
        str_left(dst, tmp, i);
    }
    else if (**p == 'R' && *(*p+1) == 'I' && *(*p+2) == 'G'
          && *(*p+3) == 'H' && *(*p+4) == 'T' && *(*p+5) == '$') {
        *p += 6;
        skip_spaces(p);
        if (**p == '(') (*p)++;
        parse_string_expr(p, tmp, MAX_STR_LEN);
        skip_spaces(p);
        if (**p == ',') (*p)++;
        i = eval_expr(p);
        skip_spaces(p);
        if (**p == ')') (*p)++;
        str_right(dst, tmp, i);
    }
    else if (**p == 'M' && *(*p+1) == 'I' && *(*p+2) == 'D' && *(*p+3) == '$') {
        int start, len;
        *p += 4;
        skip_spaces(p);
        if (**p == '(') (*p)++;
        parse_string_expr(p, tmp, MAX_STR_LEN);
        skip_spaces(p);
        if (**p == ',') (*p)++;
        start = eval_expr(p);
        skip_spaces(p);
        len = MAX_STR_LEN;
        if (**p == ',') {
            (*p)++;
            len = eval_expr(p);
        }
        skip_spaces(p);
        if (**p == ')') (*p)++;
        str_mid(dst, tmp, start, len);
    }
    else {
        return 0;
    }

    skip_spaces(p);
    while (**p == '+') {
        (*p)++;
        skip_spaces(p);
        parse_string_expr(p, tmp, MAX_STR_LEN);
        str_concat(dst, dst, tmp, max);
        skip_spaces(p);
    }

    return 1;
}
