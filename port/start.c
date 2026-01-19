#include"FreeRTOS.h"
#include"task.h"
#include "queue.h"
#include"semphr.h"
#include"timers.h"
#include"event_groups.h"


 // Force it to look at the start of Flash
// QueueHandle_t xHeartbeatQueue;
SemaphoreHandle_t xBinarySemaphore;
SemaphoreHandle_t xMutex;
volatile uint32_t shared_resource = 0;
TimerHandle_t xTimer;
EventGroupHandle_t xStartupEventGroup;

#define BIT_TASK1_READY ( 0 )
#define BIT_TASK2_READY ( 1 << 1 )
static int c=0;
static void vTask1(void *pvParameters) {
    // Simulate some work
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Set Bit 0 to say "I'm ready!"
    xEventGroupSetBits(xStartupEventGroup, BIT_TASK1_READY);
    vTaskDelete(NULL);
}

static void vTask2(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Set Bit 1 to say "I'm ready!"
    xEventGroupSetBits(xStartupEventGroup, BIT_TASK2_READY);
    vTaskDelete(NULL);
}

static void vTask3(void *pvParameters) {
    // Wait for BOTH bits to be set
    xEventGroupWaitBits(
        xStartupEventGroup,    // The group
        BIT_TASK1_READY | BIT_TASK2_READY, // Bits to wait for
        pdTRUE,                // Clear bits on exit?
        pdTRUE,                // Wait for ALL bits (AND logic)?
        portMAX_DELAY          // Wait forever
    );

    // This only prints AFTER Task 1 and Task 2 are done
    while(1) {
        // Main system logic starts here...
        c++;
    }
}
void vMyTimerCallback(TimerHandle_t xTimer) {
    // This code runs every time the timer expires
    static int timer_count = 0;
    timer_count++;
}

extern uint32_t _sidata; //src (ROM)
extern uint32_t _sdata; //destn start (RAM)
extern uint32_t _edata; //destn end
extern uint32_t _sbss;
extern uint32_t _ebss;


BaseType_t xReturn;

void start(){


    //INtialsing .data section (copy from FLASH To RAM)
    uint32_t *src = &_sidata;
    uint32_t *des = &_sdata;
    uint32_t *des_end= &_edata;
    uint32_t *des_bss= &_sbss;
    uint32_t *des_end_bss= &_ebss;



    while(des < des_end){
        *des = *src;
        des++;
        src++;
    }
    //intialise .bss section
    while(des_bss < des_end_bss){
        *des_bss++ = 0;
    }

    //  // Create a "Auto-Reload" timer that fires every 2000ms (2 seconds)
    // xTimer = xTimerCreate("MyTimer", pdMS_TO_TICKS(2000), pdTRUE, (void*)0, vMyTimerCallback);

    // if(xTimer != NULL) {
    //     xTimerStart(xTimer, 0); // Start the timer
    // }

    //    xMutex = xSemaphoreCreateMutex();
    xStartupEventGroup = xEventGroupCreate();
       if(xStartupEventGroup != NULL){
           xReturn = xTaskCreate(vTask1,"T1",70, NULL ,1, NULL );
      xReturn = xTaskCreate(vTask2,"T2",70, NULL ,1, NULL );
       xReturn = xTaskCreate(vTask3,"T3",70, NULL ,0, NULL );
        // portENABLE_INTERRUPTS();
        //  xPortStartScheduler();
       vTaskStartScheduler();
      }
    return;
}