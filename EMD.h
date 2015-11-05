/** Desenvolvido por Lucas P. Stark em 19/05/2015
 ** EMD é o formato de arquivos utilizados por Resident Evil 2
 ** ele contem modelo 3D dos personagens, inimigos e etc...
 ** Documentação: http://rewiki.regengedanken.de/wiki/.EMD_%28Resident_Evil_2_PC%29
 **/

#ifndef EMD_H
#define EMD_H

#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <list>
#include <vector>
#include "TIM.h"




typedef struct EMD_HEADER_DIRECTORY {
	unsigned int dirOffset;
	unsigned int dirCount;
} EMD_HEADER_DIRECTORY;

typedef struct EMD_SECTION_1_HEADER {
	unsigned short dataCount;
	unsigned short dataOffset;
} EMD_SECTION_1_HEADER;



typedef struct EMD_SECTION_2_HEADER {
	unsigned short relposOffset;
	unsigned short lenght;
	unsigned short count;
	unsigned short size;
} EMD_SECTION_2_HEADER;


typedef struct EMD_SEC2_RELPOS_T {
	short x;
	short y;
	short z;
} EMD_SEC2_RELPOS_T;


typedef struct EMD_SEC2_ARMATURE_T {
	unsigned short meshCount;
	unsigned short offSet;
} EMD_SEC2_ARMATURE_T;

typedef struct EMD_SEC2_VECTOR {
	float x;
	float y;
	float z;
} EMD_SEC2_VECTOR;

typedef struct EMD_SEC2_DATA_T {
	short xOffset;
	short yOffset;
	short zOffset;

	short xSpeed;
	short ySpeed;
	short zSpeed;

	unsigned char *angles;

	EMD_SEC2_VECTOR vector[500];

} EMD_SEC2_DATA_T;


typedef struct EMD_SECTION_7_HEADER {
	unsigned int lenght; // Section Lenght in Bytes
	unsigned int unk32; // Desconhecido
	unsigned int objCount; // Número de objetos no modelo
} EMD_SECTION_7_HEADER;



typedef struct EMD_MODEL_TRIANGLES_T {
	unsigned int vertexOffset;
	unsigned int vertexCount;
	unsigned int normalOffset; 
	unsigned int normalCount;
	unsigned int triOffset;
	unsigned int triCount;
	unsigned int triTextureOffset;
} EMD_MODEL_TRIANGLES_T;


typedef struct EMD_MODEL_QUADS_T {
	unsigned int vertexOffset;
	unsigned int vertexCount;
	unsigned int normalOffset; 
	unsigned int normalCount;
	unsigned int quadOffset;
	unsigned int quadCount;
	unsigned int quadTextureOffset;
} EMD_MODEL_QUADS_T;

typedef struct EMD_MODEL_OBJECT_T {
	EMD_MODEL_TRIANGLES_T triangles;
	EMD_MODEL_QUADS_T     quads;
} EMD_MODEL_OBJECT_T;


typedef struct EMD_VERTEX_T {
	signed short x;
	signed short y;
	signed short z;
	signed short zero;
} EMD_VERTEX_T;


typedef struct EMD_NORMAL_T {
	signed short x;
	signed short y;
	signed short z;
	signed short zero;
} EMD_NORMAL_T;


typedef struct EMD_TRIANGLE_T {
	unsigned short n0;
	unsigned short v0;
	unsigned short n1;
	unsigned short v1;
	unsigned short n2;
	unsigned short v2;	
} EMD_TRIANGLE_T;

typedef struct EMD_QUAD_T {
	unsigned short n0;
	unsigned short v0;
	unsigned short n1;
	unsigned short v1;
	unsigned short n2;
	unsigned short v2;	
	unsigned short n3;
	unsigned short v3;	
} EMD_QUAD_T;

typedef struct EMD_TRITEXTURE_T {
	unsigned char u0;
	unsigned char v0;
	unsigned short clutid;
	unsigned char u1;
	unsigned char v1;
	unsigned short page;
	unsigned char u2;
	unsigned char v2;
	unsigned short zero;
} EMD_TRITEXTURE_T;


typedef struct EMD_QUADTEXTURE_T {
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
} EMD_QUADTEXTURE_T;


/* Customização para melhorar a renderização */

typedef struct EMD_ANIM_INFO {
   unsigned int animStart;
   unsigned int animCount;
} EMD_ANIM_INFO;


class EMD {
public:
	EMD();

	~EMD();

	void emdDebugPrint();
	void verifyRelPos(unsigned int objNum, int *x, int *y, int *z);
	void recurs(unsigned int objNum, int*x, int*y, int*z, int var);
	void recurRelPos(unsigned int objNum, int *x, int *y, int *z, int var, int var2);
	bool emdLoadFile(std::string dir);

	EMD_MODEL_OBJECT_T         *emdObjectBuffer;

	EMD_VERTEX_T 			   **emdVertex;
	EMD_VERTEX_T               **emdquadVertex;
	EMD_NORMAL_T               **emdNormal;
	EMD_NORMAL_T               **emdquadNormal;
	EMD_TRIANGLE_T             **emdTriangle;
	EMD_QUAD_T                 **emdQuad;
	EMD_QUADTEXTURE_T          **emdQuadTexture;
	EMD_TRITEXTURE_T           **emdTritexture;
	// Section 2
	EMD_SEC2_RELPOS_T           *emdSec2RelPos;
	EMD_SEC2_ARMATURE_T         *emdSec2Armature;
	EMD_SECTION_2_HEADER         emdSec2Header;
	std::vector<EMD_SEC2_DATA_T> emdSec2Data;
	EMD_ANIM_INFO               *emdSec2AnimInfo;

	// Section 3 and Section 4
	EMD_SECTION_2_HEADER         emdSec4Header;
	std::vector<EMD_SEC2_DATA_T> emdSec4Data;
	EMD_ANIM_INFO               *emdSec4AnimInfo;
	
	EMD_SECTION_1_HEADER        *emdSec3Header;
	unsigned int                 emdSec3AnimSkelT[100][500];
	unsigned short               sizeSec3Header;

	// Section 1
	EMD_SECTION_1_HEADER        *emdSec1Header;
	unsigned char              **emdSec2Mesh;
	unsigned int                emdSize;
	unsigned int                totalAnim;
	unsigned int                emdTotalObj;
	unsigned int                emdSec1AnimSkelT[100][500];
	unsigned short              sizeSec1Header;

	TIM                         emdTimTexture;

private:
	unsigned char              *emdBufferData;
	unsigned int               *emdFileSection;

	EMD_HEADER_DIRECTORY        emdHeader;
	EMD_SECTION_7_HEADER        emdSec7Header;

};



#endif