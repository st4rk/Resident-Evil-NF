/***************************************************************************
** gameStruct é utilizada para todas as estruturas/classes relacionadas   **
** ao Player, Inimigos, Objetos(?) e por aí vai                           **
** Criado por Lucas P. Stark                                              **
** Nightmare Fiction Engine - NFE                                         **
***************************************************************************/

#include "gameStruct.h"


entity::entity() {
	modelNum       = 0;
	animSection    = 0;
	animCount      = 0;
	animOldSection = 0;
	animOldCount   = 0;
	animType       = 0;
	animRepeat     = true;

}

entity::~entity() {
	modelNum       = 0;
	animSection    = 0;
	animCount      = 0;
	animOldSection = 0;
	animOldCount   = 0;
	animType       = 0;
	animRepeat     = true;
}


void entity::setModel(unsigned int modelNum) {
	this->modelNum = modelNum;
}

void entity::setAnimSection(unsigned int animSection) {
	this->animSection = animSection;
}

void entity::setAnimType(unsigned int animType, bool animRepeat) {
	this->animType   = animType;
	this->animRepeat = animRepeat;
	this->animCount  = 0;
}

void entity::setAnimCount(unsigned int animCount) {
	this->animCount = animCount;
}

void entity::setAnimFrame(EMD_SEC2_DATA_T animFrame) {
	this->animFrame = animFrame;
}

void entity::setAnimRotationDir(unsigned int animRotationDir) {
	this->animRotationDir = animRotationDir;
}

void entity::setX(float x) {
	this->x = x;
}

void entity::setY(float y) {
	this->y = y;
}

void entity::setZ(float z) {
	this->z = z;
}

void entity::setAngle(float angle) {
	this->angle = angle;
}


unsigned int      entity::getModel()             { return modelNum;        }
 
unsigned int      entity::getAnimSection()       { return animSection;     }
unsigned int      entity::getAnimType()          { return animType;        }
unsigned int      entity::getAnimCount()         { return animCount;       }
unsigned int      entity::getAnimOldSection()    { return animOldSection;  }
unsigned int      entity::getAnimOldCount()      { return animOldCount;    }
unsigned char     entity::getAnimRotationDir()   { return animRotationDir; }
EMD_SEC2_DATA_T   entity::getAnimFrame()         { return animFrame;       }
bool              entity::getAnimRepeat()        { return animRepeat;      }

float  entity::getX()     { return x; }
float  entity::getY()     { return y; }
float  entity::getZ()     { return z; }
float  entity::getAngle() { return angle; }


enemy::enemy() {
	type  = 0;
	state = 0;
}

enemy::~enemy() {
	type  = 0;
	state = 0;
}

void enemy::setType(unsigned int type)   { this->type  = type;   }
void enemy::setState(unsigned int state) { this->state = state; }

unsigned int enemy::getType()  { return type;  }
unsigned int enemy::getState() { return state; }


player::player() {
	for (int i = 0; i < 8; i++) {
		iID[i] = 0x0;
	}
}

player::~player() {
	for (int i = 0; i < 8; i++) {
		iID[i] = 0x0;
	}
}


void player::setItemID(unsigned char slot, unsigned int iID) {
	if (slot >= 8) 
		return;

	this->iID[slot] = iID;
}

void player::setCam(unsigned int cam) { this->cam = cam; }

unsigned int player::getItemID(unsigned char slot) {
	if (slot >= 8)
		return -1;

	return iID[slot];
}

unsigned int player::getCam() { return cam; }


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

