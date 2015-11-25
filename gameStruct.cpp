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

void playerClass::setPlayerAnim(unsigned char num) {
	playerAnimNum = num;
}

void playerClass::setPlayerAnimSection(unsigned char sec) {
	animSection = sec;
}

void playerClass::setPlayerInRotatePos(unsigned char pos) {
	inRotatePos = pos;
}

void playerClass::setPlayerRDT(unsigned int RDT) {
	rdtNum = RDT;
}

void playerClass::setPlayerInMove(bool mov) { inMove = mov; }
void playerClass::setPlayerInRotate(bool rot) { inRotate = rot; }
void playerClass::setPlayerInShoot(bool shoot) { inShoot = shoot; }




float playerClass::getPlayerX() { return playerX; }
float playerClass::getPlayerY() { return playerY; }
float playerClass::getPlayerZ() { return playerZ; }

unsigned char playerClass::getPlayerCam() { return camNum; }
unsigned char playerClass::getPlayerAnim() { return playerAnimNum; }
unsigned char playerClass::getPlayerAnimSection() { return animSection; }
unsigned char playerClass::getPlayerInRotatePos() { return inRotatePos; }
unsigned short playerClass::getPlayerEMD() { return playerEMD; }
unsigned int playerClass::getPlayerRDT() { return rdtNum; }
	
bool playerClass::getPlayerInMove() { return inMove; }
bool playerClass::getPlayerInRotate() { return inRotate; }
bool playerClass::getPlayerInShoot() { return inShoot; }


// Item Stuff
int   playerClass::getPlayerItemID(int slotNum) { return item[slotNum].id; }
int   playerClass::getPlayerItemAmount(int slotNum) { return item[slotNum].amount; }
bool  playerClass::getPlayerIsAmount(int slotNum) { return item[slotNum].isAmount; }

void  playerClass::setPlayerItem(int slotNum, int id) {

}

void  playerClass::setPlayerItemAmount(int slotNum, int amount) {

}


bmp_loader_24bpp::bmp_loader_24bpp() {
	memset(bmpHeader, 0x0, 54);
	bmpBuffer = NULL;
}

bmp_loader_24bpp::~bmp_loader_24bpp() {
	delete [] bmpBuffer;
	bmpBuffer = NULL;
}

void bmp_loader_24bpp::bmpLoaderFile(std::string fileName) {
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

	bmpBuffer = new unsigned char [bmpSize];

	fread(bmpBuffer, 1, bmpSize, bmpFile);

	fclose (bmpFile);
}

