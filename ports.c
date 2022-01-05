/*
*  ports.c Manage ports control
*  This library use the ATMega2560 ports map
*/
#include "grbl.h"


const 
uint8_t workPin[38] = {
  STEP_X,STEP_Y,STEP_Z,STEP_A,
  DIR_X,DIR_Y,DIR_Z,DIR_A,
  ENABLE,
  RELAY_1,RELAY_2,
  MAX_X,MAX_Y,MAX_Z,
  MIN_X,MIN_Y,MIN_Z,
  PROBE,PAUSE,STOP,PLAY,
  JOP_VEL_1,JOP_VEL_2,JOP_VEL_3,JOP_XA,JOP_XB,JOP_YA,JOP_YB,JOP_ZA,JOP_ZB,
  ANALOG_1,ANALOG_2,ANALOG_3,ANALOG_4,
  MISO,MOSI,SCK,CS
  };

// Check if pin is in use from maping
bool pinBlocked(uint8_t pin){

  for(uint8_t j=0; j < 38; j++){
    if( workPin[j] == pin) return true;
  }
   
   
   return false;
}

// Waint for pin implementation
void waintForPin(char *line){
  
  uint8_t pinVal;         // Pin value (1-97)
  uint8_t stateVal;       // Pin state (0-1)

  if(line[4] == 'P'){
    char v[2];
    v[0]=v[1]=0;
    uint8_t vi = 0;
    for (uint8_t i = 5; i <= 6 ; i++)
    {
      if (line[i] == 'S' || line[i] == '\r' || line[i] == '\n') break;
      v[vi] = line[i];
      vi++;
    }
    pinVal = atoi(v);
     if(line[5+vi] == 'S'){
                char s[2];
                s[0] = line[6+vi];
                stateVal = atoi(s);
            }

  }
  
  protocol_execute_realtime(); // Execute suspend.
  while(digitalRead(pinVal)!= stateVal){

  }
  protocol_exec_rt_system();  // Executes run-time commands
  
}

// Precess the ports command lines
bool ports_manage(char *line){

    uint8_t pinVal;         // Pin value (1-97)
    uint8_t stateVal;       // Pin state (0-1)
    uint8_t modeVal;        // Pin mode (0-2)

   
    printPgmString(PSTR("\n"));        
        if(line[3] == 'P'){
            char v[2];
            v[0]=v[1]=0;
            uint8_t vi = 0;
            for (uint8_t i = 4; i <= 5 ; i++)
            {
              if (line[i] == 'M' || line[i] == 'R' || line[i] == 'S' || line[i] == '\r' || line[i] == '\n') break;
              v[vi] = line[i];
              vi++;
            }
            pinVal = atoi(v);
            // Check if pin is blocked
            if(pinBlocked(pinVal))return false;

            if(line[4+vi] == 'M'){
                char s[2];
                s[0] = line[5+vi];
                modeVal = atoi(s);
                pinMode((uint8_t)pinVal,(uint8_t)modeVal);
            }

            if(line[4+vi] == 'S'){
                char s[2];
                s[0] = line[5+vi];
                stateVal = atoi(s);
                digitalWrite(pinVal,stateVal);
            }

            if(line[4+vi] == 'R'){
                printPgmString(PSTR("State value = "));
                if(digitalRead(pinVal) == 0){
                  printPgmString(PSTR("LOW"));
                }else{
                  printPgmString(PSTR("HIGH"));
                }
                
                printPgmString(PSTR("\r\n"));
            }

            return true;
        }
}
