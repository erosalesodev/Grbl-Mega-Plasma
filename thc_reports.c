/**
 *  thc.c THC status reports
 * */
#include "grbl.h"

void thc_debug(char *line){
    printPgmString(PSTR("THC control response\n"));
         if(line[2] == '-'){
           printPgmString(PSTR("Status Report\n"));
             printPgmString(PSTR("Reference Voltage  = "));
            print_uint32_base10((uint16_t)analogSetVal);
            printPgmString(PSTR("\r\n"));
          }
         if (line[2] == '=')
          {
            char v[5];
            uint8_t vi = 0;
            for (uint8_t i = 3; i < (3 + 5); i++)
            {
              if (line[i] == '\r' || line[i] == '\n') break;
              v[vi] = line[i];
              vi++;
            }
            analogSetVal = atoi(v);
            printPgmString(PSTR("Reference Voltage Setted = "));
            print_uint32_base10((uint16_t)analogSetVal);
            printPgmString(PSTR("\r\n"));
          }
}

void report_thc(){
  printPgmString(PSTR("$133="));
   print_uint32_base10((uint16_t)analogSetVal);
   printPgmString(PSTR("\r\n"));
}