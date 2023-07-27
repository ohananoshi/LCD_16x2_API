/* Header: lcd_api.h

    Interface to connect in 16x2 LCD displays, based on HD44780

    Author: Guilherme Arruda

    GitHub: https://github.com/ohananoshi/LCD_16x2_API

    Created in: 18 jul 2023

    Last updated: 26 jul 2023
*/

//===================================== HEADERS ========================================

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

//============================== COMPATIBILITY ZONE ====================================

//PIC18F4550 WITH XC8 COMPILER EXAMPLE

//----------------------------------- HEADERS ------------------------------------------

//INCLUDE THE HEADERS ACCORDING TO THE MICROCONTROLLER AND COMPILER
#include <xc.h>

//----------------------------- DELAY FUNCTIONS ----------------------------------------

//DEFINE THE DELAY FUNCTION ACCORDING TO THE COMPILER 

//NECESSARY TO DELAY IN XC8 COMPILLER
#define _XTAL_FREQ 4000000

#define DELAY_US(x) __delay_us(x)
#define DELAY_MS(x) __delay_ms(x)

//-------------------------------- PINOUT ----------------------------------------------

//DEFINE THE PINOUT YOU WILL USE

#define LCD_RS PORTDbits.RD0
#define LCD_EN PORTDbits.RD1
#define LCD_D4 PORTDbits.RD4
#define LCD_D5 PORTDbits.RD5
#define LCD_D6 PORTDbits.RD6
#define LCD_D7 PORTDbits.RD7

//============================= END OF COMPATIBILITY ZONE ==============================

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

//DEFINE LCD INTERFACE MODE -------------------------------------------------------------

#define LCD_4BIT_INTERFACE
//#define LCD_8BIT_INTERFACE

//===================================== FUNCTIONS =======================================

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
    #ifndef LCD_4BIT_INTERFACE
        #ifndef LCD_8BIT_INTERFACE
            #error "LCD INTERFACE MODE NOT DEFINED"
        #endif
    #endif
}

void lcd_clear(){
    send_byte(COM_CLEAR_DISPLAY, 1);
}

void lcd_move_cursor(bool line, uint8_t column){
    send_byte(COM_SET_DDRAM_ADDRESS | (line << 6) | column, 1);
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

bool lcd_create_char(char* character_pattern, uint8_t memory_position uint8_t dot_pattern){

    if(dot_pattern == SET_CHAR_5x8){
        if(memory_position < 8){
            send_byte(COM_SET_CGRAM_ADDRESS | (memory_position << 3), 1);

            for(uint8_t i = 0; i < 8; i++){
                send_byte(character_pattern[i], 0);
            }
        }
    }
    if(dot_pattern == SET_CHAR_5x10){
        if(memory_position < 4){
            send_byte(COM_SET_CGRAM_ADDRESS | (memory_position << 4), 1);
            for(uint8_t i = 0; i < 10; i++){
                send_byte(character_pattern[i], 0);
            }
        }
    }
}

void lcd_init(uint8_t lines, uint8_t char_size){
    LCD_RS = 0;
    LCD_D7 = 0;
    LCD_D6 = 0;
    LCD_D5 = 0;
    LCD_D4 = 0;

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
    #ifndef LCD_4BIT_INTERFACE
        #ifndef LCD_8BIT_INTERFACE
            #error "LCD INTERFACE MODE NOT DEFINED"
        #endif
    #endif

    send_byte(COM_FUNCTION_SET | lines | char_size, 1);
    send_byte(COM_DISPLAY_CONTROL | CTRL_DISPLAY_ON | CTRL_CURSOR_OFF | CTRL_BLINK_OFF, 1);
    lcd_clear();
    send_byte(COM_ENTRY_MODE_SET | ENTRY_CURSOR_DECREMENT | ENTRY_DISPLAY_SHIFT_OFF, 1);

    //END INITIALIZATION
}