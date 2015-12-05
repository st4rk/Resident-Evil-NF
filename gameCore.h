/***************************************************************************
** gameCore é utilizada para renderização, pegar todas as informações e   **
** gerencialas para que o jogo funcione de forma correta                  **
** Criado por Lucas P. Stark                                              **
** Nightmare Fiction Engine - NFE                                         **
***************************************************************************/

#ifndef RENDERCORE_H
#define RENDERCORE_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
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


#include "depack_vlc.h"
#include "depack_mdec.h"
#include "file_functions.h"


#define RES_HEIGHT 640
#define RES_WIDTH  480
#define MAX_MODEL  20


class gameCore {

public:
	gameCore(int, char**);
	~gameCore();

	void renderInit();
	void renderText(float, float, float, std::string);
	void renderLoadResource();


private:

	GLuint fontTexture;
	char GAME_NAME[20];

};


void renderScene( void );


#endif