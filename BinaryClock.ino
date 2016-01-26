//Muntadhar Haydar (012516 2242)
//all libraries rights are reserved

//The LED Matrix
//CLK -> 13
//CS -> 10
//DIN -> 11
//RTC Module
//CLK -> 6
//Data IO -> 7
//CS -> 8

#include "BinClock.h"

void setup () 
{
  SPI.begin (); 
  sendByte (MAX7219_REG_SCANLIMIT, 7);   // show all 8 digits
  sendByte (MAX7219_REG_DECODEMODE, 0);  // using an led matrix (not digits)
  sendByte (MAX7219_REG_DISPLAYTEST, 0); // no display test
  for (byte col = 0; col < 8; col++) sendByte (col + 1, 0);
  sendByte (MAX7219_REG_INTENSITY, 3);  // character intensity: range: 0 to 15
  sendByte (MAX7219_REG_SHUTDOWN, 1);   // not in shutdown mode (ie. start it up) 
}

void loop () 
{
  ds1302_struct rtc;
  DS1302_clock_burst_read((uint8_t *)&rtc);
  int hr = bcd2bin(rtc.h24.Hour10, rtc.h24.Hour),
      m = bcd2bin(rtc.Minutes10, rtc.Minutes),
      sec = bcd2bin(rtc.Seconds10, rtc.Seconds);
  int t[6];
  t[0] = sec % 10;
  t[1] = sec / 10;
  t[2] = m % 10;
  t[3] = m / 10;
  t[4] = hr % 10;
  t[5] = hr / 10;
  for (int i = 0; i < 6; i++)
  {
    sendByte(8 - i, t[i]);
  }
  delay(1000);
}
