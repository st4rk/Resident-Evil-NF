#ifndef RDT_H
#define RDT_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>


/******************************************************
**           Resident Evil 1 RDT Support             **
*******************************************************/

typedef struct RDT_RE1_PART_T {
	int unk_pos_x;
	int unk_pos_y;
	int unk_pos_z;
	unsigned int unk0[2];
} RDT_RE1_PART_T;

typedef struct RDT_RE1_HEADER_T {
	char unk0;
	char num_cameras;
	char num_sound_banks;
	char unk1[3];
	unsigned short unk2[3];

	RDT_RE1_PART_T unk3[3];
} RDT_RE1_HEADER_T;

typedef struct RDT_RE1_CAMERA_POS_T {
	signed int mask_offset;
	signed int tim_mask_offset;

	signed int positionX;
	signed int positionY;
	signed int positionZ;
	signed int targetX;
	signed int targetY;
	signed int targetZ;
	signed int unk[3];
} RDT_RE1_CAMERA_POS_T;

typedef struct RDT_RE1_CAMERA_SWITCH_T {
	unsigned short to;
	unsigned short from;
	signed short x1;
	signed short z1;
	signed short x2;
	signed short z2;
	signed short x3;
	signed short z3;
	signed short x4;
	signed short z4;
} RDT_RE1_CAMERA_SWITCH_T;


typedef struct RDT_RE1_SCA_HEADER_T {
	unsigned short cx;
	unsigned short cy;
	unsigned int   counts;
} RDT_RE1_SCA_HEADER_T;


typedef struct RDT_RE1_SCA_OBJ_T {
	unsigned short x1; // D
	unsigned short z1; // W
	unsigned short x2;
	unsigned short z2;
	unsigned char  type;
	unsigned char  id;
	unsigned short floor;
} RDT_RE1_SCA_OBJ_T;

/******************************************************
** Resident Evil 1.5 and Resident Evil 2 RDT Support **
*******************************************************/

typedef struct RDT_HEADER_T {
	unsigned char nSprite;
	unsigned char nCut; // Quantidade de cameras 
	unsigned char nOmodel; 
	unsigned char nItem;
	unsigned char nDoor;
	unsigned char nRoom_at;
	unsigned char Reverb_lv;
	unsigned char nSprite_max;
} RDT_HEADER_T;


typedef struct RDT_CAMERA_POS_T {
	unsigned short endFlag;
	unsigned short viewR;
	signed int positionX;
	signed int positionY;
	signed int positionZ;
	signed int targetX;
	signed int targetY;
	signed int targetZ;
	unsigned int pSprite;
} RDT_CAMERA_POS_T;


typedef struct RDT_COLI_HEADER_T {
	signed short cx;
	signed short cz;
	unsigned int arraySum; // five 4 bytes will be plus, the total is the total of
						   // collision, the total of colission will be (arraySum - 1)
} RDT_COLI_HEADER_T;

typedef struct RDT_OFFSET6ELT_T {
	signed short X;
	signed short Z;
	unsigned short W;
	unsigned short H;
	unsigned short id;
	unsigned short type;
	unsigned int floor;
} RDT_OFFSET6ELT_T;

typedef struct RDT_LIGHT_COLOR_T {
	unsigned char R;
	unsigned char G;
	unsigned char B;
} RDT_LIGHT_COLOR_T;

typedef struct RDT_LIGHT_POS_T {
	short X;
	short Y;
	short Z;
} RDT_LIGH_POS_T;

typedef struct RDT_LIGHT_T {
	unsigned short    lightType[2];
	RDT_LIGHT_COLOR_T colorLight[3];
	RDT_LIGHT_COLOR_T colorAmbient;
	RDT_LIGHT_POS_T   lightPos[3];
	unsigned short    brightNess[3];
} RDT_LIGHT_T;

typedef struct RDT_coli_1_5 {
	unsigned short W;
	unsigned short D;
	signed short X;
	signed short Z;
	unsigned short id;
	unsigned short Floor;
} RDT_coli_1_5;


typedef struct RDT_CAMERA_SWITCH_T {
	unsigned char beFlag;
	unsigned char nFloor;
	unsigned char cam0;
	unsigned char cam1;
	signed short x1;
	signed short z1;
	signed short x2;
	signed short z2;
	signed short x3;
	signed short z3;
	signed short x4;
	signed short z4;
} RDT_CAMERA_SWITCH_T;

class RDT {
public:
	RDT();
   ~RDT();

    void rdtLoadFile(std::string fileName);
    void rdtRE1LoadFile(std::string fileName);

	RDT_CAMERA_POS_T                   *rdtCameraPos;
	RDT_HEADER_T                        rdtHeader;
	RDT_LIGHT_T                        *rdtLight;
	RDT_COLI_HEADER_T                   rdtColisionHeader;
	RDT_coli_1_5                       *rdtColissionArray;
	std::vector<RDT_CAMERA_SWITCH_T>    rdtCameraSwitch;


	RDT_RE1_HEADER_T                       rdtRE1Header;
	RDT_RE1_CAMERA_POS_T                  *rdtRE1CameraPos;
	RDT_RE1_SCA_HEADER_T                   rdtRE1ColisionHeader;
	RDT_RE1_SCA_OBJ_T                     *rdtRE1ColissionArray;
	std::vector<RDT_RE1_CAMERA_SWITCH_T>   rdtRE1CameraSwitch;

private:
	unsigned char      *rdtBuffer;
	unsigned int        rdtObjectList[22];
	unsigned int        rdtSize;
	unsigned short     *rdtSCD;



};


#endif