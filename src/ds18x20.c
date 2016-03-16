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

#define DEBUG

#ifdef DEBUG
#include <stdio.h>
void ds18x20_print_info(ds18x20_sensors_t* sensors, uint8_t index);
#endif

#define delay_10us(x) { for (uint8_t _i = 0; _i < x; ++_i) __delay_us(10); }
#define delay_10ms(x) { for (uint8_t _i = 0; _i < x; ++_i) __delay_ms(10); }

// ROM commands
#define CMD_SEARCH_ROM          0xf0
#define CMD_READ_ROM            0x33
#define CMD_MATCH_ROM           0x55
#define CMD_SKIP_ROM            0xcc
#define CMD_ALARM_SEARCH        0xec
// Function commands
#define CMD_CONVERT_T           0x44
#define CMD_WRITE_SCRATCHPAD    0x4e
#define CMD_READ_SCRATCHPAD     0xbe
#define CMD_COPY_SCRATCHPAD     0x48
#define CMD_RECALL_EEPROM       0x48
#define CMD_READ_POWER_SUPPLY   0xb4
// Conversion timing
#define T_CONV_9                94
#define T_CONV_10               188
#define T_CONV_11               375
#define T_CONV_12               750

typedef uint8_t scratchpad_t[8];

// Common parts of the scratchpad
enum
{
    SP_TEMP_LSB = 0,
    SP_TEMP_MSB,
    SP_TEMP_TH,
    SP_TEMP_TL,
    SP_CRC = 8
};

// Scratchpad for DS18B20
enum
{
    B20_SP_CONFIG = 4,
    B20_SP_RESERVED1,
    B20_SP_RESERVED2,
    B20_SP_RESERVED3
};

// Scratchpad for DS18S20
enum
{
    S20_SP_RESERVED1 = 4,
    S20_SP_RESERVED2,
    S20_SP_COUNT_REM,
    S20_SP_COUNT_PER_C
};

// Lasered ROM code indices
enum
{
    RC_FAMILY_CODE = 0,
    RC_SERIAL_1,
    RC_SERIAL_2,
    RC_SERIAL_3,
    RC_SERIAL_4,
    RC_SERIAL_5,
    RC_SERIAL_6,
    RC_CRC
};

static void select_sensor(ds18x20_sensors_t* sensors, uint8_t index)
{
    one_wire_init();
    
    one_wire_write_byte(CMD_MATCH_ROM);
    for (uint8_t i = 0; i < 8; ++i)
        one_wire_write_byte(sensors->rom_codes[index][i]); 
}

static void read_scratchpad(uint8_t* scratchpad)
{    
    one_wire_write_byte(CMD_READ_SCRATCHPAD);
    for (uint8_t i = 0; i < 9; ++i)
        scratchpad[i] = one_wire_read_byte();
}

static void write_scratchpad(uint8_t* scratchpad)
{
    one_wire_write_byte(CMD_WRITE_SCRATCHPAD);
    for (uint8_t i = 0; i < 3; ++i)
        // Only bytes 2, 3 and 4 can be written
        one_wire_write_byte(scratchpad[i + 2]);
}

int16_t convert_celsius_mul100(uint8_t family, uint8_t msb, uint8_t lsb, uint8_t resolution)
{
    int16_t raw = msb * 256 + lsb;
    if (raw & 0x8000)
        raw = ~raw + 1;
    
    int16_t temp = 0;
    
    if (family == 0x28)
    {
        uint8_t mask = ~(0x0f >> (resolution - 8)) & 0x0f;
        int8_t temp_int = raw >> 4;
        uint16_t temp_frac = (raw & mask) * 625;
        temp = temp_int * 100 + temp_frac / 100;
    }
    else if (family == 0x10)
    {
        temp = (raw >> 1) * 100 + ((raw & 1) * 50);
    }
    
    temp *= ((msb & 0x80) > 0 ? -1 : 1);
    
    return temp;
}

float convert_celsius(uint8_t family, uint8_t msb, uint8_t lsb, uint8_t resolution)
{
    int16_t raw = msb * 256 + lsb;
    if (raw & 0x8000)
        raw = ~raw + 1;
    
    float temp = 0;
    
    if (family == 0x28)
    {
        uint8_t mask = ~(0x0f >> (resolution - 8)) & 0x0f;
        int8_t temp_int = raw >> 4;
        uint16_t temp_frac = (raw & mask) * 625;
        temp = temp_int + temp_frac / 10000.0f;
    }
    else if (family == 0x10)
    {
        temp = (raw >> 1) + ((raw & 1) * 0.5f);
    }
    
    temp *= ((msb & 0x80) > 0 ? -1 : 1);
    
    return temp;
}

static uint8_t resolution_from_config(uint8_t config)
{
    return (config >> 5) + 8;
}

void ds18x20_do_conversion()
{
    if (one_wire_init() != ONE_WIRE_DEV_CONNECTED)
        return;
    
#ifdef DEBUG
    printf("Issuing do_conversion\r\n");
#endif
    
    one_wire_write_byte(CMD_SKIP_ROM);
    one_wire_write_byte(CMD_CONVERT_T);
    
    // Since we can have multiple devices on the bus, use the 
    // longest waiting time.
    delay_10ms(T_CONV_12 / 10);
}

void ds18x20_set_resolution(ds18x20_sensors_t* sensors, uint8_t index, ds18x20_resolution_t resolution)
{
    // Only DS18B20 supports resolution configuration
    if (sensors->rom_codes[index][RC_FAMILY_CODE] != DS18X20_DS18B20)
        return;
    
    scratchpad_t scratchpad;
    read_scratchpad(scratchpad);
    
    scratchpad[B20_SP_CONFIG] = 0b01111111 & (resolution << 5);
    write_scratchpad(scratchpad);
}

int16_t ds18x20_read_mul100(ds18x20_sensors_t* sensors, uint8_t index)
{
#ifdef DEBUG
    printf("Reading sensor:\r\n");
    ds18x20_print_info(sensors, index);
#endif
    
    select_sensor(sensors, index);
    
    scratchpad_t scratchpad;
    read_scratchpad(scratchpad);
    
    return convert_celsius_mul100(
            sensors->rom_codes[index][RC_FAMILY_CODE],
            scratchpad[SP_TEMP_MSB],
            scratchpad[SP_TEMP_LSB],
            resolution_from_config(scratchpad[B20_SP_CONFIG]));
}

float ds18x20_read(ds18x20_sensors_t* sensors, uint8_t index)
{
#ifdef DEBUG
    printf("Reading sensor:\r\n");
    ds18x20_print_info(sensors, index);
#endif
    
    select_sensor(sensors, index);
    
    scratchpad_t scratchpad;
    read_scratchpad(scratchpad);
    
    return convert_celsius(
            sensors->rom_codes[index][RC_FAMILY_CODE],
            scratchpad[SP_TEMP_MSB],
            scratchpad[SP_TEMP_LSB],
            resolution_from_config(scratchpad[B20_SP_CONFIG]));
}

ds18x20_result_t ds18x20_enumerate(ds18x20_sensors_t* sensors)
{
    uint16_t path = 0;
    uint8_t sensor_count = 0;
    
    sensors->sensor_count = 0;

    do
    {
        for (uint8_t i = 0; i < 8; ++i)
            sensors->rom_codes[sensor_count][i] = 0;
        
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
        ds18x20_print_info(sensors, sensor_count);
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

#ifdef DEBUG
void ds18x20_print_info(ds18x20_sensors_t* sensors, uint8_t index)
{   
    ds18x20_info_t info = ds18x20_get_info(sensors, index);
    
    // Print device type
    switch (info.sensor_type)
    {
        case DS18X20_DS18S20:
            printf("Device type: DS18S20 (Family: 10)\r\n");
            break;
            
        case DS18X20_DS18B20:
            printf("Device type: DS18B20 (Family: 28)\r\n");
            break;
            
        default:
            printf("Device type: UNKNOWN (Family: %02X)\r\n", sensors->rom_codes[index][0]);
            break; 
    }
    
    // Print serial
    printf("Device serial: ");
    for (uint8_t i = 1; i < 7; ++i)
        printf("%02X ", sensors->rom_codes[index][i]);
    
    // Print CRC
    printf("\r\nDevice ROM data CRC: %02X\r\n", info.rom_code_crc);        
} 
#endif