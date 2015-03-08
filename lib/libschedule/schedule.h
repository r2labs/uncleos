/* -*- mode: c; c-basic-offset: 4; -*- */
/* Created by Hershal Bhave and Eric Crosson 2015-03-07 */
/* Revision history: Look in Git FGT */
#ifndef __SCHEDULE__
#define __SCHEDULE__

/*! \addtogroup
 * @{
 */

/* We are using the priority scheduler */

/* Recognized priority schedulers */
#include "priority_schedule_structures.h"

#include "libhw/hardware.h"
/* #include "libut/uthash.h" */

#define SCHEDULER_DEFAULT_MAX_THREADS    16

#if !(defined(SCHEDULER_MAX_THREADS))
#define SCHEDULER_MAX_THREADS   SCHEDULER_DEFAULT_MAX_THREADS
#endif

/*! Statically allocated multiple queues of tasks */
static sched_task_pool SCHEDULER_TASK_QUEUES[SCHEDULER_MAX_THREADS];

/*! Doubly linked list of unused task queues */
static sched_task_pool* SCHEDULER_UNUSED_QUEUES = NULL;

/*! UTHash of live task queues */
/* for now it's a utlist though, don't get confused */
static sched_task_pool* SCHEDULER_QUEUES = NULL;

/*! Statically allocated task metadata structures for the scheduler to
 *  manage */
static sched_task SCHEDULER_TASKS[SCHEDULER_MAX_THREADS];

/*! Doubly linked list of unused tasks */
static sched_task* SCHEDULER_UNUSED_TASKS = NULL;


/*! Initialize all deep datastructures used by libschedule. */
void schedule_init();

void schedule(task_t, frequency_t, DEADLINE_TYPE);

/*! Schedule a pseudo-isr to be executed when a hardware event
 *  described by HW_TYPE and hw_metadata occurs. */
void schedule_aperiodic(pisr_t, HW_TYPE, hw_metadata, microseconds_t, DEADLINE_TYPE);

#endif	/* __SCHEDULE__ */

/* End Doxygen group
 * @}
 */
