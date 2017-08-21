/*
 * ex_button.hpp
 *
 *  Created on: Aug 22, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_ESPECTRO32_EXAMPLES_IDF_EX_BUTTON_HPP_
#define COMPONENTS_ESPECTRO32_EXAMPLES_IDF_EX_BUTTON_HPP_

#include <esp_log.h>
#include <ESPectro32_Board.h>

const static char *TAG_EX_BUTTON ="BTN";

static void load_ex_button() {
	//Button handlers
	ESPectro32.ButtonA().onButtonUp([]() {
		ESP_LOGI(TAG_EX_BUTTON, "Button A up");
	});

	ESPectro32.ButtonA().onDoublePressed([]() {
		ESP_LOGI(TAG_EX_BUTTON, "Button A doubled");
	});

	ESPectro32.ButtonB().onButtonUp([]() {
		ESP_LOGI(TAG_EX_BUTTON, "Button B up");
	});
}

#endif /* COMPONENTS_ESPECTRO32_EXAMPLES_IDF_EX_BUTTON_HPP_ */
