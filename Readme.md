### LCD API

#### C interface to connect in 16x2 LCD displays, based on HD44780. This API aims to work on any microcontroller and with any compiler as long as compatibility adjustments are made. 

### Headers
- stdint.h
- stdbool.h
- stdlib.h
- stdio.h
- stdarg.h

### Functions
- send_nibble

    ```C
    void send_nibble(uint8_t data)
    ```
    ##### Sends the last 4 most important bits of a byte. (0000 `0000`)
- send_byte

    ```C
    void send_byte(uint8_t data, bool instruction_mode)
    ```
    ##### Sends one byte of data according to the communication interface.
    ```C
    instruction mode == 0  Send a character
    instruction mode == 1  Send a instruction
    ```
- lcd_clear

    ```C
    void lcd_clear()
    ```
    ##### Display clear
- lcd_move_cursor

    ```C
    void lcd_move_cursor(bool line, uint8_t column)
    ```
    ##### Moves the cursor to specified line and column.
- lcd_print

    ```C
    void lcd_print(const char* string, ...)
    ```
    ##### Print formated string like _printf_ does.
- lcd_init

    ```C
    void lcd_init(uint8_t lines, uint8_t char_size)
    ```
    ##### Initialize display by function.
    ```C
    lines: 
        Use SET_1LINE_MODE or SET_2LINE_MODE
    char_size:
        Use SET_CHAR_5x10 or SET_CHAR_5x8
    ```