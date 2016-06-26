#include "vr.h"


VR::VR() {
	timer.mil     = 0;
	timer.sec     = 0;
	timer.min     = 0;
	timer.sysTick = 0;

	inFadeEnd = false;
	vrState   = VR_STATE_IN_GAME;
	ammo      = 0;

	memset(timer.buffer, 0x0, 15);
	memset(scoreBuffer,  0x0, 15);
}

VR::~VR() {
	timer.mil     = 0;
	timer.sec     = 0;
	timer.min     = 0;
	timer.sysTick = 0;

	inFadeEnd = false;
	vrState   = VR_STATE_IN_GAME;
	ammo      = 0;

	memset(scoreBuffer,  0x0, 15);
	memset(timer.buffer, 0x0, 15);
}


void VR::setGameScore(unsigned int gameScore) {
	if (this->gameScore != gameScore) {
		this->gameScore = gameScore;
		sprintf(scoreBuffer, "Score: %d", gameScore);
	}

}


void VR::gameLogic() {

	if (timer.sysTick < SDL_GetTicks()) {
		timer.sysTick = SDL_GetTicks() + 10;

		if (timer.mil > 59) {
			timer.mil = 0;
			timer.sec++;
			if (timer.sec > 59) {
				timer.sec = 0;
				timer.min++;
			} 
		} else {
			timer.mil++;
		}

	
		sprintf(timer.buffer, "%01d:%02d:%02d", timer.min, timer.sec, timer.mil);
	}
	
	misc.renderText(0.50, 0.05, 0.0, TEXT_TYPE_NORMAL, timer.buffer, 0.0f, 1.0f, 0.0f, 1.0f);
	misc.renderText(0.0, 0.90, 0.0, TEXT_TYPE_LITTLE,  scoreBuffer, 1.0f, 1.0f, 1.0f, 1.0f);


	if (gameScore >= VR_ENEMY_NUM + VR_SPECIAL) {
		if (inFadeEnd == false) {
			misc.setupFadeEffect(TYPE_FADE_IN, 0, 0, 0, 260);
			inFadeEnd = true;
		} else {

			if (!misc.isInFade()) {
				vrState = VR_STATE_IN_BEND;
			}

			misc.renderFadeEffect();
		}
	}
}

void VR::setState(unsigned int vrState) { this->vrState = vrState; }

unsigned int VR::getState() { return vrState; }