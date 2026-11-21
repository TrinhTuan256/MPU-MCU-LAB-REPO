/**
  ******************************************************************************
  * @file           : scheduler.h
  * @brief          : Header for scheduler.c module
  *                   Cooperative Scheduler implementation for STM32F103
  ******************************************************************************
  */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdint.h>
#include <stdbool.h>

/* Defines -------------------------------------------------------------------*/
#define SCH_MAX_TASKS                     40
#define NO_TASK_ID                        0

/* Return codes */
#define RETURN_NORMAL                     0
#define RETURN_ERROR                      1

/* Error codes */
#define ERROR_SCH_TOO_MANY_TASKS          1
#define ERROR_SCH_CANNOT_DELETE_TASK      2
#define ERROR_SCH_WAITING_FOR_SLAVE_TO_ACK 3
#define ERROR_SCH_WAITING_FOR_START_COMMAND_FROM_MASTER 4
#define ERROR_SCH_ONE_OR_MORE_SLAVES_DID_NOT_START 5
#define ERROR_SCH_LOST_SLAVE              6
#define ERROR_SCH_CAN_BUS_ERROR           7
#define ERROR_I2C_WRITE_BYTE_AT24C64      8

/* Task Structure ------------------------------------------------------------*/
typedef struct {
    // Pointer to the task (must be a 'void (void)' function)
    void (*pTask)(void);
    // Delay (ticks) until the function will (next) be run
    uint32_t Delay;
    // Interval (ticks) between subsequent runs (0 for one-shot tasks)
    uint32_t Period;
    // Incremented (by scheduler) when task is due to execute
    uint8_t RunMe;
    // Unique identifier for the task
    uint32_t TaskID;
} sTask;

/* Exported Variables --------------------------------------------------------*/
extern sTask SCH_tasks_G[SCH_MAX_TASKS];
extern uint8_t Error_code_G;

/* Exported Functions --------------------------------------------------------*/
/**
  * @brief  Initializes the scheduler task queue and error codes.
  */
void SCH_Init(void);

/**
  * @brief  Updates the remaining time of tasks in the queue.
  *         Invoked inside the hardware timer interrupt (e.g. TIM2 every 10 ms).
  *         Runs in O(1) time complexity.
  */
void SCH_Update(void);

/**
  * @brief  Dispatches ready tasks to execute in the main super-loop (while(1)).
  */
void SCH_Dispatch_Tasks(void);

/**
  * @brief  Adds a task to the scheduler queue.
  * @param  pFunction: Function pointer to the task callback (void func(void)).
  * @param  DELAY: Initial delay in ticks before first execution.
  * @param  PERIOD: Repetition period in ticks (0 for one-shot tasks).
  * @retval Unique TaskID if successful, or NO_TASK_ID (0) on error.
  */
uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t DELAY, uint32_t PERIOD);

/**
  * @brief  Deletes a task from the scheduler queue using its unique TaskID.
  * @param  taskID: Unique ID of the task to delete.
  * @retval RETURN_NORMAL (0) on success, RETURN_ERROR (1) on failure.
  */
uint8_t SCH_Delete_Task(uint32_t taskID);

/**
  * @brief  Reports error status (optional hook).
  */
void SCH_Report_Status(void);

/**
  * @brief  Puts MCU in sleep/idle mode when no tasks are running (optional hook).
  */
void SCH_Go_To_Sleep(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_SCHEDULER_H_ */
