/*
 *  Created on: Aug 22, 2017
 *      Author: andri
 */

#include <Arduino.h>
#include <ESPectro32_Board.h>


void setup() {
	Serial.begin(9600);

	ESPectro32.ButtonA().onButtonUp([]() {
		Serial.println("Button A up");
	});

	ESPectro32.ButtonA().onDoublePressed([]() {
		Serial.println("Button A doubled");
	});

	ESPectro32.ButtonB().onButtonUp([]() {
		Serial.println("Button B up");
	});

	ESPectro32.ButtonB().onDoublePressed([]() {
		Serial.println("Button B doubled");
	});
}

void loop() {

}