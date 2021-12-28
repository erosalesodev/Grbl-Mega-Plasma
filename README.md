# Grbl THC Integration Firmware

[TOC]

This library is written based on [grbl-Mega](https://github.com/gnea/grbl-Mega) a documentation can be found [here](https://tgit-tech.github.io/grbl-Mega-Doxy/index.html)

### Features

#### Configuration manual

- Chip select in file **config.h**

```c
// To use with RAMPS 1.4 Board, comment out the above defines and uncomment the next two defines
#define DEFAULTS_RAMPS_BOARD
#define CPU_MAP_2560_RAMPS_BOARD

```

- New libraries are added in **grbl.h**

  ```c
  // THC Integration features
  #include "adc.h"
  #include "timer.h"
  #include "thc.h"
  #include "thc_reports.h"
  #include "ports.h"
  ```

#### New libraries List

|  Library   | Description                                  |
| :--------: | -------------------------------------------- |
|    adc     | Handle ADC module in Atmega2560              |
|   timer    | Handle Timer2 configuration and interruption |
|    thc     | THC Control                                  |
| thcreports | THC status reports                           |
|   ports    | Ports management                             |

#### Global Variables Added

|   Variable   |       Type        | Description                           | Declared |
| :----------: | :---------------: | :------------------------------------ | :------: |
|  analogVal   | volatile uint16_t | Value to store analog result from ADC |   adc    |
|  hysteresis  | volatile uint16_t | Allowable voltage deviation           |   thc    |
| analogSetVal | volatile uint16_t | Value to stabilize in THC             |   thc    |
|  thcWorking  |       bool        | THC working status                    |   thc    |

#### Debug Functionalities

Debug functionalities are declared in **thcreport** use T command.

| Command | Function                              |
| :-----: | :------------------------------------ |
|   T-    | Show THC reference voltage            |
|  T=xx   | Set THC voltage reference to xx value |

#### Ports Functionalities

The port functionalities implemented allow to carry out new operations such as waiting for signal or reading and writing on pins. This allows adding new functionalities to the hardware such as valve control, waiting for the Arc Ok signal, among others.

##### M42: Switch I/O pin

Reference implementation and documentation can be found [here](https://reprap.org/wiki/G-code#M42:_Switch_I.2FO_pin): 

**Command use cases** 

`M42 Pxx Mx		// Set pin  mode`

`M42 Pxx Sx		// Set pin  state`

`M42 Pxx R		// Read pin`

**Parameters**

`P<xx> Pin number`

`<xx> This value must be between 1 and 99`

| Response | Description                   |
| -------- | ----------------------------- |
| ok       | Command executed successfully |
| error:60 | Pin locked                    |

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

**Example**

```
M42 P7 M2		// Set pin 7 to digital out
M42 P7 S1       // Set pin 7 to 1 logic
M42 P7 R		// Read pin state
```

#### How I implemented it

##### Command M42

Normally grbl is not written to interpret the M24 command and its format with parameters is why it was necessary to modify the file **gcode.h**

```
// Added ignore M42 command to procces block in ports.c
while (line[char_counter] != 0 && !(letter=='M' && int_value==42)) { // Loop until no more g-code words in line.
```

In this way, an error will not occur when processing the complete code block since only the M42 command will be processed by Grbl and the rest of the parameters will fall on **ports.c** specifically in the function **ports_manage** who is in charge of processing the parameters. Within this function were used:

[pinMode()](https://www.arduino.cc/reference/en/language/functions/digital-io/pinmode/)

[digitalWrite()](https://www.arduino.cc/reference/en/language/functions/digital-io/digitalwrite/)

[digitalRead()](https://www.arduino.cc/reference/en/language/functions/digital-io/digitalread/)

Its full implementation can be seen [here](https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/wiring_digital.c)

This functions is called from **gcode.c** in the switch cases from M commands

```
case 42:
          //Execute M42 function and if the pin is blocked return error
            if(!ports_manage(line)){FAIL(STATUS_GCODE_PIN_LOCKED);}
          break;
```



##### error:60 Pin Locked

Define error code in **report.h** as

`#define STATUS_GCODE_PIN_LOCKED 60`

**ports_manage** declared in **ports.c** function returns a boolean value

`bool ports_manage(char *line)`

This criteria is definded in **ports.c** in function

`bool pinBlocked(uint8_t pin)`

Who tell us if pin is blocked or not, you can add some pins in this function declaration to allow or not its use.
