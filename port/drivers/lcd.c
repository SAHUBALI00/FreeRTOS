#include "lcd.h"
void out_4bit_data(char out_data, uint8_t rs);
void write_data(char wr_data);

void Init_lcd(void)
{

  while (*i2c_sr2 & (1 << 1));
  I2C_Start(0x27, 0); // Open bus for this specific transaction

  out_4bit_data(0x02, 0); // function set: 4bit ,2line , 5*8

  out_4bit_data(0x28, 0); // function set: 4bit ,2line , 5*8

  out_4bit_data(0x0F, 0); // display on ,cursor blinking

  out_4bit_data(0x01, 0); // clear display

  out_4bit_data(0x06, 0); // auto increment after writing 1 char; entry mode

  I2C_Stop();
}

// rs = 0 for command, rs = 1 for data

void out_4bit_data(char out_data, uint8_t rs)
{

  uint8_t upper_nibble = (out_data & 0xF0);

  uint8_t lower_nibble = ((out_data << 4) & 0xF0);

  uint8_t ctrl = (1 << 3) | rs; // Backlight ON, RS bit set

  I2C_Write(upper_nibble | ctrl | (1 << 2));

  I2C_Write(upper_nibble | ctrl);

  I2C_Write(lower_nibble | ctrl | (1 << 2));

  I2C_Write(lower_nibble | ctrl);


}

void write_string(char *ptr)
{

  volatile long count = 0;

  while (*ptr != 0)
  {
    write_data(*ptr); // Write character to the LCD

    ptr++;

    count++;
  }

}
void write_data(char wr_data)
{
  // last 4 bits
  out_4bit_data(wr_data, 1);
}

void set_cursor_second_row(void)
{
  out_4bit_data(0xC0, 0); // Address of the second row in 16x2 LCD(DDRAM)
}

void clearDisplay()
{
  out_4bit_data(0x01, 0); // Clear screen
  vTaskDelay(pdMS_TO_TICKS(2));
}