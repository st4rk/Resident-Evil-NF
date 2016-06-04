#include "EMD1.h"

EMD1::EMD1() {

}


EMD1::~EMD1() {

}


void EMD1::loadFile(std::string fileName) {
	FILE *emd1 = NULL;
	unsigned int emd1Size = 0;
	unsigned char buffer *;

	emd1 = fopen(fileName.c_str(), "rb");

	if (emd1 == NULL) {
		std::cout << "EMD File: " << emd1 << " not found" << std::endl;
		return;
	}

	fseek (emd1, 0, SEEK_END);
	emd1Size = ftell(emd1);
	rewind(emd1);

	buffer = new unsigned char [emd1Size];


	directory = *(EMD1_DIRECTORY*)(&buffer[emd1Size - 0x10]);	


	skeletonHeader = *(EMD1_SKEL_HEADER*)(&buffer[directory.section0]);
	

	delete [] buffer;

	fclose(emd1);
}