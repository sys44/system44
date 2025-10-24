#include "../drivers/tty.h"
#include <stdarg.h>
#include "error.h"
#include "string.h"

int putc(char c) {
    tty_putc(c);
    return ERR_SUCCESS;
}

int puts(const char *s) {
    tty_puts(s);
    return ERR_SUCCESS;
}

int printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[1024];
    char *out = buffer;
    const char *p = format;

    while (*p) {
        if (*p == '%') {
            p++;
            if (*p == 's') {
                const char *str = va_arg(args, const char *);
                while (*str) {
                    if (*str == '\\' && *(str + 1)) {
                        str++;
                        switch (*str) {
                            case 'n':  *out++ = '\n'; break;
                            case 't':  *out++ = '\t'; break;
                            case '\\': *out++ = '\\'; break;
                            default:
                                *out++ = '\\';
                                *out++ = *str;
                                break;
                        }
                    } else {
                        *out++ = *str;
                    }
                    str++;
                }
            }
            else if (*p == 'c') {
                *out++ = (char)va_arg(args, int);
            }
            else if (*p == 'd') {
                int num = va_arg(args, int);
                char tmp[12];
                int len = 0;

                if (num < 0) {
                    *out++ = '-';
                    /* handle INT_MIN safely */
                    unsigned int u = (unsigned int)(-(num + 1)) + 1;
                    num = (int)u;
                }

                if (num == 0) {
                    tmp[len++] = '0';
                } else {
                    unsigned int u = (unsigned int)num;
                    while (u) {
                        tmp[len++] = (char)('0' + (u % 10));
                        u /= 10;
                    }
                }
                /* reverse into buffer */
                for (int i = len - 1; i >= 0; i--) {
                    *out++ = tmp[i];
                }
            }
            else if (*p == 'f') {
                double num = va_arg(args, double);
                if (num < 0) {
                    *out++ = '-';
                    num = -num;
                }
                int int_part = (int)num;
                double frac_part = num - (double)int_part;

                /* integer part */
                char tmp[20];
                int len = 0;
                if (int_part == 0) {
                    tmp[len++] = '0';
                } else {
                    unsigned int u = (unsigned int)int_part;
                    while (u) {
                        tmp[len++] = (char)('0' + (u % 10));
                        u /= 10;
                    }
                }
                for (int i = len - 1; i >= 0; i--) {
                    *out++ = tmp[i];
                }

                /* decimal point */
                *out++ = '.';

                /* fractional part */
                for (int i = 0; i < 6; i++) { // 6 decimal places
                    frac_part *= 10.0;
                    int digit = (int)frac_part;
                    *out++ = (char)('0' + digit);
                    frac_part -= digit;
                }
            }
            else if (*p == '%') {
                *out++ = '%';
            }
            else {
                /* unknown specifier → literal %}
                */
                *out++ = '%';
                *out++ = *p;
            }
        }
        else {
            *out++ = *p;
        }
        p++;
    }

    /* terminate and write once */
    *out = '\0';
    va_end(args);

    puts(buffer);
    return ERR_SUCCESS;
}

int clear() {
    tty_clear();
    return ERR_SUCCESS;
}