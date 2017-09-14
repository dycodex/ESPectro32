/*
 * ESPectro32_LedMatrix_Animation.cpp
 *
 *  Created on: Aug 20, 2017
 *      Author: andri
 */

#include "ESPectro32_LedMatrix_Animation.h"

ESPectro32_LedMatrix_Animation::ESPectro32_LedMatrix_Animation(){
}

ESPectro32_LedMatrix_Animation::~ESPectro32_LedMatrix_Animation() {
	stop();
}

void ESPectro32_LedMatrix_Animation::addFrameWithData(uint8_t* data, uint16_t color) {

	if (this->ledMatrix_ == NULL || frameCount_ >= ESPECTRO32_LEDMATRIX_MAX_FRAME_COUNT) {
		return;
	}

	setFrameWithData(frameCount_, data, color);
	frameCount_++;
}

void ESPectro32_LedMatrix_Animation::addFrameWithDataCallback(ESPectro32_LedMatrix_Animation::FrameDataCallback cb) {

	if (this->ledMatrix_ == NULL || frameCount_ >= ESPECTRO32_LEDMATRIX_MAX_FRAME_COUNT) {
		return;
	}

	setFrameWithDataCallback(frameCount_, cb);
	frameCount_++;
}

void ESPectro32_LedMatrix_Animation::setFrameWithData(uint8_t frameNo, uint8_t* data, uint16_t color) {

	this->ledMatrix_->setFrame(frameNo);
	this->ledMatrix_->clear();
	this->ledMatrix_->drawBitmap(0, 0, data, 7, 7, color);
}

void ESPectro32_LedMatrix_Animation::setFrameWithDataCallback(uint8_t frameNo,
		ESPectro32_LedMatrix_Animation::FrameDataCallback cb) {

	this->ledMatrix_->setFrame(frameNo);
	this->ledMatrix_->clear();

	cb(*this->ledMatrix_);
}

void ESPectro32_LedMatrix_Animation::start(
		Animator::AnimationUpdateCallback animUpdateCallback,
		Animator::AnimationFinishedCallback animFinishedCallback,
		uint16_t duration, uint16_t updateInterval, bool repeat) {

	if (this->ledMatrix_ == NULL || frameCount_ == 0) {
		return;
	}

	getAnimatorPtr()->start(animUpdateCallback, animFinishedCallback, duration, updateInterval, repeat);
}

void ESPectro32_LedMatrix_Animation::stop() {
	forceStop_ = true;
	if (animator_ != NULL) {
		animator_->stop();
	}
}

void ESPectro32_LedMatrix_Animation::run() {
	if (animator_ != NULL) {
		if (animator_->isRunning()) {
			animator_->run();
		}
	}
}

void ESPectro32_LedMatrix_Animation::start(uint16_t duration, bool repeat) {

	if (ledMatrix_ == NULL) {
		return;
	}

	if (frameCount_ == 0) {
		return;
	}

	if (duration == 0) {
		duration = frameCount_ * 300;
	}

	getAnimatorPtr()->start([this](const Animator::AnimationParam param) {

		if (forceStop_) {
			return;
		}

		float index = param.progress * frameCount_;

		if (index < frameCount_) {
			this->ledMatrix_->displayFrame(index);
		}

	}, [this, duration, repeat]() {
		//printf("LEDM ANIM DONE\n");
		if (this->animCompletedCb_ != NULL) {
			this->animCompletedCb_();
		}

//		if (repeat) {
//			this->start(duration, repeat);
//		}

	}, duration, 10, repeat);
}

Animator* ESPectro32_LedMatrix_Animation::getAnimatorPtr() {
	if (animator_ == NULL) {
		animator_ = new Animator();
	}

	return animator_;
}



ESPectro32_LedMatrix_ScrollTextAnimation::ESPectro32_LedMatrix_ScrollTextAnimation() {
}

ESPectro32_LedMatrix_ScrollTextAnimation::~ESPectro32_LedMatrix_ScrollTextAnimation() {
	if (scrolledText_ != NULL) {
		free(scrolledText_);
	}
}

void ESPectro32_LedMatrix_ScrollTextAnimation::scrollText(const char *text, uint16_t duration, bool repeat) {

	if (ledMatrix_ == NULL) {
		return;
	}

	//printf("Text length: %d\n", strlen(text));

	if (scrolledText_ == NULL) {
		int strlength = (strlen(text) + 1);
		scrolledText_ = (char*)malloc(strlength);
		strncpy(scrolledText_, text, strlen(text));
		scrolledText_[strlength] = '\0';

		//printf("Text length 2: %d %d\n", strlen(scrolledText_), strlength);
	}

	if (duration == 0) {
		duration = strlen(scrolledText_) * 50 * 6; //6 is char width in pixel
	}

	this->ledMatrix_->setFrame(0);
	this->ledMatrix_->clear();
	this->ledMatrix_->setTextSize(1);
	this->ledMatrix_->setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
	this->ledMatrix_->setTextColor(100);
	this->ledMatrix_->displayFrame(0);

	getAnimatorPtr()->start([this, duration, text](const Animator::AnimationParam param) {

		if (forceStop_) {
			return;
		}

		int x = (int)(param.progress * (strlen(scrolledText_) * 6));
		//printf("X = %d\n", x);

		this->ledMatrix_->clear();
		this->ledMatrix_->setCursor((-1*x),0);
		this->ledMatrix_->print(scrolledText_);

	}, [this]() {

		//printf("LEDM TEXT ANIM DONE\n");
		if (this->animCompletedCb_ != NULL) {
			this->animCompletedCb_();
		}

	}, duration, 50, repeat);
}

void ESPectro32_LedMatrix_Animation::clearFrames() {
	if (frameCount_ == 0) {
		return;
	}

	for(int i = 0; i < frameCount_; i++) {
		this->ledMatrix_->setFrame(i);
		this->ledMatrix_->clear();
	}

	frameCount_ = 0;

	this->ledMatrix_->begin();
}
