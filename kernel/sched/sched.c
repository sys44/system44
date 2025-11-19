#include <stdint.h>
#include <stddef.h>
#include "../core/log.h"
enum proc_state {
    UNUSED,
    RUNNABLE,
    RUNNING
};

struct context {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebx;
    uint32_t ebp;
    uint32_t eip;
};

struct proc {
    enum proc_state state;
    struct context *context;
    uint32_t stack[1024];
};
#define NPROC 8

static struct {
    struct proc proc[NPROC];
    int current;
    struct context *sched_context;
} ptable;

extern void swtch(struct context**, struct context*);

int task_l_spawn(void (*fn)(void)) {
    struct proc *p;
    uint32_t *sp;
    for(p = &ptable.proc[0]; p < &ptable.proc[NPROC]; p++) {
        if(p->state == UNUSED)
            goto found;
    }
    return -1;

found:
    sp = &p->stack[1023];
    *--sp = (uint32_t)fn;
    *--sp = 0;
    sp -= sizeof(struct context);
    p->context = (struct context*)sp;
    p->context->ebp = 0;
    p->context->ebx = 0;
    p->context->esi = 0;
    p->context->edi = 0;
    p->context->eip = (uint32_t)fn;
    p->state = RUNNABLE;
    return p - ptable.proc;
}

void m_scheduler(void) {
    struct proc *p;
    static struct context scheduler_context;
    ptable.sched_context = &scheduler_context;
    for(;;) {
        for(p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
            if(p->state != RUNNABLE)
                continue;
            ptable.current = p - ptable.proc;
            p->state = RUNNING;
            swtch(&ptable.sched_context, p->context);
            p->state = RUNNABLE;
            ptable.current = -1;
        }
    }
}

void yield(void) {
    if(ptable.current < 0) return;
    struct proc *p = &ptable.proc[ptable.current];
    p->state = RUNNABLE;
    swtch(&p->context, ptable.sched_context);
}

void sched_init(void) {
    klog("low budget scheduler (v1.00)\n");
    for(int i = 0; i < NPROC; i++) {
        ptable.proc[i].state = UNUSED;
    }
    ptable.current = -1;
    ptable.sched_context = NULL;
}
