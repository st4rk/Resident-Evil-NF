#ifndef PLD_H
#define PLD_H


#include <iostream>
#include <cstdio>
#include <cstdlib>

typedef struct PLD_HEADER_T {
	unsigned int dirOffset;
	unsigned int dirCount;
} PLD_HEADER_T;

typedef struct PLD_ANIM_HEADER_T {
	unsigned short count;
	unsigned short offset;
	unsigned short frame;
	unsigned short dummy;
} PLD_ANIM_HEADER_T;

typedef struct PDL_SEC2_HEADER_T {
	unsigned short relposOffset;
	unsigned short lenght;
	unsigned short count;
	unsigned short size;
} PLD_SEC2_HEADER_T;

typedef struct PLD_RELPOS_VECTOR {
	short x;
	short y;
	short z;
} PLD_RELPOS_VECTOR;

typedef struct PLD_SEC2_ARMATURE_T {
	unsigned short meshCount;
	unsigned short offset;
} PLD_SEC2_ARMATURE_T;

typedef struct PLD_SEC2_DATA_T {
	short xSpeed;
	short ySpeed;
	short zSpeed;

	short yOffset;

	char angles[68];
} PLD_SEC2_DATA_T;


class PLD {
public:
	 PLD();
	~PLD();

	bool PLD_LoadFile(std::string fileName);
	PLD_HEADER_T pldHeader;
private:
	unsigned char *pldBuffer;
	unsigned int   pldSize;
	unsigned int  *pldFileSection;
};



#endif