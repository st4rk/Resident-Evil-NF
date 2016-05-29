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

/*
 * Interpolation between two values
 */
int gameMath::interpolation(int n1, int n2, int p) {
	return (n1 + (p * (n2-n1)));
}


bool gameMath::mapSwitch(signed int x, signed int z, signed int x1, signed int z1, signed int x2, signed int z2, signed int x3, signed int z3, signed int x4, signed int z4) {

}

bool gameMath::collisionDetect(unsigned int colType, signed int x1, signed int z1, 
	                           signed int x2, signed int z2, 
	                           signed int x, signed int z) {

	switch (colType) {

		case COLLISION_TYPE_RECTANGLE:
			if ((x1 <= x) && (x <= (x1+x2))) {
				if ((z1 <= z) && (z <= (z1+z2))) {
					return true;
				}
			}
		break;

		case COLLISION_TYPE_ELLIPSE: {
			double RayX = (x2 - x1) / 2;
			double RayZ = (z2 - z1) / 2;
			double cX = x1 + RayX;
			double cZ = z1 + RayZ;
			double k = pow((x - cX) / RayX, 2) + pow((z - cZ) / RayZ, 2);
			if (k <= 1)
				return true;
		}

		break;

		default:
			std::cout << "The collision type: " << colType << " wasn't implemented" << std::endl;
			return false;
		break;

	}

	

	return false;
}