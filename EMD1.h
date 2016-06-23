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
#include <cstring>

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

struct EMD1_MODEL_HEADER {
	unsigned int length;
	unsigned int unknown;
	unsigned int objCount;
};

struct EMD1_MODEL_TRIANGLES {
	unsigned int vertexOffset;
	unsigned int vertexCount;
	unsigned int normalOffset;
	unsigned int normalCount;
	unsigned int triOffset;
	unsigned int triCount;
	unsigned int dummy;
};

struct EMD1_VERTEX {
	signed short x;
	signed short y;
	signed short z;
	signed short zero;
};

struct EMD1_NORMAL {
	signed short x;
	signed short y;
	signed short z;
	signed short zero;
};

struct EMD1_TRIANGLE {
	unsigned int unknown;

	unsigned char tu0;
	unsigned char tv0;
	unsigned short clutid;
	unsigned char tu1;
	unsigned char tv1;
	unsigned short page;
	unsigned char tu2;
	unsigned char tv2;

	unsigned short dummy;

	unsigned short n0;
	unsigned short v0;
	unsigned short n1;
	unsigned short v1;
	unsigned short n2;
	unsigned short v2;
};


class EMD1 {
public:
	EMD1();
   ~EMD1();


   void loadFile(std::string fileName);

	EMD1_DIRECTORY                     directory;
	EMD1_SKEL_HEADER                   skeletonHeader;
	EMD1_MODEL_HEADER                  modelHeader;

	std::vector<EMD1_ANIM_HEADER>      animStepHeader;
	std::vector<EMD1_SKEL_RELPOS>      relPos;
	std::vector<EMD1_SKEL_ARMATURE>    armature;
	std::vector<EMD1_MODEL_TRIANGLES>  modelTriangles;
	std::vector<EMD1_SKEL_ANIM>        skelAnimList;

	unsigned int                       emdSec1AnimSkelT[100][500];
	unsigned char **                   meshList;


	EMD1_NORMAL**                      normalList;
	EMD1_VERTEX**                      vertexList;
	EMD1_TRIANGLE**                    triangleList;
};


#endif