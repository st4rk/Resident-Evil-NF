/***************************************************************************
** gameSound  é utilizada para toda parte sonora da engine, isto é        **
** sound effect e background music[...]                                   **
** Criado por Lucas P. Stark                                              **
** Nightmare Fiction Engine - NFE                                         **
***************************************************************************/

#include "gameSound.h"

gameSound::gameSound() {
	backgroundMusic = NULL;
}

gameSound::~gameSound() {
	Mix_FreeMusic( backgroundMusic );
	backgroundMusic = NULL;
	SDL_Quit();
	Mix_Quit();
}

void gameSound::engineSoundInit() {
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		std::cout << "SDL não foi inicializado !" << SDL_GetError() << std::endl;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		std::cout << "SDL_Mixer não foi inicializado!" << Mix_GetError() << std::endl;
	}

	backgroundMusic = NULL;
}

void gameSound::engineLoadSound(std::string soundName) {
	backgroundMusic = Mix_LoadMUS(soundName.c_str());
	
	if (backgroundMusic == NULL) {
		std::cout << "Musica não carregada !: " << Mix_GetError() << std::endl;
	}
}

void gameSound::enginePlaySound(int rept) {
	if (Mix_PlayingMusic() == 0) {
		Mix_PlayMusic(backgroundMusic, rept);
	}
}

void gameSound::engineStopSound() {
	Mix_HaltMusic();
}

void gameSound::enginePlayerSoundEffect() {

}

void gameSound::engineStopSoundEffect() {

}

int  gameSound::enginePlayingMusic() { return (Mix_PlayingMusic()); }
