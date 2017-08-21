/*
 * ESPectro32_LedMatrix.cpp
 *
 *  Created on: Aug 19, 2017
 *      Author: andri
 */

#include "ESPectro32_LedMatrix.h"

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

ESPectro32_LedMatrix::ESPectro32_LedMatrix() : Adafruit_IS31FL3731(7, 7) {
}

ESPectro32_LedMatrix::~ESPectro32_LedMatrix() {
}

void ESPectro32_LedMatrix::drawPixel(int16_t x, int16_t y, uint16_t color) {
	// check rotation, move pixel around if necessary
	switch (getRotation()) {
	case 1:
		_swap_int16_t(x, y);
		x = 7 - x - 1;
		break;
	case 2:
		x = 7 - x - 1;
		y = 7 - y - 1;
		break;
	case 3:
		_swap_int16_t(x, y);
		y = 7 - y - 1;
		break;
	}

	if ((x < 0) || (x >= 7) || (y < 0) || (y >= 7))
		return;

	if (x > 7) {
		// x=15-x;
		// y += 8;
		x = 7 - x;
		y += 7;
	} else {
		x = 7 - x - 1;
	}

	_swap_int16_t(x, y);

	if (color > 255)
		color = 255; // PWM 8bit max

	setLEDPWM(x + y * 16, color, _frame);
	return;
}
