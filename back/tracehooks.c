// tracehooks.c

#include "FreeRTOS.h"
#include "task.h"
#include <windows.h>
#include <stdio.h>

/* Имя выходного CSV-файла */
#define TRACE_CSV_FILENAME  "task_switch_log.csv"

/* Глобальный файловый указатель */
static FILE* csvFile = NULL;

/* Для высокоточного таймера Win32 */
static LARGE_INTEGER freq;        // частота счётчика
static LARGE_INTEGER startCount;  // базовый отсчёт на момент запуска лога
static int useHighRes = 0;        // флаг: таймер инициализирован

/* Вызывается один раз перед запуском планировщика */
void vSetupTraceLog(void)
{
    /* Открываем CSV */
    csvFile = fopen(TRACE_CSV_FILENAME, "w");
    if (csvFile == NULL) {
        return;
    }
    fprintf(csvFile, "time_ms,event,task\n");

    /* Инициализируем высокоточный таймер */
    if (QueryPerformanceFrequency(&freq) && QueryPerformanceCounter(&startCount)) {
        useHighRes = 1;
    }
    else {
        useHighRes = 0;
        startCount.QuadPart = 0;
    }

    fflush(csvFile);
}

/* Вспомогательная: текущее время (ms) от старта лога */
static double getTimeMs(void)
{
    if (useHighRes) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        // разница в «тактах» делим на freq, умножаем на 1000 → ms
        return (double)(now.QuadPart - startCount.QuadPart) * 1000.0
            / (double)freq.QuadPart;
    }
    else {
        TickType_t t = xTaskGetTickCount();
        return (double)t * 1000.0 / (double)configTICK_RATE_HZ;
    }
}

/* Логика Switch Out и Switch In остаётся без изменений, только время берём из getTimeMs() */
void myTaskSwitchOut(TaskHandle_t xTask)
{
    const char* name = pcTaskGetName(xTask);
    //printf("<< Switch out of task %s\n", name);

    if (csvFile == NULL) return;
    double t_ms = getTimeMs();
    fprintf(csvFile, "%.3f,out,%s\n", t_ms, name);
    fflush(csvFile);
}

void myTaskSwitchIn(TaskHandle_t xTask)
{
    const char* name = pcTaskGetName(xTask);
    //printf(">> Switch in of task %s\n", name);

    if (csvFile == NULL) return;
    double t_ms = getTimeMs();
    fprintf(csvFile, "%.3f,in,%s\n", t_ms, name);
    fflush(csvFile);
}

void vCloseTraceLog(void)
{
    if (csvFile != NULL) {
        fclose(csvFile);
        csvFile = NULL;
    }
}
