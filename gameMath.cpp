/***************************************************************************
** gameCore é utilizado para tudo relacionado a matemática, isto é calc-  **
** de colisão entre personage me cenário[...]                             **
** Criado por Lucas P. Stark                                              **
** Nightmare Fiction Engine - NFE                                         **
***************************************************************************/
#include "gameMath.h"
#include <math.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>


gameMath::gameMath() {

}

gameMath::~gameMath() {

}


bool gameMath::mapSwitch(signed int x, signed int z, signed int x1, signed int z1, signed int x2, signed int z2, signed int x3, signed int z3, signed int x4, signed int z4) {
/*
	if ((x1 <= x) && (x <= (x3))) {
		if ((z1 <= z) && (z <= (z3))) {
			return true;
		}
	}

	if ((x2 <= x) && (x <= (x4))) {
		if ((z2 <= z) && (z <= (z4))) {
			return true;
		}
	}
*/

	int maxX, minX, maxZ, minZ;

	maxX = std::max(std::max(x1,x2),std::max(x3,x4)); //std::max(x1,x2,x3,x4);
	minX = std::min(std::min(x1,x2),std::min(x3,x4));
	maxZ = std::max(std::max(z1,z2),std::max(z3,z4));
	minZ = std::min(std::min(z1,z2),std::min(z3,z4));
	if (minX < x && x < maxX && minZ < z && z < maxZ) {
	   return true;
	} 

	return false;
}

bool gameMath::collisionDetect(unsigned int colType, signed int x_1, signed int z_1, 
	                           signed int w, signed int h, 
	                           signed int x, signed int z) {


	
	if ((x_1 <= x) && (x <= (x_1+w))) {
		if ((z_1 <= z) && (z <= (z_1+h))) {
			return true;
		}
	}
	

	return false;
}