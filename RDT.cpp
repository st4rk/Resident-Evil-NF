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

	// RE1
	if (rdtRE1CameraPos != NULL) {
		delete [] rdtRE1CameraPos;
		rdtRE1CameraPos = NULL;
	}

	if (rdtRE1ColissionArray != NULL) {
		delete [] rdtRE1ColissionArray;
		rdtRE1ColissionArray = NULL;
	}

}

void RDT::rdtRE1LoadFile(std::string fileName) {
	FILE *rdt = NULL;

	rdt = fopen(fileName.c_str(), "rb");

	if (rdt == NULL ) {
		std::cout << "Falha ao carregar RDT !" << std::endl;
		return;
	}

	// Get File size
    fseek(rdt, 0, SEEK_END);
    rdtSize = ftell(rdt);
    rewind(rdt);

    // Dynamic Allocation
    rdtBuffer = new unsigned char [rdtSize];
	fread(rdtBuffer, 1, rdtSize, rdt);
    fclose(rdt);


    // Copy file header from memory to struct
    rdtRE1Header = *(RDT_RE1_HEADER_T*)(&rdtBuffer[0]);

    // Copy rdtObjectList
    memcpy(&rdtObjectList, (rdtBuffer + sizeof(RDT_RE1_HEADER_T)), 76);
    
    // Absolute offset 0x94
    rdtRE1CameraPos = new RDT_RE1_CAMERA_POS_T[rdtRE1Header.num_cameras];



    // Load all cameras to struct
    for (int i = 0; i < rdtRE1Header.num_cameras; i++) {
    	rdtRE1CameraPos[i] = *(RDT_RE1_CAMERA_POS_T*)(&rdtBuffer[0x94] + (sizeof(RDT_RE1_CAMERA_POS_T) * i));
    }

    
    for (int i = 0; i < rdtRE1Header.num_cameras; i++) {
	    printf("---Debug---\n");
	    printf("mask offset:0x%x\n", rdtRE1CameraPos[i].mask_offset);
	    printf("tim_mask_offset; 0x%x\n", rdtRE1CameraPos[i].tim_mask_offset);
	    printf("Pos X: 0x%d\n", rdtRE1CameraPos[i].positionX);
		printf("Pos Y: 0x%d\n", rdtRE1CameraPos[i].positionY);
	    printf("Pos Z: 0x%d\n", rdtRE1CameraPos[i].positionZ);
	    printf("Tar X: 0x%d\n", rdtRE1CameraPos[i].targetX);
		printf("Tar Y: 0x%d\n", rdtRE1CameraPos[i].targetY);
	    printf("Tar Z: 0x%d\n", rdtRE1CameraPos[i].targetZ);
	}

	// Load all Switch Zone/Camera Zones

	// Offset 8 - Zonas das Cameras e Camera "Switch"
	RDT_RE1_CAMERA_SWITCH_T nodeSwitch;

	unsigned int cont = 0;
	while (true) {
		unsigned int checkInt = 0;

		checkInt = *(unsigned int*)(rdtBuffer+rdtObjectList[0]+(cont*(sizeof(RDT_RE1_CAMERA_SWITCH_T))));
		
		if (checkInt == 0xFFFFFFFF) 
			break;

		memcpy(&nodeSwitch, (rdtBuffer+rdtObjectList[0]+(cont*(sizeof(RDT_RE1_CAMERA_SWITCH_T)))), sizeof(RDT_RE1_CAMERA_SWITCH_T));
		/*
		printf("SwitchNum: %d\n", cont);
		printf("to:     0x%X\n", nodeSwitch.to);
		printf("from:   0x%X\n", nodeSwitch.from);
		printf("X1:       %d\n", nodeSwitch.x1);
		printf("Z1:       %d\n", nodeSwitch.z1);
		printf("X2:       %d\n", nodeSwitch.x2);
		printf("Z2:       %d\n", nodeSwitch.z2);
		printf("X3:       %d\n", nodeSwitch.x3);
		printf("Z3:       %d\n", nodeSwitch.z3);
		printf("X4:       %d\n", nodeSwitch.x4);
		printf("Z4:       %d\n", nodeSwitch.z4);
		*/
		rdtRE1CameraSwitch.push_back(nodeSwitch);
		cont++;
	}


	// Offset 1 - SCA Collision Boundaries for 3D Models
    rdtRE1ColisionHeader.cx = *(unsigned short*)(rdtBuffer+rdtObjectList[1]);
    rdtRE1ColisionHeader.cy = *(unsigned short*)(rdtBuffer+rdtObjectList[1]+2);

    rdtRE1ColisionHeader.counts = 0;
    for (unsigned int i = 0; i < 5; i++) {
    	rdtRE1ColisionHeader.counts += *(unsigned int*)(rdtBuffer+rdtObjectList[1]+4+(i * 4));
    }

    rdtRE1ColissionArray = new RDT_RE1_SCA_OBJ_T [rdtRE1ColisionHeader.counts - 1];

    for (unsigned short i = 0; i < (rdtRE1ColisionHeader.counts - 1); i++) {
    	memcpy(&rdtRE1ColissionArray[i], (rdtBuffer+rdtObjectList[1]+0x18+(i*sizeof(RDT_RE1_SCA_OBJ_T))), sizeof(RDT_RE1_SCA_OBJ_T));
    	printf("Colissão Nº %d\n", i);
    	printf("X1: %d\n", rdtRE1ColissionArray[i].x1);
    	printf("Z1: %d\n", rdtRE1ColissionArray[i].z1);
    	printf("X2: %d\n", rdtRE1ColissionArray[i].x2);
    	printf("Z2: %d\n", rdtRE1ColissionArray[i].z2);
    	printf("Floor: %d\n", (rdtRE1ColissionArray[i].floor / 256));
    	printf("Type: %d\n", rdtRE1ColissionArray[i].type );
    	printf("Id:   %d\n", rdtRE1ColissionArray[i].id);
    }


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