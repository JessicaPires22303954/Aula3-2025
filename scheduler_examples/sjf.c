#include "sjf.h"

#include <stdio.h>
#include <stdlib.h>

#include "msg.h"
#include <unistd.h>

/**
* /**
 * @brief Shortest Job First (SJF) scheduling algorithm.
 *
 * This function implements the SJF (Shortest Job First) scheduling algorithm.
 * If the CPU is not idle, it checks whether the currently running task has completed
 * and frees the CPU if needed.
 * If the CPU is idle, it selects the next task to run based on the one with the
 * shortest burst time (execution time) among all tasks in the ready queue.
 * The task with the smallest required CPU time is selected to run next.
 *
 * @note This is the non-preemptive version of SJF. Once a task starts running,
 *       it will not be interrupted until it finishes, even if a shorter task
 *       arrives in the queue.
 *
 * @param current_time_ms The current time in milliseconds.
 * @param rq Pointer to the ready queue containing tasks that are ready to run.
 * @param cpu_task Double pointer to the currently running task. This will be updated
 *                 to point to the task with the shortest burst time available.
 */

void sjf_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
    if (*cpu_task) {
        (*cpu_task)->ellapsed_time_ms += TICKS_MS;      // Add to the running time of the application/task
        if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) {
            // Task finished
            // Send msg to application
            msg_t msg = {
                .pid = (*cpu_task)->pid,
                .request = PROCESS_REQUEST_DONE,
                .time_ms = current_time_ms
            };
            if (write((*cpu_task)->sockfd, &msg, sizeof(msg_t)) != sizeof(msg_t)) {
                perror("write");
            }
            // Application finished and can be removed (this is FIFO after all)
            free((*cpu_task));
            (*cpu_task) = NULL;
        }
    }
    if (*cpu_task == NULL) {            // If CPU is idle
        *cpu_task = dequeue_pcb(rq);   // Get next task from ready queue (dequeue from head)
    }
}