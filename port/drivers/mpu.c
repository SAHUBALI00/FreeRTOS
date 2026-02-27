#include "mpu.h"

typedef struct
{
    int16_t Accel_X, Accel_Y, Accel_Z;
    int16_t Temperature;
    int16_t Gyro_X, Gyro_Y, Gyro_Z;

} mpu6050;

void I2C_Write(uint8_t data);
void I2C_receive(int8_t bytes[]);

void Init_MPU()
{
    //  // Wake up MPU6050
    I2C_Start(0x68, 0);
    I2C_Write(0x6B); // PWR_MGMT_1 register
    I2C_Write(0x00); // Set to 0 to wake it up
    I2C_Stop();
    while (*i2c_sr2 & (1 << 1))
        ; // Wait until the BUSY bit is cleared by hardware
}

void process_mpu_data(sensor_t *dest_struct)
{
    int8_t raw[14];
    I2C_receive(raw);

    // Accelerometer (0x3B to 0x40)
    dest_struct->Accel_X = (int16_t)((raw[0] << 8 | raw[1]) / 16384);
    // dest_struct->Accel_Y = (int16_t)((raw[2] << 8 | raw[3])/ 16384);
    // dest_struct->Accel_Z = (int16_t)((raw[4] << 8 | raw[5])/ 16384);

    // Temperature (0x41 to 0x42)
    dest_struct->Temperature = (int16_t)((raw[6] << 8 | raw[7]) / 340) + 36;

    // Gyroscope (0x43 to 0x48)
    dest_struct->Gyro_X = (int16_t)((raw[8] << 8 | raw[9]) / 131);
    // dest_struct->Gyro_Y = (int16_t)((raw[10] << 8 | raw[11])/131);
    // dest_struct->Gyro_Z = (int16_t)((raw[12] << 8 | raw[13])/131);
}

void I2C_receive(int8_t bytes[])
{
    // 1. Point to the first register (Accel X High)
    I2C_Start(0x68, 0);
    I2C_Write(0x3B);
    while (!(*i2c_sr1 & (1 << 2))); // Wait for BTF

    I2C_Stop();
    // After I2C_Stop() or before I2C_Start()
    while (*i2c_sr2 & (1 << 1)); // Wait until the BUSY bit is cleared by hardware

    // --- CRITICAL FIX: ENABLE ACK BEFORE START ---

    I2C_Start(0x68, 1); // Start in Read Mode
    *i2c_cr1 |= (1 << 10);

    for (int i = 0; i < 14; i++)
    {
        // Prepare NACK for the last byte
        if (i == 13)
        {
            *i2c_cr1 &= ~(1 << 10); // Clear ACK (NACK)
            *i2c_cr1 |= (1 << 9);   // Set STOP
        }

        while (!(*i2c_sr1 & (1 << 6)))
            ;

        bytes[i] = *i2c_dr; // Read clears RxNE
    }
}
