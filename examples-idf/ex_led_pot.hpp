/*
 * ex_led_pot.hpp
 *
 *  Created on: Sep 2, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_ESPECTRO32_EXAMPLES_IDF_EX_LED_POT_HPP_
#define COMPONENTS_ESPECTRO32_EXAMPLES_IDF_EX_LED_POT_HPP_

#include <esp_log.h>
#include <ESPectro32_Board.h>

ESPectro32_LED led(32, true);

const static char *TAG_EX_LED_POT ="LED POT";

static void load_ex_led_pot(){
	for(;;) {

	//		ESP_LOGI(TAG_EX_LED_POT, "ADC: %d %f",
	//				ESPectro32.readAnalog(ADC1_CHANNEL_6),
	//				ESPectro32.readAnalogVoltage(ADC1_CHANNEL_6));
	//		vTaskDelay(500/portTICK_PERIOD_MS);

		int val = ESPectro32.readAnalog(ADC1_CHANNEL_6);
		float percent = val*1.0f/4095; //4095 because of 12 bit analog by default
		led.setBrightnessPercentage((int)(percent*100));

		vTaskDelay(10/portTICK_PERIOD_MS);
	}
}


#endif /* COMPONENTS_ESPECTRO32_EXAMPLES_IDF_EX_LED_POT_HPP_ */
