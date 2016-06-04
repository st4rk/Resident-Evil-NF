/*===========================================*
 * Engine Camera                             * 
 *-------------------------------------------*
 * Here *must* be written all game game      * 
 * used by different game modes              *
 *===========================================*
 */


#ifndef CAM_H
#define CAM_H

#include <cmath>
#include <GL/glut.h>

#define PI 3.14159265359

enum PLAYER_CAMERA_STYLE {
   CAMERA_STYLE_RE_1       = 0,
   CAMERA_STYLE_RE_2       = 1,
   CAMERA_STYLE_SPECIAL    = 2,
   CAMERA_STYLE_SPECIAL_2  = 3,
   CAMERA_STYLE_SEL_CHAR   = 4
};


class cam {
public:
	cam();
   ~cam();

   void camList(unsigned int camNum, float x, float y, float z, float angle);

private:

	float camX;
	float camZ;
	float distX;
	float distZ;
	float distY;

};

#endif