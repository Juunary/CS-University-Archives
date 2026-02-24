#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/sched/mm.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/path.h>
#include <linux/dcache.h>
#include <linux/slab.h>
#include <asm/pgtable.h>
#include <asm/uaccess.h>

static void **syscall_table;
static asmlinkage long (*real_os_ftrace)(struct pt_regs *);
// 페이지 테이블 엔트리를 읽기/쓰기로 변경
static void make_rw(void *addr)
{
    unsigned int level;
    pte_t *pte = lookup_address((unsigned long)addr, &level);
    if (pte && !(pte->pte & _PAGE_RW)){
        pte->pte |= _PAGE_RW;
    }
}
// 메모리 읽기 전용으로 설정
static void make_ro(void *addr)
{
    unsigned int level;
    pte_t *pte = lookup_address((unsigned long)addr, &level);
    if (pte && (pte->pte & _PAGE_RW)){
        pte->pte &= ~_PAGE_RW;
    }
}
// 시스템 콜 함수
static asmlinkage long file_varea(struct pt_regs *regs)
{
    pid_t pid = (pid_t)regs->di;
    struct task_struct *task;
    struct mm_struct *mm;
    struct vm_area_struct *vma;

    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task) {
        pr_info("file_varea: no such pid %d\n", pid);
        return -ESRCH;
    }

    mm = get_task_mm(task);
    if (!mm) {
        pr_info("file_varea: mm not found for pid %d\n", pid);
        return -EINVAL;
    }

    pr_info("##### Loaded files of a process '%s(%d)' in VMA #####\n",task->comm, pid);

    for (vma = mm->mmap; vma; vma = vma->vm_next) {
        if (vma->vm_file) {
            char *page = (char *)__get_free_page(GFP_KERNEL);
            if (page) {
                char *path = d_path(&vma->vm_file->f_path, page, PAGE_SIZE);
                if (!IS_ERR(path)) {
                    pr_info("mem(%lx-%lx) code(%lx-%lx) data(%lx-%lx) heap(%lx-%lx) %s\n",vma->vm_start, vma->vm_end,mm->start_code, mm->end_code,mm->start_data, mm->end_data,mm->start_brk, mm->brk, path);
                } else {
                    pr_info("mem(%lx-%lx) code(%lx-%lx) data(%lx-%lx) heap(%lx-%lx) [path error]\n",vma->vm_start, vma->vm_end,mm->start_code, mm->end_code,mm->start_data, mm->end_data,mm->start_brk, mm->brk);
                }
                free_page((unsigned long)page);
            }
        }
    }

    pr_info("########################################################\n");
    mmput(mm);
    return 0;
}
// 모듈 초기화 함수
static int __init file_varea_init(void)
{
    syscall_table = (void **)kallsyms_lookup_name("sys_call_table");
    if (!syscall_table) {
        pr_err("file_varea: cannot find sys_call_table\n");
        return -ENOENT;
    }

    make_rw(syscall_table);
    real_os_ftrace = (void *)syscall_table[336];
    syscall_table[336] = (void *)file_varea;
    make_ro(syscall_table);

    pr_info("file_varea: module loaded\n");
    return 0;
}
// 모듈 제거 함수
static void __exit file_varea_exit(void)
{
    if (syscall_table) {
        make_rw(syscall_table);
        syscall_table[336] = (void *)real_os_ftrace;
        make_ro(syscall_table);
    }
    pr_info("file_varea: module unloaded\n");
}

module_init(file_varea_init);
module_exit(file_varea_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OSLab Student");
MODULE_DESCRIPTION("Wrap os_ftrace(336): print VMA/file path, code/data/heap for a PID");
