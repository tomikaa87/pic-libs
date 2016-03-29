// header comes here

#ifndef DS1307_H
#define	DS1307_H

#include "pic-libs-config.h"
#ifdef PIC_LIBS_USE_MODULE_DS1307

#ifndef I2C_READ_FUNC
#   warning "I2C_READ_FUNC must be defined"
#endif

#ifndef I2C_WRITE_FUNC
#   warning "I2C_WRITE_FUNC must be defined"
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum 
{
    DS1307_CTRL_OUT  = 0b10000000,
    DS1307_CTRL_SQWE = 0b00010000,
    DS1307_CTRL_RS1  = 0b00000010,
    DS1307_CTRL_RS0  = 0b00000001 
};

void ds1307_get_date(uint8_t* year, uint8_t* month, uint8_t* day, uint8_t* dow);
void ds1307_get_time(uint8_t* hour, uint8_t* minute, uint8_t* second, uint8_t* pm);
void ds1307_set_date(uint8_t year, uint8_t month, uint8_t day, uint8_t dow);
void ds1307_set_time(uint8_t hour, uint8_t minute, uint8_t second, uint8_t mode_12h, uint8_t pm);

uint8_t ds1307_get_control_reg();
void ds1307_set_control_reg(uint8_t value);

uint8_t ds1307_read_ram(uint8_t offset, uint8_t* buf, uint8_t count);
uint8_t ds1307_write_ram(uint8_t offset, uint8_t* buf, uint8_t count);

#ifdef __cplusplus
}
#endif

#endif // PIC_LIBS_USE_MODULE_DS1307

#endif // DS1307_H

