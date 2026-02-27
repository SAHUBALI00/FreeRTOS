#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H

#include <stdint.h>

typedef struct
{
    int16_t Accel_X;
    int16_t Temperature;
    int16_t Gyro_X;
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    // char *meridiem;
    char *day;
    uint8_t date; // as year would greater than 255
    uint8_t month;
    uint16_t year;
} sensor_t;


#endif