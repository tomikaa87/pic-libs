//
// Dallas DS18S20/DS18B20 driver
// pic-libs
//
// Created by Tamas Karpati on 2016-03-13.
//

#ifndef DS18X20_H
#define	DS18X20_H

#include "pic-libs-config.h"
#ifdef PIC_LIBS_USE_MODULE_DS18X20

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif
   
typedef uint8_t ds18x20_rom_code_t[8];

typedef enum
{
    DS18X20_OK,
    DS18X20_ERR
} ds18x20_result_t;

typedef enum
{
    DS18X20_DS18S20,
    DS18X20_DS18B20
} ds18x20_sensor_type_t;

typedef struct
{
    ds18x20_sensor_type_t sensor_type;
    uint8_t rom_code_crc;
} ds18x20_info_t;
    
int16_t ds18x20_read_one();
int16_t ds18x20_read(ds18x20_rom_code_t sensor);

// Enumerates all (max. 'count' number of) sensors on the bus. 
// Sensor ROM codes will be put into 'sensors'. 'count' will hold the
// number of sensors found.
ds18x20_result_t ds18x20_enumerate(ds18x20_rom_code_t* sensors, uint8_t* count);

ds18x20_info_t ds18x20_get_info(ds18x20_rom_code_t sensor);
    
#ifdef	__cplusplus
}
#endif

#endif // PIC_LIBS_USE_MODULE_DS18X20

#endif // DS18X20_H

