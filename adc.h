/**
 *       adc.h - Handle ADC module in Atmega2560
*/

#ifndef adc_h
#define adc_h

        // ADC Chanel selector
        // Do not set above 15! You will overrun other parts of ADMUX.
        #define ADC_CHANNEL_SELECT 3;


        volatile uint16_t analogVal;            // Value to store analog result


        void adc_setup();                       //Configure ADC Module
        void start_adc_convertion();            //Used to start ADC with a first convertion

#endif