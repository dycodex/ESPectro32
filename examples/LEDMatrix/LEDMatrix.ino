/*
 *  Created on: Aug 22, 2017
 *      Author: andri
 */



#include <Arduino.h>
#include <ESPectro32_Board.h>

#include <ESPectro32_LedMatrix_Animation.h>
ESPectro32_LedMatrix_Animation ledMatrixAnim;

void begin() {
	ESPectro32.begin();

	ledMatrixAnim.setLedMatrix(ESPectro32.LedMatrix());
	ledMatrixAnim.addFrameWithData((uint8_t*)LED_MATRIX_ICON_HEART);
	ledMatrixAnim.addFrameWithData((uint8_t*)LED_MATRIX_ICON_HEART_OUTLINE);
	ledMatrixAnim.addFrameWithData((uint8_t*)LED_MATRIX_ICON_HEART);
	ledMatrixAnim.addFrameWithData((uint8_t*)LED_MATRIX_ICON_HEART_OUTLINE);
	ledMatrixAnim.addFrameWithDataCallback([](ESPectro32_LedMatrix &ledM) {
		ledM.drawCircle(3, 3, 3, 200);
	});
	ledMatrixAnim.addFrameWithDataCallback([](ESPectro32_LedMatrix &ledM) {
		ledM.fillCircle(3, 3, 3, 200);
	});

	ledMatrixAnim.start(3000);
}

void loop() {

}