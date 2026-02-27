#ifndef MPU_H
#define MPU_H
#include"utilities/i2c_utility.h"
#include"stdint.h"
#include"utilities/shared_types.h"


void Init_MPU(void);
void process_mpu_data(sensor_t *dest_struct);

#endif
