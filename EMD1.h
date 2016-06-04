/*===========================================*
 * Resident Evil 1 EMD                       * 
 *-------------------------------------------*
 * Load EMD binary data                      *
 *===========================================*
 */

#ifndef EMD_1_H
#define EMD_1_H

#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>

struct EMD1_DIRECTORY {
	unsigned int section0;
	unsigned int section1;
	unsigned int section2;
	unsigned int section3;
};

struct EMD1_SKEL_RELPOS {
	short x;
	short y;
	short z;
};

struct EMD1_SKEL_HEADER {
	unsigned short relPosOffset;
	unsigned short length;
	unsigned short count;
	unsigned short size;
};

struct EMD1_SKEL_ARMATURE {
	unsigned short meshCount;
	unsigned short offset;
};

struct EMD1_SKEL_ANIM {
	short xOffset;
	short yOffset;
	short zOffset;

	short xSpeed;
	short ySpeed;
	short zSpeed;

	short angles[45];

	short unknown;
};

struct EMD1_ANIM_HEADER {
	unsigned short count;
	unsigned short offset;
};




class EMD1 {
public:
	EMD1();
   ~EMD1();


   void loadFile(std::string fileName);

private:
	EMD1_DIRECTORY                  directory;
	EMD1_SKEL_HEADER                skeletonHeader;

	std::vector<EMD1_SKEL_RELPOS>   relPos;
	std::vector<EMD1_SKEL_ARMATURE> armature;

};


#endif