/**
 *  pwm.h PWM control
 * */

#ifndef pwm_h
#define pwm_h

    void pwmTimerFreq(uint8_t timer,uint8_t mode);                      // Set the frequenzy prescaler
    void pwmConfTimer(uint8_t timer);                                   // Configure timer mode
    void pwmChannelEnable(uint8_t timer,uint8_t channel,int value);     // Enable channel output
    void pwmConfCount(uint8_t timer,uint32_t value);                    // Configure maximum count value
    uint8_t pwmManage(char *line);                                      // Manage command
    void pwmTest();
    

#endif