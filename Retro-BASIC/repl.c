#include "config.h"
#include "repl.h"
#include "io.h"
#include "vars.h"
#include "expr.h"
#include "program.h"
#include "commands.h"

static int is_digit(char c)
{
    return (c >= '0' && c <= '9');
}

static void parse_filename(char **p, char *out, int maxlen)
{
    int i = 0;
    while (**p == ' ') (*p)++;
    if (**p == '"') {
        (*p)++;
        while (**p && **p != '"' && i < maxlen - 1)
            out[i++] = *(*p)++;
        if (**p == '"') (*p)++;
    } else {
        while (**p && i < maxlen - 1)
            out[i++] = *(*p)++;
        while (i > 0 && (out[i-1] == ' ' || out[i-1] == '\t'))
            i--;
    }
    out[i] = '\0';
}

static unsigned int parse_linenum(char **p)
{
    unsigned int n = 0;
    while (**p >= '0' && **p <= '9') {
        n = (n * 10) + (**p - '0');
        (*p)++;
    }
    return n;
}

static int kw_match(char **p, const char *kw)
{
    char *start = *p;
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

static int is_var_assign(char *p)
{
    if (*p < 'A' || *p > 'Z') return 0;
    p++;
    if (*p == '$' || *p == '(') return 1;
    while (*p == ' ') p++;
    return (*p == '=');
}

static int is_string_expr(char *p)
{
    if (*p == '"') return 1;
    if (*p >= 'A' && *p <= 'Z' && *(p+1) == '$') return 1;
    if (*p == 'L' && *(p+1) == 'E' && *(p+2) == 'F' && *(p+3) == 'T' && *(p+4) == '$') return 1;
    if (*p == 'R' && *(p+1) == 'I' && *(p+2) == 'G' && *(p+3) == 'H' && *(p+4) == 'T' && *(p+5) == '$') return 1;
    if (*p == 'M' && *(p+1) == 'I' && *(p+2) == 'D' && *(p+3) == '$') return 1;
    return 0;
}

static void direct_print(char *p)
{
    char strbuf[MAX_STR_LEN];

    while (*p == ' ') p++;

    while (*p && *p != ':') {
        if (is_string_expr(p)) {
            parse_string_expr(&p, strbuf, MAX_STR_LEN);
            basic_print(strbuf);
        }
        else if (*p == ';') {
            p++;
        }
        else if (*p == ',') {
            basic_putchar('\t');
            p++;
        }
        else {
            print_num(eval_expr(&p));
        }
        while (*p == ' ') p++;
    }
    basic_print("\n");
}

void repl(void)
{
    char line[MAX_LINE];
    char *p;
    unsigned int ln;

    while (1) {

        basic_print("READY\n");

        if (!read_line(line, MAX_LINE))
            break;

        p = line;

        while (*p == ' ') p++;

        if (*p == 0) continue;

        if (kw_match(&p, "RUN") && (*p == 0 || *p == ' ')) {
            run_program();
            continue;
        }
        p = line;
        while (*p == ' ') p++;

        if (kw_match(&p, "LIST")) {
            unsigned int from = 0, to = 0;
            while (*p == ' ') p++;
            if (is_digit(*p)) {
                from = parse_linenum(&p);
                while (*p == ' ') p++;
                if (*p == '-') {
                    p++;
                    while (*p == ' ') p++;
                    if (is_digit(*p))
                        to = parse_linenum(&p);
                } else {
                    to = from;
                }
            }
            program_list(from, to);
            continue;
        }
        p = line;
        while (*p == ' ') p++;

        if (kw_match(&p, "NEW") && (*p == 0 || *p == ' ')) {
            program_init();
            vars_init();
            basic_print("OK\n");
            continue;
        }
        p = line;
        while (*p == ' ') p++;

        if (kw_match(&p, "SAVE") && (*p == 0 || *p == ' ' || *p == '"')) {
            char fname[256];
            parse_filename(&p, fname, (int)sizeof(fname));
            if (fname[0] == '\0') {
                basic_print("?MISSING FILENAME\n");
            } else if (!program_save(fname)) {
                basic_print("?CANNOT SAVE FILE\n");
            } else {
                basic_print("OK\n");
            }
            continue;
        }
        p = line;
        while (*p == ' ') p++;

        if (kw_match(&p, "LOAD") && (*p == 0 || *p == ' ' || *p == '"')) {
            char fname[256];
            parse_filename(&p, fname, (int)sizeof(fname));
            if (fname[0] == '\0') {
                basic_print("?MISSING FILENAME\n");
            } else if (!program_load(fname)) {
                basic_print("?CANNOT LOAD FILE\n");
            } else {
                basic_print("OK\n");
            }
            continue;
        }
        p = line;
        while (*p == ' ') p++;

        if (is_digit(*p)) {
            ln = parse_linenum(&p);
            while (*p == ' ') p++;
            if (*p == 0) {
                program_delete(ln);
            } else {
                program_insert(ln, p);
            }
            continue;
        }

        if (kw_match(&p, "PRINT")) {
            direct_print(p);
            continue;
        }
        p = line;
        while (*p == ' ') p++;

        if (kw_match(&p, "LET ")) {
            char var;
            char *bp;
            while (*p == ' ') p++;
            var = *p++;
            if (var >= 'A' && var <= 'Z') {
                if (*p == '$') {
                    p++;
                    while (*p == ' ') p++;
                    if (*p == '=') p++;
                    while (*p == ' ') p++;
                    parse_string_expr(&p, str_vars[var - 'A'], MAX_STR_LEN);
                } else {
                    while (*p != '=' && *p != 0) p++;
                    if (*p == '=') p++;
                    bp = p;
                    vars[var - 'A'] = eval_expr(&bp);
                }
            }
            continue;
        }
        p = line;
        while (*p == ' ') p++;

        if (kw_match(&p, "DIM ")) {
            char var;
            int s1, s2;
            while (*p == ' ') p++;
            var = *p++;
            if (var >= 'A' && var <= 'Z') {
                while (*p == ' ') p++;
                if (*p == '(') p++;
                s1 = eval_expr(&p);
                s2 = 0;
                while (*p == ' ') p++;
                if (*p == ',') { p++; s2 = eval_expr(&p); }
                while (*p == ' ') p++;
                if (*p == ')') p++;
                if (!array_dim(var - 'A', s1, s2))
                    basic_print("?BAD SUBSCRIPT\n");
            }
            continue;
        }
        p = line;
        while (*p == ' ') p++;

        if (kw_match(&p, "INPUT")) {
            char var;
            char buf[MAX_LINE];
            char *bp;
            while (*p == ' ') p++;
            if (*p == '"') {
                p++;
                while (*p && *p != '"') basic_putchar(*p++);
                if (*p == '"') p++;
                while (*p == ' ') p++;
                if (*p == ';' || *p == ',') p++;
            }
            while (*p == ' ') p++;
            var = *p;
            if (var >= 'A' && var <= 'Z') {
                p++;
                if (*p == '$') {
                    basic_print("? ");
                    read_line(buf, MAX_STR_LEN);
                    str_copy(str_vars[var - 'A'], buf, MAX_STR_LEN);
                } else {
                    basic_print("? ");
                    read_line(buf, MAX_LINE);
                    bp = buf;
                    vars[var - 'A'] = eval_expr(&bp);
                }
            }
            continue;
        }
        p = line;
        while (*p == ' ') p++;

        if (kw_match(&p, "PEEK")) {
            int addr;
            while (*p == ' ') p++;
            if (*p == '(') p++;
            addr = eval_expr(&p);
            while (*p == ' ') p++;
            if (*p == ')') p++;
            if (addr >= 0 && addr < SIM_MEM_SIZE)
                print_num((int)sim_mem[addr]);
            else
                print_num(0);
            basic_print("\n");
            continue;
        }
        p = line;
        while (*p == ' ') p++;

        if (kw_match(&p, "POKE")) {
            int addr, val;
            while (*p == ' ') p++;
            addr = eval_expr(&p);
            while (*p == ' ') p++;
            if (*p == ',') p++;
            val = eval_expr(&p);
            if (addr >= 0 && addr < SIM_MEM_SIZE)
                sim_mem[addr] = (unsigned char)val;
            continue;
        }
        p = line;
        while (*p == ' ') p++;

        if (kw_match(&p, "CLS")) {
            basic_putchar(27);
            basic_print("[2J");
            basic_putchar(27);
            basic_print("[H");
            continue;
        }
        p = line;
        while (*p == ' ') p++;

        if (kw_match(&p, "COLOR") || kw_match(&p, "DRAW") ||
            kw_match(&p, "LINE")  || kw_match(&p, "RECT") ||
            kw_match(&p, "FILL")  || kw_match(&p, "PAUSE") ||
            kw_match(&p, "CURSOR")) {
            /* re-dispatch the full line through the program statement runner */
            p = line;
            while (*p == ' ') p++;
            run_statement(p);
            continue;
        }
        p = line;
        while (*p == ' ') p++;

        if (is_var_assign(p)) {
            char var = *p++;
            if (*p == '$') {
                p++;
                while (*p == ' ') p++;
                if (*p == '=') p++;
                while (*p == ' ') p++;
                parse_string_expr(&p, str_vars[var - 'A'], MAX_STR_LEN);
            }
            else if (*p == '(') {
                int i1, i2;
                p++;
                i1 = eval_expr(&p);
                i2 = 0;
                while (*p == ' ') p++;
                if (*p == ',') { p++; i2 = eval_expr(&p); }
                while (*p == ' ') p++;
                if (*p == ')') p++;
                while (*p == ' ') p++;
                if (*p == '=') p++;
                array_set(var - 'A', i1, i2, eval_expr(&p));
            }
            else {
                while (*p != '=' && *p != 0) p++;
                if (*p == '=') p++;
                vars[var - 'A'] = eval_expr(&p);
            }
            continue;
        }

        basic_print("?SYNTAX ERROR\n");
    }
}
