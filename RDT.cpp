#include "RDT.h"


RDT::RDT() {
	memset(rdtObjectList, 0x0, 92);
}

RDT::~RDT() {
	free(rdtBuffer);
	free(rdtCameraPos);
	free(rdtLight);
	free(rdtColissionArray);
	free(rdtSCD);
	rdtSCD            = NULL;
	rdtColissionArray = NULL;
	rdtLight          = NULL;
	rdtBuffer         = NULL;
	rdtCameraPos      = NULL;
}

void RDT::rdtLoadFile(std::string fileName) {
	FILE *rdt = NULL;

	rdt = fopen(fileName.c_str(), "rb");

	if (rdt == NULL ) {
		std::cout << "Falha ao carregar RDT !" << std::endl;
		return;
	}

    fseek(rdt, 0, SEEK_END);
    rdtSize = ftell(rdt);
    rewind(rdt);

    rdtBuffer = new unsigned char [rdtSize];
	fread(rdtBuffer, 1, rdtSize, rdt);
    fclose(rdt);
    memcpy(&rdtHeader, rdtBuffer, sizeof(rdtHeader));

    memcpy(&rdtObjectList, (rdtBuffer+0x8), 92);
    

    // Offset 6 - Colisão do Personagem/Cenário

    //memcpy(&rdtColisionHeader, (rdtBuffer+rdtObjectList[6]), sizeof(RDT_COLI_HEADER_T));
    //printf("ArraySize: %d\n", rdtColisionHeader.arraySum);

    rdtColisionHeader.cx = *(unsigned short*)(rdtBuffer+rdtObjectList[6]);
    rdtColisionHeader.cz = *(unsigned short*)(rdtBuffer+rdtObjectList[6]+2);

    rdtColisionHeader.arraySum = 0;
    for (unsigned int i = 0; i < 5; i++) {
    	rdtColisionHeader.arraySum += *(unsigned int*)(rdtBuffer+rdtObjectList[6]+4+(i * 4));
    }


    rdtColissionArray = new RDT_coli_1_5 [rdtColisionHeader.arraySum - 1];

    for (unsigned short i = 0; i < (rdtColisionHeader.arraySum - 1); i++) {
    	memcpy(&rdtColissionArray[i], (rdtBuffer+rdtObjectList[6]+0x18+(i*sizeof(RDT_coli_1_5))), sizeof(RDT_coli_1_5));
    /*	printf("Colissão Nº %d\n", i);
    	printf("X: %d\n", rdtColissionArray[i].X);
    	printf("Z: %d\n", rdtColissionArray[i].Z);
    	printf("D: %d\n", rdtColissionArray[i].D);
    	printf("W: %d\n", rdtColissionArray[i].W);
    	printf("ID: %d\n", rdtColissionArray[i].id);
    	printf("Floor: %d\n", rdtColissionArray[i].Floor);
    	printf("Type: %d\n", (rdtColissionArray[i].Floor & 0x7));*/
    }

    // Offset 7 - Posição das cameras


    rdtCameraPos = new RDT_CAMERA_POS_T [rdtHeader.nCut];


    for (unsigned char i = 0; i < rdtHeader.nCut; i++) {
    	memcpy(&rdtCameraPos[i], (rdtBuffer+rdtObjectList[7]+(i*sizeof(RDT_CAMERA_POS_T))), sizeof(RDT_CAMERA_POS_T));
/*	
		printf("EndFlag: %x\n", rdtCameraPos[i].endFlag);
		printf("viewR:  %x\n", rdtCameraPos[i].viewR);
		printf("PositionX: %d\n", rdtCameraPos[i].positionX);
		printf("PositionY: %d\n", rdtCameraPos[i].positionY);
		printf("PositionZ: %d\n", rdtCameraPos[i].positionZ);
		printf("TargetX: %d\n", rdtCameraPos[i].targetX);
		printf("TargetY: %d\n", rdtCameraPos[i].targetY);
		printf("TargetZ: %d\n", rdtCameraPos[i].targetZ);
*/		
	}

	// Offset 8 - Zonas das Cameras e Camera "Switch"
	RDT_CAMERA_SWITCH_T nodeSwitch;

	unsigned int cont = 0;
	while (1) {
		unsigned int checkInt = 0;

		checkInt = *(unsigned int*)(rdtBuffer+rdtObjectList[8]+(cont*(sizeof(RDT_CAMERA_SWITCH_T))));
		
		if (checkInt == 0xFFFFFFFF) 
			break;

		memcpy(&nodeSwitch, (rdtBuffer+rdtObjectList[8]+(cont*(sizeof(RDT_CAMERA_SWITCH_T)))), sizeof(RDT_CAMERA_SWITCH_T));
		/*
		printf("SwitchNum: %d\n", cont);
		printf("beFlag: 0x%X\n", nodeSwitch.beFlag);
		printf("nFloor: 0x%X\n", nodeSwitch.nFloor);
		printf("Cam0:   0x%X\n", nodeSwitch.cam0);
		printf("Cam1:   0x%X\n", nodeSwitch.cam1);
		printf("X1:       %d\n", nodeSwitch.x1);
		printf("Z1:       %d\n", nodeSwitch.z1);
		printf("X2:       %d\n", nodeSwitch.x2);
		printf("Z2:       %d\n", nodeSwitch.z2);
		printf("X3:       %d\n", nodeSwitch.x3);
		printf("Z3:       %d\n", nodeSwitch.z3);
		printf("X4:       %d\n", nodeSwitch.x4);
		printf("Z4:       %d\n", nodeSwitch.z4);*/
		
		rdtCameraSwitch.push_back(nodeSwitch);
		cont++;
	}

	// Offset 9 - Iluminação para os modelos 3D

	rdtLight = new RDT_LIGHT_T [rdtHeader.nCut];

	for (unsigned int i = 0; i < rdtHeader.nCut; i++) {
		memcpy(&rdtLight[i], (rdtBuffer+rdtObjectList[9]+(i*sizeof(RDT_LIGHT_T))),sizeof(RDT_LIGHT_T));
/*
		printf("RDT Light %d\n", i);
		printf("Light Type: 0x%X\n", rdtLight[i].lightType[1]);
		printf("Color Light: R: %d G: %d B: %d\n", rdtLight[i].colorLight[0].R, rdtLight[i].colorLight[0].G, rdtLight[i].colorLight[0].B);
		printf("Color Ambient: R: %d G: %d B: %d\n", rdtLight[i].colorAmbient.R, rdtLight[i].colorAmbient.G, rdtLight[i].colorAmbient.B);
		printf("Light Pos: X: %d Y: %d Z: %d\n", rdtLight[i].lightPos[0].X, rdtLight[i].lightPos[0].Y, rdtLight[i].lightPos[0].Z);
		
		printf("Color Light: R: %d G: %d B: %d\n", rdtLight[i].colorLight[1].R, rdtLight[i].colorLight[1].G, rdtLight[i].colorLight[1].B);
		printf("Color Ambient: R: %d G: %d B: %d\n", rdtLight[i].colorAmbient.R, rdtLight[i].colorAmbient.G, rdtLight[i].colorAmbient.B);
		printf("Light Pos: X: %d Y: %d Z: %d\n", rdtLight[i].lightPos[1].X, rdtLight[i].lightPos[1].Y, rdtLight[i].lightPos[1].Z);
		
		printf("Color Light: R: %d G: %d B: %d\n", rdtLight[i].colorLight[2].R, rdtLight[i].colorLight[2].G, rdtLight[i].colorLight[2].B);
		printf("Color Ambient: R: %d G: %d B: %d\n", rdtLight[i].colorAmbient.R, rdtLight[i].colorAmbient.G, rdtLight[i].colorAmbient.B);
		printf("Light Pos: X: %d Y: %d Z: %d\n", rdtLight[i].lightPos[2].X, rdtLight[i].lightPos[2].Y, rdtLight[i].lightPos[2].Z);

		printf("Brightness: %d\n", rdtLight[i].brightNess[0]);
*/	
	}



}