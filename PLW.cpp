#include "PLW.h"


PLW::PLW() {

}


PLW::~PLW() {

}



bool PLW::readFile(std::string fileName) {
	FILE *arq = NULL;
	unsigned int   fileSize = 0;
	unsigned char *fileBuffer;

	arq = fopen(fileName.c_str(), "rb");


	if (arq == NULL) {
		std::cout << "File " << fileName << " not found" << std::endl;
		return false;
	}


	fseek(arq, 0, SEEK_END);
    fileSize = ftell(arq);
    rewind(arq);


    fileBuffer = new unsigned char [fileSize];

    fread(fileBuffer, fileSize, 1, arq);

    directory = *(PLW_HEADER_DIRECTORY*)(&fileBuffer[0]);

    PLW_SECTION = new unsigned int [directory.dirCount];

    for (unsigned int i = 0; i < directory.dirCount; i++) {
    	PLW_SECTION[i] = *(unsigned int*)(&fileBuffer[directory.dirOffset + (i*4)]);
    }

    /*
     * Section 0 - Animation Steps
     */
    unsigned short mgSize = *(unsigned short*)(&fileBuffer[PLW_SECTION[0] + 2]);
	
	PLW_ANIM_HEADER node;
	for (unsigned short i = 0; i < (mgSize / 4); i++) {
		node = *(PLW_ANIM_HEADER*)(&fileBuffer[PLW_SECTION[0] + (i*sizeof(PLW_ANIM_HEADER))]);
		sec1Header.push_back(node);
	}

	for (unsigned short i = 0; i < (mgSize / 4); i++) {
		for (unsigned short j = 0; j < sec1Header[j].count; j++) {
			PLWAnimSkelT[i][j] = *(unsigned int*)(&fileBuffer[PLW_SECTION[0] + sec1Header[j].offset + (j * sizeof(unsigned int))]);
		}
	}

    /* 
     * Section 1 - Skeleton
     */

    sec2Header = *(PLW_SEC2_HEADER*)(&fileBuffer[PLW_SECTION[1]]);

    PLW_SEC2_ARMATURE node_1;
    PLW_SEC2_RELPOS   node_2;
    for (unsigned int i = 0; i < sec2Header.count; i++) {
    	node_1 = *(PLW_SEC2_ARMATURE*)(&fileBuffer[PLW_SECTION[1]+sec2Header.relPosOffset+(i*sizeof(PLW_SEC2_ARMATURE))]);

    	node_2 = *(PLW_SEC2_RELPOS*)(&fileBuffer[PLW_SECTION[1]+8+(i*sizeof(PLW_SEC2_RELPOS))]);

    	sec2Armature.push_back(node_1);
    	sec2RelPos.push_back(node_2);
    }


    sec2Mesh = new unsigned char* [sec2Header.count];

    for (unsigned int i = 0; i < sec2Header.count; i++) {
    	sec2Mesh[i] = new unsigned char [sec2Armature[i].meshCount];
    }

    for (unsigned int i = 0; i < sec2Header.count; i++) {
    	for (unsigned int n = 0; n < sec2Armature[i].meshCount; n++) {
    		sec2Mesh[i][n] = fileBuffer[sec2Header.relPosOffset + sec2Armature[i].offset + n];
    	}
    }

    /*
     * Section 3 - Mesh
     */

    sec3Header = *(PLW_SECTION_3_HEADER*)(&fileBuffer[PLW_SECTION[2]]);

    PLW_MODEL_OBJECT_T node_4;
    for (unsigned int i = 0; i < (sec3Header.objCount / 2); i++) {
    	node_4 = *(PLW_MODEL_OBJECT_T*)(&fileBuffer[PLW_SECTION[2] + sizeof(PLW_SECTION_3_HEADER) + (i* sizeof(PLW_MODEL_OBJECT_T))]);
    	
    	sec3ModelBuffer.push_back(node_4);
    }

    plwVertex         = new PLW_VERTEX_T      * [(sec3Header.objCount / 2)];
    plwquadVertex     = new PLW_VERTEX_T      * [(sec3Header.objCount / 2)];
    plwNormal         = new PLW_NORMAL_T      * [(sec3Header.objCount / 2)];
    plwTriangle       = new PLW_TRIANGLE_T    * [(sec3Header.objCount / 2)];
    plwQuad           = new PLW_QUAD_T        * [(sec3Header.objCount / 2)];
    plwquadNormal     = new PLW_NORMAL_T      * [(sec3Header.objCount / 2)];
    plwQuadTexture    = new PLW_QUADTEXTURE_T * [(sec3Header.objCount / 2)];
    plwTritexture     = new PLW_TRITEXTURE_T  * [(sec3Header.objCount / 2)];


    for (unsigned int i = 0; i < (sec3Header.objCount / 2); i++) {
    	plwVertex[i]       = new PLW_VERTEX_T       [sec3ModelBuffer[i].triangles.vertexCount];
    	plwquadVertex[i]   = new PLW_VERTEX_T       [sec3ModelBuffer[i].quads.vertexCount];
    	plwNormal[i]       = new PLW_NORMAL_T       [sec3ModelBuffer[i].triangles.normalCount];
    	plwquadNormal[i]   = new PLW_NORMAL_T       [sec3ModelBuffer[i].quads.normalCount];
    	plwTriangle[i]     = new PLW_TRIANGLE_T     [sec3ModelBuffer[i].triangles.triCount];
    	plwQuad[i]         = new PLW_QUAD_T         [sec3ModelBuffer[i].quads.quadCount];
    	plwQuadTexture[i]  = new PLW_QUADTEXTURE_T  [sec3ModelBuffer[i].quads.quadCount];
    	plwTritexture[i]   = new PLW_TRITEXTURE_T   [sec3ModelBuffer[i].triangles.triCount];
    }

    for (unsigned int x = 0; x < (sec3Header.objCount / 2); x++) {
    	for (unsigned int y = 0; y < sec3ModelBuffer[x].triangles.vertexCount; y++) {
    		plwVertex[x][y] = *(PLW_VERTEX_T*)(&fileBuffer[PLW_SECTION[2] + sizeof(PLW_SECTION_3_HEADER) + 
    													   sec3ModelBuffer[x].triangles.vertexOffset + (y*(sizeof(PLW_VERTEX_T)))]);		
    	}

    	for (unsigned int y = 0; y < sec3ModelBuffer[x].quads.vertexCount; y++) {
    		plwquadVertex[x][y] = *(PLW_VERTEX_T*)(&fileBuffer[PLW_SECTION[2] + sizeof(PLW_SECTION_3_HEADER) + 
    													   sec3ModelBuffer[x].quads.vertexOffset + (y*(sizeof(PLW_VERTEX_T)))]);		
    	}

    	for (unsigned int y = 0; y < sec3ModelBuffer[x].triangles.normalCount; y++) {
    		plwNormal[x][y] = *(PLW_NORMAL_T*)(&fileBuffer[PLW_SECTION[2] + sizeof(PLW_SECTION_3_HEADER) + 
    													   sec3ModelBuffer[x].triangles.normalOffset + (y*(sizeof(PLW_NORMAL_T)))]);		
    	}

    	for (unsigned int y = 0; y < sec3ModelBuffer[x].quads.normalCount; y++) {
    		plwquadNormal[x][y] = *(PLW_NORMAL_T*)(&fileBuffer[PLW_SECTION[2] + sizeof(PLW_SECTION_3_HEADER) + 
    													   sec3ModelBuffer[x].quads.normalOffset + (y*(sizeof(PLW_NORMAL_T)))]);		
    	}

    	for (unsigned int y = 0; y < sec3ModelBuffer[x].triangles.triCount; y++) {
    		plwTriangle[x][y]   = *(PLW_TRIANGLE_T*)(&fileBuffer[PLW_SECTION[2] + sizeof(PLW_SECTION_3_HEADER) + 
    													   sec3ModelBuffer[x].triangles.triOffset + (y*(sizeof(PLW_TRIANGLE_T)))]);

    		plwTritexture[x][y] = *(PLW_TRITEXTURE_T*)(&fileBuffer[PLW_SECTION[2] + sizeof(PLW_SECTION_3_HEADER) + 
    													   sec3ModelBuffer[x].triangles.triTextureOffset + (y*(sizeof(PLW_TRITEXTURE_T)))]);
    	}


    	for (unsigned int y = 0; y < sec3ModelBuffer[x].quads.quadCount; y++) {
    		plwQuad[x][y]   = *(PLW_QUAD_T*)(&fileBuffer[PLW_SECTION[2] + sizeof(PLW_SECTION_3_HEADER) + 
    													   sec3ModelBuffer[x].quads.quadOffset + (y*(sizeof(PLW_QUAD_T)))]);
    		plwQuadTexture[x][y] = *(PLW_QUADTEXTURE_T*)(&fileBuffer[PLW_SECTION[2] + sizeof(PLW_SECTION_3_HEADER) + 
    													   sec3ModelBuffer[x].quads.quadTextureOffset + (y*(sizeof(PLW_QUADTEXTURE_T)))]);
    	}


    }

    emdTimTexture.readFromPtr((fileBuffer + PLW_SECTION[3]));


    delete [] fileBuffer;
    fclose(arq);

    return true;
}