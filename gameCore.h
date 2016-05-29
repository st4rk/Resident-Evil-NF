/***************************************************************************
** gameCore é utilizada para renderização, pegar todas as informações e   **
** gerencialas para que o jogo funcione de forma correta                  **
** Criado por Lucas P. Stark                                              **
** Nightmare Fiction Engine - NFE                                         **
***************************************************************************/

#ifndef GAMECORE_H
#define GAMECORE_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <GL/glut.h>
#include <list>
#include <math.h>
#include <SDL/SDL.h>
#include "EMD.h"
#include "PLD.h"
#include "RDT.h"
#include "gameStruct.h"
#include "gameSound.h"
#include "gameMath.h"
#include "gameEvent.h"
#include "gameMisc.h"

#include "depack_vlc.h"
#include "depack_mdec.h"
#include "file_functions.h"


#define RES_HEIGHT 640
#define RES_WIDTH  480
#define MAX_MODEL  20


enum EVENT_SYSTEM_KEY {
	EVENT_SYSTEM_KEY_UP    = 0,
	EVENT_SYSTEM_KEY_DOWN  = 1,
	EVENT_SYSTEM_KEY_LEFT  = 2,
	EVENT_SYSTEM_KEY_RIGHT = 3,
	EVENT_SYSTEM_KEY_X     = 4,
	EVENT_SYSTEM_KEY_Z     = 5,
	EVENT_SYSTEM_KEY_C     = 6
};


class gameCore {

public:
	gameCore(int, char**);
	~gameCore();

	/*
	 * OpenGL, GLUT and Texture Stuff
	 */
	void renderInit();
	void renderText(float, float, float, int, std::string, float, float, float, float);
	void renderLoadResource();
	void renderMainMenu();
	void renderGame();
	void renderSelectChar();
	void renderCamera();
	void renderStateMachine();

	/*
	 * Input
	 */
	void eventSystem_debugMenu();
	void eventSystem_downKey(int key, int x, int y);
	void eventSystem_upKey(int key, int x, int y);
	void eventSystem_keyboardDown(unsigned char key, int x, int y);
	void eventSystem_keyboardUp(unsigned char key, int x, int y);
	void eventSystem_gameAction(unsigned int key, bool pressed);

	/* 
	 * Animation
	 */
	void engineAnimation();
	
	/*
	 * System callback's and related
	 */	
	static void renderSetObj(gameCore *obj);
	static void renderCallback();
	static void eventSystemUpCallBack(int key, int x, int y) ;
	static void eventSystemDownCallBack(int key, int x, int y) ;
	static void eventSystemKeyboardUpCallBack(unsigned char key, int x, int y);
	static void eventSystemKeyboardDownCallBack(unsigned char key, int x, int y);


	/*
	 * animation timer
	 */
	unsigned int coreTmr_anim; 
	/*
	 * key Pressed
	 */

	bool keyList[0x10];
private:

	std::vector<Mix_Chunk*> mixList;
	std::vector<EMD_SEC2_DATA_T> interAnimation;
	unsigned int inInterpolation;
	unsigned int oldAnim;
	unsigned int oldSection;

	
	GLuint fontTexture;
	char GAME_NAME[20];


};


void renderScene( void );

extern gameCore *mainGame;

#endif