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
   
//typedef uint8_t ds18x20_rom_code_t[8];

typedef struct
{
    uint8_t rom_codes[DS18X20_MAX_SENSORS][8];
    uint8_t sensor_count;
} ds18x20_sensors_t;

typedef enum
{
    DS18X20_OK,
    DS18X20_ERR
} ds18x20_result_t;

typedef enum
{
    DS18X20_UNKNOWN_SENSOR = 0,
    DS18X20_DS18S20 = 0x10,
    DS18X20_DS18B20 = 0x28
} ds18x20_sensor_type_t;

typedef struct
{
    ds18x20_sensor_type_t sensor_type;
    uint8_t rom_code_crc;
} ds18x20_info_t;

typedef enum
{
    DS18X20_9_BIT = 0,
    DS18X20_10_BIT,
    DS18X20_11_BIT,
    DS18X20_12_BIT
} ds18x20_resolution_t;

// Starts temperature conversion on all sensors.
void ds18x20_do_conversion();

void ds18x20_set_resolution(ds18x20_sensors_t* sensors, uint8_t index, ds18x20_resolution_t resolution);

// Reads the temperature value from the specified sensor.
// The temperature returned is multiplied by 100 to avoid using float value.
int16_t ds18x20_read_mul100(ds18x20_sensors_t* sensors, uint8_t index);

// Reads the temperature value from the specified sensor.
// Using this function will increase the size of the compiled binary
// if you don't use floats anywhere else in the code.
float ds18x20_read(ds18x20_sensors_t* sensors, uint8_t index);

// Enumerates all (max. 'DS18X20_MAX_SENSORS' number of) sensors on the bus. 
// Sensor ROM codes will be put into 'sensors.rom_codes'. 
// 'sensors.sensor_count' will hold the number of sensors found.
ds18x20_result_t ds18x20_enumerate(ds18x20_sensors_t* sensors);

ds18x20_info_t ds18x20_get_info(ds18x20_sensors_t* sensors, uint8_t index);
    
#ifdef	__cplusplus
}
#endif

#endif // PIC_LIBS_USE_MODULE_DS18X20

#endif // DS18X20_H

