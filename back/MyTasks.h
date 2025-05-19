#ifndef MYTASKS_H
#define MYTASKS_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define mainQUEUE_RECEIVE_TASK_PRIORITY    ( tskIDLE_PRIORITY + 2 )
#define mainQUEUE_SEND_TASK_PRIORITY       ( tskIDLE_PRIORITY + 1 )

/* The rate at which data is sent to the queue.  The times are converted from
 * milliseconds to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK_RECEIVE_FREQUENCY_MS      pdMS_TO_TICKS( 300UL )

 /* The number of items the queue can hold at once. */
#define mainQUEUE_LENGTH                   ( 5 )

/* Прототипы функций‑тасок, определённых в MyTasks.c */
void argProvider1(void* pvParameters);
void argProvider2(void* pvParameters);
void summator(void* pvParameters);

/* Дескрипторы очередей, определённых в MyTasks.c */
extern QueueHandle_t xQueue1;
extern QueueHandle_t xQueue2;

#endif /* MYTASKS_H */