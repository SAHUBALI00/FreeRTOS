#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"
#include "lcd.h"

TaskHandle_t xHandlerTaskHandle = NULL;
EventGroupHandle_t xHealthCheckEvent;

#define bit0 (1<<0)
#define bit1 (1<<1)

static int c = 0;
//this is the handle task
//should have the high priority to ensure it runs immediately after the interrupt.
static void task1(void *prParameteres){
    //health check perform
    // for(int i=0; i<50000;i++);
    // xEventGroupSetBits(xHealthCheckEvent, bit0);
    // vTaskDelete(NULL);  //null represents the current task
    while(1){
        setup();
    }

}
static void task2(void *prParameteres){
    //health check perform
    //        for(int i=0; i<50000;i++);
    // xEventGroupSetBits(xHealthCheckEvent, bit1);
    // vTaskDelete(NULL);  //null represents the current task
    while(1);
}

static void vHandlerTask(void *pvParameters)
{
    UBaseType_t uxHighWaterMark;
    static int incrementData;

    xEventGroupWaitBits(xHealthCheckEvent, (bit0|bit1) ,pdFALSE,pdFALSE,portMAX_DELAY);
    while (1)
    {
        ulTaskNotifyTake(pdFALSE,portMAX_DELAY);

        incrementData++;
        uxHighWaterMark=uxTaskGetStackHighWaterMark(NULL);

    }




}
//the ISR
//in a real mCEU, this would be linked to a hardware vector
static void vExampleInterruptHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken=pdFALSE;



    vTaskNotifyGiveFromISR(xHandlerTaskHandle, &xHigherPriorityTaskWoken);


portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

}
//trigger task(hardware)
static void vTrigger(void *pvParameters){
    while (1)
    {
         for(int i=0; i<3; i++){
        //manually triggering interrupt
        vExampleInterruptHandler();
        vTaskDelay(pdMS_TO_TICKS(5000));
         }
    }

}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    // If the debugger hits this loop, you've shrunk the stack too much!
    // 'pcTaskName' will tell you which task ran out of breath.
    while(1);
}

extern uint32_t _sidata; // src (ROM)
extern uint32_t _sdata;  // destn start (RAM)
extern uint32_t _edata;  // destn end
extern uint32_t _sbss;
extern uint32_t _ebss;

BaseType_t xReturn;

void start()
{

    // INtialsing .data section (copy from FLASH To RAM)
    uint32_t *src = &_sidata;
    uint32_t *des = &_sdata;
    uint32_t *des_end = &_edata;
    uint32_t *des_bss = &_sbss;
    uint32_t *des_end_bss = &_ebss;

    while (des < des_end)
    {
        *des = *src;
        des++;
        src++;
    }
    // intialise .bss section
    while (des_bss < des_end_bss)
    {
        *des_bss++ = 0;
    }


    // xInterruptsemahore= xSemaphoreCreateCounting(10,0);
    xHealthCheckEvent=xEventGroupCreate();

    if (xHealthCheckEvent != NULL)
    {
        // xReturn = xTaskCreate(vHandlerTask, "handler", 70, NULL, 1, &xHandlerTaskHandle);
        // // xReturn = xTaskCreate(vExampleInterruptHandler, "T2", 70, NULL, 1, NULL);
        // xReturn = xTaskCreate(vTrigger, "Trigger", 70, NULL, 2, NULL);
        xReturn = xTaskCreate(task1, "task1", 70, NULL, 3, NULL);
        xReturn = xTaskCreate(task2, "task2", 70, NULL, 3, NULL);


        // portENABLE_INTERRUPTS();
        //  xPortStartScheduler();

        vTaskStartScheduler();
    }
    return;
}