#ifndef I2C_UTILITY_H
#define I2C_UTILITY_H

#include "FreeRTOS.h"
#include "semphr.h"
#include <stdint.h>

// Addresses
#define I2C1_BASE        0x40005400
#define RCC_BASE         0x40021000
#define APB1_offset            0x1C
#define APB2_offset            0x18
#define GPIOB_BASE       0x40010C00
#define GPIOB_CRL_offset       0x00

// Register Offsets
#define I2C_CR1_OFFSET   0x00
#define I2C_CR2_OFFSET   0x04
#define I2C_SR1_OFFSET   0x14
#define I2C_SR2_OFFSET   0x18
#define I2C_DR_OFFSET    0x10
#define I2C_CCR          0x1C
#define I2C_TRISE        0x20

// Pointer definitions (using your style)
extern volatile uint32_t *const abp1 ;
extern volatile uint32_t *const abp2 ;
extern volatile uint32_t *const portB ;
extern volatile uint32_t * const i2c_cr1  ;
extern volatile uint32_t * const i2c_cr2  ;
extern volatile uint32_t * const i2c_sr1 ;
extern volatile uint32_t * const i2c_sr2  ;
extern volatile uint32_t * const i2c_dr;
extern volatile uint32_t * const i2c_trise ;
extern volatile uint32_t * const i2c_ccr ;


// Function Prototypes
void I2C_Init(void);
int  I2C_Start(uint8_t address, uint8_t direction); // 0=Write, 1=Read
void I2C_Write(uint8_t data);
// uint8_t I2C_Read_Ack(void);
// uint8_t I2C_Read_Nack(void);
void I2C_Stop(void);

#endif