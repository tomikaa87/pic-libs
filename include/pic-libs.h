//
// Main header for pic-libs
// pic-libs
//
// Created by Tamas Karpati on 2016-03-13.
//

#ifndef PIC_LIBS_H
#define	PIC_LIBS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "pic-libs-config.h"
    
#ifdef PIC_LIBS_USE_MODULE_ONE_WIRE
#   include "one-wire.h"
#endif
    
#ifdef PIC_LIBS_USE_MODULE_DS18X20
#   ifndef PIC_LIBS_USE_MODULE_ONE_WIRE
#       error PIC_LIBS_USE_MODULE_ONE_WIRE must be defined in config header
#   endif
#   include "ds18x20.h"
#endif
    
#ifdef PIC_LIBS_USE_MODULE_DHT11_22
#   include "dht11_22.h"
#endif

#ifdef	__cplusplus
}
#endif

#endif // PIC_LIBS_H

