/*
 * ESPectro32_LedMatrix_Animation.h
 *
 *  Created on: Aug 20, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_LEDMATRIX_ANIMATION_H_
#define COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_LEDMATRIX_ANIMATION_H_

#include <Animator.h>
#include "ESPectro32_LedMatrix.h"
#include "ESPectro32_Board.h"

static const uint8_t PROGMEM LED_MATRIX_WIFI_ANIM_1[] =
{ B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00010000,
  B00010000,
  B00000000
};

static const uint8_t PROGMEM LED_MATRIX_WIFI_ANIM_2[] =
{ B00000000,
  B00000000,
  B00111000,
  B01000100,
  B00010000,
  B00010000,
  B00000000
};

static const uint8_t PROGMEM LED_MATRIX_WIFI_ANIM_3[] =
{ B01111100,
  B10000010,
  B00111000,
  B01000100,
  B00010000,
  B00010000,
  B00000000
};

static const uint8_t PROGMEM LED_MATRIX_DOWNLOAD_ANIM_1[] =
{ B00111000,
  B00111000,
  B00111000,
  B10111010,
  B01111100,
  B00111000,
  B00010000
};

static const uint8_t PROGMEM LED_MATRIX_DOWNLOAD_ANIM_2[] =
{ B00000000,
  B00111000,
  B00111000,
  B10111010,
  B01111100,
  B00111000,
  B00010000
};

static const uint8_t PROGMEM LED_MATRIX_DOWNLOAD_ANIM_3[] =
{ B00000000,
  B00000000,
  B00111000,
  B10111010,
  B01111100,
  B00111000,
  B00010000
};

#define ESPECTRO32_LEDMATRIX_MAX_FRAME_COUNT 	8

/**
 * @brief Base class of Led Matrix animation.
 *
 * You should call `setLedMatrix` to set Led Matrix object before doing anything.
 * Then call either `addFrameWithData` to set frame data, or `addFrameWithCallback` to provide a callback that set frame.
 * Then call `start` with duration (in milisecond) to actually start the animation.
 *
 * This class leverage the animation capability of on-board IS31FL3731 chip (that drives the Led Matrix).
 * It supports up to 8 frames. The good thing of using this chip is that you never actually store frame data in RAM,
 * as frame data is directly written to the chip.
 *
 */
class ESPectro32_LedMatrix_Animation {
public:

	typedef std::function<void(ESPectro32_LedMatrix &ledMatrix)> FrameDataCallback;

	ESPectro32_LedMatrix_Animation();
	virtual ~ESPectro32_LedMatrix_Animation();

	void setLedMatrix(ESPectro32_LedMatrix &ledM) {
		ledMatrix_ = &ledM;
	}

	ESPectro32_LedMatrix &LedMatrix() {
		return *ledMatrix_;
	}

	void onAnimationCompleted(Animator::AnimationFinishedCallback cb) {
		animCompletedCb_ = cb;
	}

	void addFrameWithData(uint8_t *data, uint16_t color = 255);
	void addFrameWithDataCallback(ESPectro32_LedMatrix_Animation::FrameDataCallback cb);
	void setFrameWithData(uint8_t frameNo, uint8_t *data, uint16_t color = 255);
	void setFrameWithDataCallback(uint8_t frameNo, ESPectro32_LedMatrix_Animation::FrameDataCallback cb);

	void start(Animator::AnimationUpdateCallback animUpdateCallback, Animator::AnimationFinishedCallback animFinishedCallback,
					   uint16_t duration = 0, uint16_t updateInterval = 0, bool repeat = false);

	/**
	 * @brief Start animation
	 *
	 * @param[in] duration How long the animation will be (for all frames).
	 * @param[in] repeat Set to true to repeat animation when finished.
	 */
	void start(uint16_t duration = 0, bool repeat = false);
	void clearFrames();
	void stop();
	void run();

	uint8_t getFrameCount() {
		return frameCount_;
	}

	inline static ESPectro32_LedMatrix_Animation &startWiFiAnimation(void) {
		static ESPectro32_LedMatrix_Animation singleton;

		singleton.setLedMatrix(ESPectro32.LedMatrix());

		singleton.addFrameWithData((uint8_t*)LED_MATRIX_WIFI_ANIM_1);
		singleton.addFrameWithData((uint8_t*)LED_MATRIX_WIFI_ANIM_2);
		singleton.addFrameWithData((uint8_t*)LED_MATRIX_WIFI_ANIM_3);
		singleton.addFrameWithDataCallback([](ESPectro32_LedMatrix &ledM) {
			ledM.clear();
		});

		singleton.start(1800, true);
		return singleton;
	}

	inline static ESPectro32_LedMatrix_Animation &startDownloadAnimation(void) {
		static ESPectro32_LedMatrix_Animation singleton;

		singleton.setLedMatrix(ESPectro32.LedMatrix());

		singleton.addFrameWithData((uint8_t*)LED_MATRIX_DOWNLOAD_ANIM_1);
		singleton.addFrameWithData((uint8_t*)LED_MATRIX_DOWNLOAD_ANIM_2);
		singleton.addFrameWithData((uint8_t*)LED_MATRIX_DOWNLOAD_ANIM_3);
//		singleton.addFrameWithDataCallback([](ESPectro32_LedMatrix &ledM) {
//			ledM.clear();
//		});

		singleton.start(1800, true);
		return singleton;
	}

protected:
	ESPectro32_LedMatrix *ledMatrix_ = NULL;
	uint8_t frameCount_ = 0;
	bool forceStop_ = false;

	Animator::AnimationFinishedCallback animCompletedCb_ = NULL;
	Animator *animator_ = NULL;
	Animator *getAnimatorPtr();
};

class ESPectro32_LedMatrix_ScrollTextAnimation: public ESPectro32_LedMatrix_Animation {
public:
	ESPectro32_LedMatrix_ScrollTextAnimation();
	~ESPectro32_LedMatrix_ScrollTextAnimation();

	/**
	 * @brief Conveniently make scrolling text animation
	 *
	 * @param[in] text Text to display.
	 * @param[in] duration How long the animation will be (for all frames).
	 * @param[in] repeat Set to true to repeat animation when finished.
	 */
	void scrollText(const char *text, uint16_t duration = 0, bool repeat = false);
private:
	char *scrolledText_ = NULL;
};

#endif /* COMPONENTS_ESPECTRO32_SRC_ESPECTRO32_LEDMATRIX_ANIMATION_H_ */
