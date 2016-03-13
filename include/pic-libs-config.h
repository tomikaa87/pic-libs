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

#define DHT11_22_TYPE_DHT22    
#define DHT11_22_USE_POWER_PIN
#define DHT11_22_POWER_PIN  (LATAbits.LATA1)
#define DHT11_22_POWER_TRIS (TRISAbits.TRISA1)    
#define DHT11_22_DATA_IN    (PORTAbits.RA2)
#define DHT11_22_DATA_OUT   (LATAbits.LATA2)
#define DHT11_22_DATA_TRIS  (TRISAbits.TRISA2)
    
#ifndef _XTAL_FREQ
#   define _XTAL_FREQ   16000000
#endif

#ifdef	__cplusplus
}
#endif

#endif // PIC_LIBS_CONFIG_H

