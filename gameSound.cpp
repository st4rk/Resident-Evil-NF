/***************************************************************************
** gameSound  é utilizada para toda parte sonora da engine, isto é        **
** sound effect e background music[...]                                   **
** Criado por Lucas P. Stark                                              **
** Nightmare Fiction Engine - NFE                                         **
***************************************************************************/

#include "gameSound.h"

gameSound::gameSound() {

}

gameSound::~gameSound() {
	// Limpa a música
	Mix_FreeMusic( backgroundMusic );
	backgroundMusic = NULL;

	SDL_Quit();
	Mix_Quit();
}

bool gameSound::engineSoundInit() {
	// Inicializa o SDL que gerencia áudio
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		std::cout << "SDL não foi inicializado !" << SDL_GetError() << std::endl;
		return false;
	}

	// Inicializa o MIX de Áudio com o samplerate de 44100hz
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		std::cout << "SDL_Mixer não foi inicializado!" << Mix_GetError() << std::endl;
		return false;
	}


	// É aqui onde fica armazenada a música de background
	// Inicializa o ponteiro com NULL
	backgroundMusic = NULL;
	return true;
}

void gameSound::engineLoadSound(std::string soundName) {
	// Carrega música do directório
	backgroundMusic = Mix_LoadMUS(soundName.c_str());
	
	// Verifica se o carregamento deu certo
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

void gameSound::enginePlaySoundEffect(Mix_Chunk *node) {
	Mix_PlayChannel(-1, node, 0);
}


int  gameSound::enginePlayingMusic() { return (Mix_PlayingMusic()); }
