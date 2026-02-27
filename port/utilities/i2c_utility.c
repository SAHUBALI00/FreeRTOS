#include "i2c_utility.h"

volatile uint32_t *const abp1 =(volatile uint32_t*) (RCC_BASE + APB1_offset);
volatile uint32_t *const abp2 =(volatile uint32_t*) (RCC_BASE + APB2_offset);
volatile uint32_t *const portB = (volatile uint32_t*) (GPIOB_BASE + GPIOB_CRL_offset);
volatile uint32_t * const i2c_cr1  = (volatile uint32_t*)(I2C1_BASE + 0x00);
volatile uint32_t * const i2c_cr2  = (volatile uint32_t*)(I2C1_BASE + 0x04);
volatile uint32_t * const i2c_sr1  = (volatile uint32_t*)(I2C1_BASE + 0x14);
volatile uint32_t * const i2c_sr2  = (volatile uint32_t*)(I2C1_BASE + 0x18);
volatile uint32_t * const i2c_dr   = (volatile uint32_t*)(I2C1_BASE + 0x10);
volatile uint32_t * const i2c_trise = (volatile uint32_t*)(I2C1_BASE + 0x20);
volatile uint32_t * const i2c_ccr = (volatile uint32_t*)(I2C1_BASE + 0x1C);

void I2C_Init(void) {
    // 1. Enable Clocks (RCC)
    // 2. Configure GPIOB Pins (PB6/PB7) for Alternate Function Open-Drain
    // 3. Configure I2C Frequency (36MHz APB1, 100KHz SCL)
    // 4. Create the Mutex
  *abp1 |= (1 << 21);   //enabling clk for i2c1
  *abp2 |= (1 << 3);   //port B enable

  *portB &= ~(15 << 24);
  *portB &= ~(15 << 28);

  *portB |= (15 << 24);
  *portB |= (15 << 28);


  *i2c_cr2 = (36 << 0);  //apb1 operating freq: 36MHZ
  *i2c_ccr = 180;       //ccr value: how to toggle scl
  *i2c_trise = 37;
  *i2c_cr1 |= (1 << 0); //peripheral enable

}

int I2C_Start(uint8_t address, uint8_t direction) {

  *i2c_cr1 |= (1 << 8); //set start bit
  while (!(*i2c_sr1 & (1 << 0)));  // 2. Wait for SB (EV5)
  *i2c_dr = (address << 1) | direction;

   while (!(*i2c_sr1 & (1 << 1)));  //addr bit  set waitn(indicates end of address transmission)
// For 7-bit addressing, the bit is set after the ACK of the byte
// ADDR is not set after a NACK reception
  uint32_t temp = *i2c_sr1;        // Read SR1

  temp = *i2c_sr2;                  //Read SR2
   (void)temp;
    return 0; // Success
}

void I2C_Write(uint8_t data) {
    while (!(*i2c_sr1 & (1 << 7))); // Wait for TxE
    *i2c_dr = data;
    while (!(*i2c_sr1 & (1 << 2))); // Wait for BTF
}

void I2C_Stop(void) {
    *i2c_cr1 |= (1 << 9);
}