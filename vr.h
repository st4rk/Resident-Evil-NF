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

private:
	tmr      timer;
	gameMisc misc;
	char scoreBuffer[15];
	unsigned int gameScore;
};


#endif