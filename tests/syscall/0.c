int main(void) {
    asm volatile (
        "push %%eax\n"
        "push %%ebx\n"
        "movl $0, %%eax\n"
        "movl %0, %%ebx\n"
        "int $0x80\n"
        "pop %%ebx\n"
        "pop %%eax\n"
        :
        : "r"("test")
        : "eax", "ebx", "ecx", "edx", "memory"
    );

    return 0;
}
