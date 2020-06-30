/*
 * Rotary encoder library for Arduino.
 */

#ifndef rotary_h
#define rotary_h

#include "Arduino.h"

// Enable this to emit codes twice per step.
//#define HALF_STEP

// Enable weak pullups
#define ENABLE_PULLUPS

// Values returned by 'process'
// No complete step yet.
#define DIR_NONE 0x0
// Clockwise step.

#ifdef ROTARY_ACTIVE_LOW
#define DIR_CW 0x20
// Anti-clockwise step.
#define DIR_CCW 0x10
#else
#define DIR_CW 0x10
// Anti-clockwise step.
#define DIR_CCW 0x20
#endif

class Rotary
{
  public:
    Rotary(char, char);
    // Process pin(s)
    unsigned char process();
  private:
    unsigned char state;
    unsigned char pin1;
    unsigned char pin2;
};

#endif
 
