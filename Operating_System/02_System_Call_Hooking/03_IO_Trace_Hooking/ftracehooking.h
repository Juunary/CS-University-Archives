#ifndef _FTRACEHOOKING_H
#define _FTRACEHOOKING_H
#include <linux/types.h>

#define TRACE_FILE_MAX 256
#define TASK_COMM_MAX  16

extern pid_t tracing_pid;
extern char  traced_comm[TASK_COMM_MAX];
extern char  traced_file[TRACE_FILE_MAX];

extern unsigned long cnt_open, cnt_close, cnt_read, cnt_write, cnt_lseek;
extern unsigned long bytes_read, bytes_written;

#endif
