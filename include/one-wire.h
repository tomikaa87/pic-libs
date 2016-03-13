//
// Dallas 1-wire driver
// pic-libs
//
// Created by Tamas Karpati on 2016-03-13.
//

#ifndef ONE_WIRE_H
#define	ONE_WIRE_H

#include "pic-libs-config.h"
#ifdef PIC_LIBS_USE_MODULE_ONE_WIRE

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif    

typedef enum
{
    ONE_WIRE_DEV_CONNECTED,
    ONE_WIRE_DEV_DISCONNECTED,
    ONE_WIRE_BUS_ERROR
} one_wire_result_t;

one_wire_result_t one_wire_init();

void one_wire_write_bit(uint8_t b);
void one_wire_write_byte(uint8_t b);

uint8_t one_wire_read_bit();
uint8_t one_wire_read_byte();

#ifdef	__cplusplus
}
#endif

#endif // PIC_LIBS_USE_MODULE_ONE_WIRE

#endif // ONE_WIRE_H

