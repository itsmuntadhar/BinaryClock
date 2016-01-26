#include <SPI.h>
#include <avr/pgmspace.h>
#include <Wire.h>
#include "RTClib.h"

//clock start
#define DS1302_SCLK_PIN   6    // Arduino pin for the Serial Clock
#define DS1302_IO_PIN     7    // Arduino pin for the Data I/O
#define DS1302_CE_PIN     8    // Arduino pin for the Chip Enable
#define bcd2bin(h,l)    (((h)*10) + (l))
#define bin2bcd_h(x)   ((x)/10)
#define bin2bcd_l(x)    ((x)%10)
#define DS1302_SECONDS           0x80
#define DS1302_MINUTES           0x82
#define DS1302_HOURS             0x84
#define DS1302_DATE              0x86
#define DS1302_MONTH             0x88
#define DS1302_DAY               0x8A
#define DS1302_YEAR              0x8C
#define DS1302_ENABLE            0x8E
#define DS1302_TRICKLE           0x90
#define DS1302_CLOCK_BURST       0xBE
#define DS1302_CLOCK_BURST_WRITE 0xBE
#define DS1302_CLOCK_BURST_READ  0xBF
#define DS1302_RAMSTART          0xC0
#define DS1302_RAMEND            0xFC
#define DS1302_RAM_BURST         0xFE
#define DS1302_RAM_BURST_WRITE   0xFE
#define DS1302_RAM_BURST_READ    0xFF
#define DS1302_D0 0
#define DS1302_D1 1
#define DS1302_D2 2
#define DS1302_D3 3
#define DS1302_D4 4
#define DS1302_D5 5
#define DS1302_D6 6
#define DS1302_D7 7
#define DS1302_READBIT DS1302_D0
typedef struct ds1302_struct
{
  uint8_t Seconds : 4;      // low decimal digit 0-9
  uint8_t Seconds10 : 3;    // high decimal digit 0-5
  uint8_t CH : 1;           // CH = Clock Halt
  uint8_t Minutes : 4;
  uint8_t Minutes10 : 3;
  uint8_t reserved1 : 1;
  union
  {
    struct
    {
      uint8_t Hour : 4;
      uint8_t Hour10 : 2;
      uint8_t reserved2 : 1;
      uint8_t hour_12_24 : 1; // 0 for 24 hour format
    } h24;
    struct
    {
      uint8_t Hour : 4;
      uint8_t Hour10 : 1;
      uint8_t AM_PM : 1;      // 0 for AM, 1 for PM
      uint8_t reserved2 : 1;
      uint8_t hour_12_24 : 1; // 1 for 12 hour format
    } h12;
  };
  uint8_t Date : 4;           // Day of month, 1 = first day
  uint8_t Date10 : 2;
  uint8_t reserved3 : 2;
  uint8_t Month : 4;          // Month, 1 = January
  uint8_t Month10 : 1;
  uint8_t reserved4 : 3;
  uint8_t Day : 3;            // Day of week, 1 = first day (any day)
  uint8_t reserved5 : 5;
  uint8_t Year : 4;           // Year, 0 = year 2000
  uint8_t Year10 : 4;
  uint8_t reserved6 : 7;
  uint8_t WP : 1;             // WP = Write Protect
};

//clock end

const byte MAX7219_REG_NOOP        = 0x0;
const byte MAX7219_REG_DIGIT0      = 0x1;
const byte MAX7219_REG_DIGIT1      = 0x2;
const byte MAX7219_REG_DIGIT2      = 0x3;
const byte MAX7219_REG_DIGIT3      = 0x4;
const byte MAX7219_REG_DIGIT4      = 0x5;
const byte MAX7219_REG_DIGIT5      = 0x6;
const byte MAX7219_REG_DIGIT6      = 0x7;
const byte MAX7219_REG_DIGIT7      = 0x8;
const byte MAX7219_REG_DECODEMODE  = 0x9;
const byte MAX7219_REG_INTENSITY   = 0xA;
const byte MAX7219_REG_SCANLIMIT   = 0xB;
const byte MAX7219_REG_SHUTDOWN    = 0xC;
const byte MAX7219_REG_DISPLAYTEST = 0xF;

void sendByte (const byte reg, const byte data);
void DS1302_clock_burst_read(uint8_t *p);
void DS1302_clock_burst_write(uint8_t *p);
uint8_t DS1302_read(int address);
void DS1302_write(int address, uint8_t data);
void _DS1302_start(void);
void _DS1302_stop(void);
uint8_t _DS1302_toggleread(void);
void _DS1302_togglewrite(uint8_t data, uint8_t release);
