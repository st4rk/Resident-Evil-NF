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

typedef struct Point {
	int X;
	int Y;
} Point;

#define COL_TYPE_RECTANGLE 1
#define COL_TYPE_TRIANGLE  3

gameMath::gameMath() {

}

gameMath::~gameMath() {

}

bool IsPointInPolygon( Point p, Point* polygon ) {
    int minX = polygon[ 0 ].X;
    int maxX = polygon[ 0 ].X;
    int minY = polygon[ 0 ].Y;
    int maxY = polygon[ 0 ].Y;

    for ( int i = 1 ; i < 4 ; i++ )
    {
        Point q = polygon[ i ];
        minX = std::min( q.X, minX );
        maxX = std::max( q.X, maxX );
        minY = std::min( q.Y, minY );
        maxY = std::max( q.Y, maxY );
    }

    if ( p.X < minX || p.X > maxX || p.Y < minY || p.Y > maxY )
    {
        return false;
    }

    // http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
    bool inside = false;
    for ( int i = 0, j = 4- 1 ; i < 4 ; j = i++ )
    {
        if ( ( polygon[ i ].Y > p.Y ) != ( polygon[ j ].Y > p.Y ) &&
             p.X < ( polygon[ j ].X - polygon[ i ].X ) * ( p.Y - polygon[ i ].Y ) / ( polygon[ j ].Y - polygon[ i ].Y ) + polygon[ i ].X )
        {
            inside = !inside;
        }
    }

    return inside;
}

bool gameMath::mapSwitch(signed int x, signed int z, signed int x1, signed int z1, signed int x2, signed int z2, signed int x3, signed int z3, signed int x4, signed int z4) {
	Point P;
	Point V[4];

	P.X = x;
	P.Y = z;

	V[0].X = x1;
	V[0].Y = z1;

	V[1].X = x2;
	V[1].Y = z2;


	V[2].X = x3;
	V[2].Y = z3;

	V[3].X = x4;
	V[3].Y = z4;

	return (IsPointInPolygon(P, V));

}

bool gameMath::collisionDetect(unsigned int colType, signed int x1, signed int z1, 
	                           signed int x2, signed int z2, 
	                           signed int x, signed int z) {

	switch (colType) {

		case COL_TYPE_RECTANGLE:
			if ((x1 <= x) && (x <= (x1+x2))) {
				if ((z1 <= z) && (z <= (z1+z2))) {
					return true;
				}
			}
		break;

		case COL_TYPE_TRIANGLE: {
			double RayX = (x2 - x1) / 2;
			double RayZ = (z2 - z1) / 2;
			double cX = x1 + RayX;
			double cZ = x1 + RayZ;
			double k = pow((x - cX) / RayX, 2) + pow((z - cZ) / RayZ, 2);
			if (k <= 1)
				return true;
		}
			return false;
		break;

		default:
			std::cout << "The collision type: " << colType << " wasn't implemented" << std::endl;
		break;

	}

	

	return false;
}