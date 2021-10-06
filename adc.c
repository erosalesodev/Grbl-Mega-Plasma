/**
        adc.c - Handle ADC module in Atmega2560
*/

#include "grbl.h"


//Configure ADC Module
void adc_setup(){
    // Initialize analog value
    analogVal = 0;
    /* Begin ADC Setup */
    // *------------- Configure Result Presentation --------------------*
    // When ADLAR = 1 (Left Adjusted)
    //---------------------------------------------------------
    //| ADC9 | ADC8 | ADC7 | ADC6 | ADC5 | ADC4 | ADC3 | ADC2 | ADCH
    //---------------------------------------------------------
    //| ADC1 | ADC0 |      |      |      |      |      |      | ADCL
    //---------------------------------------------------------
    // ADCH:ADCL = Vin * 1024 / VREF << 6
    // When ADLAR = 0
    //---------------------------------------------------------
    //|      |      |      |      |      |      | ADC9 | ADC8 | ADCH
    //---------------------------------------------------------
    //| ADC7 | ADC6 | ADC5 | ADC4 | ADC3 | ADC2 | ADC1 | ADC0 | ADCL
    //---------------------------------------------------------
    // ADCH:ADCL = Vin * 1024 / VREF
    // clear ADLAR in ADMUX (0x7C) to right-adjust the result
    // ADCL will contain lower 8 bits, ADCH upper 2 (in last two bits)
    ADMUX &= 0b11011111;
    // *-------------- Set ADC voltage reference ------------------------*
    // REFS1 REFS0 Description
    //  0     0    AREF
    //  0     1    AVcc
    //  1     0    Reserved
    //  1     1    Internal 1.1V
    // Set REFS1..0 in ADMUX (0x7C) to change reference voltage to the
    // proper source (01)
    ADMUX |= 0b01000000;
    // *--------------- Select ADC Channel ------------------------------*
    // Clear MUX3..0 in ADMUX (0x7C) in preparation for setting the 
    // analog input
    ADMUX &= 0b11110000;
    // Set MUX3..0 in ADMUX (0x7C) to read from 
    // ADC_CHANNEL_SELECT variable (Internal temp)
    ADMUX |= ADC_CHANNEL_SELECT;

    // Set ADEN in ADCSRA (0x7A) to enable the ADC.
    // Note, this instruction takes 12 ADC clocks to execute
    ADCSRA |= 0b10000000;
    
    // Auto-Triggering Mode Eanble
    // Set ADATE in ADCSRA (0x7A) to enable auto-triggering.
    // ADCSRA |= 0b00100000;
    // Clear ADTS2..0 in ADCSRB (0x7B) to set trigger mode to free running.
    // This means that as soon as an ADC has finished, the next will be
    // immediately started.
    // ADCSRB &= 0b11111000;

  
    // *----------------- ADC Prescaler Selections -----------------------*
    // ADPS2 ADPS1 ADPS0 Division Factor
    //   0     0     0          2
    //   0     0     1          2
    //   0     1     0          4
    //   0     1     1          8
    //   1     0     0         16
    //   1     0     1         32
    //   1     1     0         64
    //   1     1     1        128
    // Set the Prescaler to 128 (16000KHz/128 = 125KHz)
    // Above 200KHz 10-bit results are not reliable.
    ADCSRA |= 0b00000111;


    // *----------------- Enable ADC Interrupt ---------------------------*
    // Set ADIE in ADCSRA (0x7A) to enable the ADC interrupt.
    // Without this, the internal interrupt will not trigger.
    ADCSRA |= 0b00001000;

    // Enable global interrupts
    // AVR macro included in <avr/interrupts.h>, which the Arduino IDE
    // supplies by default.
    sei();
    /* End ADC Setup */
}

//Used to start ADC with a first convertion
void start_adc_convertion(){

  // *---------- Start first ADC conversion ---------------*
  //Set ADSC in ADCSRA, ADC Start Conversion
  //next adc interrupt will be in about 1664 mcu clocks
  ADCSRA |= (1<<ADSC);

}

//ADC Interruption
ISR(ADC_vect){

  // *----------- Read ADC value and store in analogValue variable ----------*  
  // Must read low first
  analogVal = ADCL | (ADCH << 8);
  // char mychar[15];
  // sprintf(mychar,"%04d",analogVal);
  // printString(mychar);
  //  printString("\n");
  // Needed if free-running mode isn't enabled.
  // Set ADSC in ADCSRA (0x7A) to start another ADC conversion
  ADCSRA |= 0b01000000;

}