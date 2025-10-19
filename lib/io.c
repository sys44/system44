#include "../drivers/tty.h"
#include <stdarg.h>

int putc(char c) {
    tty_putc(c);
    return c;
}

int puts(const char *s) {
    tty_puts(s);
    return 0;
}

int printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    const char *p = format;
    while (*p) {
        if (*p == '%') {
            p++;
            if (*p == 's') {
                const char *str = va_arg(args, const char *);
                tty_puts(str);
            } else if (*p == 'c') {
                char c = (char)va_arg(args, int);
                tty_putc(c);
            } else if (*p == 'd') {
                int num = va_arg(args, int);
                char buf[12];
                int i = 0;
                if (num < 0) {
                    tty_putc('-');
                    num = -num;
                }
                if (num == 0) {
                    tty_putc('0');
                } else {
                    while (num > 0) {
                        buf[i++] = (num % 10) + '0';
                        num /= 10;
                    }
                    for (int j = i - 1; j >= 0; j--) {
                        tty_putc(buf[j]);
                    }
                }
            } else if (*p == '%') {
                tty_putc('%');
            } else {
                tty_putc('%');
                tty_putc(*p);
            }
        } else {
            tty_putc(*p);
        }
        p++;
    }
    va_end(args);
    return 0;
}

int clear() {
    tty_clear();
    return 0;
}