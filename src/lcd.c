//
// Driver for HD44780/KS0066 compatible intelligent LCDs
// pic-libs
//
// Created by Tamas Karpati on 2016-03-25.
//

#include "lcd.h"

typedef enum
{
    WRITE_DATA = 0,
    WRITE_COMMAND = 1
} write_mode_t;

static void write(write_mode_t mode, uint8_t value);
static uint8_t read();

#ifdef LCD_4BIT
static void write_nibble(uint8_t value);
static uint8_t read_nibble();
#endif

void lcd_init()
{
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_EN = 0;
    
    LCD_RS_TRIS = 0;
    LCD_RW_TRIS = 0;
    LCD_EN_TRIS = 0;
 
#ifdef LCD_4BIT
    
#else
    
#endif
}

void lcd_clear()
{
}

void lcd_putc(char c)
{
}

char lcd_getc()
{
}

void lcd_puts(const char* s)
{
}

void lcd_goto(uint8_t x, uint8_t y)
{
}

void lcd_shift_screen(lcd_shift_mode_t mode)
{
}

void lcd_shift_cursor(lcd_shift_mode_t mode)
{
}

void lcd_set_entry_mode(lcd_entry_mode_t mode)
{
}

void lcd_set_cursor_mode(lcd_cursor_mode_t mode)
{
}

void lcd_define_custom_char(uint8_t slot, uint8_t* data)
{
}

void lcd_draw_progress_bar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t value, uint8_t first_slot)
{
}

void lcd_draw_custom_progress_bar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t value, uint8_t filled_block_slot, uint8_t empty_block_slot, uint8_t progress_block_slot, uint8_t end_char_slot)
{
}

void lcd_draw_vertical_progress_bar(uint8_t x, uint8_t y, uint8_t value, uint8_t custom_char_slot, uint8_t inverted)
{
}

//static void write(uint8_t cmd, uint8_t value)
//{
//#ifdef LCD_4BIT
//    
//#else
//    
//#endif
//}
//
//static uint8_t read()
//{
//    
//}

#ifdef LCD_4BIT
static void write_nibble(uint8_t value)
{
    LCD_D4_LAT = value & 1;
    LCD_D5_LAT = (value & 0b10) >> 1;
    LCD_D6_LAT = (value & 0b100) >> 2;
    LCD_D7_LAT = (value & 0b1000) >> 3;
    
    LCD_D4_TRIS = 0;
    LCD_D5_TRIS = 0;
    LCD_D6_TRIS = 0;
    LCD_D7_TRIS = 0;
    
    __delay(1);
    LCD_EN = 1;
    __delay_us(10);
    LCD_EN = 0;
    __delay_us(10);
}

static uint8_t read_nibble()
{
    
}
#endif
