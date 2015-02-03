#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"
#include "adc.h"

uint8_t number = 0;
volatile uint8_t readflag = 0;
int analog_value1;
volatile uint16_t analog_value2;
volatile uint16_t analog_value3;
volatile uint16_t analog_value4;
struct ADCList *adc_lst;


ISR(ADC_vect){

    readflag = 1;

    uint8_t low = ADCL;
    uint8_t high = ADCH;

    adc_lst->cur->value = ((high << 8) | low);
    next_adc(adc_lst);
    ADMUX = (ADMUX & 0xF0) | adc_lst->cur->channel;

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
    adc_lst = adc_init();

    // enable interrupts
    sei();

    while(1){

        if(readflag == 1){
            /*uint16_t ineg = analog_value1;*/
            /*uint16_t vpos = analog_value2;*/
            /*uint16_t vneg = analog_value3;*/
            /*uint16_t ipos = analog_value4;*/
            /*uint8_t *digits = malloc(sizeof(uint8_t)*3);*/
            /*adc_convert(vneg, digits);*/


            /*printf("%u    %u\n", vpos, ipos);*/
            /*printf("-%u.%u%u\n", digits[0], digits[1], digits[2]);*/
            struct ADCCh *current = adc_lst->front->next;
            uint8_t i;
            for( i = 0; i < adc_lst->size; i++){
                printf("%u ", current->value);
                current = current->next;
            }
            /*printf("%u\n", adc_lst->size);*/
            /*printf("%u %u", adc_lst->cur->channel, ADMUX);*/
            printf("\n");
            printf("test\n");
            readflag = 0;
            _delay_us(500000);
            /*free(digits);*/
        }

    }

    return 0;
}
