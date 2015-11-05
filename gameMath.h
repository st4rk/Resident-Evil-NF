/***************************************************************************
** gameCore é utilizado para tudo relacionado a matemática, isto é calc-  **
** de colisão entre personage me cenário[...]                             **
** Criado por Lucas P. Stark                                              **
** Nightmare Fiction Engine - NFE                                         **
***************************************************************************/

#ifndef GAME_MATH_H
#define GAME_MATH_H

#include <iostream>
#include <algorithm>

class gameMath {
public:	
	gameMath();
   ~gameMath();

   	// Detector de Colisão
  	bool collisionDetect(unsigned int colType, signed int x_1, signed int z_1, signed int w, signed int h, signed int x, signed int z);
  	bool mapSwitch(signed int x, signed int z, signed int x1, signed int z1, signed int x2, signed int z2, signed int x3, signed int z3, signed int x4, signed int z4);

private:


};








#endif