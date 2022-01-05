# Grbl THC Integration Firmware

CNC Motion Control Firmware for AlaSoluciones  Motion Control Boards based on Arduino ATMega2560.  Focused on CNC Plasma Cutting and solving problems related with it.

[TOC]

This library is written based on [grbl-Mega](https://github.com/gnea/grbl-Mega) a documentation can be found [here](https://tgit-tech.github.io/grbl-Mega-Doxy/index.html)

GCODE documentation can be found here:

[Gcode Dictionary by Duet3d](https://duet3d.dozuki.com/Wiki/Gcode)

[Gcode by RepRap](https://reprap.org/wiki/G-code)



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
// THC Integration features
#include "adc.h"
#include "timer.h"
#include "thc.h"
#include "thc_reports.h"
#include "ports.h"
```

|   Library   | Description                                  |
| :---------: | -------------------------------------------- |
|     adc     | Handle ADC module in Atmega2560              |
|    timer    | Handle Timer2 configuration and interruption |
|     thc     | THC Control                                  |
| thc_reports | THC status reports and debugging functions   |
|    ports    | Ports management                             |

### Global Variables Added

|   Variable   |       Type        | Description                           | Declared on |
| :----------: | :---------------: | :------------------------------------ | :---------: |
|  analogVal   | volatile uint16_t | Value to store analog result from ADC |     adc     |
|  hysteresis  | volatile uint16_t | Allowable voltage deviation           |     thc     |
| analogSetVal | volatile uint16_t | Value to stabilize in THC             |     thc     |
|  thcWorking  |       bool        | THC working status                    |     thc     |

### Debug Functionalities

Debug functionalities are declared in **thc_report** use T command.

| Command | Function                              |
| :-----: | :------------------------------------ |
|   T-    | Show THC reference voltage            |
|  T=xx   | Set THC voltage reference to xx value |

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



### Ports Functionalities

The port functionalities implemented allow to carry out new operations such as waiting for signal or reading and writing on pins. This allows adding new functionalities to the hardware such as valve control or waiting for the Arc Ok signal.

#### M42: Switch I/O pin

Reference implementation and documentation can be found [here](https://reprap.org/wiki/G-code#M42:_Switch_I.2FO_pin): 

**Command use cases** 

`M42 Pxx Mx		// Set pin  mode`

`M42 Pxx Sx		// Set pin  state`

`M42 Pxx R		// Read pin`

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
```

#### M226: Waint for pin

Reference implementation and documentation can be found 

**Command use cases** 

`M42 Pxx Sx		// Waint for pin state`

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
G01 X50 F50 // Move to X 50 position
M226 P37 S1 // Waint for pin 37 to change to 1 logic satus, at this point movement its stoped
G01 X0 F200 // Move to X 0 position, the movement is restarted when pin 37 change mode
```



### How I implemented it

#### Command M42

Normally grbl is not written to interpret the M24 command and its format with parameters is why it was necessary to modify the file **gcode.h**

```c
// Added ignore M42 and M226 command to procces block in ports.c
  while (line[char_counter] != 0 && !(letter=='M' && (int_value==42 || int_value==226))) { // Loop until no more g-code words in line.
```

In this way, an error will not occur when processing the complete code block since only the M42 command will be processed by Grbl and the rest of the parameters will fall on **ports.c** specifically in the function **ports_manage** who is in charge of processing the parameters. Within this function were used:

[pinMode()](https://www.arduino.cc/reference/en/language/functions/digital-io/pinmode/)

[digitalWrite()](https://www.arduino.cc/reference/en/language/functions/digital-io/digitalwrite/)

[digitalRead()](https://www.arduino.cc/reference/en/language/functions/digital-io/digitalread/)

Its full implementation can be seen [here](https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/wiring_digital.c)

This functions is called from **gcode.c** in the switch cases from M commands

```c
case 42:
          //Execute M42 function and if the pin is blocked return error
            if(!ports_manage(line)){FAIL(STATUS_GCODE_PIN_LOCKED);}
          break;
```

#### Command M226

As has been described in **M24** command  the while loop in **gcode.c** exclude the **M226** command block to be process in **ports.c** 

```c
// Added ignore M42 and M226 command to procces block in ports.c
  while (line[char_counter] != 0 && !(letter=='M' && (int_value==42 || int_value==226))) { // Loop until no more g-code words in line.
```

The function is called in the same way

```c
  		  //-----------------------------------
          // Implementing M226
          //-----------------------------------
          case 226:
            waintForPin(line);
          break;
          //-----------------------------------
```

The `waintForPin()` function is implemented as a while loop, it reads the pin state and keep in loop until its value is equal to the state pram, its params are extracted from line param, it is the command block recibed for M226.

```c
protocol_execute_realtime(); // Execute suspend.
  while(digitalRead(pinVal)!= stateVal){

  }
protocol_exec_rt_system();  // Executes run-time commands
```

`protocol_execute_realtime()`documentation can be found here.

`protocol_exec_rt_system()`documentation can be found here.

#### error:60 Pin Locked

Define error code in **report.h** as

`#define STATUS_GCODE_PIN_LOCKED 60`

**ports_manage** declared in **ports.c** function returns a boolean value

`bool ports_manage(char *line)`

This criteria is definded in **ports.c** in function

`bool pinBlocked(uint8_t pin)`

Who tell us if pin is blocked or not, you can add some pins in this function declaration to allow or not its use. In array of constants `workPin` declared in **ports.c** and the constants are defined in **ports.h**.



### You can also see others useful repos related with THC implementation

| Repo description                                             | Link                                                  |
| ------------------------------------------------------------ | ----------------------------------------------------- |
| ArdunioTHC                                                   | https://github.com/regeg/ArdunioTHC                   |
| Plasma THC                                                   | https://github.com/grblHAL/Plugin_plasma/             |
| DIY built CNC Plasma with Arduino THC (Torch Height Controller) | https://github.com/regeg/ArdunioTHC                   |
| CNC Motion Control Firmware for XmotionCNC Motion Control Boards or Arduino UNO DIY setup. Focused on CNC Plasma Cutting. | https://github.com/UnfinishedBusiness/XmotionFirmware |
| Arduino based THC that reads plasma cutter voltage and send Up and  Down signals to Plasma Torch Actuator to adjust voltage to target value. | https://github.com/HaleDesign/TorchHeightController   |
| CNC plugin for the plasma torch height controlling on the base of Arduino (ATmega328P MCU) | https://github.com/ussserrr/torch-height-control      |
| Plasma/THC plugin                                            | https://github.com/grblHAL/Plugin_plasma              |

