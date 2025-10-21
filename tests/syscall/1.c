int main(void) {
    asm volatile (
        "push %eax\n"
        "mov $1, %eax\n"
        "int $0x80\n"
        "pop %eax\n"
    );
    return 0;
}