#include <stdlib.h>
#include <avr/io.h>

#ifndef ADC_INC
#define AD_INC
#include "adc.h"
#endif

// initialize circularly linked list for ADC channel switching
struct ADCList *_adc_lst_init(){

    struct ADCList *lst = malloc(sizeof(struct ADCList));
    lst->front = malloc(sizeof(struct ADCCh));
    lst->front->next = lst->front;
    lst->front->value = 0;
    lst->size = 0;
    return lst;
}

// add ADC channel to front of list
void _add_adc(struct ADCList *lst, uint8_t num, uint8_t scale){

    struct ADCCh *newlink = malloc(sizeof(struct ADCCh));
    newlink->channel = num;
    newlink->value = 0;
    newlink->sample_count = 0;
    newlink->voltage = 0;
    newlink->scale = scale;

    newlink->next = lst->front->next;
    lst->front->next = newlink;
    lst->cur = newlink;

    lst->size++;
}

// switch current ADC channel to the next one in line
void next_adc(struct ADCList *lst){

    // make sure it's not the sentinel
    if(lst->cur->next->channel != 0)
        lst->cur = lst->cur->next;
    else
        // still might return the sentinel if no channels added
        // simple solution: add channels before trying to get a 'next' 
        lst->cur = lst->cur->next->next;
}

struct ADCList * adc_init(void){

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
    // Start ADC conversion
    // Will keep going automatically in free running mode
    ADCSRA |= _BV(ADSC);

    struct ADCList *adc_lst = _adc_lst_init();

    // 5: i neg
    // 4: v pos 
    // 3: v neg
    // 2: i pos 
    
    _add_adc(adc_lst, 2, 2);
    _add_adc(adc_lst, 3, 20);
    _add_adc(adc_lst, 4, 20);
    _add_adc(adc_lst, 5, 2);

    return adc_lst;
}

void adc_add_reading(struct ADCCh *ch, uint16_t reading){
    
    // oversample to get 12 bit resolution.
    // wait for 16 new samples (factor of 4 per extra bit)
    // then compute new value using exponentially weighted
    // running average. scale factor power of 2 for simplicity.
    reading <<= 2;
    if(ch->sample_count == 16){ 
        int32_t prev_w = ch->value - ((ch->value * ch->scale)/100);
        ch->value = ((reading * ch->scale)/100) + prev_w;
        ch->voltage = ((uint32_t)ch->value * AREF * 10000) >> 12;
        ch->sample_count = 0;
    }
    else{
        ch->samples[ch->sample_count] = reading;
        ch->sample_count++;
    }
}
