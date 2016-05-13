/***************************************************************************
** gameSound  é utilizada para toda parte sonora da engine, isto é        **
** sound effect e background music[...]                                   **
** Criado por Lucas P. Stark                                              **
** Nightmare Fiction Engine - NFE                                         **
***************************************************************************/

#ifndef GAME_SOUND_H
#define GAME_SOUND_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>


class gameSound {
public:
	gameSound();
	~gameSound();
	bool engineSoundInit();
	void engineLoadSound(std::string soundName);
	void enginePlaySound(int rept);
	void engineStopSound();
	void enginePlayerSoundEffect();
	void engineStopSoundEffect();
	int  enginePlayingMusic();
private:
	Mix_Music *backgroundMusic;

	Mix_Chunk *gunEffect;
	Mix_Chunk *walkEffect;
	Mix_Chunk *clickEffect;
	Mix_Chunk *titleEffect;
	
};

#endif
