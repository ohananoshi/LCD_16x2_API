/* Header: lcd_api.h

    Interface to connect in 16x2 LCD displays, based on HD44780

    Author: Guilherme Arruda

    GitHub: https://github.com/ohananoshi/LCD_16x2_API

    Created in: 18 jul 2023

    Last updated: 29 oct 2023
*/

//===================================== HEADERS ========================================

#ifndef _LCD_API_H
    #define _LCD_API_H
#endif

#ifndef _STDINT_H
    #include <stdint.h>
#endif

#ifndef _STDBOOL_H
    #include <stdbool.h>
#endif

#ifndef _INC_STDIO
    #include <stdio.h>
#endif

#ifndef _STDARG_H
    #ifndef _ANSI_STDARG_H_
        #include <stdarg.h>
    #endif
#endif

//============================== COMPATIBILITY ZONE ====================================

//----------------------------------- HEADERS ------------------------------------------

//INCLUDE THE HEADERS ACCORDING TO THE COMPILER

#ifndef _XC_H_
        #ifdef COMPILER_PATH
            #include COMPILER_PATH
        #else
            #include "xc.h"
        #endif
    #define _XTAL_FREQ 4000000
    #define DELAY_US(x) __delay_us(x)
    #define DELAY_MS(x) __delay_ms(x)
#endif

//============================== CONFIGURATION ZONE =====================================

//DEFINE LCD INTERFACE MODE -------------------------------------------------------------

#ifndef LCD_4BIT_INTERFACE
    #ifndef LCD_8BIT_INTERFACE
        #define LCD_4BIT_INTERFACE //STD INTERFACE CONNECTION
    #endif 
#endif

//PINOUT --------------------------------------------------------------------------------

//DEFINE THE PINOUT WILL BE USED

#ifdef LCD_4BIT_INTERFACE
    #ifndef LCD_RS
        #error "RS PIN WAS NOT DEFINED"
    #endif
    #ifndef LCD_EN
        #error "EN PIN WAS NOT DEFINED"
    #endif
    #ifndef LCD_D4
        #error "D4 PIN WAS NOT DEFINED"
    #endif
    #ifndef LCD_D5
        #error "D5 PIN WAS NOT DEFINED"
    #endif
    #ifndef LCD_D6
        #error "D6 PIN WAS NOT DEFINED"
    #endif
    #ifndef LCD_D7
        #error "D7 PIN WAS NOT DEFINED"
    #endif
#endif

#ifdef LCD_8BIT_INTERFACE
    #ifndef LCD_RS
        #error "RS PIN WAS NOT DEFINED"
    #endif
    #ifndef LCD_EN
        #error "EN PIN WAS NOT DEFINED"
    #endif
    #ifndef LCD_D0
        #error "D0 PIN WAS NOT DEFINED"
    #endif
    #ifndef LCD_D1
        #error "D1 PIN WAS NOT DEFINED"
    #endif
    #ifndef LCD_D2
        #error "D2 PIN WAS NOT DEFINED"
    #endif
    #ifndef LCD_D3
        #error "D3 PIN WAS NOT DEFINED"
    #endif
    #ifndef LCD_D4
        #error "D4 PIN WAS NOT DEFINED"
    #endif
    #ifndef LCD_D5
        #error "D5 PIN WAS NOT DEFINED"
    #endif
    #ifndef LCD_D6
        #error "D6 PIN WAS NOT DEFINED"
    #endif
    #ifndef LCD_D7
        #error "D7 PIN WAS NOT DEFINED"
    #endif
#endif

//DEFINE DOT PATTERN MODE

#ifndef CHAR_5x8
    #ifndef CHAR_5x10
        #define CHAR_5x8 //STD DOT PATTERN
    #endif
#endif

//==================================== MACROS ==========================================

//DISPLAY INSTRUCTIONS BIT MAP

//DISPLAY COMMAND BITS

#define COM_CLEAR_DISPLAY 0x01
#define COM_RETURN_HOME 0x02
#define COM_ENTRY_MODE_SET 0x04
#define COM_DISPLAY_CONTROL 0x08
#define COM_CURSOR_DISPLAY_SHIFT 0x10
#define COM_FUNCTION_SET 0x20
#define COM_SET_CGRAM_ADDRESS 0x40
#define COM_SET_DDRAM_ADDRESS 0x80

//ENTRY MODE BITS

#define ENTRY_CURSOR_INCREMENT 0x00
#define ENTRY_CURSOR_DECREMENT 0x02
#define ENTRY_DISPLAY_SHIFT_ON 0x01
#define ENTRY_DISPLAY_SHIFT_OFF 0x00

//DISPLAY ON/OFF CONTROL BITS

#define CTRL_DISPLAY_ON 0x04
#define CTRL_DISPLAY_OFF 0x00
#define CTRL_CURSOR_ON 0x02
#define CTRL_CURSOR_OFF 0x00
#define CTRL_BLINK_ON 0x01
#define CTRL_BLINK_OFF 0x00

//CURSOR OR DISPLAY SHIFT BITS

#define SHIFT_DISPLAY_SHIFT 0x08
#define SHIFT_CURSOR_MOVE 0x00
#define SHIFT_MOVE_RIGHT 0x04
#define SHIFT_MOVE_LEFT 0x00

//FUNCTION SET BITS

#define SET_8BIT_INTERFACE 0x10
#define SET_4BIT_INTERFACE 0x00
#define SET_2LINE_MODE 0x08
#define SET_1LINE_MODE 0x00
#define SET_CHAR_5x10 0x04
#define SET_CHAR_5x8 0x00

//===================================== FUNCTIONS =======================================

//-------------------------------- LOW LEVEL FUNCTIONS ----------------------------------

void send_nibble(uint8_t data){
    LCD_EN = 0;
    LCD_D7 = ((data >> 3) & 0x01);
    LCD_D6 = ((data >> 2) & 0x01);
    LCD_D5 = ((data >> 1) & 0x01);
    LCD_D4 = (data & 0x01);
    LCD_EN = 1;
    DELAY_US(40);	//execution time to write data
    LCD_EN = 0;
}

void send_byte(uint8_t data, bool instruction_mode){
    LCD_RS = !instruction_mode;
    DELAY_US(100);
    
    #ifdef LCD_4BIT_INTERFACE
        send_nibble((data >> 4));
        send_nibble((data & 0x0f));
    #endif
    #ifdef LCD_8BIT_INTERFACE
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
        DELAY_US(40); // execution time to write data
        LCD_EN = 0;
    #endif
}

//------------------------------ COMMOM USER FUNCTIONS ----------------------------------

void lcd_clear(){
    send_byte(COM_CLEAR_DISPLAY, 1);
}

void lcd_home(){
    send_byte(COM_RETURN_HOME, 1);
}

void lcd_shift_setting(uint8_t shift_option, uint8_t direction){
    send_byte(COM_CURSOR_DISPLAY_SHIFT | shift_option | direction, 1);
}

void lcd_entry_setting(uint8_t text_write_direction, uint8_t shift){
    send_byte(COM_ENTRY_MODE_SET | text_write_direction | shift, 1);
}

void lcd_cursor_move(bool line, uint8_t column){
    send_byte(COM_SET_DDRAM_ADDRESS | (line << 6) | column, 1);
}

void lcd_cursor_state(bool on_off){
    if(on_off) send_byte(COM_DISPLAY_CONTROL | CTRL_CURSOR_ON, 1);
    else send_byte(COM_DISPLAY_CONTROL | CTRL_CURSOR_OFF, 1);
}

void lcd_state(bool on_off){
    if(on_off) send_byte(COM_DISPLAY_CONTROL | CTRL_DISPLAY_ON, 1);
    else send_byte(COM_DISPLAY_CONTROL | CTRL_DISPLAY_OFF, 1);
}

void lcd_blink_state(bool on_off){
    if(on_off) send_byte(COM_DISPLAY_CONTROL | CTRL_BLINK_ON, 1);
    else send_byte(COM_DISPLAY_CONTROL | CTRL_BLINK_OFF, 1);
}

void lcd_print(const char* string, ...){
    char output_string[16];
    
    va_list args;
    va_start(args, string);
    
    vsprintf(output_string, string, args);
    va_end(args);
    
    uint8_t i = 0;
    char chr = output_string[i];
    while(chr != '\0'){
        send_byte(chr, 0);
        i++;
        chr = output_string[i];
    }
}

void lcd_print_cg(uint8_t memory_position){
    if(memory_position > 7){
        #warning "MEMORY POSITION VALUE EXCEEDS CGRAM CAPACITY"
    }
    send_byte(memory_position, 0);
}

void lcd_create_char(char* character_pattern, uint8_t memory_position){

    #ifdef CHAR_5x8
        if(memory_position < 8){
            send_byte(COM_SET_CGRAM_ADDRESS | (memory_position << 3), 1);

            for(uint8_t i = 0; i < 8; i++){
                send_byte(character_pattern[i], 0);
            }
        }
    #endif
    #ifdef CHAR_5x10
        if(memory_position < 4){
            send_byte(COM_SET_CGRAM_ADDRESS | (memory_position << 4), 1);
            for(uint8_t i = 0; i < 10; i++){
                send_byte(character_pattern[i], 0);
            }
        }
    #endif
}

//STD INITIALIZATION
void lcd_init(uint8_t lines){
    LCD_RS = 0;
    LCD_D7 = 0;
    LCD_D6 = 0;
    LCD_D5 = 0;
    LCD_D4 = 0;

    #ifdef LCD_8BIT_INTERFACE
        LCD_D3 = 0;
        LCD_D2 = 0;
        LCD_D1 = 0;
        LCD_D0 = 0;
    #endif

    DELAY_MS(50);

    #ifdef LCD_4BIT_INTERFACE
        send_nibble((COM_FUNCTION_SET >> 4) | (SET_8BIT_INTERFACE >> 4));
        DELAY_US(4500);
        send_nibble((COM_FUNCTION_SET >> 4) | (SET_8BIT_INTERFACE >> 4));
        DELAY_US(4500);
        send_nibble((COM_FUNCTION_SET >> 4) | (SET_8BIT_INTERFACE >> 4));
        DELAY_US(4500);
        send_nibble((COM_FUNCTION_SET >> 4) | (SET_4BIT_INTERFACE >> 4));
	#endif

    #ifdef LCD_8BIT_INTERFACE
        send_byte(COM_FUNCTION_SET | SET_8BIT_INTERFACE);
        _DELAY_US(4500);
        send_byte(COM_FUNCTION_SET | SET_8BIT_INTERFACE);
        DELAY_US(150);
        send_byte(COM_FUNCTION_SET | SET_8BIT_INTERFACE);
    #endif

    #ifdef CHAR_5x8
        send_byte(COM_FUNCTION_SET | lines | SET_CHAR_5x8, 1);
    #endif

    #ifdef CHAR_5x10
        send_byte(COM_FUNCTION_SET | lines | SET_CHAR_5x10, 1);
    #endif

    send_byte(COM_DISPLAY_CONTROL | CTRL_DISPLAY_ON | CTRL_CURSOR_OFF | CTRL_BLINK_OFF, 1);
    lcd_clear();
    send_byte(COM_ENTRY_MODE_SET | ENTRY_CURSOR_DECREMENT | ENTRY_DISPLAY_SHIFT_OFF, 1);

    //END INITIALIZATION
}