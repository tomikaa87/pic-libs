//
// Module configuration header
// pic-libs
//
// Created by Tamas Karpati on 2016-03-13.
//

#ifndef PIC_LIBS_CONFIG_H
#define PIC_LIBS_CONFIG_H

#ifdef  __cplusplus
extern "C" {
#endif
    
#include <xc.h>
#include <stdint.h>

#define PIC_LIBS_USE_MODULE_ONE_WIRE
#define PIC_LIBS_USE_MODULE_DS18X20
#define PIC_LIBS_USE_MODULE_DHT11_22
#define PIC_LIBS_USE_MODULE_LCD
    
#define DS18X20_MAX_SENSORS 1
    
#define ONE_WIRE_PIN                   (PORTAbits.RA0)
#define ONE_WIRE_TRIS                  (TRISAbits.TRISA0)

#define DHT11_22_TYPE_DHT22    
#define DHT11_22_USE_POWER_PIN
#define DHT11_22_POWER_PIN             (LATAbits.LATA1)
#define DHT11_22_POWER_TRIS            (TRISAbits.TRISA1)    
#define DHT11_22_DATA_IN               (PORTAbits.RA2)
#define DHT11_22_DATA_OUT              (LATAbits.LATA2)
#define DHT11_22_DATA_TRIS             (TRISAbits.TRISA2)
                                       
#define LCD_4BIT                                                            
#ifdef LCD_4BIT                        
// Pins for 4-bit configuration        
#   define LCD_D4_PORT                 (PORTCbits.RC0) 
#   define LCD_D5_PORT                 (PORTCbits.RC1)
#   define LCD_D6_PORT                 (PORTCbits.RC2)
#   define LCD_D7_PORT                 (PORTCbits.RC3)
#   define LCD_D4_LAT                  (LATCbits.LATC0)
#   define LCD_D5_LAT                  (LATCbits.LATC1)
#   define LCD_D6_LAT                  (LATCbits.LATC2)
#   define LCD_D7_LAT                  (LATCbits.LATC3)
#   define LCD_D4_TRIS                 (TRISCbits.TRISC0)
#   define LCD_D5_TRIS                 (TRISCbits.TRISC1)
#   define LCD_D6_TRIS                 (TRISCbits.TRISC2)
#   define LCD_D7_TRIS                 (TRISCbits.TRISC3)
#else
// Pins and port for 8-bit configuration
#   define LCD_PORT                    (PORTB)
#   define LCD_LAT                     (LATB)
#   define LCD_TRIS                    (TRISB)
#endif                                 
// Control pins                        
#define LCD_RS                         (LATCbits.LATC0)              
#define LCD_RW                         (LATCbits.LATC1)
#define LCD_EN                         (LATCbits.LATC2)
#define LCD_RS_TRIS                    (TRISCbits.TRISC0)              
#define LCD_RW_TRIS                    (TRISCbits.TRISC1)
#define LCD_EN_TRIS                    (TRISCbits.TRISC2)
    
#ifndef _XTAL_FREQ                     
#   define _XTAL_FREQ                  16000000
#endif

typedef uint8_t (*i2c_read_func_t)(uint8_t dev_addr, uint8_t address, uint8_t* buf, uint8_t count);
typedef uint8_t (*i2c_write_func_t)(uint8_t dev_addr, uint8_t address, uint8_t* buf, uint8_t count);
    
#ifdef  __cplusplus
}
#endif

#endif // PIC_LIBS_CONFIG_H

