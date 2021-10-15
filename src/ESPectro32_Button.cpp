/*
 * ESPectro32_Button.cpp
 *
 *  Created on: Aug 8, 2017
 *      Author: andri
 */

#include "ESPectro32_Button.h"
//#define _GLIBCXX_USE_C99
#include <string>
//#include <GPIO.h>

/* the global instance pointer */
//static xQueueHandle button_evt_queue = NULL;

#if ESPECTRO32_BUTTON_USING_INT
#define ESPECTRO_BUTTON_INTERRUPTED_EVT		BIT0
static EventGroupHandle_t espectroButtonEventGroup_ = NULL;
#endif

ESPectro32_Button::ESPectro32_Button(uint8_t gpio, boolean activeHigh):
 Task("ESPectro32_Button_Task", 2048, configMAX_PRIORITIES - 2),
gpioNumber_(gpio), activeHigh_(activeHigh) {

	//std::string taskName = "ESPectro32_Button_Task" + std::to_string((int)gpioNumber_);

	char buf[30];
	sprintf(buf, "ESPectro32_Button_Task_%d", gpioNumber_);
	std::string taskName = std::string(buf);
	setTaskName(taskName);
}

ESPectro32_Button::~ESPectro32_Button() {

}

/*
static void IRAM_ATTR ESPectro32_Button_Interrupt(void *arg) {

	uint8_t gpio_num = (uint8_t) arg;
	xQueueSendFromISR(button_evt_queue, &gpio_num, NULL);

}
*/

void ESPectro32_Button::begin() {
	this->start(NULL);
}

ESPectro32_Button::Button_State ESPectro32_Button::getState() {
	return buttonState_;
}

void ESPectro32_Button::runAsync(void *data) {
//	uint8_t currGpioNumber = *((uint8_t*)data);

	//uint8_t io_num;
	for(;;) {

		/*
		if(xQueueReceive(button_evt_queue, &io_num, (100/portTICK_PERIOD_MS))) {
			//lastKnownLevel_ = gpio_get_level((gpio_num_t)io_num);
			bool val = gpio_get_level((gpio_num_t)io_num);
			printf("GPIO[%d] intr, val: %d, gpio: %d\n", io_num, val, currGpioNumber);
			if (io_num == currGpioNumber) {
				interruptTriggered_ = true;
				//if (!runAlreadyCalled_) {
				//	examine();
				//}
			}
		}
		*/

		//run();
		examine();
		vTaskDelay(10/portTICK_PERIOD_MS);
	}
}

#if ESPECTRO32_BUTTON_USING_INT
void IRAM_ATTR ESPectro32_Button_Interrupt(void *arg) {

	ESPectro32_Button *self = (ESPectro32_Button*) arg;
	self->triggerInterrupt();
}

void ESPectro32_Button::triggerInterrupt() {
	if  (espectroButtonEventGroup_ != NULL) {
		Serial.printf("=====================\n");
//		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//		xEventGroupSetBitsFromISR(espectroButtonEventGroup_, ESPECTRO_BUTTON_INTERRUPTED_EVT, &xHigherPriorityTaskWoken);
//		portYIELD_FROM_ISR();
	}
}

#endif

void ESPectro32_Button::start(void *taskData) {
	/*
	if (button_evt_queue == NULL) {
		button_evt_queue = xQueueCreate(10, sizeof(uint8_t));
	}
	*/

	pinMode(gpioNumber_, activeHigh_? INPUT_PULLDOWN: INPUT_PULLUP);

//	ESP32CPP::GPIO::setInput((gpio_num_t)gpioNumber_);
//	ESP32CPP::GPIO::setInterruptType((gpio_num_t)gpioNumber_, (gpio_int_type_t)GPIO_INTR_ANYEDGE);
//	ESP32CPP::GPIO::addISRHandler((gpio_num_t)gpioNumber_, ESPectro32_Button_Interrupt, (void*)gpioNumber_);

#if ESPECTRO32_BUTTON_USING_INT
	if  (espectroButtonEventGroup_ == NULL) {
		espectroButtonEventGroup_ = xEventGroupCreate();
	}

	xEventGroupClearBits(espectroButtonEventGroup_, ESPECTRO_BUTTON_INTERRUPTED_EVT);
	::attachInterruptArg(gpioNumber_, ESPectro32_Button_Interrupt, (void*)this, CHANGE);

#endif

	Task::start((void*)gpioNumber_);
}

void ESPectro32_Button::examine() {

	unsigned long now = millis(); // current (relative) time in msecs.

#if ESPECTRO32_BUTTON_USING_INT
//	EventBits_t uxBits = xEventGroupWaitBits(espectroButtonEventGroup_, ESPECTRO_BUTTON_INTERRUPTED_EVT, pdTRUE, pdFALSE, 10 / portTICK_PERIOD_MS);
	EventBits_t uxBits = xEventGroupWaitBits(espectroButtonEventGroup_, ESPECTRO_BUTTON_INTERRUPTED_EVT, pdFALSE, pdFALSE, portMAX_DELAY);
	if (uxBits & ESPECTRO_BUTTON_INTERRUPTED_EVT) {
		//lanjut
		xEventGroupClearBits(espectroButtonEventGroup_, ESPECTRO_BUTTON_INTERRUPTED_EVT);
	} else {
		return;
	}

#endif

	bool pressed = isActive();
//	Serial.printf("BTN CHANGED: %d\n", pressed);

	// Implementation of the state machine
	if (buttonState_ == ESPectro32ButtonUnknown) {
		if (pressed) {
			//printf("Pressed\n");
			buttonState_ = ESPectro32ButtonPressed;
			checkingStartTime_ = now; // remember starting time
		}
	}
	else if (buttonState_ == ESPectro32ButtonPressed) {
		if ((!pressed) && ((unsigned long) (now - checkingStartTime_) < ESPECTRO32_BUTTON_DEBOUNCE_DURATION_MS)) {
			// button was released too quickly, checking done
			buttonState_ = ESPectro32ButtonUnknown;
			printf("Debounced after pressed\n");

		} else if (!pressed) {
			buttonState_ = ESPectro32ButtonReleased;
			checkingEndTime_ = now; // remember stopping time
			//printf("Released\n");

		} else if ((pressed) && ((unsigned long) (now - checkingStartTime_) > ESPECTRO32_BUTTON_PRESS_DURATION_MS)) {
			//printf("Considered Down\n");
			if (btnDownCallback_) {
				btnDownCallback_();
			}

			if (longPressedCallback_)
				longPressedCallback_();

//				if (_duringLongPressFunc)
//					_duringLongPressFunc();

			trackLongPressed_ = true;  // Keep track of long press state
			buttonState_ = ESPectro32ButtonWaitingForLongPressed;
		} else {
			// wait. Stay in this state.
		}
	}
	else if (buttonState_ == ESPectro32ButtonReleased) {

		if (!doublePressedCallback_ || (unsigned long)(now - checkingStartTime_) > ESPECTRO32_BUTTON_CLICK_DURATION_MS) {
			// this was only a single short click
			//printf("Considered Up\n");
			if (btnUpCallback_)
				btnUpCallback_();
			buttonState_ = ESPectro32ButtonUnknown; // checking done.

		} else if ((pressed) && ((unsigned long)(now - checkingEndTime_) > ESPECTRO32_BUTTON_DEBOUNCE_DURATION_MS)) {
			buttonState_ = ESPectro32ButtonSecondPressed; // step to state 3
			checkingStartTime_ = now; // remember starting time
		}
	}
	else if (buttonState_ == ESPectro32ButtonSecondPressed) {

		if (!pressed && ((unsigned long) (now - checkingStartTime_) > ESPECTRO32_BUTTON_DEBOUNCE_DURATION_MS)) {
			// this was a 2 click sequence.
			if (doublePressedCallback_)
				doublePressedCallback_();
			//printf("Considered Double\n");
			buttonState_ = ESPectro32ButtonUnknown; // checking done.
		}
	}
	else if (buttonState_ == ESPectro32ButtonWaitingForLongPressed) {
	    if (!pressed) {
	    		//printf("Considered Long\n");
	    		trackLongPressed_ = false;  // Keep track of long press state
	    		if(longPressedCallback_)
	    			longPressedCallback_();
	    		buttonState_ = ESPectro32ButtonUnknown; // checking done.
	    } else {
	    		// button is being long pressed
	    		trackLongPressed_ = true; // Keep track of long press state
	    		//if (_duringLongPressFunc) _duringLongPressFunc();
	    }
	}
}

/*
void ESPectro32_Button::run() {
	runAlreadyCalled_ = true;
	unsigned long currentMillis = millis();

	if (!interruptTriggered_) {

		if ((currentMillis - lastButtonPressedMillis_ > ESPECTRO32_BUTTON_LONG_PRESS_DURATION_MS)) {

			if (buttonState_ == ESPectro32ButtonPressed) {
				buttonState_ = ESPectro32ButtonLongPressed;
				Serial.println(F("Considered Long Pressed"));
				if (longPressedCallback_) {
					longPressedCallback_();
				}
			}
			//pressCount_ = 0;
		}

		pressCount_ = 0;
	}
	else {

		interruptTriggered_ = false;

		if ((currentMillis - lastButtonChangedMillis_) > ESPECTRO32_BUTTON_DEBOUNCE_DURATION_MS) {
			lastButtonChangedMillis_ = currentMillis;
		}
		else {
			Serial.println(F("Debounced"));
			return;
		}

		bool pressed = isActive();
		if (pressed) {
			lastButtonPressedMillis_ = currentMillis;
			//lastButtonChangedMillis_ = currentMillis;
			buttonState_ = ESPectro32ButtonPressed;
			Serial.println(F("DOWN"));
			if (btnDownCallback_) {
				btnDownCallback_();
			}
		}
		else {
			//lastButtonReleasedMillis_ = currentMillis;
			Serial.println(F("Released"));

			if (btnUpCallback_) {
				btnUpCallback_();
			}

			if (buttonState_ != ESPectro32ButtonLongPressed && currentMillis - lastButtonPressedMillis_ > ESPECTRO32_BUTTON_PRESS_DURATION_MS) {
				buttonState_ = ESPectro32ButtonReleased;
				Serial.println(F("PRESSED"));
				if (pressedCallback_) {					
					pressedCallback_();
				}

				//TODO: Double pressed not working well
				//if (currentMillis - lastButtonPressedMillis_ > ESPECTRO32_BUTTON_PRESS_DURATION_MS) {
					pressCount_++;
					Serial.printf("Pressed count %d\n", pressCount_);
				//}

				if (pressCount_ == 2) {
					pressCount_ = 0;
					Serial.println(F("DOUBLE PRESSED"));
					if (doublePressedCallback_) {
						doublePressedCallback_();
					}
				}
			}
		}
	}
}
*/

void ESPectro32_Button::onButtonDown(ButtonActionCallback cb) {
	btnDownCallback_ = cb;
}

void ESPectro32_Button::onButtonUp(ButtonActionCallback cb) {
	btnUpCallback_ = cb;
}

void ESPectro32_Button::onPressed(ButtonActionCallback cb) {
	pressedCallback_ = cb;
}

void ESPectro32_Button::onLongPressed(ButtonActionCallback cb) {
	longPressedCallback_ = cb;
}

void ESPectro32_Button::onDoublePressed(ButtonActionCallback cb) {
	doublePressedCallback_ = cb;
}

bool ESPectro32_Button::isActive() {
	int buttonState = digitalRead(gpioNumber_);
	boolean pressed = activeHigh_ ? buttonState == HIGH : buttonState == LOW;
//	bool buttonState = ESP32CPP::GPIO::read((gpio_num_t)gpioNumber_);
//	bool pressed = activeHigh_ ? buttonState : !buttonState;
	return pressed;
}

