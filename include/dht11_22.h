//
// Driver for DHT11/DHT22 digital humidity and temperature sensor
// pic-libs
//
// Created by Tamas Karpati on 2016-03-13.
//

#ifndef DHT11_22_H
#define	DHT11_22_H

#include "pic-libs-config.h"
#ifdef PIC_LIBS_USE_MODULE_DHT11_22

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
    
typedef enum
{
    DHT11_22_OK,
    DHT11_22_ERR_TIMEOUT,
    DHT11_22_ERR_TEMP_RANGE,
    DHT11_22_ERR_CHECKSUM,
    DHT11_22_ERR_COMM
} dht11_22_result_t;

// Stores the sensor values which are multiplied by 10 to avoid using float
typedef struct
{
    int16_t rel_hum_x10;
    int16_t temp_x10;
} dht11_22_values_t;

// Reads the sensor and puts the result into 'values'
dht11_22_result_t dht11_22_read(dht11_22_values_t* values);

#ifdef	__cplusplus
}
#endif

#endif // PIC_LIBS_USE_MODULE_DHT11_22

#endif // DHT11_22_H

