/* Header: lcd_api.h

    Interface to connect in 16x2 LCD displays, based on HD44780

    Author: Guilherme Arruda

    GitHub: https://github.com/ohananoshi/LCD_16x2_API

    Created in: 18 jul 2023

    Last updated: 25 jul 2023
*/

//===================================== HEADERS ========================================

//#include <xc.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

//==================================== MACROS ==========================================

//#define _XTAL_FREQ 4000000

//VCC modes

#define VCC_5 0
#define VCC_2_7 1

//Inteface modes

#define I4_BIT 0
#define I8_BIT 1
#define INTERFACE_MODE I4_BIT

//Display lines

#define ONE_LINE 0
#define TWO_LINES 1

//Font modes

#define FONT_5_8 0
#define FONT_5_10 1

//Display settings

#define DISPLAY_ON 1
#define DISPLAY_OFF 0

#define CURSOR_ON 1
#define CURSOR_OFF 0

#define CURSOR_BLINK_ON 1
#define CURSOR_BLINK_OFF 0

#define CURSOR_MOVE_RIGHT 1
#define CURSOR_MOVE_LEFT 0

#define CURSOR_FOLLOW 1
#define CURSOR_NOT_FOLLOW 0

//Define the pinout you will use

#define LCD_RS PORTDbits.RD0                                                            // (0) para comandos (1) para dados
#define LCD_EN PORTDbits.RD1
#define LCD_RW 
#define LCD_D0 
#define LCD_D1 
#define LCD_D2 
#define LCD_D3 
#define LCD_D4 PORTDbits.RD4
#define LCD_D5 PORTDbits.RD5
#define LCD_D6 PORTDbits.RD6
#define LCD_D7 PORTDbits.RD7

//===================================== MACRO FUNCTIONS ================================


//===================================== DATATYPES =======================================

//===================================== FUNCTIONS =======================================

/*void busy_wait(){
    LCD_RW = 1;
    while(LCD_D7 != 0);
    LCD_RW = 0;
}*/

void lcd_send_nibble(uint8_t data){
    LCD_EN = 0;
    LCD_D7 = (data & 0x08);
    LCD_D6 = (data & 0x04);
    LCD_D5 = (data & 0x02);
    LCD_D4 = (data & 0x01);
    LCD_EN = 1;
    __delay_us(37);	//execution time to write data
    LCD_EN = 0;
}

void lcd_send_byte(uint8_t data, bool is_instruction){
    LCD_RS = !is_instruction;
    __delay_us(100);
    
    #if INTERFACE_MODE == I4_BIT
        lcd_send_nibble((data >> 4));
        lcd_send_nibble((data & 0x0f));
    #elif INTERFACE_MODE == I8_BIT
        LCD_EN = 0;
        LCD_D7 = (data & 0x80);
        LCD_D6 = (data & 0x40);
        LCD_D5 = (data & 0x20);
        LCD_D4 = (data & 0x10);
        LCD_D3 = (data & 0x08);
        LCD_D2 = (data & 0x04);
        LCD_D1 = (data & 0x02);
        LCD_D0 = (data & 0x01);
        LCD_EN = 1;
        __delay_us(37); // execution time to write data
        LCD_EN = 0;
    #else
        #error "INTERFACE MODE ERROR: MUST BE I4_BIT OR I8_BIT"
    #endif
}

void lcd_clear(){
    LCD_RS = 0;
    //LCD_RW = 0;
    lcd_send_byte(0x01, 1);
}

void lcd_move_cursor(bool line, uint8_t column){
    uint8_t data = 0x80;
    data |= (line << 6) + column;
    lcd_send_byte(data, 1);
}

void lcd_print(const char* string, ...){
    char* output_string;
    
    va_list args;
    va_start(args, string);
    
    vsprintf(output_string, string, args);
    va_end(args);
    
    uint8_t i = 0;
    char chr = output_string[i];
    while(chr != '\0'){
        lcd_send_byte(chr, 0);
        i++;
        chr = output_string[i];
    }
}

void lcd_settings(bool display_on, bool cursor_on, bool cursor_blink, bool cursor_move_direction, bool cursor_follow_display_shift){
    //DISPLAY ON/OFF CONTROL
    uint8_t data = 0x08;
    data |= (display_on << 6);
    data |= (cursor_on << 5);
    data |= (cursor_blink << 4);
    lcd_send_byte(data, 1);
    
    __delay_ms(1);
    //busy_wait();
    
    //ENTRY MODE SET
    data = 0x04;
    data |= (cursor_move_direction <<  1);
    data |= (cursor_follow_display_shift);
    lcd_send_byte(data, 1);
}

void lcd_init(bool data_interface_mode, bool display_lines, bool font_mode, bool voltage_mode, bool std_settings, ...){
    LCD_RS = 0;
    //LCD_RW = 0;
    LCD_D7 = 0;
    LCD_D6 = 0;
    LCD_D5 = 0;
    LCD_D5 = 0;
    
    if(voltage_mode == VCC_2_7) __delay_ms(50);
    else __delay_ms(20);
    
    if(data_interface_mode == I4_BIT){
	uint8_t data = 0;

        lcd_send_nibble(0x03);
        __delay_ms(5);
        lcd_send_nibble(0x03);
        __delay_us(150);
        lcd_send_nibble(0x03);
        __delay_us(10);
	
	//busy_wait();
	
        lcd_send_nibble(0x02);
        __delay_us(40);
	
	data = 0x20;
	data |= ((data_interface_mode << 4) + (display_lines << 3) + (font_mode << 2));
	
	lcd_send_byte(data, 1);
	
	if(std_settings){
	    lcd_settings(DISPLAY_OFF, CURSOR_OFF, CURSOR_BLINK_OFF, CURSOR_MOVE_RIGHT, CURSOR_FOLLOW);
	    lcd_clear();
	}else{
	    va_list settings;
	    va_start(settings, std_settings);
	    
	    bool display_status = va_arg(settings, bool);
	    bool cursor_status = va_arg(settings, bool);
	    bool cursor_blink = va_arg(settings, bool);
	    bool cursor_direction = va_arg(settings, bool);
	    bool cursor_follow = va_arg(settings, bool);
	    
	    va_end(settings);
	    
	    lcd_settings(display_status, cursor_status, cursor_blink, cursor_direction, cursor_follow);
	    lcd_clear();
	}
    }else{
        uint8_t data = 0;

        lcd_send_byte(0x30, 1);
        __delay_ms(5);
        lcd_send_byte(0x30, 1);
        __delay_us(150);
        lcd_send_byte(0x30, 1);
        __delay_us(10);
	
	//busy_wait();
    __delay_ms(1);
	
	data = 0x30;
	data |= ((display_lines << 3) + (font_mode << 2));
	
	lcd_send_byte(data, 1);
	
	if(std_settings){
	    lcd_settings(DISPLAY_OFF, CURSOR_OFF, CURSOR_BLINK_OFF, CURSOR_MOVE_RIGHT, CURSOR_FOLLOW);
	    lcd_clear();
	}else{
	    va_list settings;
	    va_start(settings, std_settings);
	    
	    bool display_status = va_arg(settings, bool);
	    bool cursor_status = va_arg(settings, bool);
	    bool cursor_blink = va_arg(settings, bool);
	    bool cursor_direction = va_arg(settings, bool);
	    bool cursor_follow = va_arg(settings, bool);
	    
	    va_end(settings);
	    
	    lcd_settings(display_status, cursor_status, cursor_blink, cursor_direction, cursor_follow);
	    lcd_clear();
        }
    }//end initialization
}

