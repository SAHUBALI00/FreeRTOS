#include <stdint.h>
// typedef unsigned char uint8_t;
// typedef unsigned short uint16_t;
// typedef unsigned int uint32_t;
// typedef unsigned long uint64_t;
#define RCC_base_addr         0x40021000

#define APB1_offset           0x1C

#define APB2_offset           0x18

#define PORTB_base_addr       0x40010C00

#define GPIOB_CRL_offset      0x00

#define I2C_base_addr        0x40005400

#define I2C_CR1               0x00

#define I2C_CR2               0x04

#define I2C_SR1               0x14

#define I2C_SR2               0x18

#define I2C_CCR               0x1C

#define I2C_TRISE             0x20

#define I2C_DR                0x10



void I2C_START(void);

void config_I2C(void);

void I2C_Write(uint8_t data);

void I2C_Stop(void);

void init_lcd(void);

void out_4bit_data(char out_data, uint8_t rs);

void write_data(char wr_data);

void write_string(char *ptr);

void set_cursor_second_row(void);





volatile uint32_t * const apb2_reg =   (volatile uint32_t*)(RCC_base_addr + APB2_offset);

volatile uint32_t * const apb1_reg =   (volatile uint32_t*)(RCC_base_addr + APB1_offset);

volatile uint32_t * const mode_reg =   (volatile uint32_t*)(PORTB_base_addr + GPIOB_CRL_offset);

volatile uint32_t * const i2c_cr1 =    (volatile uint32_t*)(I2C_base_addr + I2C_CR1);

volatile uint32_t * const i2c_cr2 =    (volatile uint32_t*)(I2C_base_addr + I2C_CR2);

volatile uint32_t * const i2c_ccr =    (volatile uint32_t*)(I2C_base_addr + I2C_CCR);

volatile uint32_t * const i2c_trise =  (volatile uint32_t*)(I2C_base_addr + I2C_TRISE);

volatile uint32_t * const i2c_dr =     (volatile uint32_t*)(I2C_base_addr + I2C_DR);

volatile uint32_t * const i2c_sr1 =    (volatile uint32_t*)(I2C_base_addr + I2C_SR1);

volatile uint32_t * const i2c_sr2 =    (volatile uint32_t*)(I2C_base_addr + I2C_SR2);



void setup() {



  config_I2C();

  I2C_START();

  init_lcd();

  write_string("yeah 4bit mode worked");


}





void config_I2C(void) {



  *apb2_reg |= 1 << 3;         //enabling clock for portB

  *apb1_reg |= 1 << 21;         //enabling clock for i2c1





  *mode_reg &= ~(15 << 24);   // clear the four config bits of port b6

  *mode_reg &= ~(15 << 28);   //clear the bits for port b7



  *mode_reg |= (15 << 24);  //1111 to b6

  *mode_reg |= (15 << 28); // 1111 to b7



  *i2c_cr2 = (36 << 0);  //apb1 operating freq: 36MHZ

  *i2c_ccr = 180;       //ccr value: how to toggle scl

  *i2c_trise = 37;

  *i2c_cr1 |= (1 << 0); //peripheral enable

}





void I2C_START() {

  *i2c_cr1 |= (1 << 8); //set start bit

  while (!(*i2c_sr1 & (1 << 0)));  // 2. Wait for SB (EV5)

  *i2c_dr = (0x27 << 1);  //address + write bit(0)

  while (!(*i2c_sr1 & (1 << 1)));  // 4. Wait for ADDR (EV6)

  uint32_t temp = *i2c_sr1;        // Read SR1

  temp = *i2c_sr2;                  //Read SR2

  (void)temp;                //prevent unused variable warning

}



void I2C_Write(uint8_t data) {

  while (!(*i2c_sr1 & (1 << 7)));  // Wait for TxE (Transmit Empty)

  *i2c_dr = data;

  while (!(*i2c_sr1 & (1 << 2)));  // Wait for BTF (Byte Transfer Finished)

}



void I2C_Stop(void) {

  *i2c_cr1 |= (1 << 9);            // Generate STOP

}











//********************

void init_lcd(void){

  out_4bit_data(0x02,0);  //function set: 4bit ,2line , 5*8

  out_4bit_data(0x28,0);  //function set: 4bit ,2line , 5*8

  out_4bit_data(0x0F,0);   //display on ,cursor blinking

  out_4bit_data(0x01,0);  //clear display

  out_4bit_data(0x06,0);   //auto increment after writing 1 char; entry mode

}

// rs = 0 for command, rs = 1 for data

void out_4bit_data(char out_data, uint8_t rs){

  uint8_t upper_nibble = (out_data & 0xF0);

  uint8_t lower_nibble = ((out_data << 4) & 0xF0);

  uint8_t ctrl = (1 << 3) | rs; // Backlight ON, RS bit set



  I2C_Write(upper_nibble | ctrl | (1 << 2));;

  I2C_Write(upper_nibble | ctrl);

  I2C_Write(lower_nibble | ctrl | (1 << 2));

  I2C_Write(lower_nibble | ctrl);



  // If it was the 'Clear' command (0x01), wait longer

    // if (out_data == 0x01) delay(2);

    // else delay(1);

}

void write_string(char *ptr) {

  volatile long count=0;

  while (*ptr != 0) {

    if(count==16){

      set_cursor_second_row();

    }

    write_data(*ptr);                // Write character to the LCD

    ptr++;

    count++;



  }

}

void write_data(char wr_data){

  //last 4 bits

  out_4bit_data(wr_data,1);

}

void set_cursor_second_row(void) {
  out_4bit_data(0xC0,0);  // Address of the second row in 16x2 LCD(DDRAM)

}

void loop(){}