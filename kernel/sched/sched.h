#pragma once
enum proc_state {
    UNUSED,
    RUNNABLE,
    RUNNING
};

int task_l_spawn(void (*fn)(void));
void m_scheduler(void);
void yield(void);
void sched_init(void);
