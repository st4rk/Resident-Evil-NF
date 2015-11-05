/********************************************************************************************************************
 ** 					Nightmare Fiction Engine - NFE                                                             **
 **  NFE é uma engine desenvolvida por Lucas P. Stark com a finalidade de aprender mais sobre o ambiente 3D        **
 ** foi utilizado OpenGL para toda a renderização gráfica e para parte sonora foi utilizado SDL/SDL_Mixer.         **
 ** Os modelos 3D suportados são .EMD e suas texturas .TIM, ambos são modelos utilizados pela Capcom para o        **
 ** desenvolvimento da serie Resident Evil 1,2,3 (A principio apenas com suporte para o Resident Evil 2 e          **
 ** seu prototype Resident Evil 1.5). A finalidade desse projeto é apenas diversão e aprendizagem, todos os        **
 ** modelos utilizados do Resident Evil são propriedades da Capcom e suas respectivas divisões de desenvolvimento  **
 ** 30/05/2015 Revisão 0.03a                                                                                       **
 ********************************************************************************************************************/



#include "gameCore.h"

int main(int argc, char **argv) {
	gameCore myRender(argc, argv);

	myRender.renderLoadResource();
	myRender.renderInit();

	return 0;
}