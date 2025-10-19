#include "../uex/uex.h"
#include "../mm/pmm.h"

int exec (const char *file) {
    void* entry;
    struct uexAlloc alloc;
    if (uexExec(file, &entry, &alloc) == 0) {
        void (*prog)() = (void (*)())entry;
        prog();
        if (alloc.base) {
            pmm_free_pages(alloc.base, alloc.pages);
        }
    } else {
        return 1;
    }
    return 0;
}

int execv (const char *file, char *const argv[]) {
    void* entry;
    struct uexAlloc alloc;
    if (uexExec(file, &entry, &alloc) == 0) {
        void (*prog)(char *const *) = (void (*)(char *const *))entry;
        prog(argv);
        if (alloc.base) {
            pmm_free_pages(alloc.base, alloc.pages);
        }
    } else {
        return 1;
    }
    return 0;
}