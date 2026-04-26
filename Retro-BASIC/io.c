#include "config.h"
#include "io.h"
#include <stdio.h>
#include <string.h>

void basic_putchar(char c)
{
    putchar((unsigned char)c);
}

char basic_getchar(void)
{
    int c = getchar();
    if (c == EOF) return '\0';
    return (char)c;
}

void basic_print(const char *s)
{
    while (*s)
        putchar((unsigned char)*s++);
    fflush(stdout);
}

void print_num(int n)
{
    char buf[12];
    int i = 0;

    if (n == 0) {
        basic_putchar('0');
        return;
    }

    if (n < 0) {
        basic_putchar('-');
        n = -n;
    }

    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }

    while (i > 0)
        basic_putchar(buf[--i]);
}

void print_hex(unsigned int n)
{
    const char *hex = "0123456789ABCDEF";
    int i;

    basic_putchar('$');
    for (i = 12; i >= 0; i -= 4)
        basic_putchar(hex[(n >> i) & 0x0F]);
}

int read_line(char *buf, int max)
{
    char *r;
    int len;

    fflush(stdout);

    r = fgets(buf, max, stdin);
    if (!r) {
        buf[0] = '\0';
        return 0;
    }

    len = (int)strlen(buf);
    while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r'))
        buf[--len] = '\0';

    return 1;
}
