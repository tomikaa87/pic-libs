//
// Dallas DS18S20/DS18B20 driver
// pic-libs
//
// Created by Tamas Karpati on 2016-03-13.
//

#include "ds18x20.h"
#include "one-wire.h"

// TODO implement selectable resolution
#ifdef DS18X20_HIGH_RES
#   define TEMP_RESOLUTION 12
#else
#   define TEMP_RESOLUTION 9
#endif

#define delay_10us(x) { for (uint8_t _i = 0; _i < x; ++_i) __delay_us(10); }

void ds18x20_do_conversion()
{
    if (one_wire_init() != ONE_WIRE_DEV_CONNECTED)
        return;
    
    one_wire_write_byte(0xcc);
    one_wire_write_byte(0x44);
    
    delay_10us(100);
}

int16_t ds18x20_read_one()
{
    one_wire_init();
    one_wire_write_byte(0xCC);
    one_wire_write_byte(0x44);
    
    delay_10us(20);
    
    one_wire_init();
    one_wire_write_byte(0xCC);
    one_wire_write_byte(0xBE);
    
    uint8_t lo = one_wire_read_byte();
    uint8_t hi = one_wire_read_byte();
    uint16_t raw_temp = hi * 256 + lo;
    if (raw_temp & 0x8000)
        raw_temp = ~raw_temp + 1;
    
    int16_t temp = 0;
    
    uint8_t temp_int = raw_temp >> (TEMP_RESOLUTION - 8);
    temp += temp_int * 100;
    
    uint16_t temp_frac = raw_temp << (4 - (TEMP_RESOLUTION - 8));
    temp_frac &= 0xfff0;
    temp_frac *= 625;
    temp += temp_frac / 100;
    
    if (raw_temp & 0x8000)
        temp *= -1;

    return temp;
}

int16_t ds18x20_read(ds18x20_sensors_t* sensors, uint8_t index)
{
    one_wire_init();
    one_wire_write_byte(0x55);
    for (uint8_t i = 0; i < 8; ++i)
        one_wire_write_byte(sensors->rom_codes[index][i]);       
    one_wire_write_byte(0xbe);
    
    uint8_t lo = one_wire_read_byte();
    uint8_t hi = one_wire_read_byte();
    uint16_t raw_temp = hi * 256 + lo;
    if (raw_temp & 0x8000)
        raw_temp = ~raw_temp + 1;
    
    int16_t temp = 0;
    
    uint8_t temp_int = raw_temp >> (TEMP_RESOLUTION - 8);
    temp += temp_int * 100;
    
    uint16_t temp_frac = raw_temp << (4 - (TEMP_RESOLUTION - 8));
    temp_frac &= 0xfff0;
    temp_frac *= 625;
    temp += temp_frac / 100;
    
    if (raw_temp & 0x8000)
        temp *= -1;

    return temp;
}

ds18x20_result_t ds18x20_enumerate(ds18x20_sensors_t* sensors)
{
    uint16_t path = 0;
    uint8_t sensor_count = 0;

    do
    {
        if (one_wire_init())
        {
            #ifdef DEBUG
            printf("[SRCHDEV] Presence signal error\r\n");
            #endif
            
            return DS18X20_ERR;
        }   
            
        #ifdef DEBUG
        printf("[SRCHDEV] Sending SEARCH ROM command\r\n");
        #endif
        one_wire_write_byte(0xf0);
    
        uint16_t next = 0;
        uint16_t pos = 1;
        uint8_t bit_count = 0;
    
        do
        {
            uint8_t bit1 = one_wire_read_bit();
            uint8_t bit2 = one_wire_read_bit();
            
            if (bit1 == 0 && bit2 == 0)
            {
                if (pos & path)
                    bit1 = 1;
                else
                    next = (path & (pos - 1)) | pos;
                
                pos <<= 1;   
            }    
            
            one_wire_write_bit(bit1);
            
            // Save bit1 as ROM code bit
            sensors->rom_codes[sensor_count][bit_count / 8] |= bit1 << (bit_count % 8);
            
            ++bit_count;
        }
        while (bit_count < 64); 
        
        path = next;
        
        #ifdef DEBUG
        printf("[SRCHDEV] Device found.\r\n"); 
        printf("[SRCHDEV] Device info:\r\n");
        ds1820_print_device_info(rom_bits[sensor_count]);
        #endif
        
        sensor_count++;
    }
    while (path && sensor_count <= DS18X20_MAX_SENSORS);
    
    sensors->sensor_count = sensor_count;
    
    return DS18X20_OK;
}

ds18x20_info_t ds18x20_get_info(ds18x20_sensors_t* sensors, uint8_t index)
{
    ds18x20_info_t info;
    
    switch (sensors->rom_codes[index][0])
    {
        case 0x10:
            info.sensor_type = DS18X20_DS18S20;
            break;
            
        case 0x28:
            info.sensor_type = DS18X20_DS18B20;
            break;
            
        default:
            info.sensor_type = DS18X20_UNKNOWN_SENSOR;
            break;
    }
    
    info.rom_code_crc = sensors->rom_codes[index][7];
    
    return info;
}