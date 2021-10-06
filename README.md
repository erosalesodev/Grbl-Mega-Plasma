# Grbl THC Integration Firmware

#### Features

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
  ```

#### New libraries List

| Library    | Description                                  |
| ---------- | -------------------------------------------- |
| adc        | Handle ADC module in Atmega2560              |
| timer      | Handle Timer2 configuration and interruption |
| thc        | THC Control                                  |
| thcreports | THC status reports                           |



#### Global Variables Added

|   Variable   |       Type        |              Description              | Declared |
| :----------: | :---------------: | :-----------------------------------: | :------: |
|  analogVal   | volatile uint16_t | Value to store analog result from ADC |   adc    |
|  hysteresis  | volatile uint16_t |      Allowable voltage deviation      |   thc    |
| analogSetVal | volatile uint16_t |       Value to stabilize in THC       |   thc    |
|  thcWorking  |       bool        |          THC working status           |   thc    |

#### Debug Functionalities

Debug functionalities are declared in **thcreport**use T command.

| Command | Function                              |
| ------- | ------------------------------------- |
| T-      | Show THC reference voltage            |
| T=xx    | Set THC voltage reference to xx value |

