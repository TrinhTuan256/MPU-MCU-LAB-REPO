/**
  ******************************************************************************
  * @file           : scheduler.c
  * @brief          : Cooperative Scheduler implementation for STM32F103
  *                   Uses an ordered Delta Queue to achieve O(1) in SCH_Update().
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "scheduler.h"
#include <string.h>

/* Global Variables ----------------------------------------------------------*/
sTask SCH_tasks_G[SCH_MAX_TASKS];
uint8_t Error_code_G = 0;

/* Private Variables ---------------------------------------------------------*/
static uint32_t current_task_count = 0;
static uint32_t task_id_counter = 1;

/* Private Function Prototypes -----------------------------------------------*/
static uint32_t SCH_Add_Task_Internal(void (*pFunction)(void), uint32_t DELAY, uint32_t PERIOD, uint32_t taskID);

/* Functions -----------------------------------------------------------------*/

/**
  * @brief  Initializes the scheduler task queue and error codes.
  */
void SCH_Init(void) {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    memset(SCH_tasks_G, 0, sizeof(SCH_tasks_G));
    current_task_count = 0;
    task_id_counter = 1;
    Error_code_G = 0;

    __set_PRIMASK(primask);
}

/**
  * @brief  Internal helper to insert a task into the sorted Delta Queue.
  *         If taskID is NO_TASK_ID, a new unique ID is assigned.
  */
static uint32_t SCH_Add_Task_Internal(void (*pFunction)(void), uint32_t DELAY, uint32_t PERIOD, uint32_t taskID) {
    if (pFunction == NULL) {
        return NO_TASK_ID;
    }

    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    if (current_task_count >= SCH_MAX_TASKS) {
        Error_code_G = ERROR_SCH_TOO_MANY_TASKS;
        __set_PRIMASK(primask);
        return NO_TASK_ID;
    }

    uint32_t assigned_id = taskID;
    if (assigned_id == NO_TASK_ID) {
        assigned_id = task_id_counter++;
        if (task_id_counter == NO_TASK_ID) {
            task_id_counter = 1;
        }
    }

    /* Find the insertion position in the delta queue */
    uint32_t new_delay = DELAY;
    uint32_t insert_index = 0;

    while (insert_index < current_task_count && new_delay >= SCH_tasks_G[insert_index].Delay) {
        new_delay -= SCH_tasks_G[insert_index].Delay;
        insert_index++;
    }

    /* Shift subsequent tasks to make room */
    for (uint32_t j = current_task_count; j > insert_index; j--) {
        SCH_tasks_G[j] = SCH_tasks_G[j - 1];
    }

    /* Adjust the delta delay of the task immediately after the inserted position */
    if (insert_index < current_task_count) {
        SCH_tasks_G[insert_index + 1].Delay -= new_delay;
    }

    /* Insert new task entry */
    SCH_tasks_G[insert_index].pTask = pFunction;
    SCH_tasks_G[insert_index].Delay = new_delay;
    SCH_tasks_G[insert_index].Period = PERIOD;
    SCH_tasks_G[insert_index].RunMe = 0;
    SCH_tasks_G[insert_index].TaskID = assigned_id;

    current_task_count++;

    __set_PRIMASK(primask);
    return assigned_id;
}

/**
  * @brief  Public API to add a task to the scheduler queue.
  * @param  pFunction: Function pointer to task.
  * @param  DELAY: Delay (in ticks) before first execution.
  * @param  PERIOD: Interval (in ticks) between executions (0 for one-shot).
  * @retval Unique TaskID if successful, or NO_TASK_ID (0) on error.
  */
uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t DELAY, uint32_t PERIOD) {
    return SCH_Add_Task_Internal(pFunction, DELAY, PERIOD, NO_TASK_ID);
}

/**
  * @brief  Updates the remaining time of tasks in the queue.
  *         Invoked inside the hardware timer interrupt (e.g. TIM2 every 10 ms).
  *         Operates in strictly O(1) time complexity by updating only the queue head.
  */
void SCH_Update(void) {
    if (current_task_count > 0 && SCH_tasks_G[0].pTask != 0) {
        if (SCH_tasks_G[0].Delay > 0) {
            SCH_tasks_G[0].Delay--;
        }
        if (SCH_tasks_G[0].Delay == 0) {
            SCH_tasks_G[0].RunMe = 1;
        }
    }
}

/**
  * @brief  Dispatches ready tasks to execute in the main super-loop (while(1)).
  */
void SCH_Dispatch_Tasks(void) {
    while (current_task_count > 0 && 
           SCH_tasks_G[0].pTask != 0 && 
           (SCH_tasks_G[0].RunMe > 0 || SCH_tasks_G[0].Delay == 0)) {
        
        /* Save task details */
        void (*pTask)(void) = SCH_tasks_G[0].pTask;
        uint32_t period = SCH_tasks_G[0].Period;
        uint32_t taskID = SCH_tasks_G[0].TaskID;

        /* Remove task from head of queue before running so the next task becomes index 0 */
        uint32_t primask = __get_PRIMASK();
        __disable_irq();

        for (uint32_t j = 0; j < current_task_count - 1; j++) {
            SCH_tasks_G[j] = SCH_tasks_G[j + 1];
        }
        memset(&SCH_tasks_G[current_task_count - 1], 0, sizeof(sTask));
        current_task_count--;

        __set_PRIMASK(primask);

        /* Execute task callback */
        (*pTask)();

        /* If periodic, re-add it preserving the original taskID */
        if (period > 0) {
            SCH_Add_Task_Internal(pTask, period, period, taskID);
        }
    }

    SCH_Report_Status();
    SCH_Go_To_Sleep();
}

/**
  * @brief  Deletes a task from the scheduler queue using its unique TaskID.
  * @param  taskID: Unique ID of the task to delete.
  * @retval RETURN_NORMAL (0) on success, RETURN_ERROR (1) on failure.
  */
uint8_t SCH_Delete_Task(uint32_t taskID) {
    if (taskID == NO_TASK_ID) {
        return RETURN_ERROR;
    }

    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    uint8_t return_code = RETURN_ERROR;
    for (uint32_t i = 0; i < current_task_count; i++) {
        if (SCH_tasks_G[i].TaskID == taskID) {
            /* Pass the remaining delay to the following task */
            if (i + 1 < current_task_count) {
                SCH_tasks_G[i + 1].Delay += SCH_tasks_G[i].Delay;
            }

            /* Shift subsequent tasks left */
            for (uint32_t j = i; j < current_task_count - 1; j++) {
                SCH_tasks_G[j] = SCH_tasks_G[j + 1];
            }

            memset(&SCH_tasks_G[current_task_count - 1], 0, sizeof(sTask));
            current_task_count--;
            return_code = RETURN_NORMAL;
            break;
        }
    }

    if (return_code == RETURN_ERROR) {
        Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
    }

    __set_PRIMASK(primask);
    return return_code;
}

/**
  * @brief  Optional hook to report status / errors.
  */
void SCH_Report_Status(void) {
#ifdef SCH_REPORT_ERRORS
    // Error reporting logic if enabled
#endif
}

/**
  * @brief  Optional hook for low power mode when idle.
  */
void SCH_Go_To_Sleep(void) {
    // __WFI(); // Wait for Interrupt
}
