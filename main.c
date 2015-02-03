#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"

uint8_t number = 0;
volatile uint8_t readflag = 0;
volatile uint16_t analog_value1;
volatile uint16_t analog_value2;
volatile uint16_t analog_value3;
volatile uint16_t analog_value4;


ISR(ADC_vect){

    readflag = 1;
    /*uint16_t av = ADCH << 2 | ADCL >> 5;*/

    /*switch(ADMUX & 0x0F){*/

        /*case 5:*/
            /*analog_value1 = av;*/
            /*ADMUX &= 0xF0;*/
            /*ADMUX |= 0xF4;*/
            /*break;*/
        /*case 4:*/
            /*analog_value2 = av;*/
            /*ADMUX &= 0xF0;*/
            /*ADMUX |= 0xF3;*/
            /*break;*/
        /*case 3:*/
            /*analog_value3 = av;*/
            /*ADMUX &= 0xF0;*/
            /*ADMUX |= 0xF2;*/
            /*break;*/
        /*case 2:*/
            /*analog_value4 = av;*/
            /*ADMUX &= 0xF0;*/
            /*ADMUX |= 0xF5;*/
            /*break;*/
    /*}*/

    ADCSRA |= _BV(ADSC);

}

int main(void){

    DDRB |= _BV(DDB5);

    lcd_init();
    FILE lcd_stream = FDEV_SETUP_STREAM(
        put_char,
        NULL,
        _FDEV_SETUP_WRITE
    );

    stdout = &lcd_stream;
    /*write_line("A  15.35V 0.300A", 1);*/
    /*write_line("B -14.28V 0.800A", 2);*/

    ADCSRA |= (_BV(ADPS2) | _BV(ADPS1) );
    ADMUX &= ~(_BV(REFS0) | _BV(REFS1));

    ADMUX &= ~_BV(ADLAR);

    ADMUX &= 0xF0;
    ADMUX |= 0xF5;

    ADCSRB = 0xF0;


    ADCSRA |= _BV(ADEN);
    ADCSRA |= _BV(ADATE);
    ADCSRA |= _BV(ADIE);
    sei();
    ADCSRA |= _BV(ADSC);
    int counter = 0;
    while(1){

        if(readflag == 1){
            uint16_t ineg = analog_value1;
            uint16_t vpos = analog_value2;
            uint16_t vneg = analog_value3;
            uint16_t ipos = analog_value4;
            printf("%u    %u   %u\n", vpos, ipos,counter);
            printf("%u    %u\n", vneg, ineg);
            readflag = 0;
            counter++;
            _delay_us(1000000);
        }

    }

    return 0;
}
