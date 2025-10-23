#ifndef EXEC_H
#define EXEC_H

int exec (const char *file);
int execv (const char *file, char *const argv[]);

#endif // EXEC_H