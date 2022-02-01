/*
*  pwm.c Manage pwm control
*  This library use the ATMega2560 ports map
*/
#include "grbl.h"
/*
    Timer 4 and 5 are 16-bit Timer/Counter units that allows accurate program execution timing
    as event management, wave generation and signal timing measurement.The main features are:
    - True 16-bit design (i.e, Allow 16-bits PWM)
    - Three independient Ouput compare units
    - Double buffered output compare registers
    - Glitch-free, phase correct pulse with modulator(PWM)
    - Variable PWM period
    ATMega 2560 board ca use 2 – 13 and 44 – 46 pins as PWM output with a frequency of 490 Hz 
    pins 4 and 13 have also a high value configurable 980 Hz.
    The use of the Timers modules is not exclusive to the PWM outputs, but is shared with other functions.
    Using functions that require the use of these Timers will mean that we will not be able to use
    any of the PWM pins simultaneously.
    Pin Port  Digital pin  Chip pin       Alternate Function
      PH5         8           17        OC4C(Output Compare and PWM Output C for Timer/Counter4) 
      PH4         7           16        OC4B(Output Compare and PWM Output B for Timer/Counter4) 
      PH3         6           15        OC4A(Output Compare and PWM Output A for Timer/Counter4)
      PL5        44           40        OC5C(Output Compare and PWM Output C for Timer/Counter5) 
      PL4        45           39        OC5B(Output Compare and PWM Output B for Timer/Counter5) 

    Set up PWM algorithm
    1- Configure Timer to frequency
    2- Configure output pins
    3- Configure channels with duty cycle  
    
    Several registers are used to control each timer. The Timer/Counter Control Registers TCCRnA and TCCRnB
    hold the main control bits for the timer. (Note that TCCRnA and TCCRnB do not correspond to the outputs
    A and B.) These registers hold several groups of bits:
     - Waveform Generation Mode bits (WGM): these control the overall mode of the timer. (These bits are split 
     between TCCRnA and TCCRnB.)
     - Clock Select bits (CS): these control the clock prescaler
     - OCRnx The Output Compare Registers contain a 16-bit value that is continuously compared with
      the counter value (TCNTn). A match can be used to generate an Output Compare interrupt, or to generate
      a waveform output on the OCnx pin. 
     - Compare Match Output Mode bits (COMnx): these enable/disable/invert output x
     Note: According to the mode we can set maximum count value to adjust period to 0xFFF or configure in 
     OCRnA / ICRn the fixed values 0x00FF,0x01FF or 0x03FF. 

     Example:
-----------------------------------------------------------------------------------------
*-> Fclk = 16MHz
*-> in mode 3 the divor is x64 then f=16MHz/64=0.25MHz=250KHz
*-> if we set TOP value N=256 then output frequency is 250KHz/256=0.97656KHz=976.6Hz 
*-> And if a channel has configured a value of 180 the the duty_cycle=(OCRnx+1)/N=(180+1)/256=0.707=70.7%  

*/

/**
 * Configure Timer frequency 
 * 
 * @param timer 
 * @param mode 
 */
void pwmTimerFreq(uint8_t timer, uint8_t mode)
{
  /*
      Clock select bit description
      +-------+------+-------+--------+-------------------------------------------+
      |  Mode | CSn2 |  CSn1 |  CSn0  |  Description                              |
      +-------+------+-------+--------+-------------------------------------------+
      |   0   |   0  |   0   |    0   | No clock source(Timer/Counter stopped)    |
      +-------+------+-------+--------+-------------------------------------------+
      |   1   |   0  |   0   |    1   |    Clk/1  (No prescaling)                 |
      +-------+------+-------+--------+-------------------------------------------+
      |   2   |   0  |   1   |    0   |    Clk/8  (From prescaler)                |
      +-------+------+-------+--------+-------------------------------------------+
      |   3   |   0  |   1   |    1   |    Clk/64 (From prescaler)                |
      +-------+------+-------+--------+-------------------------------------------+
      |   4   |   1  |   0   |    0   |    Clk/256 (From prescaler)               |
      +-------+------+-------+--------+-------------------------------------------+
      |   5   |   1  |   0   |    1   |    Clk/1024 (From prescaler)              |
      +-------+------+-------+--------+-------------------------------------------+
      |   6   |   1  |   1   |    0   | External clock source on Tn pin.          |
      |       |      |       |        | Clock on falling edge                     |  
      +-------+------+-------+--------+-------------------------------------------+
      |   7   |   1  |   1   |    1   | External clock source on Tn pin.          |
      |       |      |       |        | Clock on rising edge                      |  
      +-------+------+-------+--------+-------------------------------------------+
    */
  // Set PWM frequency for D8, D7 & D6 --- Only this modes allowed
  if (timer == 4)
  {
    switch (mode)
    {
    case 1:
      TCCR4B = TCCR4B & 0b11111000 | 0b00000001; // set timer 5 divisor to     1 for PWM frequency
      break;
    case 2:
      TCCR4B = TCCR4B & 0b11111000 | 0b00000010; // set timer 5 divisor to     8 for PWM frequency
      break;
    case 3:
      TCCR4B = TCCR4B & 0b11111000 | 0b00000011; // set timer 5 divisor to    64 for PWM frequency
      break;
    case 4:
      TCCR4B = TCCR4B & 0b11111000 | 0b00000100; // set timer 5 divisor to   256 for PWM frequency
      break;
    case 5:
      TCCR4B = TCCR4B & 0b11111000 | 0b00000101; // set timer 4 divisor to  1024 for PWM frequency
      break;
    default:
      TCCR4B = TCCR4B & 0b11111000 | 0b00000000; // No clock source(Timer/Counter stopped)
      break;
    }
  }
  // Set PWM frequency for D44, D45 & D46 --- Only this modes allowed
  if (timer == 5)
  {
    switch (mode)
    {
    case 1:
      TCCR5B = TCCR5B & 0b11111000 | 0b00000001; // set timer 5 divisor to     1 for PWM frequency of 31372.55 Hz
      break;
    case 2:
      TCCR5B = TCCR5B & 0b11111000 | 0b00000010; // set timer 5 divisor to     8 for PWM frequency of  3921.16 Hz
      break;
    case 3:
      TCCR5B = TCCR5B & 0b11111000 | 0b00000011; // set timer 5 divisor to    64 for PWM frequency of   490.20 Hz
      break;
    case 4:
      TCCR5B = TCCR5B & 0b11111000 | 0b00000100; // set timer 5 divisor to   256 for PWM frequency of   122.55 Hz
      break;
    default:
      TCCR5B = TCCR5B & 0b11111000 | 0b00000000; // No clock source(Timer/Counter stopped)
      break;
    }
  }
}

/**
 *  Set maximum count value to handl period 
 * 
 * @param timer 
 * @param value 
 */
void pwmConfCount(uint8_t timer, uint32_t value)
{

  // TOP value  configuration
  if (timer == 4)
  {
    ICR4 = value;
  }
  if (timer == 5)
  {
    OCR5A = value;
  }
}
/**
 * Setup PWM timer  
 * 
 * @param timer 
 */
void pwmConfTimer(uint8_t timer)
{

  /**
   Waveform Generation Mode Bit Description 
  +-------+-------+--------+---------+---------+-------------------+-------+-----------+-----------+
  |  Mode | WGMn3 |  WGMn2 |  WGMn1  |  WGMn0  | Timer/Counter     |  Top  | Update of | TOVn Flag |
  |       |       | (CTCn) | (PWMn1) | (PWMn0) | Mode of Operation |       | OCRnA at  | Set on    |
  +-------+-------+--------+---------+---------+-------------------+-------+-----------+-----------+
  |   14  |   1   |   1    |    1    |    0    |     Fast PWM      | ICRn  |   BOTTOM  |    TOP    |
  +-------+-------+--------+---------+---------+-------------------+-------+-----------+-----------+
  |   15  |   1   |   1    |    1    |    1    |     Fast PWM      | OCRnA |   BOTTOM  |    TOP    | 
  +-------+-------+--------+---------+---------+-------------------+-------+-----------+-----------+
  */
  // Configure timer in mode 14 Fast PWM with top in ICRn
  if (timer == 4)
  {
    // Clear TCCRnA/TCCRnB registers
    TCCR5A = 0;
    TCCR5B = 0;
    // Configure timer as Fast PWM output
    TCCR5A = (1 << WGM51);
    TCCR5B = ((1 << WGM52) | (1 << WGM53));
  }

  // Configure timer in mode 15 Fast PWM with top in OCRnA
  if (timer == 5)
  {
    // Clear TCCRnA/TCCRnB registers
    TCCR5A = 0;
    TCCR5B = 0;
    // Configure timer as Fast PWM output
    TCCR5A = ((1 << WGM50) | (1 << WGM51));
    TCCR5B = ((1 << WGM52) | (1 << WGM53));
  }
}

/**
 *  Set PWM Duty cycle for specific channel
 * 
 * @param timer 
 * @param channel 
 * @param value 
 */
void pwmChannelEnable(uint8_t timer, uint8_t channel, int value)
{
  /*
    Fast PWM
    +---------+--------+----------------------------------------------------------------------+
    | COMnx1  | COMnx0 |        Description                                                   |   
    +---------+--------+----------------------------------------------------------------------+
    |   1     |   0    |  Clear OCnx on compare match, set OCnx at BOTTOM (non-inveting mode) |
    +---------+--------+----------------------------------------------------------------------+
    */
  if (timer == 4)
  {
    // Configure PWM value by set pin mode out and set duty cile value in non-inveting mode
    switch (channel)
    {
    case 3:
      OCR4C = value;
      DDRH |= (1 << PH5);
      TCCR4A |= (1 << COM4C1);
      break;
    case 2:
      OCR4B = value;
      DDRH |= (1 << PH4);
      TCCR4A |= (1 << COM4B1);
      break;
    case 1:
      OCR4A = value;
      DDRH |= (1 << PH3);
      TCCR4A |= (1 << COM4A1);
      break;
    }
  }
  if (timer == 5)
  {
    // Configure PWM value by set pin mode out and set duty cile value in non-inveting mode
    switch (channel)
    {
    case 3:
      OCR5C = value;
      DDRL |= (1 << PL5);
      TCCR5A |= (1 << COM5C1);
      break;
    case 2:
      OCR5B = value;
      DDRL |= (1 << PL4);
      TCCR5A |= (1 << COM5B1);
      break;
    }
  }
}

/**
 *  Disable pwm channel 
 * 
 * @param timer 
 * @param channel 
 */
void pwmDisable(uint8_t timer, uint8_t channel)
{
  /*
  Fast PWM
    +---------+--------+----------------------------------------------------------------------+
    | COMnx1  | COMnx0 |        Description                                                   |   
    +---------+--------+----------------------------------------------------------------------+
    |   0     |   0    |  Normal port operation, OCnA/OCnB/OCnC disconnected                  |
    +---------+--------+----------------------------------------------------------------------+
    */
  // Disable PWM. Output voltage is zero.
  if (timer == 4)
  {
    switch (channel)
    {
    case 3:
      TCCR4A &= ~(1 << COM4C1);
      break;
    case 2:
      TCCR4A &= ~(1 << COM4B1);
      break;
    case 1:
      TCCR4A &= ~(1 << COM4A1);
      break;
    }
  }
  if (timer == 5)
  {
    switch (channel)
    {
    case 3:
      TCCR5A &= ~(1 << COM5C1);
      break;
    case 2:
      TCCR5A &= ~(1 << COM5B1);
      break;
    }
  }
}

uint8_t pwmManage(char *line)
{

  uint8_t timer;   // Timer (4-5)
  uint8_t mode;    // Timer mode (0-5)
  uint8_t channel; // Timer channel (1-3)
  uint8_t value;   // Duty cycle value

  // Get Timer
  if (line[4] == 'T')
  {

    //Convert timer value
    timer = line[5] - '0';
    // Get mode
    if (line[6] == 'M')
    {
      mode = line[7] - '0';
      // Configure timer as Fast PWM
      pwmConfTimer(timer);
      // Configure timer frequency
      pwmTimerFreq(timer, mode);
      // OK
      return 3;
    }
    // Get maximum value
    if (line[6] == 'N')
    {
      char max[5];
      uint8_t vi = 0;
      uint32_t maxvalue = 0;
      for (uint8_t i = 7; i <= 11; i++)
      {
        if (line[i] < 48 || line[i] > 57)
          break;
        max[vi] = line[i];
        vi++;
      }
      // Convert value
      maxvalue = atoi(max);

      // Configure timer maximum count value
      pwmConfCount(timer, maxvalue);

      // OK
      return 3;
    }

    // Get channel
    if (line[6] == 'C')
    {
      //Convert channel value
      channel = line[7] - '0';
      // Get value
      if (line[8] == 'V')
      {
        char duty[5];
        uint8_t vi = 0;
        uint32_t dutycycle = 0;
        for (uint8_t i = 9; i <= 13; i++)
        {
          if (line[i] == '\r' || line[i] == '\n')
            break;
          duty[vi] = line[i];
          vi++;
        }
        // Convert value
        dutycycle = atoi(duty);
        pwmChannelEnable(timer, channel, dutycycle);
      }

      if (line[8] == 'D')
      {
        pwmDisable(timer, channel);
      }
    }
  }
  // OK
  return 3;
}

void pwmTest()
{

  // // Set up Timer5 in 16-bit Fast PWM mode (mode 14)
  // DDRL |= (1 << PL5); // Make PL5 (OC5C / Arduino D45) an output
  DDRL |= (1 << PL4); // Make PL4 (OC5B / Arduino D45) an output
  TCCR5A |= (1 << COM5B1) | (1 << WGM51) | (1 << WGM50);
  TCCR5B |= (1 << WGM53) | (1 << WGM52) | (1 << CS51) | (1 << CS50);
  OCR5A = 256; // set arbitrary duty cycle for testing
  OCR5B = 100; // set arbitrary duty cycle for testing
  // OCR5C = 200;  // set arbitrary duty cycle for testing
}
