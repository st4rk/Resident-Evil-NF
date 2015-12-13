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

	if (RDT_RE1_SCD_DATA != NULL) {
		delete [] RDT_RE1_SCD_DATA;
		RDT_RE1_SCD_DATA = NULL;
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
	    /*printf("---Debug---\n");
	    printf("mask offset:0x%x\n", rdtRE1CameraPos[i].mask_offset);
	    printf("tim_mask_offset; 0x%x\n", rdtRE1CameraPos[i].tim_mask_offset);
	    printf("Pos X: 0x%d\n", rdtRE1CameraPos[i].positionX);
		printf("Pos Y: 0x%d\n", rdtRE1CameraPos[i].positionY);
	    printf("Pos Z: 0x%d\n", rdtRE1CameraPos[i].positionZ);
	    printf("Tar X: 0x%d\n", rdtRE1CameraPos[i].targetX);
		printf("Tar Y: 0x%d\n", rdtRE1CameraPos[i].targetY);
	    printf("Tar Z: 0x%d\n", rdtRE1CameraPos[i].targetZ);*/
	}

	// Load all Switch Zone/Camera Zones

	// Offset 8 - Zonas das Cameras e Camera "Switch"
	RDT_RE1_CAMERA_SWITCH_T nodeSwitch;
	rdtRE1CameraSwitch.clear();
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
    	/*printf("Colissão Nº %d\n", i);
    	printf("X1: %d\n", rdtRE1ColissionArray[i].x1);
    	printf("Z1: %d\n", rdtRE1ColissionArray[i].z1);
    	printf("X2: %d\n", rdtRE1ColissionArray[i].x2);
    	printf("Z2: %d\n", rdtRE1ColissionArray[i].z2);
    	printf("Floor: %d\n", (rdtRE1ColissionArray[i].floor / 256));
    	printf("Type: %d\n", rdtRE1ColissionArray[i].type );
    	printf("Id:   %d\n", rdtRE1ColissionArray[i].id);*/
    }


    // Offset 6 - Scenario Data

    /* Get SCD Section Size */
    scdSize = *(unsigned short*)(rdtBuffer+rdtObjectList[6]);

    printf("ScdSize: 0x%X\n", scdSize);
    /* Allocate the necessary space */
    RDT_RE1_SCD_DATA = new unsigned char [scdSize];

    /* copy the scd data to my buffer */
    memcpy(RDT_RE1_SCD_DATA, (rdtBuffer+rdtObjectList[6]+2), scdSize);

    /* Clear total of doors */
	door_set_len = 0;

	/* Read scripts */
    rdtRE1_sca();
}


void RDT::rdtRE1_sca() {
	unsigned short i = 0;
	bool macgyver = false;
	while (i < scdSize) {
		switch (RDT_RE1_SCD_DATA[i]) {	
			case 0x00:
				// NOP
				i+= 2;
			break;

			case 0x01:
				// IF
				i+= 2;
			break;

			case 0x02:
				// ELSE
				i+= 2;
			break;

			case 0x03:
				// end if
				i+= 2;
			break;

			case 0x4:
				// bit test
				i+= 4;
			break;

			case 0x05:
				// bit op
				i+= 4;
			break;

			case 0x06:
				// obj06_test
				i+=4;
			break;

			case 0x07:
				// obj07_test
			    i+= 6;
			break;

			case 0x08:
				// stage room cam set
				i+= 4;
			break;

			case 0x09:
				// cut_Set
				i+= 2;
			break;

			case 0x0A:
				// cut set 2
				i+= 2;
			break;

			case 0x0B:
				// no idea yet
				i+= 4;
			break;

			case 0x0C:

				/* Here is stored the door informations */
				door_set_re1[door_set_len] = *(script_door_set_re1*)(&RDT_RE1_SCD_DATA[i]);

				/* DEBUG PURPOSE */
				/*
				printf("id: 0x%X\n", door_set_re1[door_set_len].id);
				printf("x:  %x\n", door_set_re1[door_set_len].x);
				printf("y:  %x\n", door_set_re1[door_set_len].y);
				printf("w:  %x\n", door_set_re1[door_set_len].w);
				printf("h:  %x\n", door_set_re1[door_set_len].h);
				printf("next x:    %d\n", door_set_re1[door_set_len].next_x);
				printf("next y:    %d\n", door_set_re1[door_set_len].next_y);
				printf("next z:    %d\n", door_set_re1[door_set_len].next_z);
				printf("next dir:  %d\n", door_set_re1[door_set_len].next_dir);
				printf("room: %d\n", ROOM(door_set_re1[door_set_len].next_stage_and_room));
				printf("stage: %d\n", STAGE(door_set_re1[door_set_len].next_stage_and_room));
				*/
				door_set_len++;
				i+= 26;
			break;

			case 0x0D:
				// item set
				i+= 18;
			break;

			case 0x0E:
				// nop again ?
				i+= 2;
			break;

			case 0x0F:
				// ?
				i+= 8;
			break;

			case 0x10:
				// obj10_test
				i+= 2;
			break;

			case 0x11:
				// obj11_test
				i+= 2;
			break;

			case 0x12:
				// item 0x12
				i+=10;
			break;

			case 0x13:
				// item 0x13
				i+= 4;
			break;

			case 0x14:
				// ?
				i+=4;
			break;

			case 0x15:

				i+= 2;
			break;

			case 0x16:
				i+= 2;
			break;

			case 0x17:
				i+= 10;
			break;

			case 0x18:
				i+= 26;
			break;

			case 0x19:
				i+=4;
			break;

			case 0x1a:
				i+= 2;
			break;

			case 0x1b:
				i+= 22;
			break;

			case 0x1c:
				i+= 6;
			break;

			case 0x1d:
				i+= 2;
			break;

			case 0x1e:
				i+= 4;
			break;

			case 0x1f:
				i+= 28;
			break;

			case 0x20:
				i+= 14;
			break;

			case 0x21:
				i+= 14;
			break;

			case 0x22:
				i+= 4;
			break;

			case 0x23:
				i+= 2;
			break;

			case 0x24:
				i+= 4;
			break;

			case 0x25:
				i+= 4;
			break;

			case 0x27:
				i+= 2;
			break;

			case 0x29:
				i+= 2;
			break;

			case 0x2a:
				i+= 12;
			break;

			case 0x2b:
				i+= 4;
			break;

			case 0x2c:
				i+= 2;
			break;

			case 0x2d:
				i+= 4;
			break;

			case 0x2f:
				i+= 4;
			break;

			case 0x30:
				i+= 12;
			break;

			case 0x31:
				i+= 4;
			break;

			case 0x32:
				i+= 4;
			break;


			case 0x34:
				i+= 8;
			break;

			case 0x35:
				i+= 4;
			break;

			case 0x36:
				i+= 4;
			break;

			case 0x37:
				i+= 4;
			break;

			case 0x38:
				i+= 4;
			break;

			case 0x39:
				i+= 2;
			break;

			case 0x3a:
				i+= 4;
			break;

			case 0x3b:
				i+= 6;
			break;

			case 0x3c:
				i+= 6;
			break;

			case 0x3d:
				i+= 12;
			break;

			case 0x3e:
				i+= 2;
			break;

			case 0x3f:
				i+= 6;
			break;


			default:
				printf("Script Num: 0x%X\n", RDT_RE1_SCD_DATA[i]);
				macgyver = true;
			break;
		}

		if (macgyver)
			break;
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