/**
 *  thc.c THC control
 * */
#include "grbl.h"

//THC Initialize
void thc_initialize()
{
  // Set default values
  hysteresis = 1;
  analogSetVal = 3;
  thcWorking = false;
}

// THC Control
void thc_update()
{

  // Skip if THC is't on
  if (thcWorking)
  {
    if ((analogVal > (analogSetVal - hysteresis)) && (analogVal < (analogSetVal + hysteresis))) //We are within our ok range
    {
      jog_z_up = false;
      jog_z_down = false;
    }
    else //We are not in range and need to deterimine direction needed to put us in range
    {
      if (analogVal > analogSetVal) //Torch is too high
      {
        jog_z_down = true;
      }
      else //Torch is too low
      {
        jog_z_up = true;
      }
    }
  }
}
