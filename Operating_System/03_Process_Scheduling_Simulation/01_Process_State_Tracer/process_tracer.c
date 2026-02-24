#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>
#include <linux/pid.h>
#include <linux/ptrace.h>
#include <linux/sched/signal.h>  
#include <linux/syscalls.h>
#include <asm/syscall_wrapper.h>
#include <asm/pgtable.h>

#define __NR_OS_FTRACE 336

static void **syscall_table;
static asmlinkage long (*orig_x64_os_ftrace)(struct pt_regs *regs);

static const char *state_to_str(long st)
{
    if (st == TASK_RUNNING)                return "Running or ready";
    if (st & TASK_UNINTERRUPTIBLE)         return "Wait with ignoring all signals";
    if (st & TASK_INTERRUPTIBLE)           return "Wait";
    if (st & __TASK_STOPPED)               return "Stopped";
    if (st == EXIT_ZOMBIE)                 return "Zombie process";
    if (st == EXIT_DEAD)                   return "Dead";
    return "etc.";
}

static void print_siblings(struct task_struct *t)
{
    struct task_struct *p = t->real_parent, *s;
    int cnt = 0;

    pr_info("[OSLab.] - its sibling process(es):\n");
    if (!p) { pr_info("[OSLab.]   > parent is NULL.\n"); return; }

    list_for_each_entry(s, &p->children, sibling) {
        if (s == t) continue;
        pr_info("[OSLab.]   > [%d] %s\n", s->pid, s->comm);
        cnt++;
    }
    if (!cnt) pr_info("[OSLab.]   > It has no sibling.\n");
    pr_info("[OSLab.]   > This process has %d sibling process(es)\n", cnt);
}

static void print_children(struct task_struct *t)
{
    struct task_struct *c;
    int cnt = 0;

    pr_info("[OSLab.] - its child process(es):\n");
    list_for_each_entry(c, &t->children, sibling) {
        pr_info("[OSLab.]   > [%d] %s\n", c->pid, c->comm);
        cnt++;
    }
    if (!cnt) pr_info("[OSLab.]   > It has no child.\n");
    pr_info("[OSLab.] ##### END OF INFORMATION #####\n");
    pr_info("[OSLab.]   > This process has %d child process(es)\n", cnt);
}

__SYSCALL_DEFINEx(1, _process_tracer, pid_t, trace_task)
{
    struct task_struct *t = NULL;
    struct pid *kpid;
    int pgid;

    if (trace_task <= 0)
        return -1;

    rcu_read_lock();
    kpid = find_vpid(trace_task);
    t = kpid ? pid_task(kpid, PIDTYPE_PID) : NULL;
    if (!t) { rcu_read_unlock(); return -1; }

    pr_info("[OSLab.] ##### TASK INFORMATION of '[%d] %s' #####\n", t->pid, t->comm);
    pr_info("[OSLab.] - task state : %s\n", state_to_str(READ_ONCE(t->state)));

    pgid = task_pgrp_nr_ns(t, &init_pid_ns);
    if (t->group_leader)
        pr_info("[OSLab.] - Process Group Leader : [%d] %s\n", pgid, t->group_leader->comm);
    else
        pr_info("[OSLab.] - Process Group ID : [%d]\n", pgid);

    pr_info("[OSLab.] - # of context-switch(es) : %lu\n", t->nvcsw + t->nivcsw);

    pr_info("[OSLab.] - Number of calling fork() : %llu\n", t->fork_count); 
    
    if (t->real_parent)
        pr_info("[OSLab.] - its parent process : [%d] %s\n",
                t->real_parent->pid, t->real_parent->comm);
    rcu_read_unlock();

    print_siblings(t);
    print_children(t);
    return trace_task;
}

static int make_rw(unsigned long addr)
{
    unsigned int level;
    pte_t *pte = lookup_address(addr, &level);
    if (!pte) return -EINVAL;
    if (!(pte->pte & _PAGE_RW))
        pte->pte |= _PAGE_RW;
    return 0;
}
static int make_ro(unsigned long addr)
{
    unsigned int level;
    pte_t *pte = lookup_address(addr, &level);
    if (!pte) return -EINVAL;
    pte->pte &= ~_PAGE_RW;
    return 0;
}

static int __init pt_init(void)
{
    syscall_table = (void **)kallsyms_lookup_name("sys_call_table");
    if (!syscall_table) return -ENOENT;

    orig_x64_os_ftrace = (void *)syscall_table[__NR_OS_FTRACE];

    if (make_rw((unsigned long)syscall_table)) return -EFAULT;
    syscall_table[__NR_OS_FTRACE] = (void *)__x64_sys_process_tracer;
    make_ro((unsigned long)syscall_table);

    pr_info("process_tracer: hooked syscall %d (os_ftrace)\n", __NR_OS_FTRACE);
    return 0;
}

static void __exit pt_exit(void)
{
    if (!syscall_table) return;

    if (make_rw((unsigned long)syscall_table) == 0) {
        syscall_table[__NR_OS_FTRACE] = (void *)orig_x64_os_ftrace;
        make_ro((unsigned long)syscall_table);
    }
    pr_info("process_tracer: restored syscall %d\n", __NR_OS_FTRACE);
}

module_init(pt_init);
module_exit(pt_exit);
MODULE_LICENSE("GPL");
