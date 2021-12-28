/*
*  ports.c Manage ports control
*  This library use the ATMega2560 ports map
*/
#include "grbl.h"

bool pinBlocked(uint8_t pin){
   if( pin == 40) return true;
   
   return false;
}

void waintForPin(uint8_t pin,uint8_t status){
  
  
   system_set_exec_state_flag(EXEC_FEED_HOLD); // Use feed hold for program pause.
   protocol_execute_realtime(); // Execute suspend.

  while(digitalRead(pin)!=status){
    
  }

  system_set_exec_state_flag(EXEC_CYCLE_START); 
  protocol_execute_realtime(); // Execute.

printPgmString(PSTR("Pin ready\r\n"));
}
/*
  Precess the ports command lines
*/
bool ports_manage(char *line){

    uint8_t pinVal;         // Pin value (1-97)
    uint8_t stateVal;       // Pin state (0-1)
    uint8_t modeVal;        // Pin mode (0-2)

    // printPgmString(PSTR("Ports control response\n"));
    // printString(line);
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
            // printPgmString(PSTR("Pin value = "));
            // print_uint32_base10((uint8_t)pinVal);
            // printPgmString(PSTR("\r\n"));
            if(pinBlocked(pinVal))return false;

            if(line[4+vi] == 'M'){
                char s[2];
                s[0] = line[5+vi];
                modeVal = atoi(s);
                // printPgmString(PSTR("Mode value = "));
                // print_uint32_base10((uint8_t)modeVal);
                // printPgmString(PSTR("\r\n"));
                pinMode((uint8_t)pinVal,(uint8_t)modeVal);
            }

            if(line[4+vi] == 'S'){
                char s[2];
                s[0] = line[5+vi];
                stateVal = atoi(s);
                // printPgmString(PSTR("State value = "));
                // print_uint32_base10((uint8_t)stateVal);
                // printPgmString(PSTR("\r\n"));
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
