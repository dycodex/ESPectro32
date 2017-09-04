/*
 *  Created on: Aug 22, 2017
 *      Author: andri
 */

#include <Arduino.h>
#include <ESPectro32_Board.h>

void setup() {
	Serial.begin(9600);
	ESPectro32.begin();
}

void loop() {
	Serial.printf("Photo Transistor: %d %f, Hall Sensor: %d",
		ESPectro32.readPhotoTransistorValue(),
		ESPectro32.readPhotoTransistorVoltage(),
		ESPectro32.readOnBoardHallSensor());
	
	delay(500);
}

