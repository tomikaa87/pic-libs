//
// Dallas 1-wire driver
// pic-libs
//
// Created by Tamas Karpati on 2016-03-13.
//

#include "one-wire.h"

#if (!defined(ONE_WIRE_PIN) || !defined(ONE_WIRE_TRIS))
#   warning "ONE_WIRE_PIN and ONE_WIRE_TRIS must be defined"
#endif

#define delay_10us(x) { for (uint8_t i = 0; i < x; ++i) __delay_us(10); }

#define pin_float() { ONE_WIRE_TRIS = 1; }
#define pin_low()   { ONE_WIRE_PIN = 0; ONE_WIRE_TRIS = 0; }
#define pin_high()  { ONE_WIRE_PIN = 1; ONE_WIRE_TRIS = 0; }

one_wire_result_t one_wire_init()
{
    pin_float();
    pin_low();
    delay_10us(60);
    pin_float();
    delay_10us(8);
    uint8_t presence = ONE_WIRE_PIN;
    delay_10us(60);
    uint8_t status = ONE_WIRE_PIN;
    
    if (!status)
        return ONE_WIRE_BUS_ERROR;
    
    if (presence)
        return ONE_WIRE_DEV_DISCONNECTED;
    
    return ONE_WIRE_DEV_CONNECTED;
}

void one_wire_write_bit(uint8_t b)
{
    pin_float();
    pin_low();
    __delay_us(5);
    if (b)
        pin_float();
    delay_10us(6);
    pin_high();
    __delay_us(5);
}

void one_wire_write_byte(uint8_t b)
{
    for (uint8_t i = 0; i < 8; ++i)
    {
        one_wire_write_bit(b & 1);
        b >>= 1;
    }
}

uint8_t one_wire_read_bit()
{
    pin_float();
    pin_low();
    __delay_us(10);
    pin_float();
    __delay_us(10);
    uint8_t data = ONE_WIRE_PIN;
    delay_10us(4);
    
    return data; 
}

uint8_t one_wire_read_byte()
{
    uint8_t data = 0;
    for (uint8_t i = 0; i < 8; ++i)
    {
        if (one_wire_read_bit())
            data |= (1 << i);  
    }   
    
    return data; 
}