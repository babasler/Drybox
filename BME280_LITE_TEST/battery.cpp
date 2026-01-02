#include "battery.h"

float getVbatt()
{
  uint32_t Vbatt = 0;
  //for (int i = 0; i < 16; i++)
  //{
    //Vbatt += analogReadMilliVolts(A0); // Read and accumulate ADC voltage
  //}
  //return (2 * Vbatt / 16 / 1000.0); // Adjust for 1:2 divider and convert to volts
  return 50;
}

uint8_t mapFloat(float x, float in_min, float in_max)
{
  float val;
  val = (x - in_min) * (100) / (in_max - in_min);
  if (val < 0)
  {
    val = 0;
  }
  else if (val > 100)
  {
    val = 100;
  }
  return (uint8_t)val;
}