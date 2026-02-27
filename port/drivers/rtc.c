#include"rtc.h"


// typedef struct{
//   uint8_t seconds ;
//   uint8_t minutes;
//   uint8_t hours;
//   char *meridiem;
//   char *day;
//   uint8_t date;  // as year would greater than 255
//   uint8_t month;
//   uint16_t year;
// }RTC_time;


uint8_t bcdToDec(uint8_t val);
void RTC_ReadTime(uint8_t data[]);
uint8_t decToBcd(int val) ;



enum Day {
    MONDAY = 1,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
    SUNDAY
};

// RTC_time rtc_data;


void RTC_SetTime(uint8_t sec, uint8_t minutes, uint8_t hour,uint8_t day,uint8_t date,uint8_t month,uint8_t year) {
    I2C_Start(0x68, 0);

    I2C_Write(0x00); // Start at Seconds register
    I2C_Write(decToBcd(sec)); // Bit 7 is 0, so clock starts!
    I2C_Write(decToBcd(minutes));
    I2C_Write(decToBcd(hour));
    I2C_Write(decToBcd(day));
    I2C_Write(decToBcd(date));
    I2C_Write(decToBcd(month));
    I2C_Write(decToBcd(year));


    I2C_Stop();
}

void RTC_ReadTime(uint8_t data[]){
  I2C_Start(0x68, 0);
  I2C_Write(0x00);   //point of first register

  I2C_Stop();

  while (*i2c_sr2 & (1 << 1)); // Wait until the BUSY bit is cleared by hardware

  *i2c_cr1 |= (1 << 10);   //ENABLE ACK BEFORE ReSTART to start reading ---
  I2C_Start(0x68, 1); // Start in Read Mode


  for(int i = 0; i < 6; i++) {
        while (!(*i2c_sr1 & (1 << 6))); // Wait for RxNE
        data[i] = *i2c_dr;
        *i2c_cr1 |= (1 << 10);          // Ensure ACK is still set
    }

    // 6. The "AVR NACK" equivalent for the 7th byte
    *i2c_cr1 &= ~(1 << 10); // Set NACK (Match: TWCR = (1 << TWINT) | (1 << TWEN))
    *i2c_cr1 |= (1 << 9);   // Set STOP

    while (!(*i2c_sr1 & (1 << 6))); // Wait for last byte
    data[6] = *i2c_dr;

    I2C_Stop();

}


void process_rtc_data(sensor_t *dest_struct){
  uint8_t raw[7];
  RTC_ReadTime(raw);


  dest_struct->seconds  =  bcdToDec(raw[0]);
  dest_struct->minutes  =  bcdToDec(raw[1]);
  uint8_t hourData    = (raw[2]);
  dest_struct->hours    = bcdToDec(hourData & 0x1F);

  int dayNumber      =  bcdToDec(raw[3]);
  switch(dayNumber){
    case MONDAY:     dest_struct->day = "Monday";  break;
    case TUESDAY:     dest_struct->day = "Tuesday";  break;
    case WEDNESDAY:   dest_struct->day = "Wednesday";  break;
    case THURSDAY:    dest_struct->day = "Thursday";  break;
    case FRIDAY:      dest_struct->day = "Friday";  break;
    case SATURDAY:    dest_struct->day = "Saturday";  break;
    case SUNDAY:      dest_struct->day = "Sunday";  break;
    default:         dest_struct->day = "Invalid day";  break;  // Handle invalid input (not 1-7)
  }
  dest_struct->date     =  bcdToDec(raw[4]);
  dest_struct->month    =  bcdToDec(raw[5]);
  uint8_t year     =  bcdToDec(raw[6]);
  dest_struct->year= (uint16_t) (year+2000);


}

uint8_t decToBcd(int val) {
  return (uint8_t)( (val/10 << 4) | (val % 10) );
}

uint8_t bcdToDec(uint8_t val) {
  return (uint8_t)( (val/16*10) + (val%16) );
}
