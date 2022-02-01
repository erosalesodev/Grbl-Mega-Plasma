# Grbl Mega Plasma Firmware

CNC Motion Control Firmware for [MecanicaLG](https://www.mecanicalg.com/)  Motion Control Boards based on Arduino ATMega2560.  Optimized for CNC Plasma Cutting and improving functionalities related with it process.

[TOC]

This library is written based on [grbl-Mega](https://github.com/gnea/grbl-Mega) a documentation can be found [here](https://tgit-tech.github.io/grbl-Mega-Doxy/index.html)

GCODE documentation can be found here:

[Gcode Dictionary by Duet3d](https://duet3d.dozuki.com/Wiki/Gcode)

[Gcode by RepRap](https://reprap.org/wiki/G-code)

Arduino functions used in this project are documented and explained [here](https://garretlab.web.fc2.com/en/arduino/inside/)

------

### Features

- All original GRBL features left intact
- Added new commands and error codes
- Added new libraries
- Built in Arc Voltage Torch Height Control. Uses A0 to read arc voltage from the plasma cutter and makes adjustments on a 1ms intervol.

### Configuration

Chip select in file **config.h**

```c
// To use with RAMPS 1.4 Board, comment out the above defines and uncomment the next two defines
#define DEFAULTS_RAMPS_BOARD
#define CPU_MAP_2560_RAMPS_BOARD

```

### New libraries List

New libraries are added in **grbl.h**

```c
// Integration features
#include "adc.h"
#include "timer.h"
#include "thc.h"
#include "thc_reports.h"
#include "ports.h"
#include "pwm.h"
```

|   Library   | Description                                  |
| :---------: | -------------------------------------------- |
|     adc     | Handle ADC module in Atmega2560              |
|    timer    | Handle Timer2 configuration and interruption |
|     thc     | THC Control                                  |
| thc_reports | THC status reports and debugging functions   |
|    ports    | Ports management                             |
|     pwm     | Handle pwm output module                     |

### Global Variables Added

|   Variable   |       Type        | Description                           | Declared on |
| :----------: | :---------------: | :------------------------------------ | :---------: |
|  analogVal   | volatile uint16_t | Value to store analog result from ADC |     adc     |
|  hysteresis  | volatile uint16_t | Allowable voltage deviation           |     thc     |
| analogSetVal | volatile uint16_t | Value to stabilize in THC             |     thc     |
|  thcWorking  |       bool        | THC working status                    |     thc     |

------



### THC Functionalities

#### ADC use

For THC purpose there is a ADC channel, it can be selected in file **adc.h** defined in variable `ADC_THC_CHANNEL_SELECT` by default its value is equal to 3.

```
// ADC THC Chanel selector
// Do not set above 15! You will overrun other parts of ADMUX.
        #define ADC_THC_CHANNEL_SELECT 3;

```

For configuartion use the  `adc_setup()` function and for start first convertion use `start_adc_convertion()` defined in **adc.c**.

#### Timer use

Timer2 module was used to handle THC timing, all function are implemented in **timer.c** file

`timer_setup()`  function is used to configure timer and the interruption subroutine can be found here to.

#### THC work mode

##### Reports

Command "?" sends the next response from Grbl the implementation can be found in **report.c** in fuction `report_realtime_status()`

```
<Idle|MPos:0.000,0.000,0.000|FS:0,0|Pn:XYZ|WCO:0.000,0.000,0.000>
```

When THC is working variable `thcWorking` is set `true` and the "?" command response is

```
<Idle|MPos:0.000,0.000,0.000|FS:0,0|Pn:XYZ|WCO:0.000,0.000,0.000|THC:0,0>
```

The THC param bring the fallow information

`THC:<analogSetVal>,<analogVal>`

Command $$ give the information about several params including `$133`,its show the value of nominal voltage of THC, the implementation is in **report.c** in function  `report_status_message()`, it calls `report_thc()` from **thc_repots.c**

##### Ark Ok signal

------

### Ports Functionalities

The port functionalities implemented allow to carry out new operations such as waiting for signal or reading and writing on pins. This allows adding new functionalities to the hardware such as valve control or waiting for the Arc Ok signal. Also a pwm control and a alarm system are implemented. 

#### M42: Switch I/O pin

Reference implementation and documentation can be found [here](https://reprap.org/wiki/G-code#M42:_Switch_I.2FO_pin): 

**Command use cases** 

`M42 Pxx Mx		// Set pin  mode`

`M42 Pxx Sx		// Set pin  state`

`M42 Pxx R		// Read pin`

[^Warning]: The  params must be in the following order P,M,S,R

**Parameters**

`P<xx> Pin number`

`<xx> This value must be between 1 and 99`

`M<x>` Pin mode

| Mode | Description                                                  |
| :--: | :----------------------------------------------------------- |
|  0   | Set pin as INPUT make the bit of DDR to 0, and the bit of PORT to 0 |
|  1   | Set pin as INPUT_PULLUP make the bit of DDR to 0, and the bit of PORT to 1 |
|  2   | Set pin as OUTPUT make the bit of DDR to 1                   |

`S<x>` Pin state

| State | Description |
| :---: | ----------- |
|   0   | 0 logic     |
|   1   | 1 logic     |

`R` Read

| Response | Description         |
| -------- | ------------------- |
| LOW      | If pin is a 0 logic |
| HIGH     | If pin is 1 logic   |

**Response**

| Response | Description                   |
| -------- | ----------------------------- |
| ok       | Command executed successfully |
| error:60 | Pin locked                    |

**Example**

```
M42 P7 M2		// Set pin 7 to digital out
M42 P7 S1       // Set pin 7 to 1 logic
M42 P7 R		// Read pin state

//-----------------------------------
As same way you can send those commands as a single block folowing the priority
M42 P7 M2 S1   // Set pin 7 to digital out and to 1 logic state
M42 P7 R       // Read pin state
```

#### M226: Waint for pin Async

Reference implementation and documentation can be found [here](https://reprap.org/wiki/G-code#M226:_Wait_for_pin_state).

This process is executed asynchronously, this means that when receiving the command, the movement will be stopped after execute all the buffer stored and will not be restored until the pin wait for the state is completed.

**Command use cases** 

`M226 Pxx Sx		// Waint for pin state`

**Parameters**

`P<xx> Pin number`

`<xx> This value must be between 1 and 99`

`S<x>` Pin state

| State | Description |
| :---: | ----------- |
|   0   | 0 logic     |
|   1   | 1 logic     |

**Response**

| Response | Description                   |
| -------- | ----------------------------- |
| ok       | Command executed successfully |
| error:60 | Pin locked                    |

**Example**

```
G01 X50 F50 // Move to X 50 position with feedrate of 50
M226 P37 S1 // Waint for pin 37 to change to 1 logic satus, at this point movement isn't stoped untli finsih previous movements
G01 X0 F200 // Move to X 0 position, the movement is restarted when pin 37 change mode
```

#### M227: Waint for pin Sync

Same params as **M226** but in this case movement is stoped as well as command arrives

**Example**

```
G01 X50 F50 // Move to X 50 position
M226 P37 S1 // Waint for pin 37 to change to 1 logic satus, at this point movement is stoped
G01 X0 F200 // Move to X 0 position, the movement is restarted when pin 37 change mode
```

#### M219: PWM

For pwm purpose are used timer 4 and 5 module it can be used to drive a servo motor, laser module or digital-analog converter (DAC).

For this purpose are used the next pinout:

| Pin Port | Digital pin | Chip pin | Alternate Function                                       |
| -------- | ----------- | -------- | -------------------------------------------------------- |
| PH5      | 8           | 17       | OC4C(Output Compare and PWM Output C for Timer/Counter4) |
| PH4      | 7           | 16       | OC4B(Output Compare and PWM Output B for Timer/Counter4) |
| PH3      | 6           | 15       | OC4A(Output Compare and PWM Output A for Timer/Counter4) |
| PL5      | 44          | 40       | OC5C(Output Compare and PWM Output C for Timer/Counter5) |
| PL4      | 45          | 39       | OC5B(Output Compare and PWM Output B for Timer/Counter5) |

[^Warning]: timer4 is used by spindle module too, ensure you are not in conflic.

**Command use cases** 

`M219 Tx Mx		  // Configure timer mode`

`M219 Tx Nx		  // Configure timer max value for set period`

`M219 Tx Cx Vx	  // Configure timer channel value`

`M219 Tx Cx D	  // Disable timer channel`

**Parameters**

`T<x> Timer number`

`<x> This value must be between 4 or 5`

`M<x> Timer mode` 

`<x> This value must be between 0 and 5`

PWM mode is configured as Fast PWM with `OCRnA` register as TOP value in timer 5 case and `ICRn` in timer 4 case.

| Mode | Description                                           |
| :--: | ----------------------------------------------------- |
|  0   | No clock source(Timer/Counter stopped)                |
|  1   | Set timer divisor to 1 (proper frequency)             |
|  2   | Set timer divisor to 8                                |
|  3   | Set timer divisor to 64                               |
|  4   | Set timer divisor to 256                              |
|  5   | Set timer divisor to 1024 (only abailable in timer 4) |

`N<x> Maximum count value` 

`<x> This value must be between 0 (pwm disabled) and 0xffff(maximun 16-bit register size)` 

Output frequency can be calculated as
$$
f_{OCnxPWM}=f_{clk_{IO}}/N*(1+TOP)
$$

`C<x> Channe select`

`<x> This value must be between 1 and 3 in case of timer 4 and in timer 5 case must be 2 or 3`

`D Used for disable channel`

`V<x> This value is for set duty cycle in channel`
$$
DutyCycle=(OCRnx+1)/N
$$

```
Example:
-----------------------------------------------------------------------------------------
*-> Fclk = 16MHz
*-> in mode 3 the divor is x64 then f=16MHz/64=0.25MHz=250KHz
*-> if we set TOP value N=256 then output frequency is 250KHz/256=0.97656KHz=976.6Hz 
*-> And if a channel has configured a value of 180 the the duty_cycle=(OCRnx+1)/N=(180+1)/256=0.707=70.7%
```

**Example**

```
M219 T5 M3		      // Congirure timer 5 as Fast PWM prescaler x64
M219 T5 N256          // Set maximum count value in timer 5
M219 T5 C2 V180	      // Set timer 5 channel 2 output as 70.7% duty cycle
M219 T5 C2 D		  // Disable channel 2 timer 5	
```

#### Alarms System

There are some signals with priority of atention than trigger alarms, this alarms are for exaple when the tourch crash and fall or other works params are wrong, when one of those alams are detected the movement is stoped to fix the problem fisically, when all is put in order by the operator then the movement is restored manually by a command and its restored from the last coordenate to don't loose the cout.

| Alarm Message             | Meaning            | Port/Pin |
| ------------------------- | ------------------ | -------- |
| [MSG:Power Fault]         | Power Source fault | PB5      |
| [MSG:Alarm Out Servo X1]  | Alarm OUT Servo X1 | PB7      |
| [MSG:Alarm Out Servo X2]  | Alarm OUT Servo X2 | PK5      |
| [MSG:Alarm Out Servo Y]   | Alarm OUT Servo Y  | PK6      |
| [MSG:Alarm Tourch Signal] | Tourch Signal      | PB6      |

This alarms are located in ports B and K are handle by interruption subroutines by port status change.

#### M229: Initialize all alarms

Initialize all alarms, port B and K cases. When this command is recived alarms pin are configured and interrupts enabled. 

**Command use case** 

`M229`

#### M230: Disable all alarms

Disable all alarms, port B and K cases. When this command is recived interrupts by port change are disabled. 

**Command use case** 

`M230`

#### M231: Enable alarm mask

Enable alarm mask, port B or K cases. Allows detect port change interrupt in this pin.

**Command use case** 

`M231 <n><x>`

[^Warning]: Alarms must be initialized to have a correct  behavior of interrupts

**Parameters**

`<n>` Port of alarms (must be **B** or **K**)

`<x>` Alarm pin (must be between 5 and 7 in port B and in case of port K must be 5 or 6)

**Example**

```
M231 B5 Enable port B pin 5 alarm interrupt by port change state
```

#### M232: Disable alarm mask

Disable alarm mask, port B or K cases.  Disable detect port change interrupt in this pin.

**Command use case** 

`M232 <n><x>`

**Parameters**

`<n>` Port of alarms (must be **B** or **K**)

`<x>` Alarm pin (must be between 5 and 7 in port B and in case of port K must be 5 or 6)

**Example**

```
M232 K5 Disable port K pin 5 alarm interrupt by port change state
```

#### ~ : Restore movement after port alarm stop

This command is implemented in grbl by default, we addeded a function to restore alarm once movement is enabled.

------

### How I implemented it

Normally grbl is not written to interpret the M command and its format with multiple params its why it was necessary to modify the file **gcode.h** to process the blocks in separated functions

```c
// Added ignore process blocks M command implemented
  while (line[char_counter] != 0 && !(letter=='M' && (int_value==42 || int_value==219 || (int_value>=227 && int_value<=233))))
```

#### Command M42

 This command is implemented in **ports.c** specifically in the function `ports_manage()` who is in charge of processing the parameters. Within this function were used:

[pinMode()](https://www.arduino.cc/reference/en/language/functions/digital-io/pinmode/)

[digitalWrite()](https://www.arduino.cc/reference/en/language/functions/digital-io/digitalwrite/)

[digitalRead()](https://www.arduino.cc/reference/en/language/functions/digital-io/digitalread/)

Its full implementation can be seen [here](https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/wiring_digital.c)

This functions is called from **gcode.c** in the switch cases from M commands

```c
// Implementing M42
//-----------------------------------
          case 42:
//Execute M42 function and if the pin is blocked or bad format is reported return error
          result = ports_manage(line);
            if(result==2){FAIL(STATUS_GCODE_PIN_LOCKED);}
            else if(result==1){FAIL(STATUS_EXPECTED_COMMAND_LETTER);}
          break;
```

`ports_manage()` function returns 3 states

| State | Meaning        |
| ----- | -------------- |
| 1     | Format problem |
| 2     | Pin blocked    |
| 3     | Ok             |

If the function detect and error it will be reported by serial communication.

#### Command M226

The function is called in the same way

```c
  		   //-----------------------------------
           // Implementing M226
          //-----------------------------------
          case 226:
          result = waintForPinAsync(line);
          if(result==2){FAIL(STATUS_GCODE_PIN_LOCKED);}
          else if(result==1){FAIL(STATUS_INVALID_STATEMENT);}
          break;
```

The `waintForPinSync()` function is implemented as a while loop, it reads the pin state and keep in loop until its value is equal to the state pram, its params are extracted from line param, it is the command block recibed for M226.

[^Warning]: Before block finish all remaining buffered motions.

```c
 protocol_buffer_synchronize(); // Sync and finish all remaining buffered motions before moving on.
protocol_execute_realtime(); // Execute suspend.
  while(digitalRead(pinVal)!= stateVal){

  }
protocol_exec_rt_system();  // Executes run-time commands
```

`protocol_buffer_synchronize()` documentation can be found [here](https://tgit-tech.github.io/grbl-Mega-Doxy/protocol_8c.html#a913b1b87c08659f91faadb9fbe2eb059).

`protocol_execute_realtime()`documentation can be found [here](https://tgit-tech.github.io/grbl-Mega-Doxy/protocol_8h.html#a89eab4380ee456455a0564f31ad264bb).

`protocol_exec_rt_system()`documentation can be found [here](https://tgit-tech.github.io/grbl-Mega-Doxy/protocol_8h.html#a11463088d9083a40732b4c22d7e57a12).

#### Command M227

This command is implemented similar as **M226** the main difference is the synchronism, in this case when command is received the movement is stopped immediately  

```c
protocol_execute_realtime(); // Execute suspend.
  while(digitalRead(pinVal)!= stateVal){

  }
protocol_exec_rt_system();  // Executes run-time commands
```

[^Warning]: Block movement when the command is received, don't finish it until state is set for the spected.

#### Command 219

For this command implementation was consider the **spindle_control.c** file and the function [analogWrite()](https://garretlab.web.fc2.com/en/arduino/inside/hardware/arduino/avr/cores/arduino/wiring_analog.c/analogWrite.html) for manage the configuration.

The set up PWM algorithm is described here:

- Configure Timer to frequency :

For timer configuration are used the function `pwmTimerFreq()` which set the fraquency divisor using the clock select bits (CS) these control the clock prescaler, `pwmConfCount()` according to the mode we can set maximum count value to adjust period setting the `OCRnA` / `ICRn` registers value and `pwmConfTimer()` set the waveform generation mode bits (WGM) these control the overall mode of the timer (These bits are split between `TCCRnA` and `TCCRnB`registers)

- Configure output pins for channels with duty cycle :

The function`pwmChannelEnable()` set the `DDRx` to set the pin as pull-up output and `OCRxn`set the value for duty cycle, then the COMnx1 allows to set the channel as clear  on compare match `OCnx` and set at BOTTOM this is th non-inveting mode.

- Disable PWM output

In this case by setting COMnx1/COMnx0 to logical 0 then we set the normal port operation, OCnA/OCnB/OCnC disconnected 

#### Alarms System

This system is handle by interrupt so we have the `ISR(PCINT0_vect)` implemented in **port.c** for port B change pin interrupt and `ISR(PCINT2_vect)` already implemented by grbl using pins 1,2 and 3.

For initialize alarms we use `alarmsInit()` function explained in **M229** command, then when a alarm is triggered we read the port status and make a OR logical operation whit `PORTB_PCINTERRUPT_STATE_MASK` to check which alarm is in wrong state, you can modify this mask in **port.c** file deppending of your hardware.

Then we set  the global variable `alarmTriggered` to true, it tell as when a alarm was detected, and store in `activeAlarmPort` and `activeAlarmBit` the respective values of the alarm case to re-enable it when movement is restored. Then execute `alarmDisable()` function with this updated values to avoid a re-triggered of the interrupt and stop movement with `system_set_exec_state_flag(EXEC_FEED_HOLD)` function, a documentation can be found [here](https://tgit-tech.github.io/grbl-Mega-Doxy/system_8c.html#a383996fcb8a383c1a0bbd315d1836847) .

For alarm feedback its used the function `report_feedback_message()` implemented in **report.c**.

 New codes has been added for this purpose defined in **report.h**.

```c
// Alarms code implemented
#define MESSAGE_POWER_SOURCE_FAULT 15
#define MESSAGE_ALARM_OUT_SERVO_X1 16
#define MESSAGE_ALARM_OUT_SERVO_X2 17
#define MESSAGE_ALARM_OUT_SERVO_Y 18
#define MESSAGE_ALARM_TOURCH_SIGNAL 19
```

#### Command M229

Command is implemented in **port.c** file in function `alarmsInit()`.

For enable all interrupts we follow the confuration path:

- Configure `DDRx` register to set pin s as inputs.
- Configure `PORTx` register to enable internal pull-up resistors, normal high operation.
- Configure `PCMSK` register to enable specific pins of the pin change interrupt.
- Set specific bit in `PCICR` to enable pin change interrupt.

#### Command M230

Command is implemented in **port.c** file in function `alarmsDisable()`.

Disable pin change interrupt in `PCICR` register.

#### Command ~

To the original grbl function we added in **serial.c** ,in the **ISR** function for serial comunication `ISR(SERIAL_RX)` a check for variable `alarmTriggered` and if its true enable the alarm with `alarmEnable()`.

```c
case CMD_CYCLE_START:   system_set_exec_state_flag(EXEC_CYCLE_START);  // Set as true
                        if(alarmTriggered)alarmEnable(activeAlarmPort,activeAlarmBit);
                        break; 
```



#### error:60 Pin Locked

Define error code in **report.h** as

`#define STATUS_GCODE_PIN_LOCKED 60`

**ports_manage** declared in **ports.c** function returns a boolean value

`bool ports_manage(char *line)`

This criteria is definded in **ports.c** in function

`bool pinBlocked(uint8_t pin)`

Who tell us if pin is blocked or not, you can add some pins in this function declaration to allow or not its use. In array of constants `workPin` declared in **ports.c** and the constants are defined in **ports.h**.

[^Warning:]:  When you modify the workPin array also ensure modify workPinSize value with new size

------

### Licence

This is copyrighted software that is released under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 license

The unofficial summary is:

You can use it as it is for non-commercial purposes.  If  you modify it, you must make the modifications public and attribute the  original source to me.

(If you want to license it for commerical use, please contact me.)

------

### You can also see others useful repos related with THC implementation and other stuff

| Repo description                                             | Link                                                  |
| ------------------------------------------------------------ | ----------------------------------------------------- |
| ArdunioTHC                                                   | https://github.com/regeg/ArdunioTHC                   |
| Plasma THC                                                   | https://github.com/grblHAL/Plugin_plasma/             |
| DIY built CNC Plasma with Arduino THC (Torch Height Controller) | https://github.com/regeg/ArdunioTHC                   |
| CNC Motion Control Firmware for XmotionCNC Motion Control Boards or Arduino UNO DIY setup. Focused on CNC Plasma Cutting. | https://github.com/UnfinishedBusiness/XmotionFirmware |
| Arduino based THC that reads plasma cutter voltage and send Up and  Down signals to Plasma Torch Actuator to adjust voltage to target value. | https://github.com/HaleDesign/TorchHeightController   |
| CNC plugin for the plasma torch height controlling on the base of Arduino (ATmega328P MCU) | https://github.com/ussserrr/torch-height-control      |
| Plasma/THC plugin                                            | https://github.com/grblHAL/Plugin_plasma              |
| GRBL-LPC1769                                                 | https://github.com/gnea/grbl-LPC                      |
| DIY Arduino based Torch Height Controller                    | https://github.com/bluestang123/plasmacnc             |
| CNC plugin for the plasma torch height controlling on the base of Arduino (ATmega328P MCU) | https://github.com/ussserrr/torch-height-control      |
| LinuxCNC                                                     | https://github.com/LinuxCNC/linuxcnc                  |
| Marlin Firmware                                              | https://github.com/MarlinFirmware/Marlin              |
| Repetier Firmware                                            | https://github.com/repetier/Repetier-Firmware         |

------

If you want to support me you can do it in

- [buymeacoffe](https://www.buymeacoffee.com/erosalesodev)

As many other, brought to you with   :black_heart: ,lack of :bed: and lots of :coffee: under :headphones:. 

**Powered by MecanicaLG and AlaSoluciones ©2022** 
