#include "cam.h"


cam::cam() {
	camX  = 0;
	camZ  = 0;
	distX = 0;
	distZ = 0;
	distY = 0;
}


cam::~cam() {
	camX  = 0;
	camZ  = 0;
	distX = 0;
	distZ = 0;
	distY = 0;
}

/*
 * camList
 * This function handle all cameras used in game
 */

void cam::camList(unsigned int camNum, float x, float y, float z, float angle) {

    switch (camNum) {
        case CAMERA_STYLE_SEL_CHAR: {

            float camX = 0  - sin((angle  *  PI/180))  * 800.0f;
            float camZ = 0  - cos((angle  *  PI/180))  * 800.0f;
            float distX =    -cos((angle  *  PI/180))  * 3500.0;
            float distZ =     sin((angle  *  PI/180))  * 3500.0;
            float distY = -2200.0f;
            gluLookAt(     camX + distX,  distY, camZ + distZ,
                           camX, -2600.0f, camZ,   
                           0.0f, -0.1f, 0.0f);

        }            

        case CAMERA_STYLE_RE_1:
	   //     gluLookAt(playerRDT.rdtRE1CameraPos[mainPlayer.getCam()].positionX, playerRDT.rdtRE1CameraPos[mainPlayer.getCam()].positionY, playerRDT.rdtRE1CameraPos[mainPlayer.getCam()].positionZ,
	   //               playerRDT.rdtRE1CameraPos[mainPlayer.getCam()].targetX, playerRDT.rdtRE1CameraPos[mainPlayer.getCam()].targetY, playerRDT.rdtRE1CameraPos[mainPlayer.getCam()].targetZ,   
	   //               0.0f,   -0.1f,   0.0f);
    
        break;

        case CAMERA_STYLE_RE_2:
	   //     gluLookAt(playerRDT.rdtCameraPos[mainPlayer.getCam()].positionX, playerRDT.rdtCameraPos[mainPlayer.getCam()].positionY, playerRDT.rdtCameraPos[mainPlayer.getCam()].positionZ,
	   //               playerRDT.rdtCameraPos[mainPlayer.getCam()].targetX, playerRDT.rdtCameraPos[mainPlayer.getCam()].targetY, playerRDT.rdtCameraPos[mainPlayer.getCam()].targetZ,   
	   //               0.0f,   -0.1f,   0.0f);
        break;

        case CAMERA_STYLE_SPECIAL: {
            camX  = x  - sin((angle * PI/180)) * 800.0f;
            camZ  = z  - cos((angle * PI/180)) * 800.0f;
            distX =    -cos((angle * PI/180))  * 3500.0;
            distZ =     sin((angle * PI/180))  * 3500.0;
            distY =    -3000.0f;
            gluLookAt(     camX + distX,  distY, camZ + distZ,
                           camX, -2500.0f, camZ,   
                           0.0f, -0.1f, 0.0f);
        }

        break;


        case CAMERA_STYLE_SPECIAL_2: {
            camX  = x  - sin((angle * PI/180)) * 500.0f;
            camZ  = z  - cos((angle * PI/180)) * 500.0f;
            distX =     -cos((angle * PI/180))  * 1000.0;
            distZ =      sin((angle * PI/180))  * 1000.0;
            distY =     -2800.0f;
            gluLookAt(     camX + distX,  distY, camZ + distZ,
                           camX, -2700.0f, camZ,   
                           0.0f, -0.1f, 0.0f);    
        }
        break;


    }
}