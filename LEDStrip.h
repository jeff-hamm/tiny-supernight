#ifndef _INC_LEDSTRIP_H
#define _INC_LEDSTRIP_H
#include "Pixel.h"
#include "State.h"
#define GREEN_PIN 0
#define RED_PIN 1
#define BLUE_PIN 4
static const int brightnessIncrement = 20;
static const int colorIncrement = 5;
int rgb_pins[] = { RED_PIN, GREEN_PIN, BLUE_PIN};

float fadePos = 0;
int fadeDirection = 1;


void initStrip() {
	pinMode(GREEN_PIN, OUTPUT);
	pinMode(RED_PIN, OUTPUT);
	pinMode(BLUE_PIN, OUTPUT);
}

void setFade(Pixel from, Pixel to, int brightness = -1, float fadeSpeed = -1) {
	s.fadeFrom = from;
	s.fadeTo = to;
	s.currentColor = from;
	if(brightness > -1 && brightness < 256)
		s.brightness = brightness;
	if(fadeSpeed >= 0)
		s.fadeSpeed = fadeSpeed;
	s.mode = FADE;
}

static Pixel colorWheel(int number) {
	// the indeces are numbered from 0-20 
	int row = number/4;
	int column = number %4;
	uint8_t h,sat,v;
	// the third column is shades of white
	if(column == 3) {
		// we'll use a cyan
		h = 128;
		sat = (row*51);
	}
	// the rest are typical hues
	else {
		sat = 255;
		// columns set the start position on the wheel
		h = 0 + (85*column);
		// rows are the distance between
		h += 17*row;
	}
	v = s.brightness;
	return LEDFxUtilities::HSVtoRGB(h,sat,v);
}

void setColor(Pixel p) {
	s.currentColor = p;
	s.mode = SOLID;
}

void setColor(uint8_t r, uint8_t g, uint8_t b) {
	setColor(cRGB(r,g,b));
}


uint8_t clampAdd(uint8_t v, uint8_t i) {
	if(v < (255-i))
		return v + i;
	else
		return 255;
} 

uint8_t clampSubtract(uint8_t v, uint8_t i) {
	if( v > i)
		return v - i;
	else
		return 0;
}
void incrementColor(uint8_t & c) {
	c = clampAdd(c, colorIncrement);
}
void decrementColor(uint8_t & c) {
	c = clampSubtract(c, colorIncrement);
}

void showColor() {
	analogWrite(RED_PIN, scale8(s.currentColor.r, s.brightness));
	analogWrite(GREEN_PIN, scale8(s.currentColor.g, s.brightness));
	analogWrite(BLUE_PIN, scale8(s.currentColor.b, s.brightness));
}

void fade() {
	if(s.play) {
		fadePos += (s.fadeSpeed*(float)fadeDirection);
		if(fadePos > 1.0) {
			fadePos = 1;
			fadeDirection = -1;
		}
		if(fadePos < 0) {
			fadeDirection = 1;
			fadePos = 0;
		}
		s.currentColor = LEDFxUtilities::interpolate(s.fadeFrom, s.fadeTo, fadePos);
	}
	showColor(); 
}




#endif
