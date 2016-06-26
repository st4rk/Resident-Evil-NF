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
float gameMath::interpolation(float n1, float n2, float p) {
	return ((n1 * p) + (n2 * (1.0f - p)));
}

float gameMath::d2Point(float x1, float y1, float z1,
                        float x2, float y2, float z2) {

	return sqrt(pow((x2-x1),2) + pow((z2-z1), 2));
}


bool gameMath::collisionShoot(float px, float py, float pz,
                              float nx, float ny, float nz,
                              float angle) {

	rectangle rect1;
	rectangle rect2;
    float D = sqrt(pow((px - nx),2) + 
    			   pow((pz - nz),2));

	px = px + cos((angle * M_PI/180)) * D;
	pz = pz - sin((angle * M_PI/180)) * D;

	rect1.x = (px - 128);
	rect1.y = (pz - 128);
	rect1.h = 256;
	rect1.w = 256;

	rect2.x = (nx - 512);
	rect2.y = (nz - 512);
	rect2.h = 1024;
	rect2.w = 1024;


	if ( (rect1.x < (rect2.x + rect2.w)) &&
		 ((rect1.x + rect1.w) > rect2.x) &&
		 (rect1.y < (rect2.y + rect2.h)) &&
		 ((rect1.h + rect1.y) > rect2.y)) {
		return true;
	}

	return false;


}


bool gameMath::collisionRectangleHW(int px, int py, int pz,
								    int nx, int ny, int nz,
								    int h,  int w) {

	rectangle rect1;
	rectangle rect2;

	rect1.x = (px - (h/2));
	rect1.y = (pz - (w/2));
	rect1.h = h;
	rect1.w = w;

	rect2.x = (nx - 512);
	rect2.y = (nz - 512);
	rect2.h = 1024;
	rect2.w = 1024;


	if ( (rect1.x < (rect2.x + rect2.w)) &&
		 ((rect1.x + rect1.w) > rect2.x) &&
		 (rect1.y < (rect2.y + rect2.h)) &&
		 ((rect1.h + rect1.y) > rect2.y)) {
		return true;
	}

	return false;
}

bool gameMath::collisionRectangle(int px, int py, int pz,
								  int nx, int ny, int nz) {

	rectangle rect1;
	rectangle rect2;

	rect1.x = (px - 512);
	rect1.y = (pz - 512);
	rect1.h = 1024;
	rect1.w = 1024;

	rect2.x = (nx - 512);
	rect2.y = (nz - 512);
	rect2.h = 1024;
	rect2.w = 1024;


	if ( (rect1.x < (rect2.x + rect2.w)) &&
		 ((rect1.x + rect1.w) > rect2.x) &&
		 (rect1.y < (rect2.y + rect2.h)) &&
		 ((rect1.h + rect1.y) > rect2.y)) {
		return true;
	}

	return false;
}

bool gameMath::mapSwitch(signed int x, signed int z, signed int x1, signed int z1, signed int x2, signed int z2, signed int x3, signed int z3, signed int x4, signed int z4) {
	return 0;
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