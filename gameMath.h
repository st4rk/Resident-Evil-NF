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

enum COLLISION_TYPE {
	COLLISION_TYPE_RECTANGLE = 1,
	COLLISION_TYPE_ELLIPSE   = 3
};

struct rectangle {
	int x;
	int y;


	int h;
	int w;
};


class gameMath {
public:	
	gameMath();
   ~gameMath();

   	// Detector de Colisão
  	bool collisionDetect(unsigned int colType, signed int x_1, signed int z_1, signed int w, signed int h, signed int x, signed int z);
  	bool mapSwitch(signed int x, signed int z, signed int x1, signed int z1, signed int x2, signed int z2, signed int x3, signed int z3, signed int x4, signed int z4);

  	bool collisionRectangle(int px, int py, int pz,
  							int nx, int ny, int nz);

  	float interpolation(float n1, float n2, float p);
private:


};








#endif