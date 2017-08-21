/*
 * ESPectro32_RGBLED.h
 *
 *  Created on: Aug 19, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_RGBLED_H_
#define COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_RGBLED_H_

#include <WS2812.h>
#include "ESPectro32_Constants.h"

/**
 * @brief RGB LED class
 *
 * You can use this class to work with on-board RGB LED based on WS2812/WS2813, or known as Neopixel. Hey, you can use it for externally connected WS2812/WS2813.
 *
 */
class ESPectro32_RGBLED: public WS2812 {
public:
	ESPectro32_RGBLED(gpio_num_t gpioNum = ESPECTRO32_RGBLED_GPIO);
	virtual ~ESPectro32_RGBLED();
};

#endif /* COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_RGBLED_H_ */
