/*===========================================*
 * Virtual Really Mode                       * 
 *-------------------------------------------*
 * Here *must* be written all game logic     * 
 * uses in the virtual really mode           *
 *===========================================*
 */

#ifndef VR_H
#define VR_H

#include <SDL/SDL.h>
#include "gameMisc.h"

#define VR_ENEMY_NUM 5
#define VR_SPECIAL  1


enum VR_STATE {
	VR_STATE_IN_GAME = 0,
	VR_STATE_IN_BEND = 1,
	VR_STATE_IN_END  = 2
};

struct tmr {
	int mil;
	int sec;
	int min;

	unsigned int sysTick;

	char buffer[15];
};

class VR {
public:
	VR();
   ~VR();

    void setGameScore(unsigned int gameScore);
    void gameLogic();
    void setState(unsigned int vrState);

    unsigned int getState();

	tmr      timer;


	unsigned int ammo;
	unsigned int gameScore;
private:
	gameMisc misc;
	char scoreBuffer[15];
	unsigned int vrState;

	bool inFadeEnd;
};


#endif