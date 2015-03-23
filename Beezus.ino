#include "PinChangeInterruptSimple.h"
#include "Pixel.h"
#include "LEDFxUtilities.h"
#include <EEPROM.h>
#include "State.h"
#include "IRReader.h"
#include "LEDStrip.h"

static bool record = false;
static bool setFadeMode = false;
struct state s;

void setup() {
	initStrip();
	readerInit(pressButton);
	// load the current state from EEPROM.
	s = loadState(0);
}


void loop() {
	checkPulse();
	if(s.mode == SOLID)
		showColor();
	else if (s.mode == FADE)
		fade();

}


// When an IR button press is detected, this is called with the index of the button.
void pressButton(int button) {
	if(s.newCmd) {
		if(button > 3 && button < 24) {
			Pixel color = colorWheel(button-4);
			setColor(color);
			if(setFadeMode) {
				if(equals(s.fadeFrom,EMPTY_COLOR)) {
					s.fadeFrom = color;
				}
				else {
					setFade(s.fadeFrom, color);					
					setFadeMode = false;
				}
			}
		}
		else {
			switch(button) {
			case 0:
				s.brightness = clampAdd(s.brightness, brightnessIncrement);
				break;
			case 1:
				s.brightness = clampSubtract(s.brightness, brightnessIncrement);
				break;
			case 2:
				s.play = !s.play;
				break;
			case 3:
				if(s.brightness > 0)
					s.brightness = 0;
				else
					s.brightness = 255;
				break;
				// color increment/decrement
			case 24: incrementColor(s.currentColor.r); incrementColor(s.fadeFrom.r); incrementColor(s.fadeTo.r); break; 
			case 25: incrementColor(s.currentColor.g); incrementColor(s.fadeFrom.g); incrementColor(s.fadeTo.g); break; 
			case 26: incrementColor(s.currentColor.b); incrementColor(s.fadeFrom.b); incrementColor(s.fadeTo.b); break; 
			case 28: decrementColor(s.currentColor.r); decrementColor(s.fadeFrom.r); decrementColor(s.fadeTo.r); break; 
			case 29: decrementColor(s.currentColor.g); decrementColor(s.fadeFrom.g); decrementColor(s.fadeTo.g); break; 
			case 30: decrementColor(s.currentColor.b); decrementColor(s.fadeFrom.b); decrementColor(s.fadeTo.b); break; 
				// Speed
			case 27: s.fadeSpeed *= 1.1; break;
			case 31: s.fadeSpeed /= 1.1; break;

			case 35: record = true; break;
			case 39: setFadeMode = true; 
				s.fadeFrom = EMPTY_COLOR;
				s.fadeTo = EMPTY_COLOR;
				break;
				// DIY
			case 32: case 33: case 34: case 36: case 37: case 38:
				if(record) {
					saveState(button-31, s);
					record = false;
				}
				else
					copyState(s, loadState(button-31));
				break;

				// Builtin
			case 40: setStripes(); break;
			case 41: setFlowers(); break;
			case 42: setGrass(); break;
			case 43: setWings(); break;
			}
		}
		saveState(0, s);
		s.newCmd = false;
	}
}



void setStripes() {
	setFade(cRGB(255,105,0),	// from
		cRGB(255,40,0),			// to
		140,					// brightness
		.00005);				// speed
}

void setFlowers() {
	setFade(cRGB(185,0,255),	// from
		cRGB(255, 0, 181),			// to
		140,					// brightness
		.00005);				// speed
}

void setGrass() {
	setFade(cRGB(25,255,0),	// from
		cRGB(0,255,10),			// to
		140,					// brightness
		.00005);				// speed
}

void setWings() {
	setFade(cRGB(255,255,75),	// from
		cRGB(64,190,255),			// to
		120,					// brightness
		.00005);				// speed
}


