/*
 *  Created on: Aug 22, 2017
 *      Author: andri
 */

#include <Arduino.h>
#include <ESPectro32_Board.h>
#include <ESPectro32_RGBLED_Animation.h>

RgbLedColor_t aCol(200, 0, 80);
ESPectro32_RGBLED_GlowingAnimation glowAnim(ESPectro32.RgbLed(), aCol);

#include <ESPectro32_LedMatrix_Animation.h>
ESPectro32_LedMatrix_Animation ledMatrixAnim;

void begin() {
	ESPectro32.begin();
	ESPectro32.LED().setAnimation(ESPectro_LED_Animation_Fading, 3000, 3);

	glowAnim.start(3000, 3);
}

void loop() {

}