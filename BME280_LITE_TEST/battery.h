#ifndef _BATTERY_H_
#define _BATTERY_H_

#include <Arduino.h>

float getVbatt();
uint8_t mapFloat(float x, float in_min, float in_max);

#endif