/*
 * WS2812Animation.cpp
 *
 *  Created on: May 22, 2017
 *      Author: andri
 */

#include "Animator.h"

#define ANIMATOR_INITED_EVT 				BIT0
#define ANIMATOR_START_REQ_EVT			BIT1
#define ANIMATOR_STOP_REQ_EVT 			BIT2

static EventGroupHandle_t animatorEventGroup_ = NULL;

Animator::Animator():
Task("ws2812AnimatorTask", 4096, configMAX_PRIORITIES - 3){

	if (animatorEventGroup_ == NULL) {
		animatorEventGroup_ = xEventGroupCreate();
	}

	xEventGroupClearBits(animatorEventGroup_, ANIMATOR_INITED_EVT);
	xEventGroupClearBits(animatorEventGroup_, ANIMATOR_START_REQ_EVT);
	xEventGroupClearBits(animatorEventGroup_, ANIMATOR_STOP_REQ_EVT);
}

Animator::~Animator() {
	stop();
}

void Animator::start(AnimationUpdateCallback animUpdateCallback,
		AnimationFinishedCallback animFinishedCallback, uint16_t duration,
		uint16_t updateInterval, bool repeat) {

	if (updateInterval > 0) {
		updateInterval_ = updateInterval;
	}

	repeat_ = repeat;
	duration_ = duration;
	animationUpdateCallback_ = animUpdateCallback;
	animationFinishedCallback_ = animFinishedCallback;
//	running_ = true;

	elapsed_ = 0;

	xEventGroupSetBits(animatorEventGroup_, ANIMATOR_START_REQ_EVT);
	xEventGroupClearBits(animatorEventGroup_, ANIMATOR_STOP_REQ_EVT);

	Task::start();
}

void Animator::doRepeat() {
	elapsed_ = 0;
}

bool Animator::isRunning() {
	EventBits_t uxBits = xEventGroupWaitBits(animatorEventGroup_, ANIMATOR_START_REQ_EVT, false, false, 1/portTICK_PERIOD_MS);
	if (uxBits & ANIMATOR_START_REQ_EVT) {
		return true;
	}

	return false;
}

void Animator::stop() {

	xEventGroupSetBits(animatorEventGroup_, ANIMATOR_STOP_REQ_EVT);
	xEventGroupClearBits(animatorEventGroup_, ANIMATOR_START_REQ_EVT);

//	running_ = false;
	if (animationFinishedCallback_) {
		animationFinishedCallback_();
	}

	Task::stop();
}

void Animator::run() {
//	if (!running_) {
//		return;
//	}

//	EventBits_t uxBits;
//
//	uxBits = xEventGroupWaitBits(animatorEventGroup_, ANIMATOR_START_REQ_EVT, false, false, portMAX_DELAY); //blocking
//	if (uxBits & ANIMATOR_START_REQ_EVT) {
//	}

//	uxBits = xEventGroupWaitBits(animatorEventGroup_, ANIMATOR_STOP_REQ_EVT, false, false, 1/portTICK_PERIOD_MS);
//	if (uxBits & ANIMATOR_STOP_REQ_EVT) {
//		return;
//	}

	uint32_t currentTick = millis();
	uint32_t delta = currentTick - lastUpdated_;

	if (delta > updateInterval_) {

		if (duration_ != 0 && elapsed_ > duration_) {
			if (!repeat_) {
				stop();
			}
			else {
				doRepeat();
			}

			return;
		}

		AnimationParam param;

		param.duration = duration_;
		param.elapsed = elapsed_;
		param.progress = (duration_ == 0)? 0: (float)(param.elapsed) / (float)param.duration;
		param.state = (param.elapsed == 0) ? AnimationState_Started : (duration_ != 0 && elapsed_ >= duration_)? AnimationState_Completed: AnimationState_Progress;

		if (animationUpdateCallback_) {
			animationUpdateCallback_(param);
		}

		elapsed_ += updateInterval_;

		lastUpdated_ = currentTick;
	}
}

void Animator::runAsync(void* data) {

	EventBits_t uxBits;

	uxBits = xEventGroupWaitBits(animatorEventGroup_, ANIMATOR_START_REQ_EVT, false, false, portMAX_DELAY); //blocking
	if (uxBits & ANIMATOR_START_REQ_EVT) {
	}

	for(;;) {
		uxBits = xEventGroupWaitBits(animatorEventGroup_, ANIMATOR_STOP_REQ_EVT, false, false, 1/portTICK_PERIOD_MS);
		if (uxBits & ANIMATOR_STOP_REQ_EVT) {
			break;
		}

		run();
		delay(1);
	}
}

int Animator::getStep(long elapsed, long t, int v) {
	return ((elapsed)%t)*v/t;
}

float Animator::getStepFloat(long elapsed, long t, float v) {
	return ((elapsed)%t)*v/t;
}


