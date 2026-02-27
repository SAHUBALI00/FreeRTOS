#ifndef lcd_H
#define lcd_H
#include "../utilities/i2c_utility.h"

void Init_lcd(void);
void write_string(char *ptr);
void set_cursor_second_row(void);
void clearDisplay(void);


#endif
