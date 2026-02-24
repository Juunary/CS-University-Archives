#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>
#include <linux/ptrace.h>  
#include <linux/sched.h>   
#include <linux/types.h>
#include <linux/syscalls.h>
#include <asm/syscall_wrapper.h>
#include <asm/pgtable.h>  

#define __NR_OS_FTRACE 336

static void **syscall_table;

// 기존의 os_ftrace 함수 포인터를 저장할 변수
static asmlinkage long (*orig_x64_os_ftrace)(struct pt_regs *regs);

// 새로운 시스템 콜 함수
__SYSCALL_DEFINEx(1, _my_ftrace, pid_t, pid)
{    
    pr_info("os_ftrace() hooked! os_ftrace -> my_ftrace, PID=[%d]\n", pid);

    return 0;
}

// 페이지 테이블 엔트리를 수정하여 메모리 보호 속성을 변경하는 함수들
static int make_rw(unsigned long addr)
{
    unsigned int level;
    pte_t *pte = lookup_address(addr, &level);
    if (!pte) {
        pr_err("make_rw: lookup_address returned NULL\n");
        return -EINVAL;
    }
    if (!(pte->pte & _PAGE_RW))
        pte->pte |= _PAGE_RW;
    return 0;
}
// 페이지 테이블 엔트리를 수정하여 메모리 보호 속성을 변경하는 함수들
static int make_ro(unsigned long addr)
{
    unsigned int level;
    pte_t *pte = lookup_address(addr, &level);
    if (!pte) {
        pr_err("make_ro: lookup_address returned NULL\n");
        return -EINVAL;
    }
    pte->pte &= ~_PAGE_RW;
    return 0;
}
// 모듈 초기화 및 종료 함수
static int __init hook_init(void)
{
    int ret;

    syscall_table = (void **)kallsyms_lookup_name("sys_call_table");
    if (!syscall_table) {
        pr_err("hook_init: cannot find sys_call_table\n");
        return -ENOENT;
    }

    // 기존 os_ftrace 함수 포인터 저장
    orig_x64_os_ftrace = (void *)syscall_table[__NR_OS_FTRACE];
    if (!orig_x64_os_ftrace)
        pr_warn("hook_init: original os_ftrace entry is NULL?\n");

    ret = make_rw((unsigned long)syscall_table);
    if (ret) {
        pr_err("hook_init: make_rw failed\n");
        return ret;
    }

    // 새로운 시스템 콜 함수 등록
    syscall_table[__NR_OS_FTRACE] = (void *)__x64_sys_my_ftrace;

    // 후킹 후 복원 
    make_ro((unsigned long)syscall_table);

    pr_info("hook_init: hooked syscall %d (os_ftrace)\n", __NR_OS_FTRACE);
    return 0;
}

static void __exit hook_exit(void)
{
    int ret;

    if (!syscall_table)
        return;

    ret = make_rw((unsigned long)syscall_table);
    if (ret) {
        pr_err("hook_exit: make_rw failed\n");
        return;
    }

    if (orig_x64_os_ftrace)
        syscall_table[__NR_OS_FTRACE] = (void *)orig_x64_os_ftrace;

    make_ro((unsigned long)syscall_table);

    pr_info("hook_exit: restored syscall %d (os_ftrace)\n", __NR_OS_FTRACE);
}

module_init(hook_init);
module_exit(hook_exit);
MODULE_LICENSE("GPL");
