#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"

uint8_t number = 0;
volatile uint8_t readflag = 0;
int analog_value1;
volatile uint16_t analog_value2;
volatile uint16_t analog_value3;
volatile uint16_t analog_value4;

ISR(ADC_vect){

    readflag = 1;
    uint8_t low = ADCL;
    uint8_t high = ADCH;
    uint16_t av = (high << 8) | low;

    switch(ADMUX & 0x0F){

        case 5:
            analog_value1 = av;
            ADMUX &= 0xF0;
            ADMUX |= 0x04;
            break;
        case 4:
            analog_value2 = av;
            ADMUX &= 0xF0;
            ADMUX |= 0x03;
            break;
        case 3:
            analog_value3 = av;
            ADMUX &= 0xF0;
            ADMUX |= 0x02;
            break;
        case 2:
            analog_value4 = av;
            ADMUX &= 0xF0;
            ADMUX |= 0x05;
            break;
    }

}

void adc_init(void){

    // set prescaler for 125khz ( with 8MHz clock)
    ADCSRA |= (_BV(ADPS2) | _BV(ADPS1) );
    // set reference to AREF and turn off left-adjust for reading
    ADMUX &= ~(_BV(REFS0) | _BV(REFS1) | _BV(ADLAR));

    // start with ADC5
    ADMUX |= 0x05;

    // free running mode
    ADCSRB = 0xF0;

    // Enable the ADC 
    ADCSRA |= _BV(ADEN);
    // Enable auto trigger
    ADCSRA |= _BV(ADATE);
    // Enable ADC interrupt
    ADCSRA |= _BV(ADIE);
    sei();
    // Start ADC conversion
    // Will keep going automatically in free running mode
    ADCSRA |= _BV(ADSC);
}

void adc_convert(uint16_t reading, uint8_t *digits){

    digits[0] = (reading * 5)/1024;
    digits[1] = ((reading * 5) % 1024)/100;
    digits[2] = (((reading * 5) % 1024) % 100)/10;

    if(digits[1] > 9){
        digits[0]++;
        digits[1] = 0;
    }
}

int main(void){

    DDRB |= _BV(DDB5);

    // initialize the LCD
    lcd_init();
    
    // use the char lcd as stdout
    FILE lcd_stream = FDEV_SETUP_STREAM(
        put_char,
        NULL,
        _FDEV_SETUP_WRITE
    );

    stdout = &lcd_stream;

    // set up the ADC
    adc_init();

    while(1){

        if(readflag == 1){
            uint16_t ineg = analog_value1;
            uint16_t vpos = analog_value2;
            uint16_t vneg = analog_value3;
            uint16_t ipos = analog_value4;
            uint8_t *digits = malloc(sizeof(uint8_t)*3);
            adc_convert(vneg, digits);


            printf("%u    %u\n", vpos, ipos);
            printf("-%u.%u%u\n", digits[0], digits[1], digits[2]);
            readflag = 0;
            _delay_us(100000);
            free(digits);
        }

    }

    return 0;
}
