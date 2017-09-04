//
// Created by Andri Yadi on 3/17/17.
//

#include "ESPectro32_LED.h"

// use first channel of 16 channels (started from zero)
#define LEDC_CHANNEL_0     0

// use 13 bit precission for LEDC timer
#define LEDC_TIMER_13_BIT  13

// use 5000 Hz as a LEDC base frequency
#define LEDC_BASE_FREQ     5000

#define LEDC_VALUE_MAX     255

// Arduino like analogWrite
// value has to be between 0 and valueMax
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = LEDC_VALUE_MAX) {
  // calculate duty, 8191 from 2 ^ 13 - 1
  uint32_t duty = (8191 / valueMax) * min(value, valueMax);

  // write duty to LEDC
  ledcWrite(channel, duty);
}

ESPectro32_LED::ESPectro32_LED(uint8_t pin, boolean activeHigh): pin_(pin), activeHigh_(activeHigh) {

}

ESPectro32_LED::~ESPectro32_LED() {

    if (animator_ != NULL) {
    		stopAnimation();
        delete animator_;
        animator_ = NULL;
    }

    disableBrightnessControl();
}

void ESPectro32_LED::begin() {
    pinMode(pin_, OUTPUT);
    turnOff();
}

void ESPectro32_LED::turnOn() {
    digitalWrite(pin_, activeHigh_? HIGH: LOW);
}

void ESPectro32_LED::turnOff() {
    digitalWrite(pin_, activeHigh_? LOW: HIGH);
}

boolean ESPectro32_LED::isOn() {
    int val = digitalRead(pin_);
    return activeHigh_? (val == HIGH): (val == LOW);
}

byte ESPectro32_LED::getPin() {
    return pin_;
}

void ESPectro32_LED::toggle() {
    if (isOn()) {
        turnOff();
    }
    else {
        turnOn();
    }
}

void ESPectro32_LED::blink(uint32_t interval, uint32_t count) {

    stopAnimation();

    getAnimatorPtr()->setAnimation(ESPectro_LED_Animation_Blink, interval);
    getAnimatorPtr()->setTimeout(interval*count);

    startAnimation();
}

void ESPectro32_LED::fade(uint32_t duration, uint32_t count) {

    stopAnimation();

    getAnimatorPtr()->setAnimation(ESPectro_LED_Animation_Fading, duration);
    getAnimatorPtr()->setTimeout(count*duration);

    startAnimation();
}

void ESPectro32_LED::setAnimation(ESPectro32_LED_AnimationType animType, uint32_t speed, uint32_t count) {
    stopAnimation();

    getAnimatorPtr()->setAnimation(animType, speed);
    getAnimatorPtr()->setTimeout(count*speed);

    startAnimation();
}

void ESPectro32_LED::startAnimation() {

    if (animator_ == NULL) {
        return;
    }

    animator_->start();
}

void ESPectro32_LED::stopAnimation() {

    if (animator_ == NULL || !animator_->isAnimating()) {
    		return;
    }

    if (animator_ != NULL) {
        animator_->stop();

        delete animator_;
        animator_ = NULL;
    }

    //black all
    turnOff();
}

bool ESPectro32_LED::isAnimating() const {
    return (animator_ != NULL && animator_->isAnimating());
}

void ESPectro32_LED::setAnimationTimeout(unsigned long timeOut) {
    if (animator_ != NULL) {
    		animator_->setTimeout(timeOut);
    }
}

ESPectro32_LED_Animator *ESPectro32_LED::getAnimatorPtr() {
	if (animator_ == NULL) {
		animator_ = new ESPectro32_LED_Animator();
		animator_->initPWM(pin_, activeHigh_);
	}

	return animator_;
}

void ESPectro32_LED::setBrightnessPercentage(uint8_t percent) {
	getLedPwmPtr()->setDutyPercentage(activeHigh_? percent: (100 - percent));
}

void ESPectro32_LED::disableBrightnessControl() {
	if (ledPwm_ != NULL) {
		ledPwm_->stop();
		delete ledPwm_;
		ledPwm_ = NULL;
	}
}

PWM* ESPectro32_LED::getLedPwmPtr() {
	if (ledPwm_ == NULL) {
		ledPwm_ = new PWM(pin_, LEDC_BASE_FREQ);
	}

	return ledPwm_;
}



ESPectro32_LED_Animator::ESPectro32_LED_Animator():
		Task("ESPectro32_LED_Animator_Task", 2048, configMAX_PRIORITIES - 5)
{
    maxOut_ = 0xFFFFFF;
    speed_ = 1000;
    lastRefreshTime_ = 0;
    refreshMillis_ = 1000/50;
}

ESPectro32_LED_Animator::~ESPectro32_LED_Animator() {
	free(pins_);
}

void ESPectro32_LED_Animator::initPWM(byte pin, bool activeHigh) {

	activeHigh_ = activeHigh;
    numLeds_ = 1;
    numPins_ = 1;

    pins_ = (byte*)malloc(1);
    pins_[0] = pin;

    //pinMode(pin, OUTPUT);
    ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
    ledcAttachPin(pin, LEDC_CHANNEL_0);

    // allocate and clear leds array
    ledColors_ = (RgbLedColor_t *)malloc(3*numLeds_);
    memset(ledColors_, 0, 3*numLeds_);
}

void ESPectro32_LED_Animator::setAnimation(ESPectro32_LED_AnimationType animType, uint32_t speed) {

    // is there any change?
    if (this->animationType_ == animType && this->speed_ == speed)
        return;

    animationType_ = animType;
    speed_ = speed;

    palette_.numColors = 1;
    palette_.colors = (RgbLedColor_t*)malloc(3);
    palette_.colors[0] = RgbLedColor_t(0xFFFFFF);

    setAnimationFunc(animationType_);
    //animStartTime_ = millis();
}

void ESPectro32_LED_Animator::setAnimationFunc(ESPectro32_LED_AnimationType animType) {
    switch(animType)
    {
        case ESPectro_LED_Animation_ON:                     animFunc_ = &ESPectro32_LED_Animator::on;                    break;
        case ESPectro_LED_Animation_OFF:                    animFunc_ = &ESPectro32_LED_Animator::off;                   break;
        case ESPectro_LED_Animation_Blink:                  animFunc_ = &ESPectro32_LED_Animator::blink;                 break;
        case ESPectro_LED_Animation_Fading:                 animFunc_ = &ESPectro32_LED_Animator::fade;                  break;
        case ESPectro_LED_Animation_Strobo:                 animFunc_ = &ESPectro32_LED_Animator::strobo;                break;
        default:                                            animFunc_ = &ESPectro32_LED_Animator::off;
    }
}

int ESPectro32_LED_Animator::getStep(long t0, long t, int v)
{
    return ((millis()-t0)%t)*v/t;
}

float ESPectro32_LED_Animator::getStepFloat(long t0, long t, float v)
{
    return ((millis()-t0)%t)*v/t;
}

bool ESPectro32_LED_Animator::runAnimation() {

    // skip the refresh if not enough time has passed since last update
    unsigned long cTime = millis();

//    if (cTime - animStartTime_ > animTimeOut_) {
//        	Serial.println("ANIM STOPPED");
//    		stop();
//    		return false;
//    	}

    if (cTime < (lastRefreshTime_ + refreshMillis_))
        return false;

    // calculate real refresh rate
    refreshRate_ = 1000/(cTime - lastRefreshTime_);

    lastRefreshTime_ = cTime;

    if (animFunc_ != NULL)
    {
        (this->*animFunc_)();

        //PWM
        for(int i=0; i < numLeds_; i++)
        {
            int j = 3*i;

            if (numPins_ == 1) { //for 1 color LED
//                analogWrite(pins_[0], (ledColors_[i].r * maxOut_.b) >> 8);
            		if (activeHigh_) {
            			ledcAnalogWrite(LEDC_CHANNEL_0, ((ledColors_[i].red * maxOut_.red) >> 8));
            		}
            		else {
            			ledcAnalogWrite(LEDC_CHANNEL_0, LEDC_VALUE_MAX - ((ledColors_[i].red * maxOut_.red) >> 8));
            		}
            }
            else {
//                analogWrite(pins_[j], (ledColors_[i].r * maxOut_.r) >> 8);
//                analogWrite(pins_[j + 1], (ledColors_[i].g * maxOut_.g) >> 8);
//                analogWrite(pins_[j + 2], (ledColors_[i].b * maxOut_.b) >> 8);
            }
        }
    }

    return true;
}

void ESPectro32_LED_Animator::setRefreshRate(int refreshRate) {
    this->refreshMillis_ = 1000/refreshRate;
}

void ESPectro32_LED_Animator::on() {
    for(int i = 0; i < numLeds_; i++)
    {
        ledColors_[i] = palette_.colors[0];
    }
}

void ESPectro32_LED_Animator::off() {
    for(int i = 0; i < numLeds_; i++)
    {
        ledColors_[i] = 0x000000;
    }
}

void ESPectro32_LED_Animator::blink() {
    int t = getStep(animStartTime_, speed_, 2);
    int k = (t+1)%2;
    for(int x = 0; x < numLeds_; x++)
    {
    		ledColors_[x] = palette_.colors[0].scale(k);
    }
}

void ESPectro32_LED_Animator::fade() {
    float s = getStepFloat(animStartTime_, speed_, TWO_PI);
    float k = (-cos(s)+1)/2;

    for(int x=0; x < numLeds_; x++)
    {
        ledColors_[x] = palette_.colors[0].scale(k);
    }
}

void ESPectro32_LED_Animator::strobo()
{
    int t = getStep(animStartTime_, speed_, 10);

    RgbLedColor_t c = palette_.colors[0].scale(t==0);
    for(int x=0; x < numLeds_; x++)
    {
        ledColors_[x] = c;
    }
}

void ESPectro32_LED_Animator::runAsync(void *data) {
	for(;;) {
//		if (!animating_) {
//			Serial.println("BREAK");
//			break;
//		}
		if (millis() - animStartTime_ >= (animTimeOut_)) {
			//Serial.println("ANIM STOPPED");
			stop();
			break;
		}

		runAnimation();
		vTaskDelay(10/portTICK_PERIOD_MS);
	}

	Task::stop();
}

void ESPectro32_LED_Animator::start() {

	animating_ = true;
	animStartTime_ = millis();

	Task::start(NULL);
}

void ESPectro32_LED_Animator::stop() {

//	Serial.println("ANIM STOPPED");
	animating_ = false;

//	if (activeHigh_) {
//		ledcAnalogWrite(LEDC_CHANNEL_0, 2 >> 8);
//	} else {
//		ledcAnalogWrite(LEDC_CHANNEL_0, LEDC_VALUE_MAX - (2 >> 8));
//	}

	if (numPins_ == 1) {
		ledcDetachPin(pins_[0]);
		digitalWrite(pins_[0], activeHigh_? LOW: HIGH);
	}

	//Task::stop();
}

