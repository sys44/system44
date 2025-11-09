#pragma once
#include <stddef.h>
#include "kfs.h"

typedef struct file file_t;

file_t *vfs_l_open(const char *path);
int vfs_l_read(file_t *f, void *buf, size_t count);
int vfs_l_write(file_t *f, const void *buf, size_t count);
