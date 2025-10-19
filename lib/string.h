#ifndef STRING_H
#define STRING_H

#include <stddef.h>

size_t strlen(const char *s);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
char *strtok(char *str, const char *delim);
char *strchr(const char *s, int c);

#endif
