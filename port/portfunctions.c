#include "FreeRTOS.h"
#include "task.h"

void* memset(void *dst, int value, unsigned int size) {
    unsigned char *ptr = (unsigned char *)dst;
    unsigned char byte_value = (unsigned char)value;

    // Fill memory byte by byte
    while (size > 0) {
        *ptr = byte_value;
        ptr++;
        size--;
    }

    return dst; // Return original pointer as per standard
}
void* memcpy(void *dest, const void *src, unsigned int n) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;

    while (n--) {
        *d++ = *s++;
    }

    return dest;
}
// Add this to start.c
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    /* This function will be called if a task overflows its stack. */
    /* pcTaskName contains the name of the offending task. */

    // For now, just sit in a loop so you can catch it with a debugger
    while(1);
}