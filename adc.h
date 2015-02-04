#include <avr/io.h>

#define AREF 5

struct ADCCh{
    // ADC channel
    uint8_t channel;
    // samples taken
    uint16_t samples[16];
    // count of samples to put towards next value
    uint8_t sample_count;
    // raw adc value
    uint16_t value;
    // computed voltage
    uint32_t voltage;
    // scale for exp moving average
    uint8_t scale;
    // next channel
    struct  ADCCh *next;
};

struct ADCList{
    uint16_t size;
    struct ADCCh *front;
    struct ADCCh *cur;
};

struct ADCList *_adc_lst_init();
void _add_adc(struct ADCList *lst, uint8_t num, uint8_t scale);

struct ADCList * adc_init();
void next_adc(struct ADCList *lst);
void adc_add_reading(struct ADCCh *ch, uint16_t reading);
