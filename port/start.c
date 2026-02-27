#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include"./drivers/lcd.h"
#include"./drivers/rtc.h"
#include"./drivers/mpu.h"
#include"semphr.h"
#include"./utilities/i2c_utility.h"
#include"./utilities/shared_types.h"

EventGroupHandle_t xSensorEventGroup;
QueueHandle_t xDisplayQueue;
SemaphoreHandle_t xI2CMutex;

#define BIT_MPU_READY (1 << 0)
#define BIT_RTC_READY (1 << 1)
typedef struct
{
    char line1[17];
    char line2[17];
} LCD_Msg_t; // message packet

sensor_t sensor_data;

static void read_MPU(void *prParameteres)
{
    // this will read the mpu data and save in sensor_data struct
    // after saving, set mpu_readybit
     if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdPASS) {
         Init_MPU();
        xSemaphoreGive(xI2CMutex);
    }
    while(1){
        if(xSemaphoreTake(xI2CMutex,portMAX_DELAY)==pdPASS){
            process_mpu_data(&sensor_data);
            xEventGroupSetBits(xSensorEventGroup,BIT_MPU_READY);
            xSemaphoreGive(xI2CMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }

}
static void read_RTC(void *prParameteres)
{

    //set the time just for once
    if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdPASS) {
        RTC_SetTime(0,30,02,6,13,2,26);
        xSemaphoreGive(xI2CMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(500));  //delay required
     //repeatedly read data  from rtc
     while(1){
        if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdPASS) {
        process_rtc_data(&sensor_data);
        xEventGroupSetBits(xSensorEventGroup,BIT_RTC_READY);
        xSemaphoreGive(xI2CMutex);
     }
    vTaskDelay(pdMS_TO_TICKS(1000));  //delay required
     }

}
static void vDisplayTask(void *pvParameters)
{
    LCD_Msg_t receivedFrame;
   if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdPASS) {
         Init_lcd();
        xSemaphoreGive(xI2CMutex);
    }
    // this will take that msg from queue and send it to display
    while(1){
        if(xQueueReceive(xDisplayQueue,&receivedFrame,portMAX_DELAY)==pdPASS){
            // Use the Mutex because RTC/MPU might be using I2C!
            if (xSemaphoreTake(xI2CMutex, portMAX_DELAY) == pdPASS) {
                I2C_Start(0x27, 0);
                clearDisplay();
                write_string(receivedFrame.line1);
                set_cursor_second_row();
                write_string(receivedFrame.line2);
                I2C_Stop();
                xSemaphoreGive(xI2CMutex);
        }
    }
}
}
void append(char *string, char arr[])
{   char *ptr=arr;
     while(*ptr !='\0'){
      ptr++;
    }
    while(*string !='\0'){
     *ptr=*string;
     string++;
     ptr++;
    }
    *ptr='\0';
}
// Instead of returning a pointer, we fill a buffer provided by the caller
void IntToStr(int data, char *buf) {
    int i = 0, j = 0;
    char temp[12]; // Temporary storage to flip the number

    if (data == 0) { buf[i++] = '0'; buf[i] = '\0'; return; }
    if (data < 0) { buf[i++] = '-'; data = -data; }

    // Convert digits to characters (backwards)
    while (data > 0) {
        temp[j++] = (data % 10) + '0';
        data /= 10;
    }
    // Reverse them into the final buffer
    while (j > 0) {
        buf[i++] = temp[--j];
    }
    buf[i] = '\0'; // Null terminator
}
void vSupervisorTask(void *pvParameters)
{
    LCD_Msg_t nextFrame;
    char valStr[12]; // Increased slightly for safety (signed 16-bit + null)

    while (1)
    {
        // 1. Wait for fresh data from both sensors
        xEventGroupWaitBits(xSensorEventGroup,
                            BIT_MPU_READY | BIT_RTC_READY,
                            pdTRUE,
                            pdTRUE,
                            portMAX_DELAY);

        // --- SCREEN 1: RTC DATA ---
        nextFrame.line1[0] = '\0'; // Initialize Row 1
        append("RTC:", nextFrame.line1);
        IntToStr(sensor_data.hours, valStr);   append(valStr, nextFrame.line1);
        append(":", nextFrame.line1);
        IntToStr(sensor_data.minutes, valStr); append(valStr, nextFrame.line1);
        append(":", nextFrame.line1);
        IntToStr(sensor_data.seconds, valStr); append(valStr, nextFrame.line1);

        nextFrame.line2[0] = '\0'; // Initialize Row 2
        append(sensor_data.day, nextFrame.line2);
        append(" ", nextFrame.line2);
        IntToStr(sensor_data.date, valStr);  append(valStr, nextFrame.line2);
        append("/", nextFrame.line2);
        IntToStr(sensor_data.month, valStr); append(valStr, nextFrame.line2);
        append("/", nextFrame.line2);
        IntToStr(sensor_data.year, valStr);  append(valStr, nextFrame.line2);

        // Non-blocking send for Screen 1
        if (xQueueSend(xDisplayQueue, &nextFrame, 0) == pdPASS) {
            vTaskDelay(pdMS_TO_TICKS(2000)); // Only wait if we successfully pushed to LCD
        }

        // --- SCREEN 2: MPU DATA ---
        nextFrame.line1[0] = '\0'; // CRITICAL: Clear line1 for the second screen!
        append("AX:", nextFrame.line1);
        IntToStr(sensor_data.Accel_X, valStr); append(valStr, nextFrame.line1);
        append(" GX:", nextFrame.line1);
        IntToStr(sensor_data.Gyro_X, valStr);  append(valStr, nextFrame.line1);

        nextFrame.line2[0] = '\0';
        append("Temp: ", nextFrame.line2);
        IntToStr(sensor_data.Temperature, valStr); append(valStr, nextFrame.line2);

        // Non-blocking send for Screen 2
        if (xQueueSend(xDisplayQueue, &nextFrame, 0) == pdPASS) {
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
}
// bootup data copy fromInt ROM To RAM

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

    I2C_Init();
    xSensorEventGroup = xEventGroupCreate();
    xDisplayQueue = xQueueCreate(1, sizeof(LCD_Msg_t));
    xI2CMutex=xSemaphoreCreateMutex();
    if (xSensorEventGroup != NULL && xDisplayQueue != NULL && xI2CMutex!=NULL)
    {
    xTaskCreate(read_MPU, "MPU", 128, NULL, 3, NULL);       // High Priority
    xTaskCreate(read_RTC, "RTC", 128, NULL, 2, NULL);       // Medium-Low
    xTaskCreate(vSupervisorTask, "Super", 256, NULL, 2, NULL); // Medium
    xTaskCreate( vDisplayTask, "LCD", 128, NULL, 1, NULL);   // Lowest Priority

        // portENABLE_INTERRUPTS();
        //  xPortStartScheduler();

        vTaskStartScheduler();
    }
    return;
}