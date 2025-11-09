#include "vfs.h"
#include "../lib/string.h"
#include "../lib/memory.h"
#include "../drivers/tty.h"

file_t *vfs_l_open(const char *path) {
    if (strcmp(path, "/dev/tty") == 0) {
        return (file_t *)1;
    }
    struct kfs_file *kf = kfs_find(path);
    if (kf) return (file_t *)kf;
    return NULL;
}

int vfs_l_write(file_t *f, const void *buf, size_t count) {
    if (!f) return -1;
    if (f == (file_t *)1){
        char tmp[512];
        size_t len = (count >= sizeof(tmp)) ? sizeof(tmp) - 1 : count;
        memcpy(tmp, buf, len);
        tmp[len] = '\0';
        tty_puts(tmp);
        return (int)len;
    }
    struct kfs_file *kf = (struct kfs_file *)f;
    return kfs_write(kf->name, buf, count);
}

int vfs_l_read(file_t *f, void *buf, size_t count) {
    if (!f) return -1;
    struct kfs_file *kf = (struct kfs_file *)f;
    return kfs_read(kf->name, buf);
}
