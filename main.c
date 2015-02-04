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

void display_convert(uint16_t input, char *out, uint8_t num){

    uint8_t * digits = malloc(sizeof(uint8_t)*num);
    uint8_t i;
    for ( i=num; i--; ){
        digits[i] = input % 10;
        input /= 10;
    }
    // this is ugly... i gave up.
    if(num == 4){
        if(digits[0] > 0)
            sprintf(out, "%u%u.%u%u", digits[0], digits[1], digits[2], digits[3]);
        else
            sprintf(out, "%u.%u%u", digits[1], digits[2], digits[3]);
    }
    if(num == 3){
        if(digits[0] > 0 && digits[1] > 0)
            sprintf(out, "%u%u%u", digits[0], digits[1], digits[2]);
        else if(digits[0] == 0 && digits[1] != 0)
            sprintf(out, "%u%u", digits[1], digits[2]);
        else
            sprintf(out, "%u", digits[2]);
    }
    free(digits);
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
    char *ineg_d = malloc(sizeof(char)*3);
    char *plus = malloc(sizeof(char));

    while(1){
        if (TCNT1 == 50000){
            // 0: i neg
            // 1: v pos 
            // 2: v neg
            // 3: i pos 

            // vpos = 3.341*adc + 412.7 (scale 10000)
            // vneg = 3.346*adc + 603 

            struct ADCCh *cur = adc_lst->front->next;
            
            uint16_t ineg = (((cur->voltage*173/100) + 6)/100);
            display_convert(ineg, ineg_d, 3);

            cur = cur->next;
            uint16_t vpos = (((cur->voltage*3341/1000) + 412)/100);
            display_convert(vpos, vpos_d, 4);

            cur = cur->next;
            uint16_t vneg = (((cur->voltage*3346/1000) + 603)/100);
            display_convert(vneg, vneg_d, 4);

            cur = cur->next;
            uint16_t ipos = (((cur->voltage*173)/100) + 6)/100;
            display_convert(ipos, ipos_d, 3);

            // due to opamp gain, we can't really see over 855mA
            if(ipos == 855)
                sprintf(plus, "%s", "+");
            else
                sprintf(plus, "%s", " ");

            printf("A  %5sV %3smA%s", vpos_d, ipos_d, plus);
            printf("\n");

            if(ineg == 855)
                sprintf(plus, "%s", "+");
            else
                sprintf(plus, "%s", " ");

            printf("B -%5sV %3smA%s", vneg_d, ineg_d, plus);
            printf("\n");
            
            // reset timer
            TCNT1 = 0;
        }

    }

    return 0;
}
