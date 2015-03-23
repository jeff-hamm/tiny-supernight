#ifndef _INC_IRREADER_H
#define _INC_IRREADER_H
#include "State.h"
#define IR_PIN 3
const static int NUM_CODES = 44;
static const uint8_t codes[] = {
  92,93,65,64,
  //------------------
  88,89,69,68,
  84,85,73,72,
  80,81,77,76,
  28,29,30,31,
  24,25,26,27,
  //-------------------
  20,21,22,23,
  16,17,18,19,
  12,13,14,15,
  8,9,10,11,
  //-------------------
  4,5,6,7
};

static volatile int pinState = HIGH;
static volatile unsigned long lastChange = 0;
static volatile unsigned long currentTime;
static volatile unsigned long pulseLen;
static volatile boolean pulse = false;
static void (*pressButtonCB)(int);

// Detects a change in state on the IR pin and records the relevant details. 
void IRPinServiceRoutine() { 
  pinState = !pinState;
  // timing won't be exact, because we're running in here, hopefully this'll do though   
  currentTime = micros();
  pulseLen = currentTime - lastChange;
  lastChange = currentTime;
  pulse = true;
}

void readerInit(void (*buttonCallback)(int)) {
  pinMode(IR_PIN, INPUT);
  attachPcInterrupt(3, IRPinServiceRoutine, CHANGE);
  pressButtonCB = buttonCallback;
}

uint8_t readByte() {
  byte retVal = 0;
  for(int i = 0; i < 8; i++) {
    byte b = 0;
    pulse = false;
    while(!pulse);
    // both bits start with a 562 us LOW pulse
    if(pulseLen < 400 || pulseLen > 700)
      return 0;
    pulse = false;
    while(!pulse);
    // 1 bits are then HIGH for 1.6875ms
    if(pulseLen > 1300 && pulseLen < 2000)
      b = 1;
    // 0 bits are HIGH for 564 us
    else if( pulseLen > 200 && pulseLen < 800)
      b = 0;
    else
      return 0;
    retVal |= b << i;
  }
  return retVal;
}

// Checks for an IR pulse, calls the button press calback with the button index if a pulse is found.
boolean checkPulse() {
  // if we got a pulse, hang out here 'till we got the whole signal. 
  // the first change should be HIGH > LOW
  if(pulse && pinState == LOW ) {
    if(pulseLen < 45000 )
      return false;

    pulse = false;
    while(!pulse);
    // first pulse should have length ~9ms and leave us HIGH
    if(pulseLen < 5000 || pulseLen > 10000 )
      return false;
    // then we should hang HIGH for 4.5ms then drop LOW
    pulse = false;
    while(!pulse);
    if(pulseLen > 4000 && pulseLen < 5000) {
      // read the address
      int  a1 = readByte();
      int a2 = readByte();
      int c1 = readByte();
      if(c1 == 0)
        return false;
      int c2 = readByte();
      if(c2 == 0)
        return false;
      if((c1 & c2) != 0)
        return false;
      s.button = c1;
      s.newCmd = true;
 
	  for(int i = 0; i < NUM_CODES; i++) {
		if(codes[i] == s.button) {
		  pressButtonCB(i);
		}
	  }
      return true;
    }
    // repeat code
    else if(pulseLen >2000 && pulseLen < 3000) {
      pulse = false;
      while(!pulse);
      // 562 us LOW pulse to end the repeat signal
      if(pulseLen < 400 || pulseLen > 700)
        return false;

      s.newCmd = true;
 
	  for(int i = 0; i < NUM_CODES; i++) {
		if(codes[i] == s.button) {
		  pressButtonCB(i);
		}
	  }
      return true;
    }
    else {
      return false;
    }
  }
  return false;
}

boolean delayPulse(int delay) {
  unsigned long startTime = millis();
  while((millis() - startTime) < delay)
    if(checkPulse())
      return true;
  return false;
} 



uint8_t scale8( uint8_t i, uint8_t scale)
{
  return ((int)i * (int)(scale) ) >> 8;
}

#endif