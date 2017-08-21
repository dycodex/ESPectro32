/*
 * ex_adc.hpp
 *
 *  Created on: Aug 22, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_ESPECTRO32_EXAMPLES_IDF_EX_ADC_HPP_
#define COMPONENTS_ESPECTRO32_EXAMPLES_IDF_EX_ADC_HPP_

#include <esp_log.h>
#include <ESPectro32_Board.h>

const static char *TAG_EX_ADC ="ADC";

static void load_ex_adc(){
	for(;;) {
		ESP_LOGI(TAG_EX_ADC, "Photo TR: %d %f, Hall Sensor: %d",
				ESPectro32.readPhotoTransistorValue(),
				ESPectro32.readPhotoTransistorVoltage(),
				ESPectro32.readOnBoardHallSensor());
		vTaskDelay(500/portTICK_PERIOD_MS);
	}
}

#endif /* COMPONENTS_ESPECTRO32_EXAMPLES_IDF_EX_ADC_HPP_ */
