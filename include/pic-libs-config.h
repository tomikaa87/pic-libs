//
// Module configuration header
// pic-libs
//
// Created by Tamas Karpati on 2016-03-13.
//

#ifndef PIC_LIBS_CONFIG_H
#define	PIC_LIBS_CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <xc.h>

#define PIC_LIBS_USE_MODULE_ONE_WIRE
#define PIC_LIBS_USE_MODULE_DS18X20
#define PIC_LIBS_USE_MODULE_DHT11_22
#define PIC_LIBS_USE_MODULE_LCD
    
#define ONE_WIRE_PIN        (PORTAbits.RA0)
#define ONE_WIRE_TRIS       (TRISAbits.TRISA0)
    
#ifndef _XTAL_FREQ
#   define _XTAL_FREQ   16000000
#endif

#ifdef	__cplusplus
}
#endif

#endif // PIC_LIBS_CONFIG_H

