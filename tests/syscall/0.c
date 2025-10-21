int main(void) {
    static const char msg[] = "test";

    asm volatile (
        "push %%eax\n"
        "push %%ebx\n"
        "movl $0, %%eax\n"     /* user-specified: set eax = 0 */
        "movl %0, %%ebx\n"     /* move address of msg into ebx */
        "int $0x80\n"
        "pop %%ebx\n"
        "pop %%eax\n"
        :
        : "r"(msg)
        : "eax", "ebx", "ecx", "edx", "memory"
    );

    return 0;
}
