//
// Created by Andri Yadi on 3/17/17.
//

#ifndef ESPECTROCORE_ESPECTRO_LED_H
#define ESPECTROCORE_ESPECTRO_LED_H


#include <Arduino.h>
#include <functional>

#include "ESPectro32_Constants.h"
#include <RgbLedColor.h>
#include <Task.h>
#include <PWM.h>

/**
 * @brief The LED animation supported, for now
 */
enum ESPectro32_LED_AnimationType {
        ESPectro_LED_Animation_ON,
        ESPectro_LED_Animation_OFF,
        ESPectro_LED_Animation_Blink,
        ESPectro_LED_Animation_Fading,
        ESPectro_LED_Animation_Strobo
    };

/**
 * @brief The LED animator
 */
class ESPectro32_LED_Animator: public Task {
public:

    ESPectro32_LED_Animator();
    ~ESPectro32_LED_Animator();

    void initPWM(byte pin, bool activeHigh = false);
    void setAnimation(ESPectro32_LED_AnimationType animType, uint32_t speed);
    bool runAnimation();

    void setBrightness(RgbLedColor_t maxOut)
    {
        this->maxOut_ = maxOut;
    }

    /**
	* Sets the maximum refresh rate in Hz (default value is 50 Hz).
	* May be useful to reduce flickering in some cases.
	*/
    void setRefreshRate(int refreshRate);

    void runAsync(void *data);
    void start();
    void stop();
    void setTimeout(unsigned long timeOut) {
    		animTimeOut_  = timeOut;
    }

    bool isAnimating() {
    		return animating_;
    }

private:

    ESPectro32_LED_AnimationType animationType_ = ESPectro_LED_Animation_OFF;
    bool activeHigh_;
    uint32_t speed_;
    uint32_t refreshMillis_;
    uint32_t refreshRate_;   // current refresh rate
    unsigned long animStartTime_, lastRefreshTime_;
    unsigned long animTimeOut_ = 30000;
    bool animating_ = false;

    byte *pins_ = NULL;     // pins where the leds are attached to
    byte numLeds_ = 1, numPins_ = 1;
    RgbLedColor_t maxOut_;
    RgbLedColor_t *ledColors_; // array to store leds brightness values
    RgbLedPalette_t palette_;

    void (ESPectro32_LED_Animator::*animFunc_)();
    void setAnimationFunc(ESPectro32_LED_AnimationType animType);

    int getStep(long t0, long t, int v);
    float getStepFloat(long t0, long t, float v);

    void on();
    void off();
    void blink();
    void fade();
    void strobo();
};

/**
 * @brief Represent LED
 */
class ESPectro32_LED {
public:

    ESPectro32_LED(uint8_t pin = ESPECTRO32_LED_PIN, bool activeHigh = false);
    ~ESPectro32_LED();

    /**
     * @brief Must be called first before anything else.
     */
    void begin();
    void turnOn();
    void turnOff();
    boolean isOn();
    byte getPin();
    void toggle();

    void blink(uint32_t interval = 500, uint32_t count = UINT16_MAX);
    void fade(uint32_t duration = 2000, uint32_t count = UINT16_MAX);

    void startAnimation();
    void stopAnimation();
    bool isAnimating() const;
    void setAnimation(ESPectro32_LED_AnimationType animType, uint32_t speed, uint32_t count = UINT16_MAX);
    void setAnimationTimeout(unsigned long timeOut);

    void setBrightnessPercentage(uint8_t percent);
    void disableBrightnessControl();

private:
    uint8_t pin_;
    bool activeHigh_;

    ESPectro32_LED_Animator *animator_ = NULL;
    ESPectro32_LED_Animator *getAnimatorPtr();

    PWM *ledPwm_ = NULL;
    PWM *getLedPwmPtr();
};


#endif //ESPECTROCORE_ESPECTRO_LED_H
