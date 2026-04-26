#include "vars.h"

int vars[MAX_VARS];
char str_vars[MAX_STRINGS][MAX_STR_LEN];
struct array_def arrays[MAX_ARRAY];
unsigned char sim_mem[SIM_MEM_SIZE];

void vars_init(void)
{
    int i, j;

    for (i = 0; i < MAX_VARS; i++)
        vars[i] = 0;

    for (i = 0; i < MAX_STRINGS; i++)
        str_vars[i][0] = 0;

    for (i = 0; i < MAX_ARRAY; i++) {
        arrays[i].used = 0;
        arrays[i].dim = 0;
        arrays[i].size1 = 0;
        arrays[i].size2 = 0;
        for (j = 0; j < MAX_ARRAY_SZ; j++)
            arrays[i].data[j] = 0;
    }
}

int array_get(int idx, int i1, int i2)
{
    int pos;
    if (idx < 0 || idx >= MAX_ARRAY) return 0;
    if (!arrays[idx].used) return 0;

    if (i1 < 0 || i1 >= arrays[idx].size1) return 0;
    if (arrays[idx].dim == 2) {
        if (i2 < 0 || i2 >= arrays[idx].size2) return 0;
        pos = i1 * arrays[idx].size2 + i2;
    } else {
        pos = i1;
    }

    if (pos < 0 || pos >= MAX_ARRAY_SZ) return 0;
    return arrays[idx].data[pos];
}

void array_set(int idx, int i1, int i2, int val)
{
    int pos;
    if (idx < 0 || idx >= MAX_ARRAY) return;
    if (!arrays[idx].used) return;

    if (i1 < 0 || i1 >= arrays[idx].size1) return;
    if (arrays[idx].dim == 2) {
        if (i2 < 0 || i2 >= arrays[idx].size2) return;
        pos = i1 * arrays[idx].size2 + i2;
    } else {
        pos = i1;
    }

    if (pos < 0 || pos >= MAX_ARRAY_SZ) return;
    arrays[idx].data[pos] = val;
}

int array_dim(int idx, int s1, int s2)
{
    if (idx < 0 || idx >= MAX_ARRAY) return 0;
    arrays[idx].used = 1;
    arrays[idx].size1 = (unsigned char)s1;

    if (s2 > 0) {
        arrays[idx].dim = 2;
        arrays[idx].size2 = (unsigned char)s2;
    } else {
        arrays[idx].dim = 1;
        arrays[idx].size2 = 0;
    }

    return 1;
}

int str_len(const char *s)
{
    int n = 0;
    while (s[n]) n++;
    return n;
}

void str_copy(char *dst, const char *src, int max)
{
    int i = 0;
    while (src[i] && i < max - 1) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = 0;
}

void str_left(char *dst, const char *src, int n)
{
    int i = 0;
    while (src[i] && i < n && i < MAX_STR_LEN - 1) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = 0;
}

void str_right(char *dst, const char *src, int n)
{
    int len = str_len(src);
    int start = len - n;
    int i = 0;

    if (start < 0) start = 0;

    while (src[start] && i < MAX_STR_LEN - 1) {
        dst[i++] = src[start++];
    }
    dst[i] = 0;
}

void str_mid(char *dst, const char *src, int start, int len)
{
    int i = 0;

    if (start < 1) start = 1;
    start--;

    while (src[start] && i < len && i < MAX_STR_LEN - 1) {
        dst[i++] = src[start++];
    }
    dst[i] = 0;
}

void str_concat(char *dst, const char *a, const char *b, int max)
{
    int i = 0;

    while (a[i] && i < max - 1) {
        dst[i] = a[i];
        i++;
    }

    while (*b && i < max - 1) {
        dst[i++] = *b++;
    }

    dst[i] = 0;
}

int str_equal(const char *a, const char *b)
{
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    return (*a == *b);
}
