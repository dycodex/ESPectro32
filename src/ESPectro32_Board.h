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

/** \example examples/ADCRead/ADCRead.ino
 *  \example examples/Button/Button.ino
 *  \example examples/LEDMatrix/LEDMatrix.ino
 *  \example examples/Neopixel/Neopixel.ino
 *  \example examples-idf/ex_adc.hpp
 *  \example examples-idf/ex_sdcard.hpp
 *  \example examples-idf/ex_led_pot.hpp
 *  \example examples-idf/ex_button.hpp
 *  \example examples-idf/ex_leds_anim.hpp
 *  \example examples-idf/ex_wifi_conn_anim.hpp
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
	 * @param[in] pin GPIO number where the LED attached. Default to `ESPECTRO32_LED_PIN`
	 *
	 * @return The LED object.
	 */
	ESPectro32_LED &LED(uint8_t pin = ESPECTRO32_LED_PIN);
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

	void analogWrite(int pwmPin, uint32_t val);
	void stopPWM();

	/**
	 * @brief Call this method explicitly before any SD Card-related operations
	 */
	bool beginSDCard(uint8_t ssPin = ESPECTRO32_SDCARD_CSPIN);

private:
	ESPectro32_RGBLED *rgbLed_ = NULL;
	ESPectro32_LED *led_ = NULL;
	ESPectro32_Button *buttonA_ = NULL, *buttonB_ = NULL;
	ESPectro32_LedMatrix *ledMatrix_ = NULL;

	PWM *pwm_ = NULL;
	int pwmPin_ = 255;
	PWM *getPwmPtr(int pwmPin);
};

extern ESPectro32_Board ESPectro32;

#endif /* COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_BOARD_H_ */
