/*===========================================*
 * Player Weapon                             * 
 *-------------------------------------------*
 * This class loads .PLW files               *
 *===========================================*
 */


#ifndef PLW_H
#define PLW_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "TIM.h"

struct PLW_HEADER_DIRECTORY {
	unsigned int dirOffset;
	unsigned int dirCount;
};

struct PLW_ANIM_HEADER {
	unsigned short count;
	unsigned short offset;
	unsigned short frame;
	unsigned short unknown; // padding ?
};

struct PLW_SEC2_HEADER {
	unsigned short relPosOffset;
	unsigned short length;
	unsigned short count;
	unsigned short size;
};

struct PLW_SEC2_RELPOS {
	short x;
	short y;
	short z;
};


struct PLW_SEC2_ARMATURE {
	unsigned short meshCount;
	unsigned short offset;
};

struct PLW_SEC2_DATA {
	short xSpeed;
	short ySpeed;
	short zSpeed;

	short yOffset;

	char angles[68];
};

struct PLW_SECTION_3_HEADER {
	unsigned int lenght; // Section Lenght in Bytes
	unsigned int unk32; // Desconhecido
	unsigned int objCount; // Número de objetos no modelo
};



struct PLW_MODEL_TRIANGLES_T {
	unsigned int vertexOffset;
	unsigned int vertexCount;
	unsigned int normalOffset; 
	unsigned int normalCount;
	unsigned int triOffset;
	unsigned int triCount;
	unsigned int triTextureOffset;
};


struct PLW_MODEL_QUADS_T {
	unsigned int vertexOffset;
	unsigned int vertexCount;
	unsigned int normalOffset; 
	unsigned int normalCount;
	unsigned int quadOffset;
	unsigned int quadCount;
	unsigned int quadTextureOffset;
};

struct PLW_MODEL_OBJECT_T {
	PLW_MODEL_TRIANGLES_T triangles;
	PLW_MODEL_QUADS_T     quads;
};


struct PLW_VERTEX_T {
	signed short x;
	signed short y;
	signed short z;
	signed short zero;
};


struct PLW_NORMAL_T {
	signed short x;
	signed short y;
	signed short z;
	signed short zero;
};


struct PLW_TRIANGLE_T {
	unsigned short n0;
	unsigned short v0;
	unsigned short n1;
	unsigned short v1;
	unsigned short n2;
	unsigned short v2;	
};

struct PLW_QUAD_T {
	unsigned short n0;
	unsigned short v0;
	unsigned short n1;
	unsigned short v1;
	unsigned short n2;
	unsigned short v2;	
	unsigned short n3;
	unsigned short v3;	
};

struct PLW_TRITEXTURE_T {
	unsigned char u0;
	unsigned char v0;
	unsigned short clutid;
	unsigned char u1;
	unsigned char v1;
	unsigned short page;
	unsigned char u2;
	unsigned char v2;
	unsigned short zero;
};


struct PLW_QUADTEXTURE_T {
	unsigned char u0;
	unsigned char v0;
	unsigned short clutid;
	unsigned char u1;
	unsigned char v1;
	unsigned short page;
	unsigned char u2;
	unsigned char v2;
	unsigned short zero;
	unsigned char u3;
	unsigned char v3;
	unsigned short zero2;
};

class PLW {
public:
	PLW();
   ~PLW();

    bool readFile(std::string fileName);

	PLW_HEADER_DIRECTORY directory;
	PLW_SEC2_HEADER      sec2Header;
	PLW_SECTION_3_HEADER sec3Header;

	PLW_VERTEX_T 			   **plwVertex;
	PLW_VERTEX_T               **plwquadVertex;
	PLW_NORMAL_T               **plwNormal;
	PLW_NORMAL_T               **plwquadNormal;
	PLW_TRIANGLE_T             **plwTriangle;
	PLW_QUAD_T                 **plwQuad;
	PLW_QUADTEXTURE_T          **plwQuadTexture;
	PLW_TRITEXTURE_T           **plwTritexture;

	std::vector<PLW_ANIM_HEADER>    sec1Header;

	std::vector<PLW_SEC2_ARMATURE>  sec2Armature;
	std::vector<PLW_SEC2_RELPOS>    sec2RelPos;

	std::vector<PLW_MODEL_OBJECT_T> sec3ModelBuffer;

	unsigned int                   PLWAnimSkelT[100][500];
	unsigned int  *PLW_SECTION;
	unsigned char **               sec2Mesh;

	TIM                            emdTimTexture;

};

#endif