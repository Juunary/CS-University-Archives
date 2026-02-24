#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/ptrace.h>
#include <linux/sched.h>
#include <asm/pgtable.h>
#include "ftracehooking.h"

MODULE_LICENSE("GPL");
#define NR_OS_FTRACE 336

// 전역 변수
pid_t tracing_pid = 0;
char  traced_comm[TASK_COMM_MAX] = "";
char  traced_file[TRACE_FILE_MAX] = "";
unsigned long cnt_open=0, cnt_close=0, cnt_read=0, cnt_write=0, cnt_lseek=0;
unsigned long bytes_read=0, bytes_written=0;

EXPORT_SYMBOL(tracing_pid);
EXPORT_SYMBOL(traced_comm);
EXPORT_SYMBOL(traced_file);
EXPORT_SYMBOL(cnt_open); EXPORT_SYMBOL(cnt_close);
EXPORT_SYMBOL(cnt_read); EXPORT_SYMBOL(cnt_write); EXPORT_SYMBOL(cnt_lseek);
EXPORT_SYMBOL(bytes_read); EXPORT_SYMBOL(bytes_written);

// 후킹 관련
static void **syscall_table;
static asmlinkage long (*orig_os_ftrace)(struct pt_regs *);

// 페이지 보호 비트 수정
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

// 후킹 함수
static asmlinkage long my_ftrace(struct pt_regs *regs)
{
    pid_t pid = (pid_t)regs->di;

    if (pid > 0) {
        tracing_pid = pid;
        get_task_comm(traced_comm, current);
        cnt_open=cnt_close=cnt_read=cnt_write=cnt_lseek=0;
        bytes_read=bytes_written=0;
        traced_file[0]='\0';
        return 0; // 원본 호출하지 않음(요구사항대로 대체 메시지)
    } else {
        
        printk(KERN_INFO "OS Assignment 2 ftrace [%d] Start\n", tracing_pid); // tracing_pid > 0
        printk(KERN_INFO "[2021202003] a.out %s file[%s] stats [x] read - %lu / written - %lu\n",
               traced_comm, traced_file[0]?traced_file:"N/A", bytes_read, bytes_written);
        printk(KERN_INFO "open[%lu] close[%lu] read[%lu] write[%lu] lseek[%lu]\n",
               cnt_open, cnt_close, cnt_read, cnt_write, cnt_lseek);
        printk(KERN_INFO "OS Assignment 2 ftrace [%d] End\n", tracing_pid);
        tracing_pid = 0;
        return 0; // 원본 호출하지 않음
    }
}
// 모듈 초기화 시 후킹
static int __init init_mod(void){
    syscall_table = (void**)kallsyms_lookup_name("sys_call_table");
    if(!syscall_table) return -ENOENT;
    
    orig_os_ftrace = (void*)syscall_table[NR_OS_FTRACE];
    if (make_rw((unsigned long)syscall_table)) return -EFAULT;
    syscall_table[NR_OS_FTRACE] = (void*)my_ftrace;   // 원본 호출하지 않음
    make_ro((unsigned long)syscall_table);
    
    pr_info("ftracehooking: hooked os_ftrace(336)\n");
    return 0;
}
// 모듈 제거 시 원상복구
static void __exit exit_mod(void){
    if(syscall_table){
        if (make_rw((unsigned long)syscall_table)==0) {
            syscall_table[NR_OS_FTRACE] = (void*)orig_os_ftrace;
            make_ro((unsigned long)syscall_table);
        }
    }
    pr_info("ftracehooking: restored os_ftrace\n");
}
module_init(init_mod);
module_exit(exit_mod);
