#ifndef RTC_H
#define RTC_H
#include<stdint.h>
#include"../utilities/i2c_utility.h"
#include"utilities/shared_types.h"

void RTC_SetTime(uint8_t sec, uint8_t minutes, uint8_t hour,uint8_t day,uint8_t date,uint8_t month,uint8_t year) ;
void process_rtc_data(sensor_t *dest_struct);


#endif