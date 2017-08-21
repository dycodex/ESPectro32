/*
 * ESPectro32_RGBLED.cpp
 *
 *  Created on: Aug 19, 2017
 *      Author: andri
 */

#include "ESPectro32_RGBLED.h"

ESPectro32_RGBLED::ESPectro32_RGBLED(gpio_num_t gpioNum):
WS2812(gpioNum, ESPECTRO32_RGBLED_COUNT) {
}

ESPectro32_RGBLED::~ESPectro32_RGBLED() {
}

