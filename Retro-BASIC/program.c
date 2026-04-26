#include "program.h"
#include "io.h"
#include <stdio.h>

unsigned char program[MAX_PROG];
unsigned int prog_size = 0;
struct line_entry line_index[MAX_LINES];
unsigned int line_count = 0;

void program_init(void)
{
    unsigned int i;
    prog_size = 0;
    line_count = 0;
    for (i = 0; i < MAX_PROG; i++)
        program[i] = 0;
}

static int str_length(const char *s)
{
    int n = 0;
    while (s[n]) n++;
    return n;
}

static void mem_copy(unsigned char *dst, unsigned char *src, unsigned int n)
{
    unsigned int i;
    if (dst < src) {
        for (i = 0; i < n; i++)
            dst[i] = src[i];
    } else {
        for (i = n; i > 0; i--)
            dst[i-1] = src[i-1];
    }
}

void program_rebuild_index(void)
{
    unsigned int pc = 0;
    unsigned int hi, lo;

    line_count = 0;

    while (pc < prog_size && line_count < MAX_LINES) {
        if (pc + 1 >= prog_size) break;

        hi = program[pc];
        lo = program[pc + 1];

        line_index[line_count].linenum = (hi << 8) | lo;
        line_index[line_count].offset = pc;
        line_count++;

        pc += 2;
        while (pc < prog_size && program[pc] != 0)
            pc++;
        pc++;
    }
}

int program_find_line(unsigned int linenum)
{
    unsigned int i;
    for (i = 0; i < line_count; i++) {
        if (line_index[i].linenum == linenum)
            return (int)i;
    }
    return -1;
}

void program_delete(unsigned int linenum)
{
    int idx;
    unsigned int start, end, len;

    idx = program_find_line(linenum);
    if (idx < 0) return;

    start = line_index[idx].offset;
    end = start + 2;
    while (end < prog_size && program[end] != 0)
        end++;
    end++;

    len = prog_size - end;
    if (len > 0)
        mem_copy(&program[start], &program[end], len);

    prog_size -= (end - start);
    program_rebuild_index();
}

static unsigned int find_insert_pos(unsigned int linenum)
{
    unsigned int i;
    for (i = 0; i < line_count; i++) {
        if (line_index[i].linenum >= linenum)
            return line_index[i].offset;
    }
    return prog_size;
}

void program_insert(unsigned int linenum, const char *text)
{
    unsigned int tlen, needed, pos;
    int i;

    program_delete(linenum);

    tlen = (unsigned int)str_length(text);
    if (tlen == 0) return;

    needed = 2 + tlen + 1;
    if (prog_size + needed > MAX_PROG) {
        basic_print("?OUT OF MEMORY\n");
        return;
    }

    pos = find_insert_pos(linenum);

    if (pos < prog_size)
        mem_copy(&program[pos + needed], &program[pos], prog_size - pos);

    program[pos] = (unsigned char)(linenum >> 8);
    program[pos + 1] = (unsigned char)(linenum & 0xFF);

    for (i = 0; text[i]; i++)
        program[pos + 2 + i] = text[i];
    program[pos + 2 + i] = 0;

    prog_size += needed;
    program_rebuild_index();
}

void program_list(unsigned int from, unsigned int to)
{
    unsigned int i, pc;

    for (i = 0; i < line_count; i++) {
        if (line_index[i].linenum < from) continue;
        if (to > 0 && line_index[i].linenum > to) break;

        print_num((int)line_index[i].linenum);
        basic_putchar(' ');

        pc = line_index[i].offset + 2;
        while (pc < prog_size && program[pc] != 0) {
            basic_putchar(program[pc]);
            pc++;
        }
        basic_print("\n");
    }
}

int program_save(const char *filename)
{
    FILE *f;
    unsigned int i, pc;
    int n, tmp, d, digits;
    char nbuf[8];

    f = fopen(filename, "w");
    if (!f) return 0;

    for (i = 0; i < line_count; i++) {
        n = (int)line_index[i].linenum;
        tmp = n;
        digits = 0;
        do { digits++; tmp /= 10; } while (tmp > 0);
        tmp = n;
        for (d = digits - 1; d >= 0; d--) {
            nbuf[d] = (char)('0' + (tmp % 10));
            tmp /= 10;
        }
        nbuf[digits] = '\0';
        fputs(nbuf, f);
        fputc(' ', f);

        pc = line_index[i].offset + 2;
        while (pc < prog_size && program[pc] != 0) {
            fputc((char)program[pc], f);
            pc++;
        }
        fputc('\n', f);
    }
    fclose(f);
    return 1;
}

int program_load(const char *filename)
{
    FILE *f;
    char line[MAX_LINE];
    char *p;
    unsigned int linenum;
    int len;

    f = fopen(filename, "r");
    if (!f) return 0;

    program_init();

    while (fgets(line, MAX_LINE, f)) {
        len = 0;
        while (line[len]) len++;
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            line[--len] = '\0';

        p = line;
        while (*p == ' ') p++;
        if (*p < '0' || *p > '9') continue;

        linenum = 0;
        while (*p >= '0' && *p <= '9')
            linenum = linenum * 10 + (unsigned int)(*p++ - '0');
        while (*p == ' ') p++;

        program_insert(linenum, p);
    }
    fclose(f);
    return 1;
}
