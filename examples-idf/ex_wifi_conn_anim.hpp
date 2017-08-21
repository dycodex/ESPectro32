/*
 * wifi_conn_anim.hpp
 *
 *  Created on: Aug 21, 2017
 *      Author: andri
 */

#ifndef MAIN_EXAMPLES_WIFI_CONN_ANIM_HPP_
#define MAIN_EXAMPLES_WIFI_CONN_ANIM_HPP_

#include <esp_log.h>
#include "sdkconfig.h"
#include <Arduino.h>

#include <ESPectro32_Board.h>

#include <WiFiManager.h>
WiFiManager wifiMgr;

#include <ESPectro32_LedMatrix_Animation.h>
ESPectro32_LedMatrix_Animation ledMatrixAnim;
ESPectro32_LedMatrix_ScrollTextAnimation ledMatrixTextAnim;

static const uint8_t PROGMEM LED_MATRIX_WIFI_1[] =
{ B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00010000,
  B00010000,
  B00000000
};

static const uint8_t PROGMEM LED_MATRIX_WIFI_2[] =
{ B00000000,
  B00000000,
  B00111000,
  B01000100,
  B00010000,
  B00010000,
  B00000000
};

static const uint8_t PROGMEM LED_MATRIX_WIFI_3[] =
{ B01111100,
  B10000010,
  B00111000,
  B01000100,
  B00010000,
  B00010000,
  B00000000
};

static void load_ex_wifi_conn_anim() {

	ledMatrixAnim.setLedMatrix(ESPectro32.LedMatrix());
	ledMatrixTextAnim.setLedMatrix(ESPectro32.LedMatrix());

	ledMatrixAnim.addFrameWithData((uint8_t*)LED_MATRIX_WIFI_1);
	ledMatrixAnim.addFrameWithData((uint8_t*)LED_MATRIX_WIFI_2);
	ledMatrixAnim.addFrameWithData((uint8_t*)LED_MATRIX_WIFI_3);
	ledMatrixAnim.addFrameWithDataCallback([](ESPectro32_LedMatrix &ledM) {
		ledM.clear();
	});

	ledMatrixAnim.start(1800, true);

	wifiMgr.onWiFiConnected([](bool newConn) {
		ESP_LOGI("WIFI", "IP: %s", wifiMgr.getStationIpAddress().c_str());
	});

	wifiMgr.begin(WIFI_MODE_STA, true);

	//Actually start
	wifiMgr.start();

	if (wifiMgr.waitForConnection()) {
		ledMatrixAnim.stop();

		delay(1000);

		ledMatrixTextAnim.onAnimationCompleted([]() {
			delay(1000);
			ESPectro32.LedMatrix().displayFrame(2); //display third frame as ending
		});

		ledMatrixTextAnim.scrollText(wifiMgr.getStationIpAddress().c_str(), 5000);
	}
}


#endif /* MAIN_EXAMPLES_WIFI_CONN_ANIM_HPP_ */
