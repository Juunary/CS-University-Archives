#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched/signal.h> 
#include <linux/uaccess.h>
#include <linux/cred.h>
#include <linux/uidgid.h>
#include <linux/sched/cputime.h> 
#include <linux/time64.h>

#define KWU_DIR   "proc_2021202003"
#define KWU_FILE  "processInfo"

static struct proc_dir_entry *kwu_dir;
static struct proc_dir_entry *kwu_file;

// 필터링할 PID 저장 변수
static int kwu_target_pid = -1;

// 프로세스 상태 문자를 반환
static char kwu_state_char(const struct task_struct *t)
{
    if (t->exit_state) {
        if (t->exit_state & EXIT_ZOMBIE) return 'Z';
        if (t->exit_state & EXIT_DEAD)   return 'X';
    }
    if (t->state == TASK_RUNNING)                 return 'R';
    if (t->state & TASK_INTERRUPTIBLE)            return 'S';
    if (t->state & TASK_UNINTERRUPTIBLE)          return 'D';
    if (t->state & __TASK_STOPPED)                return 'T';
    if (t->state & __TASK_TRACED)                 return 't';
#ifdef TASK_PARKED
    if (t->state & TASK_PARKED)                   return 'P';
#endif
#ifdef TASK_IDLE
    if (t->state & TASK_IDLE)                     return 'I';
#endif
    return '?';
}

// 사용자 시간과 시스템 시간을 틱 단위로 반환
static void kwu_get_times_ticks(struct task_struct *p,
                                unsigned long *ut, unsigned long *st)
{
    u64 ut_ns = 0, st_ns = 0;
    task_cputime(p, &ut_ns, &st_ns);
    *ut = (unsigned long)(ut_ns * HZ / NSEC_PER_SEC);
    *st = (unsigned long)(st_ns * HZ / NSEC_PER_SEC);
}
// 단일 프로세스 정보 출력
static void kwu_print_one(struct seq_file *m, struct task_struct *t)
{
    kuid_t uidk = task_uid(t);
    kgid_t gidk = t->cred->gid; 
    uid_t  uid  = from_kuid(&init_user_ns, uidk);
    gid_t  gid  = from_kgid(&init_user_ns, gidk);
    unsigned long ut = 0, st = 0;

    kwu_get_times_ticks(t, &ut, &st);

    seq_printf(m, "%-6d %-6d %-6u %-6u %-10lu %-10lu %-6c %s\n",
               t->pid,
               t->real_parent ? t->real_parent->pid : 0,
               uid, gid, ut, st,
               kwu_state_char(t),
               t->comm);
}

// procfs 출력 함수
static int kwu_show(struct seq_file *m, void *v)
{
    struct task_struct *p;

    seq_printf(m,
        "Pid    PPid   Uid    Gid    utime      stime      State  Name\n"
        "-----------------------------------------------------------------\n");

    if (kwu_target_pid >= 0) {
        p = pid_task(find_vpid(kwu_target_pid), PIDTYPE_PID);
        if (p)
            kwu_print_one(m, p);
        return 0;
    }

    rcu_read_lock();
    for_each_process(p) {
        kwu_print_one(m, p);
    }
    rcu_read_unlock();
    return 0;
}

static int kwu_open(struct inode *inode, struct file *file)
{
    return single_open(file, kwu_show, NULL);
}

// procfs 쓰기 함수: PID 필터 설정
static ssize_t kwu_write(struct file *file, const char __user *buf,
                         size_t len, loff_t *ppos)
{
    char tmp[32];
    long val;

    if (len >= sizeof(tmp))
        len = sizeof(tmp) - 1;

    if (copy_from_user(tmp, buf, len))
        return -EFAULT;

    tmp[len] = '\0';

    if (kstrtol(strim(tmp), 10, &val))
        val = -1;

    if (val < 0)
        kwu_target_pid = -1;
    else
        kwu_target_pid = (int)val;

    return len;
}
// procfs 파일 연산 구조체
static const struct file_operations kwu_fops = {
    .owner   = THIS_MODULE,
    .open    = kwu_open,
    .read    = seq_read,
    .write   = kwu_write,
    .llseek  = seq_lseek,
    .release = single_release,
};
// 모듈 초기화 및 종료 함수
static int __init kwu_init(void)
{
    kwu_dir = proc_mkdir(KWU_DIR, NULL);
    if (!kwu_dir)
        return -ENOMEM;

    kwu_file = proc_create(KWU_FILE, 0666, kwu_dir, &kwu_fops);
    if (!kwu_file) {
        remove_proc_entry(KWU_DIR, NULL);
        return -ENOMEM;
    }

    pr_info("proc_info(2021202003): /proc/%s/%s ready\n", KWU_DIR, KWU_FILE);
    return 0;
}

static void __exit kwu_exit(void)
{
    if (kwu_file)
        remove_proc_entry(KWU_FILE, kwu_dir);
    if (kwu_dir)
        remove_proc_entry(KWU_DIR, NULL);
    pr_info("proc_info(2021202003): unloaded\n");
}

module_init(kwu_init);
module_exit(kwu_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("2021202003");
MODULE_DESCRIPTION("OSLab A5-1: procfs process info (PID filter via write)");
