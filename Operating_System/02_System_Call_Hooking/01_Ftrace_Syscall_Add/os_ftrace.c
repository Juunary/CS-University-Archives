#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/types.h>

SYSCALL_DEFINE1(os_ftrace, pid_t, pid)
{
    // 출력 메시지 
    printk(KERN_INFO "ORIGINAL ftrace( ) called! PID is [ %d ]\n", pid);
    return 0;
}
