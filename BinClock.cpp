#include "BinClock.h"

RTC_DS1307 rtc;

void sendByte (const byte reg, const byte data)
{    
  digitalWrite (SS, LOW);
  SPI.transfer (reg);
  SPI.transfer (data);
  digitalWrite (SS, HIGH); 
}

void DS1302_clock_burst_read(uint8_t *p)
{
  int i;
  _DS1302_start();
  _DS1302_togglewrite(DS1302_CLOCK_BURST_READ, true);
  for (i = 0; i<8; i++)
  {
    *p++ = _DS1302_toggleread();
  }
  _DS1302_stop();
}
void DS1302_clock_burst_write(uint8_t *p)
{
  int i;
  _DS1302_start();
  _DS1302_togglewrite(DS1302_CLOCK_BURST_WRITE, false);

  for (i = 0; i<8; i++)
  {
    _DS1302_togglewrite(*p++, false);
  }
  _DS1302_stop();
}
uint8_t DS1302_read(int address)
{
  uint8_t data;
  bitSet(address, DS1302_READBIT);
  _DS1302_start();
  _DS1302_togglewrite(address, true);
  data = _DS1302_toggleread();
  _DS1302_stop();
  return (data);
}
void DS1302_write(int address, uint8_t data)
{
  // clear lowest bit (read bit) in address
  bitClear(address, DS1302_READBIT);
  _DS1302_start();
  _DS1302_togglewrite(address, false);
  _DS1302_togglewrite(data, false);
  _DS1302_stop();
}
void _DS1302_start(void)
{
  digitalWrite(DS1302_CE_PIN, LOW); // default, not enabled
  pinMode(DS1302_CE_PIN, OUTPUT);
  digitalWrite(DS1302_SCLK_PIN, LOW); // default, clock low
  pinMode(DS1302_SCLK_PIN, OUTPUT);
  pinMode(DS1302_IO_PIN, OUTPUT);
  digitalWrite(DS1302_CE_PIN, HIGH); // start the session
  delayMicroseconds(4);           // tCC = 4us
}
void _DS1302_stop(void)
{
  // Set CE low
  digitalWrite(DS1302_CE_PIN, LOW);
  delayMicroseconds(4);           // tCWH = 4us
}
uint8_t _DS1302_toggleread(void)
{
  uint8_t i, data;

  data = 0;
  for (i = 0; i <= 7; i++)
  {
    digitalWrite(DS1302_SCLK_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(DS1302_SCLK_PIN, LOW);
    delayMicroseconds(1);        // tCL=1000ns, tCDD=800ns

                   // read bit, and set it in place in 'data' variable
    bitWrite(data, i, digitalRead(DS1302_IO_PIN));
  }
  return(data);
}
void _DS1302_togglewrite(uint8_t data, uint8_t release)
{
  int i;

  for (i = 0; i <= 7; i++)
  {
    // set a bit of the data on the I/O-line
    digitalWrite(DS1302_IO_PIN, bitRead(data, i));
    delayMicroseconds(1);     // tDC = 200ns

                  // clock up, data is read by DS1302
    digitalWrite(DS1302_SCLK_PIN, HIGH);
    delayMicroseconds(1);     // tCH = 1000ns, tCDH = 800ns

    if (release && i == 7)
    {
      pinMode(DS1302_IO_PIN, INPUT);
    }
    else
    {
      digitalWrite(DS1302_SCLK_PIN, LOW);
      delayMicroseconds(1);       // tCL=1000ns, tCDD=800ns
    }
  }
}