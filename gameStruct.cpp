/***************************************************************************
** gameStruct é utilizada para todas as estruturas/classes relacionadas   **
** ao Player, Inimigos, Objetos(?) e por aí vai                           **
** Criado por Lucas P. Stark                                              **
** Nightmare Fiction Engine - NFE                                         **
***************************************************************************/

#include "gameStruct.h"


// Classe do personagem principal, todas as informações a serem utilizadas por 
// ele vai ficar nessa classe, aqui você pode pegar informações do eixo x,y,z
// que o personagem se encontra, como o número da animação, como também o modelo

playerClass::playerClass() {
	inMove = false;
}

playerClass::~playerClass() {

}

void playerClass::setPlayerCam(unsigned char num) {
	camNum = num;
}

void playerClass::setPlayerEMD(unsigned short num) {
	playerEMD = num;
}

void playerClass::setPlayerX(float x) {
	playerX = x;
}

void playerClass::setPlayerY(float y) {
	playerY = y;
}

void playerClass::setPlayerZ(float z) {
	playerZ = z;
}

void playerClass::setPlayerAngle(float n) {
	p = n;
}

void playerClass::setPlayerAnimCount(int n) {
	animCount = n;
}

void playerClass::setPlayerAnim(unsigned char num, bool repeat) {
	playerAnimNum = num;
	animRepeat = repeat;
}

void playerClass::setPlayerAnimSection(unsigned char sec) {
	animSection = sec;
}

void playerClass::setPlayerRDT(unsigned int RDT) {
	rdtNum = RDT;
}

void  playerClass::setPlayerItem(int slotNum, int id) {

}

void  playerClass::setPlayerItemAmount(int slotNum, int amount) {

}

void playerClass::setPlayerEMDAnim(EMD_SEC2_DATA_T n) { anim = n; }
void playerClass::setPlayerInRotation(unsigned char inRotation) { this->inRotation = inRotation; }

float playerClass::getPlayerX() { return playerX; }
float playerClass::getPlayerY() { return playerY; }
float playerClass::getPlayerZ() { return playerZ; }
float playerClass::getPlayerAngle() { return p; }

unsigned char  playerClass::getPlayerCam() { return camNum; }
unsigned char  playerClass::getPlayerAnim() { return playerAnimNum; }
unsigned char  playerClass::getPlayerAnimSection() { return animSection; }
unsigned char  playerClass::getPlayerInRotation() { return inRotation; }

unsigned short playerClass::getPlayerEMD() { return playerEMD; }
unsigned int   playerClass::getPlayerRDT() { return rdtNum; }
unsigned int   playerClass::getPlayerAnimCount() { return animCount; }

bool playerClass::getPlayerIsAmount(int slotNum) { return item[slotNum].isAmount; }
bool playerClass::getPlayerRepeatAnim() { return animRepeat; }

int   playerClass::getPlayerItemID(int slotNum) { return item[slotNum].id; }
int   playerClass::getPlayerItemAmount(int slotNum) { return item[slotNum].amount; }


EMD_SEC2_DATA_T playerClass::getPlayerEMDAnim() { return anim; }



/****************
** Enemy Class **
*****************/

enemyClass::enemyClass() {

}

enemyClass::~enemyClass() {

}


float enemyClass::getX() { return x; }
float enemyClass::getY() { return y; }
float enemyClass::getZ() { return z; }
float enemyClass::getAngle() { return p; }

int   enemyClass::getEMD() { return emd; }

unsigned int enemyClass::getAnimCount() { return animCount; }

void  enemyClass::setX(float X) { x = X; }
void  enemyClass::setY(float Y) { y = Y; }
void  enemyClass::setZ(float Z) { z = Z; }
void  enemyClass::setEMD(int n) { emd = n; }
void  enemyClass::setAngle(float n) { p = n; }
void  enemyClass::setAnim(EMD_SEC2_DATA_T n) { anim = n; }
void  enemyClass::setAnimCount(unsigned int n) { animCount = n; }

EMD_SEC2_DATA_T enemyClass::getAnim() { return anim; }

bmp_loader_24bpp::bmp_loader_24bpp() {
	memset(bmpHeader, 0x0, 54);
	bmpBuffer = NULL;
}

bmp_loader_24bpp::~bmp_loader_24bpp() {
	delete [] bmpBuffer;
	bmpBuffer = NULL;
}

void bmp_loader_24bpp::bmpLoaderFile(std::string fileName, int type) {
	FILE *bmpFile = NULL;

	bmpFile = fopen(fileName.c_str(), "rb");


	if (bmpBuffer != NULL) {
		delete [] bmpBuffer;
		bmpBuffer = NULL;
	}

	if (bmpFile == NULL) {
		std::cout << "Erro ao carregar bitmap !" << std::endl;
		return;
	}

	if ( fread (bmpHeader, 1, 54, bmpFile) != 54) {
		std::cout << "Erro ao carregar bitmap, provavelmente não é um bitmap (?) " << std::endl;
		return;
	}

	if ((bmpHeader[0] != 'B') || (bmpHeader[1] != 'M')) {
		std::cout << "Arquivo nao é um bitmap !" << std::endl;
		return;
	}

	bmpDataInit = *(int*)&(bmpHeader[0x0A]);
	bmpSize     = *(int*)&(bmpHeader[0x22]);
	bmpWidth    = *(int*)&(bmpHeader[0x12]);
	bmpHeight   = *(int*)&(bmpHeader[0x16]);

	if (type == 1) {
		bmpBuffer = new unsigned char [bmpHeight * bmpWidth * 4];

		unsigned char *buffer = new unsigned char[bmpSize];
		fread(buffer, 1, bmpSize, bmpFile);

		unsigned int j = 0;
		for (unsigned int i = 0; i < bmpSize; i+=3, j+= 4) {

			if ((buffer[i] == 0) && (buffer[i+1] == 0) && (buffer[i+2] == 0)) {
				bmpBuffer[j]   = 0;
				bmpBuffer[j+1] = 0;
				bmpBuffer[j+2] = 0;
				bmpBuffer[j+3] = 0;
			} else {
				bmpBuffer[j]   = buffer[i];
				bmpBuffer[j+1] = buffer[i+1];
				bmpBuffer[j+2] = buffer[i+2];
				bmpBuffer[j+3] = 255;
			}
		}
	} else {
		bmpBuffer = new unsigned char [bmpSize];
		fread(bmpBuffer, 1, bmpSize, bmpFile);
	}

	fclose (bmpFile);
}

