//
// Driver for DS1307 64x8, Serial, I2C Real-Time Clock
// pic-libs
//
// Created by Tamas Karpati on 2016-03-29.
//

#include "ds1307.h"

typedef enum 
{
    //               ADDR      BIT 7 | BIT 6 | BIT 5 | BIT 4 | BIT 3 | BIT 2 | BIT 1 | BIT 0 | FUNCTION | RANGE 
    REG_SECONDS_CH = 0x00,  // CH    | 10 Seconds            | Seconds                       | Seconds  | 00-59     
    REG_MINUTES    = 0x01,  // 0     | 10 Minutes            | Minutes                       | Minutes  | 00-59
    REG_HOURS      = 0x02,  // 0     | 12 h  | AM/PM | 10 h  | Hours                         | Hours    | 1-12 + AM/PM
                            // 0     | 24 h  | 10 h          | Hours                         | Hours    | 00-23
    REG_DAY        = 0x03,  // 0                                     | Day                   | Day      | 01-07
    REG_DATE       = 0x04,  // 0             | 10 Date       | Date                          | Date     | 01-31
    REG_MONTH      = 0x05,  // 0                     | 10 M  | Month                         | Month    | 01-12
    REG_YEAR       = 0x06,  // 10 Year                       | Year                          | Year     | 00-99
    REG_CONTROL    = 0x07,  // OUT   | 0     | 0     | SQWE  | 0     | 0     | RS1   | RS0   | Control  | --
    REG_RAM_BASE   = 0x08,
    REG_RAM_END    = 0x3f
} reg_addr_t;

enum 
{
    REG_MASK_CH          = 0b10000000,
    REG_MASK_SECONDS     = 0b01111111,
    REG_MASK_12_24h      = 0b01000000,
    REG_MASK_AM_PM       = 0b00100000,
    REG_MASK_HOUR_12h    = 0b00011111,
    REG_MASK_HOUR_24h    = 0b00111111
}

enum
{
    DEVICE_ADDR  = 0b11010000
};

static ds1307_config_t _config;

static uint8_t read_reg(reg_addr_t address)
{
    uint8_t buf;
    _config.i2c_read_func(DEVICE_ADDR, address, &buf, 1);
    return buf;
}

static void write_reg(reg_addr_t address, uint8_t value)
{
    _config.i2c_write_func(DEVICE_ADDR, address, &value, 1);
}

static uint8_t bcd_to_dec(uint8_t bcd)
{
    return (bcd >> 4) * 10 + (bcd & 0x0f);
}

static uint8_t dec_to_bcd(uint8_t dec)
{
    return ((dec % 10) << 4) | (dec / 10);
}

void ds1307_set_config(ds1307_config_t config)
{
    _config = config;
}

void ds1307_get_date(uint8_t* year, uint8_t* month, uint8_t* day, uint8_t* dow)
{
    uint8_t buf[4];
    _config.i2c_read_func(DEVICE_ADDR, REG_DAY, buf, sizeof (buf));
    
    *dow = buf[0];
    *day = bcd_to_dec(buf[1]);
    *month = bcd_to_dec(buf[2]);
    *year = bcd_to_dec(buf[3]);
}

void ds1307_get_time(uint8_t* hour, uint8_t* minute, uint8_t* second, uint8_t* pm)
{
    uint8_t buf[3];
    _config.i2c_read_func(DEVICE_ADDR, REG_SECONDS_CH, buf, sizeof (buf));
    
    *second = bcd_to_dec(buf[0] & REG_MASK_SECONDS);
    *minute = bcd_to_dec(buf[1]);
    
    uint8_t hour_reg = buf[2];
    
    if (hour_reg & REG_MASK_12_24h)
    {
        // 12-hour mode
        if (pm != NULL)
            *pm = hour_reg & REG_MASK_AM_PM ? 1 : 0;
        *hour = bcd_to_dec(hour_reg & REG_MASK_HOUR_12h);
    }
    else
    {
        // 24-hour mode
        *hour = bcd_to_dec(hour_reg & REG_MASK_HOUR_24h);
    }
}

void ds1307_set_date(uint8_t year, uint8_t month, uint8_t day, uint8_t dow)
{
    uint8_t buf[4];
    buf[0] = dec_to_bcd(dow);    
    buf[1] = dec_to_bcd(day);
    buf[2] = dec_to_bcd(month);
    buf[3] = dec_to_bcd(year);
    
    _config.i2c_write_func(DEVICE_ADDR, REG_DAY, buf, sizeof (buf));
}

void ds1307_set_time(uint8_t hour, uint8_t minute, uint8_t second, uint8_t mode_12h, uint8_t pm)
{
    uint8_t buf[3];
    buf[0] = dec_to_bcd(second) & REG_MASK_SECONDS;
    buf[1] = dec_to_bcd(minute);
    
    if (mode_12h)
        buf[2] = (dec_to_bcd(hour) & REG_MASK_HOUR_12h) | REG_MASK_12_24h | (pm ? REG_MASK_AM_PM : 0);
    else
        buf[2] = dec_to_bcd(hour) & REG_MASK_HOUR_24h;
    
    _config.i2c_write_func(DEVICE_ADDR, REG_SECONDS_CH, buf, sizeof (buf));
}

uint8_t ds1307_get_control_reg()
{
    uint8_t buf;
    _config.i2c_read_func(DEVICE_ADDR, REG_CONTROL, &buf, 1);
    return buf;
}

void ds1307_set_control_reg(uint8_t value)
{
    _config.i2c_write_func(DEVICE_ADDR, REG_CONTROL, &value, 1);
}

uint8_t ds1307_read_ram(uint8_t offset, uint8_t* buf, uint8_t count)
{   
    uint8_t read_count = max(0, min(count, REG_RAM_END - REG_RAM_BASE - offset));
    if (read_count == 0)
        return 0;
    
    _config.i2c_read_func(DEVICE_ADDR, REG_RAM_BASE + offset, buf, read_count);
    
    return read_count;
}

uint8_t ds1307_write_ram(uint8_t offset, uint8_t* buf, uint8_t count)
{
    uint8_t write_count = max(0, min(count, REG_RAM_END - REG_RAM_BASE - offset));
    if (write_count == 0)
        return 0;
    
    _config.i2c_write_func(DEVICE_ADDR, REG_RAM_BASE + offset, buf, write_count);
    
    return write_count;
}