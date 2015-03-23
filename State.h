#ifndef _INC_SERIALIZER_H
#define _INC_SERIALIZER_H
#include <EEPROM.h>
#define MAGIC_NUMBER 0xA0

enum Mode {
  SOLID,
  FADE
};

struct state {
	Mode mode;
	uint8_t brightness;
	Pixel fadeFrom;
	Pixel currentColor;
	Pixel fadeTo;
	volatile uint8_t button;
	boolean newCmd;
	float fadeSpeed;
	bool play;
} state;
extern struct state s;

void initState(struct state & toInit) {
	toInit.play = true;
	toInit.mode = SOLID;
	toInit.button = 0;
	toInit.newCmd = false;
	toInit.brightness = 255;
	toInit.fadeSpeed = .00005;
	toInit.currentColor = EMPTY_COLOR;
	toInit.fadeFrom = EMPTY_COLOR;
	toInit.fadeTo = EMPTY_COLOR;

}

void EUpdate(int position, uint8_t value) {
	if(EEPROM.read(position) != value)
		EEPROM.write(position, value);
}

float readFloat(int & position) {
	union {float f; uint32_t u32;} u;
	byte * v = (byte*)&u.u32;
	for(int i = 0; i < 4; i++)
		v[i] = EEPROM.read(position++);
	return u.f;
}

void writeFloat(int & position, float value) {
	union {float f; uint32_t u32;} u;
	u.f = value;
	byte * v = (byte*)&u.u32;
	for(int i = 0; i < 4; i++)
		EUpdate(position++, v[i]);
}

void writeColor(int & position, Pixel p) {
	EUpdate(position++, p.r);
	EUpdate(position++, p.g);
	EUpdate(position++, p.b);
}

Pixel readColor(int & position) {
	return cRGB(
		EEPROM.read(position++),
		EEPROM.read(position++),
		EEPROM.read(position++));
		
}

// Saves the given state at the given state index
void saveState(int stateNumber, struct state saving) {
  cli();
  // this size is likely wrong due to padding, but gives us as least as much space as we need
  int position = stateNumber*sizeof(struct state);
  EUpdate(position++,MAGIC_NUMBER);
  for(int i = 0; i < sizeof(struct state); i++)
	  EUpdate(position + i, ((uint8_t*)&saving)[i]);
/*  EUpdate(position++, saving.mode);
  EUpdate(position++, saving.brightness);
  EUpdate(position++, saving.button);
  writeColor(position, saving.currentColor);
  writeColor(position, saving.fadeFrom);
  writeColor(position, saving.fadeTo);
  writeFloat(position, saving.fadeSpeed);
  EUpdate(position++, saving.play);*/
  sei();
}

// Loads the current state from EEPROM. Returns default values if there is nothing at that position
struct state loadState(int stateNumber) {
  cli();
  struct state loading;
  // this size is likely wrong due to padding, but gives us as least as much space as we need
  int position = stateNumber*sizeof(struct state);
  if(EEPROM.read(position++) == MAGIC_NUMBER) {
	  for(int i = 0; i < sizeof(struct state); i++)
		  ((byte*)&loading)[i] = EEPROM.read(position+i);
	  /*
	  loading.mode = (Mode)EEPROM.read(position++);
	  loading.brightness = EEPROM.read(position++);
	  uint8_t b = loading.button;
	  loading.button = EEPROM.read(position++);
	  if(b != loading.button)
		  loading.newCmd = true;
	  loading.currentColor = readColor(position);
	  loading.fadeFrom = readColor(position);
	  loading.fadeTo = readColor(position);
	  loading.fadeSpeed = readFloat(position);
	  loading.play = EEPROM.read(position++);*/
  }
  else
	  initState(loading);
  sei();
  return loading;
}

void copyState(struct state & to, struct state from) {
	to = from;
}
#endif