/*
*  ports.c Manage ports control
*  This library use the ATMega2560 ports map
*/
#include "grbl.h"

// Blocked pins
const uint8_t workPin[38] = {
    STEP_X, STEP_Y, STEP_Z, STEP_A,
    DIR_X, DIR_Y, DIR_Z, DIR_A,
    ENABLE,
    RELAY_1, RELAY_2,
    MAX_X, MAX_Y, MAX_Z,
    MIN_X, MIN_Y, MIN_Z,
    PROBE, PAUSE, STOP, PLAY,
    JOP_VEL_1, JOP_VEL_2, JOP_VEL_3, JOP_XA, JOP_XB, JOP_YA, JOP_YB, JOP_ZA, JOP_ZB,
    ANALOG_1, ANALOG_2, ANALOG_3, ANALOG_4,
    MISO, MOSI, SCK, CS};
const workPinSize = 38;
/**
 * Check if pin is in use from maping
 * 
 * @param pin 
 * @return true // Pin can be used 
 * @return false  // Pin blocked 
 */
bool pinBlocked(uint8_t pin)
{

  for (uint8_t j = 0; j < workPinSize; j++)
  {
    if (workPin[j] == pin)
      return true;
  }
  return false;
}
/**
 * Synchronic waint for pin implementation
 * 
 * @param line 
 * @return uint8_t // Result of command process
 * // 1 - Format problem
 * // 2 - Pin blocked
 * // 3 - Ok
 */
uint8_t waintForPinSync(char *line)
{

  uint8_t pinVal;   // Pin value (1-97)
  uint8_t stateVal; // Pin state (0-1)

  // Get pin value
  if (line[4] == 'P')
  {
    char v[2];
    v[0] = v[1] = 0;
    uint8_t vi = 0;
    for (uint8_t i = 5; i <= 6; i++)
    {
      if (line[i] == 'S' || line[i] == '\r' || line[i] == '\n')
        break;
      v[vi] = line[i];
      vi++;
    }
    //Convert pin value
    pinVal = atoi(v);
    // Check if pin is blocked
    if (pinBlocked(pinVal)) return 2;
    // Get state value 
    if (line[5 + vi] == 'S')
    {
      char s[2];
      s[0] = line[6 + vi];
      stateVal = atoi(s);
    }
     else
    {
      // Bad format
      return 1;
    }
  }
   else
    {
      // Bad format
      return 1;
    }

  protocol_buffer_synchronize(); // Sync and finish all remaining buffered motions before moving on.
  protocol_execute_realtime();   // Execute suspend.
  while (digitalRead(pinVal) != stateVal)
  {
  }
  protocol_exec_rt_system(); // Executes run-time commands

   return 3; // OK
}
/**
 * Waint for pin implementation
 * 
 * @param line 
 * @return uint8_t // Result of command process
 * // 1 - Format problem
 * // 2 - Pin blocked
 * // 3 - Ok
 */
uint8_t waintForPinAsync(char *line)
{

  uint8_t pinVal;   // Pin value (1-97)
  uint8_t stateVal; // Pin state (0-1)

  // Get pin value
  if (line[4] == 'P')
  {
    char v[2];
    v[0] = v[1] = 0;
    uint8_t vi = 0;
    for (uint8_t i = 5; i <= 6; i++)
    {
      if (line[i] == 'S' || line[i] == '\r' || line[i] == '\n')
        break;
      v[vi] = line[i];
      vi++;
    }
    //Convert pin value
    pinVal = atoi(v);
    // Check if pin is blocked
    if (pinBlocked(pinVal)) return 2;
    // Get state value 
    if (line[5 + vi] == 'S')
    {
      char s[2];
      s[0] = line[6 + vi];
      stateVal = atoi(s);
    }
    else
    {
      // Bad format
      return 1;
    }
  }
  else
  {
    // Bad format
    return 1;
  }

  protocol_execute_realtime(); // Execute suspend.
  while (digitalRead(pinVal) != stateVal)
  {
  }
  protocol_exec_rt_system(); // Executes run-time commands

  return 3; // OK

}

/**
 * Precess the ports command lines 
 * All params must be in the rigth order and use correct letters
 * 
 * @param line // Command line
 * @return uint8_t // Result of command process
 * // 1 - Format problem
 * // 2 - Pin blocked
 * // 3 - Ok
 */
uint8_t ports_manage(char *line)
{

  uint8_t pinVal;   // Pin value (1-97)
  uint8_t stateVal; // Pin state (0-1)
  uint8_t modeVal;  // Pin mode (0-2)

  // Get pinvalue
  if (line[3] == 'P')
  {
    char v[2];
    v[0] = v[1] = 0;
    uint8_t vi = 0;
    for (uint8_t i = 4; i <= 5; i++)
    {
      if (line[i] == 'M' || line[i] == 'R' || line[i] == 'S' || line[i] == '\r' || line[i] == '\n')
        break;
      v[vi] = line[i];
      vi++;
    }
    //Convert pin value
    pinVal = atoi(v);
    // Check if pin is blocked
    if (pinBlocked(pinVal)) return 2;
    // Get pin mode
    if (line[4 + vi] == 'M')
    {
      char s[2];
      s[0] = line[5 + vi];
      modeVal = atoi(s);
      pinMode((uint8_t)pinVal, (uint8_t)modeVal);
    }
    
    // Get pin state
    if (line[4 + vi] == 'S')
    {
      char s[2];
      s[0] = line[5 + vi];
      // Convert value
      stateVal = atoi(s);
      // Write value to pin
      digitalWrite(pinVal, stateVal);
    }
    
    // Check for read command
    if (line[4 + vi] == 'R')
    {
      // Return pin value
      if (digitalRead(pinVal) == 0)
      {
        printPgmString(PSTR("LOW"));
      }
      else
      {
        printPgmString(PSTR("HIGH"));
      }
      printPgmString(PSTR("\r\n"));
    }
    
  }
  else
  {
    // Bad format
    return 1;
  }
  // OK
  return 3;
}

/**
 * Configure alarm pin interruption
 * 
 */
void alarmsInit()
{
  alarmTriggered = false;
  /*
  +----------+------+--------------------+----------------------+-----------+
  | PIN name | Port | Digital pin number |       Alarm          |  INTR     |
  +----------+------+--------------------+----------------------+-----------+
  |    A13   |  PK5 |       84           |  Alarm OUT Servo X2  |  PCINT21  |
  +----------+------+--------------------+----------------------+-----------+ 
  |    A14   |  PK6 |       83           |  Alarm OUT Servo Y   |  PCINT22  |
  +----------+------+--------------------+----------------------+-----------+
  |    D11   |  PB5 |       24           |  Power Source fault  |  PCINT5   |
  +----------+------+--------------------+----------------------+-----------+ 
  |    D12   |  PB6 |       25           |  Tourch Signal       |  PCINT6   |
  +----------+------+--------------------+----------------------+-----------+ 
  |    D13   |  PB7 |       26           |  Alarm OUT Servo X1  |  PCINT7   |
  +----------+------+--------------------+----------------------+-----------+ 
  */ 
 
 // Port B Configuration
  DDRB &= ~((1<<POWER_SOURCE_FAULT_BIT)|(1<<ALARM_TOURCH_SIGNAL_BIT)|(1<<ALARM_OUT_SERVO_X1_BIT)); // Configure as input pins
  PORTB |= ((1<<POWER_SOURCE_FAULT_BIT)|(1<<ALARM_TOURCH_SIGNAL_BIT)|(1<<ALARM_OUT_SERVO_X1_BIT));   // Enable internal pull-up resistors. Normal high operation.
  PCMSK0 |= ((1<<POWER_SOURCE_FAULT_BIT)|(1<<ALARM_TOURCH_SIGNAL_BIT)|(1<<ALARM_OUT_SERVO_X1_BIT));  // Enable specific pins of the Pin Change Interrupt
  PCICR |= (1 << PCIE0);   // Enable Pin Change Interrupt

  // Port K Configuration
  DDRK &= ~((1<<ALARM_OUT_SERVO_X2_BIT)|(1<<ALARM_OUT_SERVO_Y_BIT)); // Configure as input pins
  PORTK |= ((1<<ALARM_OUT_SERVO_X2_BIT)|(1<<ALARM_OUT_SERVO_Y_BIT));   // Enable internal pull-up resistors. Normal high operation.
  PCMSK2 |= ((1<<ALARM_OUT_SERVO_X2_BIT)|(1<<ALARM_OUT_SERVO_Y_BIT));  // Enable specific pins of the Pin Change Interrupt
  PCICR |= (1 << PCIE2);   // Enable Pin Change Interrupt
}

/**
 * Disable all alarms 
 * 
 */
void alarmsDisable(){
  
  // Disable Specific Pin Change Interrupt Port B
  PCMSK0 &= ~((1<<POWER_SOURCE_FAULT_BIT)|(1<<ALARM_TOURCH_SIGNAL_BIT)|(1<<ALARM_OUT_SERVO_X1_BIT));  // Disable specific pins of the Pin Change Interrupt

   // Disable Specific Pin Change Interrupt Port K
  PCMSK2 &= ~((1<<ALARM_OUT_SERVO_X2_BIT)|(1<<ALARM_OUT_SERVO_Y_BIT));  // Enable specific pins of the Pin Change Interrupt
}

/**
 * Enable alarm by pin and port 
 * 
 * @param port 
 * @param bit 
 */
void alarmDisable(char port,uint8_t bit){
  if(port=='B'){
    PCMSK0 &= ~(1<<bit);
  }else{
    if(port=='K'){
      PCMSK2 &= ~(1<<bit);
    }
  }
}

/**
 * Disable alarm by pin and port 
 * 
 * @param port 
 * @param bit 
 */
void alarmEnable(char port,uint8_t bit){
  if(port=='B'){
    PCMSK0 |= (1<<bit);
  }else{
    if(port=='K'){
      PCMSK2 |= (1<<bit);
    }
  }

  alarmTriggered = false;
}

/**
 *  Restore last alarm triggered and movement
 * 
 */
void movementRestore(){
 
  system_set_exec_state_flag(EXEC_CYCLE_START);
}

// Pin change interrupt
ISR(PCINT0_vect)
{    
    if(alarmTriggered) return;
    
    // Read port B, only 5,6 and 7 pins and check interruption state mask
    uint8_t pin = (PINB & ((1<<POWER_SOURCE_FAULT_BIT)|(1<<ALARM_TOURCH_SIGNAL_BIT)|(1<<ALARM_OUT_SERVO_X1_BIT)));
    // Check for correct state of pins
    pin ^= PORTB_PCINTERRUPT_STATE_MASK;
     // Report feedback in each case and feed hold    
      if(((pin & (1<<POWER_SOURCE_FAULT_BIT)))){
        alarmTriggered = true;
        activeAlarmPort = 'B';
        activeAlarmBit = POWER_SOURCE_FAULT_BIT;
        alarmDisable(activeAlarmPort,activeAlarmBit);
        system_set_exec_state_flag(EXEC_FEED_HOLD);     
        report_feedback_message(MESSAGE_POWER_SOURCE_FAULT);
      }else if((pin & (1<<ALARM_TOURCH_SIGNAL_BIT))){
        alarmTriggered = true;
        activeAlarmPort = 'B';
        activeAlarmBit = ALARM_TOURCH_SIGNAL_BIT;
        alarmDisable(activeAlarmPort,activeAlarmBit);
        system_set_exec_state_flag(EXEC_FEED_HOLD);     
        report_feedback_message(MESSAGE_ALARM_TOURCH_SIGNAL);
      }else if((pin & (1<<ALARM_OUT_SERVO_X1_BIT))){
        alarmTriggered = true;
        activeAlarmPort = 'B';
        activeAlarmBit = ALARM_OUT_SERVO_X1_BIT;
        alarmDisable(activeAlarmPort,activeAlarmBit);
        system_set_exec_state_flag(EXEC_FEED_HOLD);     
        report_feedback_message(MESSAGE_ALARM_OUT_SERVO_X1);
      }
    
}

/**
 * Disable Stepper, assume disable 0 logic
 * 
 * */
void stepperDisable(char *line){
   
    if(line[3]){
     if(line[3]=='X'){
       // X Disable - Pin D38
         STEPPER_DISABLE_PORT(0) &= ~(1 << STEPPER_DISABLE_BIT(0));
     }else if(line[3]=='Y'){
       // Y Disable - Pin A2
        STEPPER_DISABLE_PORT(1) &= ~(1 << STEPPER_DISABLE_BIT(1));
     }else if(line[3]=='Z'){
       // Z Disable - Pin A8
        STEPPER_DISABLE_PORT(2) &= ~(1 << STEPPER_DISABLE_BIT(2));
     }
    }else{
      STEPPER_DISABLE_PORT(0) &= ~(1 << STEPPER_DISABLE_BIT(0));
      STEPPER_DISABLE_PORT(1) &= ~(1 << STEPPER_DISABLE_BIT(1));
      STEPPER_DISABLE_PORT(2) &= ~(1 << STEPPER_DISABLE_BIT(2));
    }
}

/**
 * Enable Stepper, assume enable 1 logic
 * 
 * */
void stepperEnable(char *line){
   
    if(line[3]){
     if(line[3]=='X'){
       // X Enable - Pin D38
         STEPPER_DISABLE_PORT(0) |= (1 << STEPPER_DISABLE_BIT(0));
     }else if(line[3]=='Y'){
       // Y Enable - Pin A2
        STEPPER_DISABLE_PORT(1) |= (1 << STEPPER_DISABLE_BIT(1));
     }else if(line[3]=='Z'){
       // Z Enable - Pin A8
        STEPPER_DISABLE_PORT(2) |= (1 << STEPPER_DISABLE_BIT(2));
     }
    }else{
      STEPPER_DISABLE_PORT(0) |= (1 << STEPPER_DISABLE_BIT(0));
      STEPPER_DISABLE_PORT(1) |= (1 << STEPPER_DISABLE_BIT(1));
      STEPPER_DISABLE_PORT(2) |= (1 << STEPPER_DISABLE_BIT(2));
    }
}