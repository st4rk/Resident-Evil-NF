#include "EMD1.h"

EMD1::EMD1() {

}


EMD1::~EMD1() {
	for (unsigned int i = 0; i < armature.size(); i++) {
		delete [] meshList[i];
	}

	delete [] meshList;

	meshList = NULL;
}


void EMD1::loadFile(std::string fileName) {
	FILE *emd1 = NULL;
	unsigned int emd1Size = 0;
	unsigned char *buffer;

	emd1 = fopen(fileName.c_str(), "rb");

	if (emd1 == NULL) {
		std::cout << "EMD File: " << emd1 << " not found" << std::endl;
		return;
	}

	fseek (emd1, 0, SEEK_END);
	emd1Size = ftell(emd1);
	rewind(emd1);

	buffer = new unsigned char [emd1Size];

	fread(buffer, 1, emd1Size, emd1);

	directory = *(EMD1_DIRECTORY*)(&buffer[emd1Size - 0x10]);	

	/*
	 * Section 0
	 */
	skeletonHeader = *(EMD1_SKEL_HEADER*)(&buffer[directory.section0]);
	
	for (unsigned short i = 0; i < skeletonHeader.count; i++) {
		EMD1_SKEL_RELPOS   node;
		EMD1_SKEL_ARMATURE node2;
		node  = *(EMD1_SKEL_RELPOS*)(&buffer[directory.section0 + sizeof(EMD1_SKEL_HEADER)
			                                + (i * sizeof(EMD1_SKEL_RELPOS))]);


		node2 = *(EMD1_SKEL_ARMATURE*)(&buffer[directory.section0 + skeletonHeader.relPosOffset
			                                + (i * sizeof(EMD1_SKEL_ARMATURE))]);

		relPos.push_back(node);
		armature.push_back(node2);
	}

	meshList = new unsigned char *[skeletonHeader.count];

	for (unsigned short i = 0; i < skeletonHeader.count; i++) {
		meshList[i] = new unsigned char [armature[i].meshCount];
	}

	for (unsigned int i = 0; i < armature.size(); i++) {
		for (unsigned int n = 0; n < armature[i].meshCount; n++) {
			memcpy(&meshList[i][n], (buffer + directory.section0 + skeletonHeader.relPosOffset
				                     + armature[i].offset + n), sizeof(unsigned char));  
		}
	}

	/*
	 * Section 1
	 */	

	unsigned short secSize = *(unsigned short*)(&buffer[directory.section1 + 2]);
	secSize = (secSize / 4);

	for (unsigned short i = 0; i < secSize; i++) {
		EMD1_ANIM_HEADER node;

		node = *(EMD1_ANIM_HEADER*)(&buffer[directory.section1 + (i * sizeof(EMD1_ANIM_HEADER))]);

		animStepHeader.push_back(node);
	}

    for (unsigned short i = 0; i < secSize; i++) {
        for(unsigned short j = 0; j < animStepHeader[i].count; j++) {
            memcpy(&emdSec1AnimSkelT[i][j], (buffer+directory.section1+animStepHeader[i].offset+(j*sizeof(unsigned int))), sizeof(unsigned int));
        }
    }

    /*
     * Section 2
     */

    modelHeader = *(EMD1_MODEL_HEADER*)(&buffer[directory.section2]);

    EMD1_MODEL_TRIANGLES node;
    for (unsigned int i = 0; i < modelHeader.objCount; i++) {
    	node = *(EMD1_MODEL_TRIANGLES*)(&buffer[directory.section2 + sizeof(EMD1_MODEL_HEADER) + 
    											(i * sizeof(EMD1_MODEL_TRIANGLES))]);

    	modelTriangles.push_back(node);
    }

    normalList      = new EMD1_NORMAL*   [modelHeader.objCount];
    vertexList      = new EMD1_VERTEX*   [modelHeader.objCount];
    triangleList    = new EMD1_TRIANGLE* [modelHeader.objCount];

    for (unsigned int i = 0; i < modelHeader.objCount; i++) {
    	normalList[i]     = new EMD1_NORMAL   [modelTriangles[i].normalCount];
    	vertexList[i]     = new EMD1_VERTEX   [modelTriangles[i].vertexCount];
    	triangleList[i]   = new EMD1_TRIANGLE [modelTriangles[i].triCount];
    }

    for (unsigned int i = 0; i < modelHeader.objCount; i++) {
    	
    	for (unsigned int j = 0; j < modelTriangles[i].normalCount; j++) {
    		normalList[i][j]    = *(EMD1_NORMAL*)(&buffer[directory.section2 + sizeof(EMD1_MODEL_HEADER) +
    										             modelTriangles[i].normalOffset]);
    	}

		for (unsigned int j = 0; j < modelTriangles[i].vertexCount; j++) {
	    	vertexList[i][j]   = *(EMD1_VERTEX*)(&buffer[directory.section2 + sizeof(EMD1_MODEL_HEADER) +
	    										         modelTriangles[i].vertexOffset]);
		}

		for (unsigned int j = 0; j < modelTriangles[i].triCount; j++) {
		    triangleList[i][j] = *(EMD1_TRIANGLE*)(&buffer[directory.section2 + sizeof(EMD1_MODEL_HEADER) +
		    										       modelTriangles[i].triOffset]);
		}
    }

    EMD1_SKEL_ANIM skelNode;
	for (unsigned short i = 0; i < secSize; i++) {
	    for(unsigned short j = 0; j < animStepHeader[i].count; j++) {
	    	skelNode = *(EMD1_SKEL_ANIM*)(&buffer[directory.section0 + skeletonHeader.length + 
	    		                                 ((emdSec1AnimSkelT[i][j] & 0x0FFF) * skeletonHeader.size)]);

	    	skelAnimList.push_back(skelNode);
		}
	}


	delete [] buffer;
	fclose(emd1);
}