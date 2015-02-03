#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"
#include "main.h"

#ifndef ADC_INC
#define ADC_INC
#include "adc.h"
#endif

ISR(ADC_vect){

    readflag = 1;

    uint8_t low = ADCL;
    uint8_t high = ADCH;
    uint16_t reading = ((high << 8) | low);

    adc_add_reading(adc_lst->cur, reading);
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
    readflag = 0;

    // set up timer, prescaler 64
    TCCR1B |= (1 << CS10 | 1 << CS11);
    uint8_t counter = 0;
    while(1){
        if (TCNT1 == 50000){
            struct ADCCh *current = adc_lst->front->next;
            uint8_t i;

            current = current->next;
            printf("%3u ", current->samples[1]);
            current = current->next;
            printf("%3u ", current->samples[2]);
            current = current->next;
            printf("%3u ", current->samples[3]);
            printf("\n");

            current = adc_lst->front->next;

            printf("%3u ", current->channel);
            current = current->next;
            printf("%3u ", current->channel);
            current = current->next;
            printf("%3u ", current->channel);
            current = current->next;
            printf("%3u", current->channel);
            printf("\n");
            
            // reset timer
            TCNT1 = 0;
        }

    }

    return 0;
}
