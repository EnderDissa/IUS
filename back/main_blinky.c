/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/******************************************************************************
 * This project provides two demo applications.  A simple blinky style project,
 * and a more comprehensive test and demo application.  The
 * mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting in main.c is used to select
 * between the two.  See the notes on using mainCREATE_SIMPLE_BLINKY_DEMO_ONLY
 * in main.c.  This file implements the simply blinky version.
 *
 * This file only contains the source code that is specific to the basic demo.
 * Generic functions, such FreeRTOS hook functions, are defined in main.c.
 ******************************************************************************
 *
 * main_blinky() creates one queue, one software timer, and two tasks.  It then
 * starts the scheduler.
 *
 * The Queue Send Task:
 * The queue send task is implemented by the prvQueueSendTask() function in
 * this file.  It uses vTaskDelayUntil() to create a periodic task that sends
 * the value 100 to the queue every 200 (simulated) milliseconds.
 *
 * The Queue Send Software Timer:
 * The timer is an auto-reload timer with a period of two (simulated) seconds.
 * Its callback function writes the value 200 to the queue.  The callback
 * function is implemented by prvQueueSendTimerCallback() within this file.
 *
 * The Queue Receive Task:
 * The queue receive task is implemented by the prvQueueReceiveTask() function
 * in this file.  prvQueueReceiveTask() waits for data to arrive on the queue.
 * When data is received, the task checks the value of the data, then outputs a
 * message to indicate if the data came from the queue send task or the queue
 * send software timer.
 */

/* Standard includes. */
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <inttypes.h>

/* Kernel includes. */
#include "FreeRTOS.h"
// #include "FreeRTOS_Sockets.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY    ( tskIDLE_PRIORITY + 2 )
#define mainQUEUE_SEND_TASK_PRIORITY       ( tskIDLE_PRIORITY + 1 )

/* The rate at which data is sent to the queue.  The times are converted from
 * milliseconds to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK_RECEIVE_FREQUENCY_MS      pdMS_TO_TICKS( 300UL )

/* The number of items the queue can hold at once. */
#define mainQUEUE_LENGTH                   ( 5 )

/*-----------------------------------------------------------*/

/*
 * The tasks as described in the comments at the top of this file.
 */
static void argProvider1(void * pvParameters);
static void argProvider2(void * pvParameters);
static void summator(void * pvParameters);


/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue1 = NULL;
static QueueHandle_t xQueue2 = NULL;

/*-----------------------------------------------------------*/

/*** SEE THE COMMENTS AT THE TOP OF THIS FILE ***/
void main_blinky( void )
{
    srand(time(NULL));

    /* Create the queue. */
    xQueue1 = xQueueCreate( mainQUEUE_LENGTH, sizeof( int ) );
    xQueue2 = xQueueCreate( mainQUEUE_LENGTH, sizeof( int ) );


    /* TODO TraceRecorder (Tweak 4): Setting a name for the queue (optional). */
    vTraceSetQueueName(xQueue1, "Sum-Queue1");
    vTraceSetQueueName(xQueue2, "Sum-Queue2");

    if( xQueue1 != NULL && xQueue2 != NULL )
    {
        /* Start the two tasks as described in the comments at the top of this
         * file. */
        xTaskCreate( argProvider1,             /* The function that implements the task. */
                     "argProvider1",                            /* The text name assigned to the task - for debug only as it is not used by the kernel. */
                     configMINIMAL_STACK_SIZE,        /* The size of the stack to allocate to the task. */
                     NULL,                            /* The parameter passed to the task - not used in this simple case. */
                     tskIDLE_PRIORITY, /* The priority assigned to the task. */
                     NULL );                          /* The task handle is not required, so NULL is passed. */

        xTaskCreate( argProvider2, "argProvider2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
        
        xTaskCreate( summator, "TX", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

                                   /* The scheduler has not started so use a block time of 0. */

        /* Start the tasks and timer running. */
        vTaskStartScheduler();
    }

    /* If all is well, the scheduler will now be running, and the following
     * line will never be reached.  If the following line does execute, then
     * there was insufficient FreeRTOS heap memory available for the idle and/or
     * timer tasks	to be created.  See the memory management section on the
     * FreeRTOS web site for more details.  NOTE: This demo uses static allocation
     * for the idle and timer tasks so this line should never execute. */
    for( ; ; )
    {
    }
}
/*-----------------------------------------------------------*/

static void argProvider1( void * pvParameters )
{
    TickType_t currentTick = xTaskGetTickCount();
    int arg;
    /* Prevent the compiler warning about the unused parameter. */
    ( void ) pvParameters;

    for( ; ; )
    {
        // printf("ArgProvider1 is running\n");
        while(xTaskGetTickCount() - currentTick < pdMS_TO_TICKS(650)) {
            continue;
        }
        currentTick = xTaskGetTickCount();
        arg = rand() % 5000;
        if (xQueueSend( xQueue1, (void *) &arg, ( TickType_t ) 10)!= pdPASS) {
            printf("Error sending arg 1\n");
        }
    }
}

static void argProvider2( void * pvParameters )
{
    TickType_t currentTick = xTaskGetTickCount();
    int arg;

    /* Prevent the compiler warning about the unused parameter. */
    ( void ) pvParameters;

    for( ; ; )
    {
        // printf("ArgProvider2 is running\n");
        while(xTaskGetTickCount() - currentTick < pdMS_TO_TICKS(500)) {
            continue;
        }
        currentTick = xTaskGetTickCount();
        arg = rand() % 5000;
        if (xQueueSend( xQueue2, (void *) &arg, ( TickType_t ) 10)!= pdPASS) {
            printf("Error sending arg 2\n");
        }
    }
}
/*-----------------------------------------------------------*/

// static void prvQueueSendTimerCallback( TimerHandle_t xTimerHandle )
// {
//     const uint32_t ulValueToSend = mainVALUE_SENT_FROM_TIMER;

//     /* This is the software timer callback function.  The software timer has a
//      * period of two seconds and is reset each time a key is pressed.  This
//      * callback function will execute if the timer expires, which will only happen
//      * if a key is not pressed for two seconds. */

//     /* Avoid compiler warnings resulting from the unused parameter. */
//     ( void ) xTimerHandle;

//     /* Send to the queue - causing the queue receive task to unblock and
//      * write out a message.  This function is called from the timer/daemon task, so
//      * must not block.  Hence the block time is set to 0. */
//     xQueueSend( xQueueTimer, &ulValueToSend, 0U );
// }
/*-----------------------------------------------------------*/

static void summator( void * pvParameters )
{
    int arg1, arg2;
    char arg1Vld, arg2Vld;
    TickType_t xNextWakeTime;
    const TickType_t xBlockTime = mainTASK_RECEIVE_FREQUENCY_MS;

    /* Prevent the compiler warning about the unused parameter. */
    ( void ) pvParameters;
    xNextWakeTime = xTaskGetTickCount();

    /* TraceRecorder: Registering a channel name for the user events. */
    TraceStringHandle_t xUserEventLogChannel;
    xTraceStringRegister("Log", &xUserEventLogChannel);

    arg1Vld = 0;
    arg2Vld = 0;

    for( ; ; )
    {
        vTaskDelayUntil( &xNextWakeTime, xBlockTime );
        
        if (!arg1Vld) {
            if (xQueueReceive( xQueue1, &arg1, ( TickType_t ) 10 ) == pdPASS) {
                printf("Received arg1\n");
                arg1Vld = 1;
            }
        }
        if (!arg2Vld) {
            if (xQueueReceive( xQueue2, &arg2, ( TickType_t ) 10 ) == pdPASS) {
                printf("Received arg2\n");
                arg2Vld = 1;
            }
        }

        /*  To get here both values must have been received from the queues */
        if( arg1Vld && arg2Vld )
        {
            printf( "Sum: %d\n", arg1 + arg2 );
            arg1Vld = 0;
            arg2Vld = 0;
            
            xTracePrint(xUserEventLogChannel, "Arguments received from task");
        }
        else
        {
            printf( "Waiting for arguments\n" );

            xTracePrint(xUserEventLogChannel,
            		"Arguments are not ready");
        }
    }
}

/*-----------------------------------------------------------*/
