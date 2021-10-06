/**
 *  timer.c Handle Timer2 configuration and interruption
 * */

#include "grbl.h"


 //Setup Timer2 to fire every 1ms
void timer_setup(){
  TCCR2B = 0x00;        //Disbale Timer2 while we set it up
  TCNT2  = 130;         //Reset Timer Count to 130 out of 255
  TIFR2  = 0x00;        //Timer2 INT Flag Reg: Clear Timer Overflow Flag
  TIMSK2 = 0x01;        //Timer2 INT Reg: Timer2 Overflow Interrupt Enable
  TCCR2A = 0x00;        //Timer2 Control Reg A: Wave Gen Mode normal
  TCCR2B = 0x05;        //Timer2 Control Reg B: Timer Prescaler set to 128
}

// Z Axis step
void step_z(){
            //Step
            PORTL |= (1 << STEP_BIT_2);     // set pin Z step high
            _delay_us(10);
            PORTL &= ~(1 << STEP_BIT_2);    // set pin Z step low
}
void set_z_dir_low(){
            PORTL &= ~(1 << DIRECTION_BIT_2);    // set pin Z dir low
            _delay_us(10);
}
void set_z_dir_high(){
            PORTL |= (1 << DIRECTION_BIT_2);     // set Z dir high
            _delay_us(10);
}

//Fires every 1/8 of a ms, 125uS
ISR(TIMER2_OVF_vect){
  
    if(jog_z_up){
            // Check direction in setting dir mask
            if (settings.dir_invert_mask & (1 << 2)){
             set_z_dir_high();
            }
            else{ 
              set_z_dir_low();
            }
            
            step_z(); //Step
            sys_position[Z_AXIS]++; // Update position

    }else if (jog_z_down){

            if (settings.dir_invert_mask & (1 << 2)){
              set_z_dir_low();
            }
            else
            {
              set_z_dir_high();
            }

            step_z(); //Step
            sys_position[Z_AXIS]--;// Update position
    }
  

  // Timing critical
  if (millis_timer > 7) //8 cycles is one millisecond
  {
    thc_update(); //Once a millisecond, evaluate what the THC should be doing
    millis_timer = 0;
  }
 
  TCNT2 = 223;           // Reset Timer to 130 out of 255
  TIFR2 = 0x00;          // Timer2 INT Flag Reg: Clear Timer Overflow Flag
  millis_timer++;        // 1ms counter update 
}
