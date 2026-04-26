#include "config.h"
#include "commands.h"
#include "io.h"
#include "vars.h"
#include "expr.h"
#include "program.h"
#include <time.h>

static unsigned int gosub_stack[MAX_GOSUB];
static int gosub_sp = 0;

struct for_state {
    char var;
    int limit;
    int step;
    unsigned int line_idx;
};

static struct for_state for_stack[MAX_FOR];
static int for_sp = 0;

static unsigned int while_stack[MAX_WHILE];
static int while_sp = 0;

static unsigned int repeat_stack[MAX_REPEAT];
static int repeat_sp = 0;

static unsigned int do_stack[MAX_DO];
static int do_sp = 0;

static unsigned char if_skip[MAX_IF_NEST];
static int if_depth = 0;

static int running = 0;
static unsigned int cur_ci;
static unsigned int cur_linenum;
static char cur_line[MAX_LINE];

void error_msg(const char *msg, unsigned int linenum)
{
    basic_print("?");
    basic_print(msg);
    if (linenum > 0) {
        basic_print(" IN ");
        print_num((int)linenum);
    }
    basic_print("\n");
}

static int is_var_assign(char *p)
{
    if (*p < 'A' || *p > 'Z') return 0;
    p++;
    if (*p == '$' || *p == '(') return 1;
    while (*p == ' ') p++;
    return (*p == '=');
}

static int cmd_match(char **p, const char *cmd)
{
    char *start = *p;
    while (*cmd) {
        if (**p != *cmd) {
            *p = start;
            return 0;
        }
        (*p)++;
        cmd++;
    }
    return 1;
}

static int find_line_idx(unsigned int linenum)
{
    unsigned int i;
    for (i = 0; i < line_count; i++) {
        if (line_index[i].linenum == linenum)
            return (int)i;
    }
    return -1;
}

static void exec_print(char *p)
{
    char strbuf[MAX_STR_LEN];
    char *end;

    skip_spaces(&p);

    end = p;
    while (*end) end++;
    if (end > p) end--;
    while (end > p && *end == ' ') end--;

    while (*p && *p != ':') {
        if (*p == '"' || (*p >= 'A' && *p <= 'Z' && *(p+1) == '$')
            || (*p == 'L' && *(p+1) == 'E' && *(p+2) == 'F' && *(p+3) == 'T' && *(p+4) == '$')
            || (*p == 'R' && *(p+1) == 'I' && *(p+2) == 'G' && *(p+3) == 'H' && *(p+4) == 'T' && *(p+5) == '$')
            || (*p == 'M' && *(p+1) == 'I' && *(p+2) == 'D' && *(p+3) == '$')) {
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
        skip_spaces(&p);
    }

    if (*p != ';')
        basic_print("\n");
}

static void exec_input(char *p)
{
    char buf[MAX_LINE];
    char var;
    char *bp;

    skip_spaces(&p);

    if (*p == '"') {
        p++;
        while (*p && *p != '"')
            basic_putchar(*p++);
        if (*p == '"') p++;
        skip_spaces(&p);
        if (*p == ';' || *p == ',') p++;
    }

    skip_spaces(&p);
    var = *p;

    if (var < 'A' || var > 'Z') {
        error_msg("SYNTAX ERROR", cur_linenum);
        return;
    }

    p++;

    if (*p == '$') {
        basic_print("? ");
        read_line(buf, MAX_STR_LEN);
        str_copy(str_vars[var - 'A'], buf, MAX_STR_LEN);
    }
    else {
        basic_print("? ");
        read_line(buf, MAX_LINE);
        bp = buf;
        vars[var - 'A'] = eval_expr(&bp);
    }
}

static void exec_let(char *p)
{
    char var;
    int i1, i2;

    skip_spaces(&p);
    var = *p++;

    if (var < 'A' || var > 'Z') {
        error_msg("SYNTAX ERROR", cur_linenum);
        return;
    }

    if (*p == '$') {
        char strbuf[MAX_STR_LEN];
        p++;
        skip_spaces(&p);
        if (*p == '=') p++;
        skip_spaces(&p);
        parse_string_expr(&p, strbuf, MAX_STR_LEN);
        str_copy(str_vars[var - 'A'], strbuf, MAX_STR_LEN);
        return;
    }

    if (*p == '(') {
        p++;
        i1 = eval_expr(&p);
        i2 = 0;
        skip_spaces(&p);
        if (*p == ',') {
            p++;
            i2 = eval_expr(&p);
        }
        skip_spaces(&p);
        if (*p == ')') p++;
        skip_spaces(&p);
        if (*p == '=') p++;
        array_set(var - 'A', i1, i2, eval_expr(&p));
        return;
    }

    while (*p != '=' && *p != 0) p++;
    if (*p == '=') p++;

    vars[var - 'A'] = eval_expr(&p);
}

static void exec_dim(char *p)
{
    char var;
    int s1, s2;

    skip_spaces(&p);
    var = *p++;

    if (var < 'A' || var > 'Z') {
        error_msg("SYNTAX ERROR", cur_linenum);
        return;
    }

    skip_spaces(&p);
    if (*p == '(') p++;

    s1 = eval_expr(&p);
    s2 = 0;

    skip_spaces(&p);
    if (*p == ',') {
        p++;
        s2 = eval_expr(&p);
    }

    skip_spaces(&p);
    if (*p == ')') p++;

    if (!array_dim(var - 'A', s1, s2))
        error_msg("BAD SUBSCRIPT", cur_linenum);
}

static void exec_poke(char *p)
{
    int addr, val;

    skip_spaces(&p);
    addr = eval_expr(&p);
    skip_spaces(&p);
    if (*p == ',') p++;
    val = eval_expr(&p);

    if (addr >= 0 && addr < SIM_MEM_SIZE)
        sim_mem[addr] = (unsigned char)val;
}

static void exec_goto(char *p)
{
    int idx;
    int val;

    skip_spaces(&p);
    val = eval_expr(&p);
    idx = find_line_idx((unsigned int)val);
    if (idx < 0) {
        error_msg("UNDEF'D LINE", cur_linenum);
        running = 0;
    } else {
        cur_ci = (unsigned int)idx;
    }
}

static void exec_gosub(char *p)
{
    int idx;
    int val;

    skip_spaces(&p);
    val = eval_expr(&p);
    idx = find_line_idx((unsigned int)val);
    if (idx < 0) {
        error_msg("UNDEF'D LINE", cur_linenum);
        running = 0;
    } else {
        if (gosub_sp >= MAX_GOSUB) {
            error_msg("GOSUB OVERFLOW", cur_linenum);
            running = 0;
        } else {
            gosub_stack[gosub_sp++] = cur_ci + 1;
            cur_ci = (unsigned int)idx;
        }
    }
}

static void exec_for(char *p)
{
    char var;
    int limit, step;

    skip_spaces(&p);
    var = *p++;
    skip_spaces(&p);
    if (*p == '=') p++;
    vars[var - 'A'] = eval_expr(&p);
    skip_spaces(&p);
    cmd_match(&p, "TO");
    skip_spaces(&p);
    limit = eval_expr(&p);
    step = 1;
    skip_spaces(&p);
    if (cmd_match(&p, "STEP")) {
        skip_spaces(&p);
        step = eval_expr(&p);
    }

    if (for_sp >= MAX_FOR) {
        error_msg("FOR OVERFLOW", cur_linenum);
        running = 0;
    } else {
        for_stack[for_sp].var = var;
        for_stack[for_sp].limit = limit;
        for_stack[for_sp].step = step;
        for_stack[for_sp].line_idx = cur_ci + 1;
        for_sp++;
    }
}

static void exec_next(char *p)
{
    char var;
    int fi, i;

    skip_spaces(&p);
    var = *p;

    if (for_sp <= 0) {
        error_msg("NEXT WITHOUT FOR", cur_linenum);
        running = 0;
        return;
    }

    fi = for_sp - 1;

    if (var >= 'A' && var <= 'Z') {
        fi = -1;
        for (i = for_sp - 1; i >= 0; i--) {
            if (for_stack[i].var == var) {
                fi = i;
                break;
            }
        }
        if (fi < 0) {
            error_msg("NEXT WITHOUT FOR", cur_linenum);
            running = 0;
            return;
        }
    }

    vars[for_stack[fi].var - 'A'] += for_stack[fi].step;

    if ((for_stack[fi].step > 0 &&
         vars[for_stack[fi].var - 'A'] <= for_stack[fi].limit) ||
        (for_stack[fi].step < 0 &&
         vars[for_stack[fi].var - 'A'] >= for_stack[fi].limit)) {
        cur_ci = for_stack[fi].line_idx;
    } else {
        for_sp = fi;
        cur_ci++;
    }
}

static void skip_while_block(void)
{
    int depth;
    unsigned int wpc;
    int wi;
    char *wp;

    depth = 1;
    cur_ci++;
    while (cur_ci < line_count && depth > 0) {
        wpc = line_index[cur_ci].offset + 2;
        wi = 0;
        while (wpc < prog_size && program[wpc] != 0 && wi < MAX_LINE - 1)
            cur_line[wi++] = program[wpc++];
        cur_line[wi] = 0;
        wp = cur_line;
        skip_spaces(&wp);
        if (cmd_match(&wp, "WHILE")) depth++;
        else if (cmd_match(&wp, "WEND")) depth--;
        if (depth > 0) cur_ci++;
    }
    cur_ci++;
}

static void exec_while(char *p)
{
    int val;

    skip_spaces(&p);
    val = eval_expr(&p);
    if (val) {
        if (while_sp >= MAX_WHILE) {
            error_msg("WHILE OVERFLOW", cur_linenum);
            running = 0;
        } else {
            while_stack[while_sp++] = cur_ci;
        }
        cur_ci++;
    } else {
        skip_while_block();
    }
}

static void exec_do_loop(char *p)
{
    int val;

    if (do_sp <= 0) {
        error_msg("LOOP WITHOUT DO", cur_linenum);
        running = 0;
        return;
    }

    skip_spaces(&p);
    if (cmd_match(&p, "WHILE")) {
        skip_spaces(&p);
        val = eval_expr(&p);
        if (val) {
            cur_ci = do_stack[do_sp - 1];
        } else {
            do_sp--;
            cur_ci++;
        }
    }
    else if (cmd_match(&p, "UNTIL")) {
        skip_spaces(&p);
        val = eval_expr(&p);
        if (val) {
            do_sp--;
            cur_ci++;
        } else {
            cur_ci = do_stack[do_sp - 1];
        }
    }
    else {
        cur_ci = do_stack[do_sp - 1];
    }
}

static void exec_color(char *p);
static void exec_draw(char *p);
static void exec_gline(char *p);
static void exec_rect(char *p);
static void exec_fill(char *p);
static void exec_pause(char *p);
static void exec_cursor(char *p);

static void exec_if(char *p)
{
    int val;
    unsigned int saved_ci;

    skip_spaces(&p);
    val = eval_expr(&p);
    skip_spaces(&p);

    if (cmd_match(&p, "THEN")) {
        skip_spaces(&p);
        if (*p >= '0' && *p <= '9') {
            if (val) {
                int target = eval_expr(&p);
                int idx = find_line_idx((unsigned int)target);
                if (idx < 0) {
                    error_msg("UNDEF'D LINE", cur_linenum);
                    running = 0;
                } else {
                    cur_ci = (unsigned int)idx;
                }
                return;
            }
        }
        else if (*p != 0) {
            if (val) {
                saved_ci = cur_ci;
                run_statement(p);
                if (cur_ci != saved_ci)
                    return;
            }
        }
        else {
            if_depth++;
            if (if_depth <= MAX_IF_NEST)
                if_skip[if_depth - 1] = val ? 0 : 1;
        }
    }

    cur_ci++;
}

void run_statement(char *p)
{
    skip_spaces(&p);

    if (cmd_match(&p, "REM")) {
        return;
    }
    if (cmd_match(&p, "PRINT")) {
        exec_print(p);
    }
    else if (cmd_match(&p, "INPUT")) {
        exec_input(p);
    }
    else if (cmd_match(&p, "LET ")) {
        exec_let(p);
    }
    else if (cmd_match(&p, "GOTO")) {
        exec_goto(p);
    }
    else if (cmd_match(&p, "GOSUB")) {
        exec_gosub(p);
    }
    else if (cmd_match(&p, "POKE")) {
        exec_poke(p);
    }
    else if (cmd_match(&p, "COLOR")) {
        exec_color(p);
    }
    else if (cmd_match(&p, "DRAW")) {
        exec_draw(p);
    }
    else if (cmd_match(&p, "LINE")) {
        exec_gline(p);
    }
    else if (cmd_match(&p, "RECT")) {
        exec_rect(p);
    }
    else if (cmd_match(&p, "FILL")) {
        exec_fill(p);
    }
    else if (cmd_match(&p, "PAUSE")) {
        exec_pause(p);
    }
    else if (cmd_match(&p, "CURSOR")) {
        exec_cursor(p);
    }
    else if (is_var_assign(p)) {
        exec_let(p);
    }
    else if (*p != 0) {
        error_msg("SYNTAX ERROR", cur_linenum);
        running = 0;
    }
}

static int is_str_expr(const char *p)
{
    if (*p == '"') return 1;
    if (*p >= 'A' && *p <= 'Z' && *(p+1) == '$') return 1;
    if (*p == 'L' && *(p+1) == 'E' && *(p+2) == 'F' && *(p+3) == 'T' && *(p+4) == '$') return 1;
    if (*p == 'R' && *(p+1) == 'I' && *(p+2) == 'G' && *(p+3) == 'H' && *(p+4) == 'T' && *(p+5) == '$') return 1;
    if (*p == 'M' && *(p+1) == 'I' && *(p+2) == 'D' && *(p+3) == '$') return 1;
    return 0;
}

static char parse_draw_char(char **p, char def)
{
    char strbuf[MAX_STR_LEN];
    if (is_str_expr(*p)) {
        parse_string_expr(p, strbuf, MAX_STR_LEN);
        return strbuf[0] ? strbuf[0] : def;
    }
    return (char)(eval_expr(p) & 0xFF);
}

static void goto_xy(int x, int y)
{
    basic_putchar(27);
    basic_putchar('[');
    print_num(y);
    basic_putchar(';');
    print_num(x);
    basic_putchar('H');
}

static void put_char_at(int x, int y, char c)
{
    goto_xy(x, y);
    basic_putchar(c);
}

static void exec_color(char *p)
{
    int fg, bg;
    skip_spaces(&p);
    fg = eval_expr(&p);
    skip_spaces(&p);
    bg = -1;
    if (*p == ',') { p++; bg = eval_expr(&p); }

    if (fg < 0) {
        basic_putchar(27); basic_print("[0m");
        return;
    }
    basic_putchar(27); basic_putchar('[');
    print_num(fg > 7 ? 90 + (fg - 8) : 30 + fg);
    basic_putchar('m');
    if (bg >= 0) {
        basic_putchar(27); basic_putchar('[');
        print_num(bg > 7 ? 100 + (bg - 8) : 40 + bg);
        basic_putchar('m');
    }
}

static void exec_draw(char *p)
{
    int x, y;
    char strbuf[MAX_STR_LEN];

    skip_spaces(&p);
    x = eval_expr(&p);
    skip_spaces(&p);
    if (*p == ',') p++;
    y = eval_expr(&p);
    skip_spaces(&p);
    if (*p == ',') p++;
    skip_spaces(&p);

    goto_xy(x, y);
    if (is_str_expr(p)) {
        parse_string_expr(&p, strbuf, MAX_STR_LEN);
        basic_print(strbuf);
    } else {
        basic_putchar((char)(eval_expr(&p) & 0xFF));
    }
}

static void exec_gline(char *p)
{
    int x1, y1, x2, y2, dx, dy, sx, sy, err, e2;
    char ch;

    skip_spaces(&p);
    x1 = eval_expr(&p); skip_spaces(&p);
    if (*p == ',') p++;
    y1 = eval_expr(&p); skip_spaces(&p);
    if (*p == ',') p++;
    x2 = eval_expr(&p); skip_spaces(&p);
    if (*p == ',') p++;
    y2 = eval_expr(&p); skip_spaces(&p);
    ch = '*';
    if (*p == ',') { p++; skip_spaces(&p); ch = parse_draw_char(&p, '*'); }

    dx = x2 - x1; if (dx < 0) dx = -dx;
    dy = y2 - y1; if (dy < 0) dy = -dy;
    sx = (x1 < x2) ? 1 : -1;
    sy = (y1 < y2) ? 1 : -1;
    err = dx - dy;
    for (;;) {
        put_char_at(x1, y1, ch);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx)  { err += dx; y1 += sy; }
    }
}

static void exec_rect(char *p)
{
    int x1, y1, x2, y2, x, y, tmp;
    char ch;

    skip_spaces(&p);
    x1 = eval_expr(&p); skip_spaces(&p);
    if (*p == ',') p++;
    y1 = eval_expr(&p); skip_spaces(&p);
    if (*p == ',') p++;
    x2 = eval_expr(&p); skip_spaces(&p);
    if (*p == ',') p++;
    y2 = eval_expr(&p); skip_spaces(&p);
    ch = '*';
    if (*p == ',') { p++; skip_spaces(&p); ch = parse_draw_char(&p, '*'); }

    if (x1 > x2) { tmp = x1; x1 = x2; x2 = tmp; }
    if (y1 > y2) { tmp = y1; y1 = y2; y2 = tmp; }

    for (x = x1; x <= x2; x++) {
        put_char_at(x, y1, ch);
        put_char_at(x, y2, ch);
    }
    for (y = y1 + 1; y < y2; y++) {
        put_char_at(x1, y, ch);
        put_char_at(x2, y, ch);
    }
}

static void exec_fill(char *p)
{
    int x1, y1, x2, y2, x, y, tmp;
    char ch;

    skip_spaces(&p);
    x1 = eval_expr(&p); skip_spaces(&p);
    if (*p == ',') p++;
    y1 = eval_expr(&p); skip_spaces(&p);
    if (*p == ',') p++;
    x2 = eval_expr(&p); skip_spaces(&p);
    if (*p == ',') p++;
    y2 = eval_expr(&p); skip_spaces(&p);
    ch = ' ';
    if (*p == ',') { p++; skip_spaces(&p); ch = parse_draw_char(&p, ' '); }

    if (x1 > x2) { tmp = x1; x1 = x2; x2 = tmp; }
    if (y1 > y2) { tmp = y1; y1 = y2; y2 = tmp; }

    for (y = y1; y <= y2; y++)
        for (x = x1; x <= x2; x++)
            put_char_at(x, y, ch);
}

static void exec_pause(char *p)
{
    int ms;
    clock_t start, until;
    skip_spaces(&p);
    ms = eval_expr(&p);
    if (ms > 0) {
        start = clock();
        until = start + (clock_t)ms * CLOCKS_PER_SEC / 1000;
        while (clock() < until);
    }
}

static void exec_cursor(char *p)
{
    int show;
    skip_spaces(&p);
    show = eval_expr(&p);
    basic_putchar(27);
    if (show) basic_print("[?25h");
    else      basic_print("[?25l");
}

static void exec_locate(char *p)
{
    int row, col;

    skip_spaces(&p);
    row = eval_expr(&p);
    skip_spaces(&p);
    if (*p == ',') p++;
    col = eval_expr(&p);
    basic_putchar(27);
    basic_putchar('[');
    print_num(row);
    basic_putchar(';');
    print_num(col);
    basic_putchar('H');
}

static void load_line(void)
{
    unsigned int pc;
    int i;

    cur_linenum = line_index[cur_ci].linenum;
    pc = line_index[cur_ci].offset + 2;

    i = 0;
    while (pc < prog_size && program[pc] != 0 && i < MAX_LINE - 1)
        cur_line[i++] = program[pc++];
    cur_line[i] = 0;
}

void run_program(void)
{
    char *p;
    int val;

    gosub_sp = 0;
    for_sp = 0;
    while_sp = 0;
    repeat_sp = 0;
    do_sp = 0;
    if_depth = 0;
    running = 1;

    program_rebuild_index();

    cur_ci = 0;

    while (cur_ci < line_count && running) {

        load_line();
        p = cur_line;
        skip_spaces(&p);

        if (if_depth > 0 && if_skip[if_depth - 1]) {
            if (cmd_match(&p, "IF")) {
                if_depth++;
                if (if_depth <= MAX_IF_NEST)
                    if_skip[if_depth - 1] = 1;
            }
            else if (cmd_match(&p, "ELSE")) {
                if (if_depth == 1 || !if_skip[if_depth - 2])
                    if_skip[if_depth - 1] = 0;
            }
            else if (cmd_match(&p, "ENDIF")) {
                if_depth--;
            }
            cur_ci++;
            continue;
        }

        if (cmd_match(&p, "REM")) {
            cur_ci++;
            continue;
        }

        if (cmd_match(&p, "PRINT")) {
            exec_print(p);
            cur_ci++;
        }
        else if (cmd_match(&p, "INPUT")) {
            exec_input(p);
            cur_ci++;
        }
        else if (cmd_match(&p, "LET ")) {
            exec_let(p);
            cur_ci++;
        }
        else if (cmd_match(&p, "DIM ")) {
            exec_dim(p);
            cur_ci++;
        }
        else if (cmd_match(&p, "GOTO")) {
            exec_goto(p);
        }
        else if (cmd_match(&p, "GOSUB")) {
            exec_gosub(p);
        }
        else if (cmd_match(&p, "RETURN")) {
            if (gosub_sp <= 0) {
                error_msg("RETURN WITHOUT GOSUB", cur_linenum);
                running = 0;
            } else {
                cur_ci = gosub_stack[--gosub_sp];
            }
        }
        else if (cmd_match(&p, "FOR")) {
            exec_for(p);
            cur_ci++;
        }
        else if (cmd_match(&p, "NEXT")) {
            exec_next(p);
        }
        else if (cmd_match(&p, "WHILE")) {
            exec_while(p);
        }
        else if (cmd_match(&p, "WEND")) {
            if (while_sp <= 0) {
                error_msg("WEND WITHOUT WHILE", cur_linenum);
                running = 0;
            } else {
                cur_ci = while_stack[--while_sp];
            }
        }
        else if (cmd_match(&p, "REPEAT")) {
            if (repeat_sp >= MAX_REPEAT) {
                error_msg("REPEAT OVERFLOW", cur_linenum);
                running = 0;
            } else {
                repeat_stack[repeat_sp++] = cur_ci + 1;
            }
            cur_ci++;
        }
        else if (cmd_match(&p, "UNTIL")) {
            if (repeat_sp <= 0) {
                error_msg("UNTIL WITHOUT REPEAT", cur_linenum);
                running = 0;
            } else {
                skip_spaces(&p);
                val = eval_expr(&p);
                if (val) {
                    repeat_sp--;
                    cur_ci++;
                } else {
                    cur_ci = repeat_stack[repeat_sp - 1];
                }
            }
        }
        else if (cmd_match(&p, "DO")) {
            if (do_sp >= MAX_DO) {
                error_msg("DO OVERFLOW", cur_linenum);
                running = 0;
            } else {
                do_stack[do_sp++] = cur_ci + 1;
            }
            cur_ci++;
        }
        else if (cmd_match(&p, "LOOP")) {
            exec_do_loop(p);
        }
        else if (cmd_match(&p, "IF")) {
            exec_if(p);
        }
        else if (cmd_match(&p, "ELSE")) {
            if (if_depth > 0)
                if_skip[if_depth - 1] = 1;
            cur_ci++;
        }
        else if (cmd_match(&p, "ENDIF")) {
            if (if_depth > 0)
                if_depth--;
            cur_ci++;
        }
        else if (cmd_match(&p, "POKE")) {
            exec_poke(p);
            cur_ci++;
        }
        else if (cmd_match(&p, "COLOR")) {
            exec_color(p);
            cur_ci++;
        }
        else if (cmd_match(&p, "DRAW")) {
            exec_draw(p);
            cur_ci++;
        }
        else if (cmd_match(&p, "LINE")) {
            exec_gline(p);
            cur_ci++;
        }
        else if (cmd_match(&p, "RECT")) {
            exec_rect(p);
            cur_ci++;
        }
        else if (cmd_match(&p, "FILL")) {
            exec_fill(p);
            cur_ci++;
        }
        else if (cmd_match(&p, "PAUSE")) {
            exec_pause(p);
            cur_ci++;
        }
        else if (cmd_match(&p, "CURSOR")) {
            exec_cursor(p);
            cur_ci++;
        }
        else if (cmd_match(&p, "CLS")) {
            basic_putchar(27);
            basic_print("[2J");
            basic_putchar(27);
            basic_print("[H");
            cur_ci++;
        }
        else if (cmd_match(&p, "LOCATE")) {
            exec_locate(p);
            cur_ci++;
        }
        else if (cmd_match(&p, "END")) {
            running = 0;
        }
        else if (cmd_match(&p, "STOP")) {
            basic_print("BREAK IN ");
            print_num((int)cur_linenum);
            basic_print("\n");
            running = 0;
        }
        else if (is_var_assign(p)) {
            exec_let(p);
            cur_ci++;
        }
        else if (*p != 0) {
            error_msg("SYNTAX ERROR", cur_linenum);
            running = 0;
        }
        else {
            cur_ci++;
        }
    }
}
