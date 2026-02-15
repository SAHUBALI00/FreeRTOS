#ifndef _CONFIG_H__
#define _CONFIG_H__

#define configTICK_TYPE_WIDTH_IN_BITS TICK_TYPE_WIDTH_32_BITS
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 5
#define configMINIMAL_STACK_SIZE 0x100                 //256 words(=1 kB)
#define configMAX_PRIORITIES 5
#define configUSE_PREEMPTION 1
#define configUSE_IDLE_HOOK 0
#define configUSE_TICK_HOOK 0
#define configTOTAL_HEAP_SIZE 0x1000   //4kb
#define configCPU_CLOCK_HZ 8000000     //CPU CLOCK FRQ 8MHZ
#define configTICK_RATE_HZ 10          //10 TIMES ROUND ROBIN IN A SECOND
#define INCLUDE_vTaskDelay    1
#define INCLUDE_vTaskDelete    1
#define vApplicationTickHook 1
#define configUSE_MUTEXES    1
#define configUSE_COUNTING_SEMAPHORES 1
#define configCHECK_FOR_STACK_OVERFLOW    2
#define INCLUDE_uxTaskGetStackHighWaterMark    1
#define configUSE_APPLICATION_TASK_TAG    1

#define configUSE_TIMERS             1
#define configTIMER_TASK_PRIORITY    (configMAX_PRIORITIES - 1)
#define configTIMER_QUEUE_LENGTH     10
#define configTIMER_TASK_STACK_DEPTH configMINIMAL_STACK_SIZE

#endif