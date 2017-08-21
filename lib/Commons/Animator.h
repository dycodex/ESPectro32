/*
 * WS2812Animation.h
 *
 *  Created on: May 22, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_MAGICBUTTON_SRC_WS2812ANIMATOR_H_
#define COMPONENTS_MAGICBUTTON_SRC_WS2812ANIMATOR_H_

#include <Arduino.h>
#include <functional>
#include "Task.h"

class Animator: Task {
public:

	enum AnimationState
	{
		AnimationState_Started,
		AnimationState_Progress,
		AnimationState_Completed
	};

	struct AnimationParam
	{
		float progress;
		uint16_t duration;
		uint16_t elapsed = 0;
		AnimationState state;
	};

	typedef std::function<void(const AnimationParam &param)> AnimationUpdateCallback;
	typedef std::function<void()> AnimationFinishedCallback;


	Animator();
	virtual ~Animator();

	void start(AnimationUpdateCallback animUpdateCallback, AnimationFinishedCallback animFinishedCallback,
			   uint16_t duration = 0, uint16_t updateInterval = 0, bool repeat = false);
	void stop();
	void run();
	void runAsync(void *data);
	boolean isRunning() {
		return running_;
	}

	int getStep(long elapsed, long t, int v);
	float getStepFloat(long elapsed, long t, float v);
private:
	AnimationUpdateCallback animationUpdateCallback_ = NULL;
	AnimationFinishedCallback animationFinishedCallback_ = NULL;
	boolean running_ = false;
	uint16_t updateInterval_ = 100, elapsed_ = 0, duration_ = 0;
	uint32_t lastUpdated_ = 0;
	bool repeat_ = false;
	void doRepeat();
};

#endif /* COMPONENTS_MAGICBUTTON_SRC_WS2812ANIMATOR_H_ */
