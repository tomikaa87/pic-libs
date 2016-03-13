//
// Driver for DHT11/DHT22 digital humidity and temperature sensor
// pic-libs
//
// Created by Tamas Karpati on 2016-03-13.
//

#include "dht11_22.h"

#ifdef DHT11_22_USE_POWER_PIN
#   if (!defined(DHT11_22_POWER_PIN) || !defined(DHT11_22_POWER_TRIS))
#       warning "DHT11_22_POWER_PIN and DHT11_22_POWER_TRIS must be defined"
#   endif
#endif

#if (!defined(DHT11_22_DATA_TRIS) || !defined(DHT11_22_DATA_IN) || !defined(DHT11_22_DATA_OUT))
#   warning "DHT11_22_DATA_TRIS, DHT11_22_DATA_IN and DHT11_22_DATA_OUT must be defined"
#endif

#ifdef DHT11_22_TYPE_DHT22
#   define wait_init() { for (uint8_t _i = 0; _i < 150; ++_i) __delay_ms(10); }
#   define wait_req()  __delay_ms(1)
#else
#   define wait_init() { for (uint8_t _i = 0; _i < 20; ++_i) __delay_ms(10); }
#   define wait_req()  __delay_ms(10); __delay_ms(8)
#endif

//#define DHT_USE_RESPONSE_WORKAROUND
//#define DHT_DEBUG

#ifdef DHT_DEBUG
#   include <stdio.h> // printf()
#endif

dht11_22_result_t dht11_22_read(dht11_22_values_t* values)
{
    uint8_t samples[43];
    uint8_t data[5] = { 0xff };

    // Power up the sensor
#ifdef DHT11_22_USE_POWER_PIN
    DHT11_22_POWER_PIN = 1;
    DHT11_22_POWER_TRIS = 0;

     // Wait for the sensor to initialize
    wait_init();
#endif

#ifdef DHT_USE_WEAK_PULL_UP
    DHT_WEAK_PULL_UP = 1;
#endif

    // Send 'Request data' signal, wake up the sensor
    DHT11_22_DATA_OUT = 0;
    DHT11_22_DATA_TRIS = 0;
    wait_req();
    DHT11_22_DATA_TRIS = 1;

#ifdef DHT_USE_RESPONSE_WORKAROUND
    // Skip first two edges of response
    for (uint8_t i = 0; i < 2; ++i) 
    {
        uint8_t counter = 0;
        while (DHT11_22_DATA_IN == 0 && counter++ < 254)
            continue;
#if 0
        if (counter >= 254)
            return DHT11_22_ERR_TIMEOUT;
#endif
        counter = 0;
        while (DHT11_22_DATA_IN == 1 && counter++ < 254)
            continue;
        if (counter >= 254)
            return DHT11_22_ERR_TIMEOUT;
    }
#endif

    // Sample 2+40+1 data bits from the sensor by measuring time between edges.
    for (uint8_t i = 0; i < 43; ++i) 
    {
        uint8_t counter = 0;
        // l->h
        while (DHT11_22_DATA_IN == 0 && counter++ < 254)
            continue;
#if 0
        if (counter >= 254)
            return DHT11_22_ERR_TIMEOUT;
#endif
        // h->l
        counter = 0;
        while (DHT11_22_DATA_IN == 1 && counter++ < 254)
            continue;
        samples[i] = counter;
    }

    // Power down the sensor to save power
#ifdef DHT11_22_USE_POWER_PIN
    DHT11_22_POWER_PIN = 0;
#endif

#ifdef DHT_USE_WEAK_PULL_UP
    DHT_WEAK_PULL_UP = 0;
#endif

#ifdef DHT_DEBUG
    printf("samples:");
    for (uint8_t i = 0; i < 42; ++i)
        printf(" %u", samples[i]);
    printf("\r\n");
#endif

    // Calculate bit threshold time (counter value for logical '1')
    uint8_t bit_time_min = 255;
    uint8_t bit_time_max = 0;
    uint8_t errors = 0;
    for (uint8_t i = 2; i < 42; ++i) 
    {
        uint8_t sample = samples[i];
        
        // Find the smallest value but filter out zeroes
        if (sample < bit_time_min && sample != 0)
            bit_time_min = sample;
        // Find the largest value but filter out 255s
        if (sample > bit_time_max && sample != 255)
            bit_time_max = sample;
        
        if (sample >= 254)
            errors++;
    }

    if (errors > 3)
        return DHT11_22_ERR_TIMEOUT;

    // Calculate threshold between logic '0' and '1'
    uint8_t bit_time_threshold = ((uint16_t)bit_time_min + (uint16_t)bit_time_max) / 2;
    // Decrease the threshold by 33% to get stable readings
    bit_time_threshold -= bit_time_threshold / 3;
    
#ifdef DHT_DEBUG
    printf("bit_time_min = %d, bit_time_max = %d, bit_time_threshold = %d\r\n",
            bit_time_min, bit_time_max, bit_time_threshold);
#endif

    // In case of invalid bit times, stop with communication error
    if (bit_time_min == bit_time_max || bit_time_threshold == 0)
        return DHT11_22_ERR_COMM;

#ifdef DHT_DEBUG
    printf("DHT raw data:");
#endif
    uint8_t temp_range_error = 1;

    // Convert raw data
    for (uint8_t i = 0; i < 5; ++i) 
    {
        // Assemble a data byte from raw bits
        uint8_t data_byte = 0;
        for (uint8_t bit_index = 0; bit_index < 8; ++bit_index) {
            if (samples[i * 8 + bit_index + 2] > bit_time_threshold)
                data_byte |= (1 << (7 - bit_index));
        }

        if (data_byte < 0xff && temp_range_error)
            temp_range_error = 0;

        data[i] = data_byte;
        
#ifdef DHT_DEBUG
        printf(" %02X", data_byte);
#endif
    }
#ifdef DHT_DEBUG
    printf("\r\n");
#endif

    if (temp_range_error)
        return DHT11_22_ERR_TEMP_RANGE;

    // Verify checksum by adding four data bytes together
    uint8_t sum = 0;
    for (uint8_t i = 0; i < 4; ++i)
        sum += data[i];
    if (sum != data[4])
    {
        // DHT11 sometimes cuts the last bit of the data stream.
        // Try again by changing the last checksum bit.
        if ((sum & 0xfe) != data[4]) 
        {
#ifdef DHT_DEBUG
            printf("samples:");
            for (uint8_t i = 0; i < 43; ++i) 
                printf(" %u", samples[i]);
            printf("\r\n");
            printf("bit_time_min = %d, bit_time_max = %d, bit_time_threshold = %d\r\n",
                   bit_time_min, bit_time_max, bit_time_threshold);
#endif
            return DHT11_22_ERR_CHECKSUM;
        }
    }
    
    // Using multiplication instead of bit-shifting saves some ROM space
    values->rel_hum = data[3] * 256 + data[2];
    values->temp = data[1] * 256 + data[0];

    return DHT11_22_OK;
}