#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"

uint8_t number = 0;
volatile uint8_t readflag = 0;
volatile uint16_t  analog_value1;
volatile uint16_t  analog_value2;
volatile uint16_t  analog_value3;
volatile uint16_t  analog_value4;

ISR(ADC_vect){

    readflag = 1;

    switch(ADMUX & 0x0F){

        case 5:
            analog_value1 = ADCH;
            ADMUX &= 0xF0;
            ADMUX |= 0xF4;
            break;
        case 4:
            analog_value2 = ADCH;
            ADMUX &= 0xF0;
            ADMUX |= 0xF3;
            break;
        case 3:
            analog_value3 = ADCH;
            ADMUX &= 0xF0;
            ADMUX |= 0xF2;
            break;
        case 2:
            analog_value4 = ADCH;
            ADMUX &= 0xF0;
            ADMUX |= 0xF5;
            break;
    }

    ADCSRA |= _BV(ADSC);

}

int main(void){

    DDRB |= _BV(DDB5);

    lcd_init();
    /*write_line("A  15.35V 0.300A", 1);*/
    /*write_line("B -14.28V 0.800A", 2);*/

    ADCSRA |= (_BV(ADPS2) | _BV(ADPS1) );
    ADMUX |= _BV(REFS0);
    ADMUX |= _BV(ADLAR);

    ADMUX |= 0x05;

    ADCSRB = 0xF0;


    ADCSRA |= _BV(ADEN);
    ADCSRA |= _BV(ADATE);
    ADCSRA |= _BV(ADIE);
    ADCSRA |= _BV(ADSC);
    sei();

 

    char out1[16];
    char out2[16];


    while(1){

        if(readflag == 1){
            uint16_t ineg = analog_value1;
            uint16_t vpos = analog_value2;
            uint16_t vneg = analog_value3;
            uint16_t ipos = analog_value4;
            sprintf(out1, "%d    %d", vpos, ipos);
            sprintf(out2, "%d    %d", vneg, ineg);
            write_line(out1, 1);
            write_line(out2, 2);
            readflag = 0;
            _delay_us(100000);
        }

    }

    return 0;
}
