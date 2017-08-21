/*
 * ESPectro32_LedMatrix.h
 *
 *  Created on: Aug 19, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_LEDMATRIX_H_
#define COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_LEDMATRIX_H_

#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>
#include <functional>

static const uint8_t PROGMEM LED_MATRIX_EMOTICON_SMILE[] =
{ B00000000,
  B01101100,
  B01101100,
  B00000000,
  B01000100,
  B00111000,
  B00000000
};

static const uint8_t PROGMEM LED_MATRIX_EMOTICON_SAD[] =
{ B00000000,
  B00101000,
  B01000100,
  B00000000,
  B00000000,
  B00111000,
  B01000100,
  B00000000
};

static const uint8_t PROGMEM LED_MATRIX_ICON_HEART[] =
{ B01101100,
  B11101110,
  B11111110,
  B01111100,
  B00111000,
  B00010000,
  B00000000,
  B00000000
};

static const uint8_t PROGMEM LED_MATRIX_ICON_HEART_OUTLINE[] =
{ B01101100,
  B10101010,
  B10111010,
  B01000100,
  B00101000,
  B00010000,
  B00000000,
  B00000000
};

static const uint8_t PROGMEM LED_MATRIX_ICON_HEART_SMALL[] =
{ B00000000,
  B01101100,
  B01111100,
  B00111000,
  B00010000,
  B00000000,
  B00000000,
  B00000000
};

//Add your own icon bitmap :)

/**
 * @brief The class represents Led Matrix
 *
 * It overrides drawPixel method of its base class, as the matrix size is different.
 */
class ESPectro32_LedMatrix: public Adafruit_IS31FL3731 {
public:
	ESPectro32_LedMatrix();
	virtual ~ESPectro32_LedMatrix();
	void drawPixel(int16_t x, int16_t y, uint16_t color);
};

#endif /* COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_LEDMATRIX_H_ */
