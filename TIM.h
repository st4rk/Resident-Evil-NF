#ifndef TIM_H
#define TIM_H


#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>


#define RGB555(R,G,B) ((((unsigned int)(B))<<10)|(((unsigned int)(G))<<5)|(((unsigned int)(R))))

typedef struct TIM_DATA {
	unsigned int MAGIC_NUM;
	unsigned int TIM_BPP;
	unsigned int UNK_32;
	unsigned short paletteOrgX;
	unsigned short paletteOrgY;
	unsigned short unk;
	unsigned short numbCluts;
	unsigned char* clutData;
	unsigned int UNK_32_2;
	unsigned short imageOrgX;
	unsigned short imageOrgY;
	unsigned short imageWidth; // mult by 2 to get the actually width
	unsigned short imageHeight;
} TIM_DATA ;

class TIM {
public:
	TIM();

   ~TIM();

   void timLoad(std::string imgName);
	TIM_DATA timTexture;
	unsigned char *rawTexture;
private:
	unsigned int **colorTable;
	unsigned char *timData;
	unsigned int timSize;

};
 


#endif