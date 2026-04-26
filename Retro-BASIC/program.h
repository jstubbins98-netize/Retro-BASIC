#ifndef PROGRAM_H
#define PROGRAM_H

#include "config.h"

struct line_entry {
    unsigned int linenum;
    unsigned int offset;
};

extern unsigned char program[MAX_PROG];
extern unsigned int prog_size;
extern struct line_entry line_index[MAX_LINES];
extern unsigned int line_count;

void program_init(void);
void program_insert(unsigned int linenum, const char *text);
void program_delete(unsigned int linenum);
void program_list(unsigned int from, unsigned int to);
int program_find_line(unsigned int linenum);
void program_rebuild_index(void);
int program_save(const char *filename);
int program_load(const char *filename);

#endif
