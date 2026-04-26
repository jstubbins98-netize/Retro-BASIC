#ifndef VARS_H
#define VARS_H

#include "config.h"

extern int vars[MAX_VARS];

extern char str_vars[MAX_STRINGS][MAX_STR_LEN];

struct array_def {
    unsigned char used;
    unsigned char dim;
    unsigned char size1;
    unsigned char size2;
    int data[MAX_ARRAY_SZ];
};

extern struct array_def arrays[MAX_ARRAY];

extern unsigned char sim_mem[SIM_MEM_SIZE];

void vars_init(void);
int array_get(int idx, int i1, int i2);
void array_set(int idx, int i1, int i2, int val);
int array_dim(int idx, int s1, int s2);

int str_len(const char *s);
void str_copy(char *dst, const char *src, int max);
void str_left(char *dst, const char *src, int n);
void str_right(char *dst, const char *src, int n);
void str_mid(char *dst, const char *src, int start, int len);
void str_concat(char *dst, const char *a, const char *b, int max);
int str_equal(const char *a, const char *b);

#endif
