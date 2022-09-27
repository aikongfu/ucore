#include <defs.h>
#include <list.h>
#include <proc.h>
#include <assert.h>
#include <sched_mlfq.h>

static void
mlfq_init(struct run_queue *rq) {
    
}

static void
mlfq_enqueue(struct run_queue *rq, struct proc_struct *proc) {
    
}

static void
mlfq_dequeue(struct run_queue *rq, struct proc_struct *proc) {
  
}

static struct proc_struct *
mlfq_pick_next(struct run_queue *rq) {

}

/*
 * mlfq_proc_tick works with the tick event of current process. You
 * should check whether the time slices for current process is
 * exhausted and update the proc struct ``proc''. proc->time_slice
 * denotes the time slices left for current
 * process. proc->need_resched is the flag variable for process
 * switching.
 * 
 * 时钟中断处理
 * 检查当前proc的时间片是否耗尽
 * 更新proc->time_slice为当前proc剩余的时间片
 */
static void
mlfq_proc_tick(struct run_queue *rq, struct proc_struct *proc) {
     if (proc->time_slice > 0) {
          proc->time_slice--;
     }
     if (proc->time_slice == 0) {
          proc->need_resched = 1;
     }
}

struct sched_class sched_mlfq_class = {
     .name = "mlfq_scheduler",
     .init = mlfq_init,
     .enqueue = mlfq_enqueue,
     .dequeue = mlfq_dequeue,
     .pick_next = mlfq_pick_next,
     .proc_tick = mlfq_proc_tick,
};
