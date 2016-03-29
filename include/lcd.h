//
// Driver for HD44780/KS0066 compatible intelligent LCDs
// pic-libs
//
// Created by Tamas Karpati on 2016-03-13.
//

#ifndef LCD_H
#define	LCD_H

#include "pic-libs-config.h"
#ifdef PIC_LIBS_USE_MODULE_LCD

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    LCD_DEC_NOSHIFT = 0x04,
    LCD_DEC_SHIFT = 0x05,
    LCD_INC_NOSHIFT = 0x06,
    LCD_INC_SHIFT = 0x07
} lcd_entry_mode_t;

typedef enum
{
    LCD_OFF = 0x0c,
    LCD_BLINK = 0x0d,
    LCD_UNDERLINE = 0x0e
} lcd_cursor_mode_t;

typedef enum
{
    LCD_LEFT = 0,
    LCD_RIGHT
} lcd_shift_mode_t;

void lcd_init();
void lcd_clear();

void lcd_putc(char c);
char lcd_getc();

void lcd_puts(const char* s);

void lcd_goto(uint8_t x, uint8_t y);

void lcd_shift_screen(lcd_shift_mode_t mode);
void lcd_shift_cursor(lcd_shift_mode_t mode);

void lcd_set_entry_mode(lcd_entry_mode_t mode);
void lcd_set_cursor_mode(lcd_cursor_mode_t mode);

void lcd_define_custom_char(uint8_t slot, uint8_t* data);

void lcd_draw_progress_bar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t value, uint8_t first_slot);
void lcd_draw_custom_progress_bar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t value, uint8_t filled_block_slot, uint8_t empty_block_slot, uint8_t progress_block_slot, uint8_t end_char_slot);
void lcd_draw_vertical_progress_bar(uint8_t x, uint8_t y, uint8_t value, uint8_t custom_char_slot, uint8_t inverted);

#ifdef __cplusplus
}
#endif

#endif // PIC_LIBS_USE_MODULE_LCD

#endif // LCD_H

