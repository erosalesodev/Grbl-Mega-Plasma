/*
* ports.h Ports control
*/
#ifndef ports_h
#define ports_h
            uint8_t ports_manage(char *line);                     // Manage protocol for ports usages
            uint8_t waintForPinSync(char *line);                  // Implements waint for pin Sync
            uint8_t waintForPinAsync(char *line);                 // Implements waint for pin Async
            void alarmsInit();                                    // Initialize alarms
            void alarmsDisable();                                 // Initialize alarms
            void alarmDisable(char port,uint8_t bit);             // Disable alarm  
            void alarmEnable(char port,uint8_t bit);              // Enable alarm
            void movementRestore();                               // Restore movement  
#endif

// Port interruptions
#define POWER_SOURCE_FAULT_BIT 5
#define ALARM_TOURCH_SIGNAL_BIT 6
#define ALARM_OUT_SERVO_X1_BIT 7
#define PORTB_PCINTERRUPT_STATE_MASK 0b10111000

bool alarmTriggered;
char activeAlarmPort;
uint8_t activeAlarmBit;

// Maping work pins
#define STEP_X 54
#define STEP_Y 60
#define STEP_Z 46
#define STEP_A 26

#define DIR_X 55
#define DIR_Y 61
#define DIR_Z 48
#define DIR_A 28

#define ENABLE 40

#define RELAY_1 38
#define RELAY_2 39

#define OUT_1 41
#define OUT_2 42
#define OUT_3 43
#define OUT_4 44
#define OUT_5 45
#define OUT_6 47
#define OUT_7 49

#define MAX_X 3
#define MAX_Y 15
#define MAX_Z 19

#define MIN_X 2
#define MIN_Y 14
#define MIN_Z 18

#define IN_1 30
#define IN_2 29
#define IN_3 27
#define IN_4 25
#define IN_5 24
#define IN_6 32
#define IN_7 33
#define IN_8 34
#define IN_9 35
#define IN_10 36
#define IN_11 37

#define PROBE 31
#define PAUSE 23
#define STOP 22
#define PLAY 13

#define JOP_VEL_1 10
#define JOP_VEL_2 11
#define JOP_VEL_3 12

#define JOP_XA 4
#define JOP_XB 5
#define JOP_YA 6
#define JOP_YB 7
#define JOP_ZA 8
#define JOP_ZB 9

#define ANALOG_1 94
#define ANALOG_2 95
#define ANALOG_3 92
#define ANALOG_4 93

#define MISO 50
#define MOSI 51
#define SCK 52
#define CS 53