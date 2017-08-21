/*
 * ESPectro32.h
 *
 *  Created on: Aug 8, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_BOARD_H_
#define COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_BOARD_H_

#include <esp_log.h>
#include <WS2812.h>
#include <Arduino.h>
#include <Wire.h>
#include "ESPectro32_Constants.h"
#include "ESPectro32_LED.h"
#include "ESPectro32_RGBLED.h"
#include "ESPectro32_Button.h"
#include "ESPectro32_LedMatrix.h"
#include <Arduino.h>
#include <Wire.h>
#include <driver/adc.h>

#define ESPECTRO32_DEBUG_PRINT(...)  ESP_LOGD("ESPECTRO32", __VA_ARGS__);
#define ESPECTRO32_INFO_PRINT(...)   ESP_LOGI("ESPECTRO32", __VA_ARGS__);
#define ESPECTRO32_ERROR_PRINT(...)  ESP_LOGE("ESPECTRO32", __VA_ARGS__);

/**
 * @brief The holy grail of ESPectro32. The gate to awesomeness of the board.
 */
class ESPectro32_Board {
public:
	ESPectro32_Board();
	virtual ~ESPectro32_Board();

	/**
	 * @brief Must be called as soon as possible to do initialization stuffs.
	 */
	bool begin();

	ESPectro32_RGBLED& RgbLed();

	//LED convenient methods
	/**
	 * @brief Get LED object.
	 *
	 * This method is for getting LED object so that you can work with it, either turn it on/off, blink it, toggle it, or fade it in/out.
	 *
	 * @return The LED object.
	 */
	ESPectro32_LED &LED();
	void turnOnLED();
	void turnOffLED();
	void blinkLED(uint32_t interval = 500, uint32_t count = UINT16_MAX);
	void fadeLED(uint32_t duration = 1000, uint32_t count = UINT16_MAX);
	void stopLEDAnimation();
	void toggleLED();

	//Buttons
	/**
	 * @brief Get Button "A" object.
	 *
	 * This method is for getting Button "A" object so that you can attach event handlers in order to detect click, double click, and long press.
	 * Button A is connected to GPIO 0.
	 *
	 * @return The Button A object.
	 */
	ESPectro32_Button &ButtonA();

	/**
	 * @brief Get Button "B" object.
	 *
	 * This method is for getting Button "B" object so that you can attach event handlers in order to detect click, double click, and long press.
	 * Button A is connected to GPIO 4.
	 *
	 * @return The Button B object.
	 */
	ESPectro32_Button &ButtonB();

	/**
	 * @brief Get Led Matrix object.
	 *
	 * This method is for getting Led Matrix object so that you can start working with it.
	 * You can use it to display shapes, icons, text, or pixel-by-pixel manipulation.
	 *
	 * @return The Led Matrix object.
	 */
	ESPectro32_LedMatrix &LedMatrix();

	//Analog
	/**
	 * @brief Conveniently read analog data
	 *
	 * @param[in] channel ADC1 channel to read. Values can be: ADC1_CHANNEL_0 - ADC1_CHANNEL_7
	 * @param[in] bitWidth How wide the bit to capture. Values can be: ADC_WIDTH_9Bit (511 max), ADC_WIDTH_10Bit (1023 max), ADC_WIDTH_11Bit (2047 max), ADC_WIDTH_12Bit (4095 max)
	 * @param[in] atten Attenuation level. Values can be: ADC_ATTEN_0db, ADC_ATTEN_2_5db, ADC_ATTEN_6db, ADC_ATTEN_11db
	 *
	 * @return The analog reading in integer
	 */
	int readAnalog(adc1_channel_t channel, adc_bits_width_t bitWidth = ADC_WIDTH_12Bit, adc_atten_t atten = ADC_ATTEN_6db);

	/**
	 * @brief Conveniently read analog data converted to its voltage value
	 *
	 * @param[in] channel ADC1 channel to read. Values can be: ADC1_CHANNEL_0 - ADC1_CHANNEL_7
	 * @param[in] bitWidth How wide the bit to capture. Values can be: ADC_WIDTH_9Bit (511 max), ADC_WIDTH_10Bit (1023 max), ADC_WIDTH_11Bit (2047 max), ADC_WIDTH_12Bit (4095 max)
	 * @param[in] atten Attenuation level. Values can be: ADC_ATTEN_0db, ADC_ATTEN_2_5db, ADC_ATTEN_6db, ADC_ATTEN_11db
	 *
	 * @return The analog reading in Volt
	 */
	float readAnalogVoltage(adc1_channel_t channel, adc_bits_width_t bitWidth = ADC_WIDTH_12Bit, adc_atten_t atten = ADC_ATTEN_6db);
	int readPhotoTransistorValue(adc_bits_width_t bitWidth = ADC_WIDTH_12Bit, adc_atten_t atten = ADC_ATTEN_6db);
	float readPhotoTransistorVoltage(adc_bits_width_t bitWidth = ADC_WIDTH_12Bit, adc_atten_t atten = ADC_ATTEN_6db);

	int readOnBoardHallSensor();

	/**
	 * @brief Scan attached I2C peripherals
	 */
	void scanI2C();

private:
	ESPectro32_RGBLED *rgbLed_ = NULL;
	ESPectro32_LED *led_ = NULL;
	ESPectro32_Button *buttonA_ = NULL, *buttonB_ = NULL;
	ESPectro32_LedMatrix *ledMatrix_ = NULL;
};

extern ESPectro32_Board ESPectro32;

#endif /* COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_BOARD_H_ */
