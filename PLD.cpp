#include "PLD.h"




PLD::PLD () {

}

PLD::~PLD() {
	delete [] pldBuffer;
	pldBuffer = NULL;
}



bool PLD::PLD_LoadFile(std::string fileName) {
	FILE *pld = NULL;

	pld = fopen(fileName.c_str(), "rb");

	if (pld == NULL) {
		std::cout << "Nao foi possivel encontrar o PDL !" << std::endl;
		return false;
	}

	fseek(pld, 0, SEEK_END);
	pldSize = ftell(pld);
	rewind(pld);

	pldBuffer = new unsigned char[pldSize];

	fread(pldBuffer, pldSize, 0, pld);

	// PLD HEADER
	pldHeader = *(PLD_HEADER_T*)&(pldBuffer[0]);

	pldFileSection = new unsigned int [pldHeader.dirCount];

	for (unsigned int i = 0; i < (pldHeader.dirCount); i++) {
		pldFileSection[i] = *(unsigned int*)(pldBuffer+pldHeader.dirOffset+(i*4));
		printf("Section: %d - Offset: 0x%X\n", i, pldFileSection[i]);
	}
	


	fclose(pld);
	return true;
}