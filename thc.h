/**
 *  thc.h THC control
 * */

#ifndef thc_h
#define thc_h

            bool thcWorking;                   // THC working state
            volatile bool jog_z_up;		        // Torch must be rised
            volatile bool jog_z_down;	        // Torch must be move down
            volatile uint16_t hysteresis;       // Allowable voltage deviation
            volatile uint16_t analogSetVal;     // Voltage to stabilize in thc

            void thc_initialize();              //THC Initialize
            void thc_update();                  // THC Control
#endif