// interrupts.c
/* Interrupts (comments added by VeryEpicKebap) */


/* Believe me when i say this but this actually took longer than the entire bootloader. Bootlooping, timer freezing the system etc.
Eventually, i found a way
Just slap a big chunk of ASM in the file and call it a day. (that also took an extra hour)
-VeryEpicKebap
*/



#include "../drivers/tty.h"
#include "../kernel/panic.h"
#include <stdint.h>
#include "../kernel/log.h"

extern void isr_default_stub(void);
extern void isr0_stub(void);
extern void irq0_stub(void);
extern void irq1_stub(void);
extern void syscall_stub(void);

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI   0x20
#define PIT_CMD   0x43
#define PIT_CH0   0x40
#define PIT_BASE  1193182U
static uint8_t pic_master_mask = 0xFF;
static uint8_t pic_slave_mask  = 0xFF;

/* PIT */
void pitsetfreq(uint32_t hz) {
    if  (hz == 0) return;
    uint16_t div = (uint16_t) (PIT_BASE / hz);
    outb(PIT_CMD, 0x36);
    outb(PIT_CH0, div & 0xFF);
    outb(PIT_CH0, (div >> 8) & 0xFF);
}

/* Remap the PIC  */
void pic_remap(uint8_t offset1, uint8_t offset2) {
    uint8_t a1 = inb(PIC1_DATA);
    uint8_t a2 = inb(PIC2_DATA);
    outb(PIC1_CMD, 0x11);
    outb(PIC2_CMD, 0x11);
    outb(PIC1_DATA, offset1);
    outb(PIC2_DATA, offset2);
    outb(PIC1_DATA, 4);
    outb(PIC2_DATA, 2);
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
    pic_master_mask = a1;
    pic_slave_mask  = a2;
}


/* Helpers */
void pic_setm(uint8_t mask1, uint8_t mask2) {
    pic_master_mask = mask1;
    pic_slave_mask  = mask2;
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

void pic_unm(uint8_t irq) {
    if (irq < 8) {
        pic_master_mask &= ~(1 << irq);
        outb(PIC1_DATA, pic_master_mask);
    } else {
        irq -= 8;
        pic_slave_mask &= ~(1 << irq);
        outb(PIC2_DATA, pic_slave_mask);
    }
}

void pic_eoi(uint8_t irq) {
    if (irq >= 8) outb(PIC2_CMD, PIC_EOI);
    outb(PIC1_CMD, PIC_EOI);
}

#define IDT_SIZE 256
typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  type_attr;
    uint16_t offset_high;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

static idt_entry_t idt[IDT_SIZE] __attribute__((aligned(8)));
static idt_ptr_t idt_ptr;

volatile uint32_t ticks = 0;

static void setgate(uint8_t n, uint32_t handler) {
    idt[n].offset_low  = handler & 0xFFFF;
    idt[n].selector    = 0x08;
    idt[n].zero        = 0;
    idt[n].type_attr   = 0x8E;
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

static inline void ldidt(void *p) {
    __asm__ volatile("lidt (%0)" :: "r"(p) : "memory");
}


void isr0h(uint32_t num) {
    (void)num;
    panic("cpu exception 0");
}

void irq0h(void) {
    ticks++;
    pic_eoi(0);
}

void irq1h(void) {
    pic_eoi(1);
}

uint32_t syscallh(uint32_t num, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    switch(num) {
        case 0: 
            tty_puts("\ntest syscall 0\n");
            return 0;
        case 1:
            tty_puts("\nsyscall with arg: ");
            // arg1, arg2, arg3
            return arg1; // return what we want
        default:
            return -1;
    }
}

/* Critical part (I spent like 5 hours on this thing
If you're a developer, please don't touch this part unless if necessary */
asm(
".text\n"
".globl isr_default_stub\n"
"isr_default_stub:\n"
"  pusha\n"
"  popa\n"
"  iret\n"

".globl isr0_stub\n"
"isr0_stub:\n"
"  pusha\n"
"  pushl $0\n"
"  call isr0h\n"
"  addl $4, %esp\n"
"  popa\n"
"  iret\n"

".globl irq0_stub\n"
"irq0_stub:\n"
"  pusha\n"
"  call irq0h\n"
"  popa\n"
"  iret\n"

".globl irq1_stub\n"
"irq1_stub:\n"
"  pusha\n"
"  call irq1h\n"
"  popa\n"
"  iret\n"

".global syscall_stub\n" // function for sigm 
"syscall_stub:\n"
"  pushl %ebp\n"        // save base pointer
"  pushl %edi\n"        // save edi
"  pushl %esi\n"        // save esi
"  pushl %edx\n"        // arg 3
"  pushl %ecx\n"        // arg 2
"  pushl %ebx\n"        // arg 1
"  pushl %eax\n"        // syscall number
"  call syscallh\n"     // call handler
"  addl $28, %esp\n"    // clean up 7 pushes
"  iret\n"
);

/* Finally set up interrupts */
void int_init(void) {
    klog("idt: init\n");
    for (int i = 0; i < IDT_SIZE; ++i) setgate(i, (uint32_t)isr_default_stub);
    setgate(0x00, (uint32_t)isr0_stub);
    pic_remap(0x20, 0x28);
    setgate(0x20 + 0, (uint32_t)irq0_stub);
    setgate(0x20 + 1, (uint32_t)irq1_stub);
    /* The syscall vector is 0x80 */
    setgate(0x80, (uint32_t)syscall_stub);
    idt_ptr.limit = sizeof(idt_entry_t) * IDT_SIZE - 1;
    idt_ptr.base  = (uint32_t)&idt;
    pic_setm(0xFC, 0xFF);
    ldidt(&idt_ptr);
}
