#include <avr/io.h>

// ADC reference voltage
#define AREF 5
// Weight factor * 100 for moving average
#define SCALE 80 

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
    // next channel
    struct  ADCCh *next;
};

struct ADCList{
    uint16_t size;
    struct ADCCh *front;
    struct ADCCh *cur;
};

struct ADCList *_adc_lst_init();
void _add_adc(struct ADCList *lst, uint8_t num);

struct ADCList * adc_init();
void next_adc(struct ADCList *lst);
void adc_add_reading(struct ADCCh *ch, uint16_t reading);
