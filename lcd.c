#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include "lcd.h"

volatile uint8_t *port[4] = {
   &DATA_4P,
   &DATA_5P,
   &DATA_6P,
   &DATA_7P
};

const uint8_t pin[4] = {
    DATA_4,
    DATA_5,
    DATA_6,
    DATA_7
};

void _pulse_enable(void){
    
    EN_P &= ~EN;
    _delay_us(100);
    EN_P |= EN;
    _delay_us(100);
    EN_P &= ~EN;
    _delay_us(100);
}

void _set_pin(volatile uint8_t *port, const uint8_t pin, uint8_t value){
    if(value)
        *port |= pin;
    else
        *port &= ~pin;
}

void _write_byte(uint8_t to_write){

    uint8_t bottom = to_write & 0x0F;
    uint8_t top = (to_write >> 4) & 0x0F;
    
    int i;

    for(i=0; i < 4; i++)
        _set_pin(port[i], pin[i], (top >> i & 0x01));
    _pulse_enable();

    for(i=0; i < 4; i++)
        _set_pin(port[i], pin[i], (bottom >> i & 0x01));
    _pulse_enable();

}

void _write_nibble(uint8_t to_write){

    uint8_t top = (to_write >> 4) & 0x0F;
    int i;

    for(i=0; i < 4; i++)
        _set_pin(port[i], pin[i], (top >> i & 0x01));
    _pulse_enable();

}

void write_line(char* string, int line){

    int i = 0;
    int length = strlen(string);

    RS_P &= ~RS;

    if(line == 1)
        _write_byte(0x80);
    else
        _write_byte(0x80 + 64);

    RS_P |= RS;
    for(i = 0; i < length; i++)
        _write_byte((uint8_t)string[i]);

    for(i = length; i < 16; i++)
        _write_byte(0x20);

    RS_P &= ~RS;

}

void put_char(char c, FILE *stream){
    RS_P &= ~RS;
    // For new line flag, clear the next line then set the cursor 
    // at the beginning of it. Also reset the flag.
    if(_line_change){
        if(_cur_line == 1){
            write_line(" ", 2);
            _write_byte(0x80 + 64);
            _line_change = 0;
            _cur_line = 2;
        }
        else{
            write_line(" ", 1);
            _write_byte(0x80);
            _line_change = 0;
            _cur_line = 1;
        }
    }
    // If we get a new line, set the flag
    if(c == '\n'){
        _line_change = 1;
    }
    else{
        RS_P |= RS;
        _write_byte(c);
        RS_P &= ~RS;
    }
}

void lcd_init(void){

    // Set data pins to output
    DD_7R |= DD_7;
    DD_6R |= DD_6;
    DD_5R |= DD_5;
    DD_4R |= DD_4;

    // Set EN and RS pins to output
    DD_ENR |= DD_EN;
    DD_RSR |= DD_RS;

    // Set everything low. RS needs to be low for chip to accept
    // commands (versus characters)
    EN_P &= ~EN;
    RS_P &= ~RS;

    DATA_7P &= ~DATA_7;
    DATA_6P &= ~DATA_6;
    DATA_5P &= ~DATA_5;
    DATA_4P &= ~DATA_4;

    // Setup procedure from datasheet: 
    
    // Wait for full voltage
    _delay_ms(100);

    // Send "8-bit mode" 3 times, even though we're going to use 4 bits
    // Wait specified time between each send
    _write_nibble(0x30);
    _delay_us(4500);

    _write_nibble(0x30);
    _delay_us(4500);

    _write_nibble(0x30);
    _delay_us(150);

    // Finally, put into 4 bit mode
    _write_nibble(0x20);
    _delay_us(150);

    // Setup complete, now configure the display
    uint8_t command;
    command = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
    _write_byte((LCD_FUNCTIONSET | command));
    _delay_us(60);
    
    command = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
    _write_byte((LCD_DISPLAYCONTROL | command));
    
    _write_byte(LCD_CLEARDISPLAY);
    
    command = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    _write_byte(LCD_ENTRYMODESET | command);

    _write_byte(0x02);
    _delay_us(2000);

    _cur_line = 1;
    _line_change = 0;
}
