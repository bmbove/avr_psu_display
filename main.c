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

void display_convert(uint16_t input, char *out){

    uint8_t i;
    uint8_t digits[4];
    for (i = 4; i--; ){
        digits[i] = input % 10;
        input /= 10;
    }
    sprintf(out, "%u%u.%u%u", digits[0], digits[1], digits[2], digits[3]);
}


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

    char *vpos_d = malloc(sizeof(char)*6);
    char *vneg_d = malloc(sizeof(char)*6);
    char *ipos_d = malloc(sizeof(char)*6);
    char *ineg_d = malloc(sizeof(char)*6);

    while(1){
        if (TCNT1 == 50000){
            // 0: i neg
            // 1: v pos 
            // 2: v neg
            // 3: i pos 

            // vpos = 3.341*adc + 412.7 (scale 10000)
            // vneg = 3.346*adc + 603 

            struct ADCCh *cur = adc_lst->front->next;
            
            cur = cur->next;
            uint16_t vpos = (((cur->voltage*3341/1000) + 412)/100);
            display_convert(vpos, vpos_d);

            cur = cur->next;
            uint16_t vneg = (((cur->voltage*3346/1000) + 603)/100);
            display_convert(vneg, vneg_d);

            printf("A  %5sV %4smA", vpos_d, "0");
            printf("\n");

            printf("B -%5sV %4smA", vneg_d, "0");
            printf("\n");
            
            // reset timer
            TCNT1 = 0;
        }

    }

    return 0;
}
