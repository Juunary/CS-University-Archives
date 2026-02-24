#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/ptrace.h>
#include <linux/uaccess.h>
#include <asm/pgtable.h>
#include "ftracehooking.h"

MODULE_LICENSE("GPL");

static void **syscall_table;
static asmlinkage long (*orig_openat)(struct pt_regs *);
static asmlinkage long (*orig_read)(struct pt_regs *);
static asmlinkage long (*orig_write)(struct pt_regs *);
static asmlinkage long (*orig_lseek)(struct pt_regs *);
static asmlinkage long (*orig_close)(struct pt_regs *);

static int make_rw(unsigned long addr){
    unsigned int lvl; pte_t *pte = lookup_address(addr, &lvl);
    if(!pte) return -EINVAL;
    if(!(pte->pte & _PAGE_RW)) pte->pte |= _PAGE_RW;
    return 0;
}
static int make_ro(unsigned long addr){
    unsigned int lvl; pte_t *pte = lookup_address(addr, &lvl);
    if(!pte) return -EINVAL;
    pte->pte &= ~_PAGE_RW; return 0;
}
// 후킹된 시스템 콜들
static asmlinkage long ftrace_openat(struct pt_regs *regs){
    long ret = orig_openat(regs);
    if (tracing_pid && current->pid == tracing_pid){
        cnt_open++;
        if (!traced_file[0]) {
            const char __user *ufn = (const char __user*)regs->si;
            long n = strncpy_from_user(traced_file, ufn, TRACE_FILE_MAX-1);
            if (n>0 && n<TRACE_FILE_MAX) traced_file[n]='\0';
        }
    }
    return ret;
}
// 후킹된 read 시스템 콜 함수
static asmlinkage long ftrace_read(struct pt_regs *regs){
    long ret = orig_read(regs);
    if (tracing_pid && current->pid == tracing_pid){
        cnt_read++;
        if (ret>0) bytes_read += ret;
    }
    return ret;
}

// 후킹된 write 시스템 콜 함수
static asmlinkage long ftrace_write(struct pt_regs *regs){
    long ret = orig_write(regs);
    if (tracing_pid && current->pid == tracing_pid){
        cnt_write++;
        if (ret>0) bytes_written += ret;
    }
    return ret;
}
// 후킹된 lseek 시스템 콜 함수
static asmlinkage long ftrace_lseek(struct pt_regs *regs){
    long ret = orig_lseek(regs);
    if (tracing_pid && current->pid == tracing_pid) cnt_lseek++;
    return ret;
}
// 후킹된 close 시스템 콜 함수
static asmlinkage long ftrace_close(struct pt_regs *regs){
    long ret = orig_close(regs);
    if (tracing_pid && current->pid == tracing_pid) cnt_close++;
    return ret;
}
// 모듈 초기화 시 후킹
static int __init init_mod(void){
    syscall_table = (void**)kallsyms_lookup_name("sys_call_table");
    if(!syscall_table) return -ENOENT;

    if (make_rw((unsigned long)syscall_table)) return -EFAULT;
    orig_openat = (void*)syscall_table[__NR_openat];
    orig_read   = (void*)syscall_table[__NR_read];
    orig_write  = (void*)syscall_table[__NR_write];
    orig_lseek  = (void*)syscall_table[__NR_lseek];
    orig_close  = (void*)syscall_table[__NR_close];

    syscall_table[__NR_openat] = (void*)ftrace_openat;
    syscall_table[__NR_read]   = (void*)ftrace_read;
    syscall_table[__NR_write]  = (void*)ftrace_write;
    syscall_table[__NR_lseek]  = (void*)ftrace_lseek;
    syscall_table[__NR_close]  = (void*)ftrace_close;

    make_ro((unsigned long)syscall_table);
    pr_info("iotracehooking: hooked openat/read/write/lseek/close\n");
    return 0;
}
// 모듈 제거 시 원상복구
static void __exit exit_mod(void){
    if(syscall_table){
        if (make_rw((unsigned long)syscall_table)==0){
            syscall_table[__NR_openat] = (void*)orig_openat;
            syscall_table[__NR_read]   = (void*)orig_read;
            syscall_table[__NR_write]  = (void*)orig_write;
            syscall_table[__NR_lseek]  = (void*)orig_lseek;
            syscall_table[__NR_close]  = (void*)orig_close;
            make_ro((unsigned long)syscall_table);
        }
    }
    pr_info("iotracehooking: restored\n");
}
module_init(init_mod);
module_exit(exit_mod);
