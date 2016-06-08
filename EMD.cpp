#include "EMD.h"

EMD::EMD() {
    emdObjectBuffer = NULL;
    emdFileSection  = NULL;
    emdBufferData   = NULL;
    emdSize = 0;
}


EMD::~EMD() {
    /* Limpa Allocação da Matriz */

    for (unsigned int i = 0; i < emdTotalObj; i++) {
        free(emdVertex[i]);
        free(emdquadVertex[i]);
        free(emdquadNormal[i]);
        free(emdNormal[i]);
        free(emdTriangle[i]);
        free(emdQuad[i]);
        free(emdTritexture[i]);
        free(emdQuadTexture[i]);
    }

    for (unsigned int i = 0; i < emdSec2Header.count; i++) {
        free(emdSec2Mesh[i]);
    }

    free(emdBufferData);
    free(emdFileSection);
    free(emdObjectBuffer);

    free(emdSec2Armature);

    emdSec2AnimInfo  = NULL;
    emdTritexture    = NULL;
    emdQuadTexture   = NULL;
    emdSec2Armature  = NULL;
    emdSec2Mesh      = NULL;
    emdSec2RelPos    = NULL;
    emdquadNormal    = NULL;
    emdObjectBuffer  = NULL;
    emdFileSection   = NULL;
    emdBufferData    = NULL;
    emdNormal        = NULL;
    emdVertex        = NULL;
    emdQuad          = NULL;
    emdTriangle      = NULL;
    emdquadVertex    = NULL;
    emdSec2Armature  = NULL;

}


void EMD::emdDebugPrint() {/*
    std::cout << "-- EMD Debug File --" << std::endl;
    std::cout << "EMD Size: " << emdSize << std::endl;
    std::cout << "-- EMD Header Info --" << std::endl;
    printf("dirOffset: 0x%X\n", emdHeader.dirOffset);
    printf("dircount:  0x%X\n", emdHeader.dirCount);
    std::cout << "File/Directory Section" << std::endl;
    
    for(int i = 0; i < emdHeader.dirCount; i++) {
        printf("Section %d: 0x%X \n", i, emdFileSection[i]);
    }

    std::cout << "-- File/Section 7 --" << std::endl;
    printf("Lenght: 0x%X\n", emdSec7Header.lenght);
    printf("Unk32: 0x%X\n",  emdSec7Header.unk32);
    printf("objCount: 0x%X\n", emdSec7Header.objCount);
    printf("Valor: %x\n", emdSec2Header.relposOffset);

    std::cout << "ObjectBuffer: " << std::endl;
    for (int i = 0; i < emdTotalObj; i++) {
        std::cout << "emdObjectBuffer " << i << std::endl;
        printf("Triangles Vertex Offset: %x\n", emdObjectBuffer[i].triangles.vertexOffset);
        printf("Triangles Vertex Count: %x\n", emdObjectBuffer[i].triangles.vertexCount);
        printf("Triangles TriOffset: %x\n", emdObjectBuffer[i].triangles.triOffset);
        printf("Triangles TriCount:  %x\n", emdObjectBuffer[i].triangles.triCount);

        printf("Quads Vertex Offset: %x\n", emdObjectBuffer[i].quads.vertexOffset);
        printf("Quads Vertex Count: %x\n", emdObjectBuffer[i].quads.vertexCount);

        printf("Quads Quad Offset: %x\n", emdObjectBuffer[i].quads.quadOffset);
        printf("Quads Quad  Count: %x\n", emdObjectBuffer[i].quads.quadCount);
        std::cout << "---------------------" << std::endl;
    }
    std::cout << "-- EMD Sec 2--" << std::endl;

    for (unsigned int i = 0; i < emdSec2Header.count; i++) {
        std::cout << "X : " << emdSec2RelPos[i].x << std::endl;
        std::cout << "Y : " << emdSec2RelPos[i].y << std::endl;
        std::cout << "Z : " << emdSec2RelPos[i].z << std::endl;
    }


        std::cout << "TRIANGULO DO BOY" << std::endl;
        for (unsigned int y = 0; y < emdObjectBuffer[0].triangles.vertexCount; y++) {
            std::cout << "X: " << emdVertex[0][y].x << std::endl;
            std::cout << "Y: " << emdVertex[0][y].y << std::endl;
            std::cout << "Z: " << emdVertex[0][y].z << std::endl;
        }

        std::cout << "QUADRADO DO BOY" << std::endl;
        for (unsigned int y = 0; y < emdObjectBuffer[0].quads.vertexCount; y++) {
            std::cout << "X: " << emdquadVertex[0][y].x << std::endl;
            std::cout << "Y: " << emdquadVertex[0][y].y << std::endl;
            std::cout << "Z: " << emdquadVertex[0][y].z << std::endl;
        }


        std::cout << " ----" << std::endl;



    for (unsigned int x = 0; x < emdTotalObj; x++) {
        for (unsigned int y = 0; y < emdObjectBuffer[x].triangles.triCount; y++) {
            std::cout << "n0: " << emdTriangle[x][y].n0 << std::endl;
            std::cout << "v0: " << emdTriangle[x][y].v0 << std::endl;

            std::cout << "n1: " << emdTriangle[x][y].n1 << std::endl;
            std::cout << "v1: " << emdTriangle[x][y].v1 << std::endl;
            std::cout << "n2: " << emdTriangle[x][y].n2 << std::endl;
            std::cout << "v2: " << emdTriangle[x][y].v2 << std::endl;
        }
    } 

	*/

}

bool EMD::emdLoadFile(std::string dir) {
    FILE *emdFile = NULL;
    char timName[50];
    // Carregar TIM(Textura)
    memset(timName, 0x0, 50);
    dir.copy(timName, (dir.size() - 4), 0);
    sprintf(timName,"%s.TIM", timName);

    emdTimTexture.timLoad(timName);


    emdFile = fopen(dir.c_str(), "rb");


    if (emdFile == NULL) {
        std::cout << "Falha ao carregar emdFile !" << std::endl;
        return false;
    }

    fseek(emdFile, 0, SEEK_END);
    emdSize = ftell(emdFile);
    rewind(emdFile);

    emdBufferData = (unsigned char*) malloc(sizeof(unsigned char) * emdSize);

    fread(emdBufferData, emdSize, 1, emdFile);

    // Carrega os dados para as struct do EMD 
    memcpy(&emdHeader.dirOffset, emdBufferData, sizeof(unsigned int));
    memcpy(&emdHeader.dirCount,  emdBufferData+4, sizeof(unsigned int));

    emdFileSection = (unsigned int*) malloc(sizeof(unsigned int) * emdHeader.dirCount);

    for (unsigned int i = 0; i < (emdHeader.dirCount); i++) {
        memcpy(&emdFileSection[i], (emdBufferData+emdHeader.dirOffset)+(i*4), sizeof(unsigned int));
    }

    // Section 1 - Animação PT 1

    memcpy(&sizeSec1Header, (emdBufferData+emdFileSection[1] + 2), sizeof(unsigned short));

    sizeSec1Header = sizeSec1Header;

    emdSec1Header    = (EMD_SECTION_1_HEADER*) malloc (sizeof(EMD_SECTION_1_HEADER) * (sizeSec1Header / 4));
 
    unsigned short sizeSec1_f = (sizeSec1Header / 4);

    for (unsigned short i = 0; i < sizeSec1_f; i++) {
        memcpy(&emdSec1Header[i], (emdBufferData+emdFileSection[1]+(i*sizeof(EMD_SECTION_1_HEADER))), sizeof(EMD_SECTION_1_HEADER));
     }

    for (unsigned short i = 0; i < sizeSec1_f; i++) {
        for(unsigned short j = 0; j < emdSec1Header[i].dataCount; j++) {
            memcpy(&emdSec1AnimSkelT[i][j], (emdBufferData+emdFileSection[1]+emdSec1Header[i].dataOffset+(j*sizeof(unsigned int))), sizeof(unsigned int));
         }
    }

    // Section 3

    memcpy(&sizeSec3Header, (emdBufferData+emdFileSection[3] + 2), sizeof(unsigned short));

    sizeSec3Header = sizeSec3Header;

    emdSec3Header    = (EMD_SECTION_1_HEADER*) malloc (sizeof(EMD_SECTION_1_HEADER) * (sizeSec3Header / 4));
 
    unsigned short sizeSec3_f = (sizeSec3Header / 4);

    for (unsigned short i = 0; i < sizeSec3_f; i++) {
        memcpy(&emdSec3Header[i], (emdBufferData+emdFileSection[3]+(i*sizeof(EMD_SECTION_1_HEADER))), sizeof(EMD_SECTION_1_HEADER));
     }

    for (unsigned short i = 0; i < sizeSec3_f; i++) {
        for(unsigned short j = 0; j < emdSec3Header[i].dataCount; j++) {
            memcpy(&emdSec3AnimSkelT[i][j], (emdBufferData+emdFileSection[3]+emdSec3Header[i].dataOffset+(j*sizeof(unsigned int))), sizeof(unsigned int));
         }
    }

    // Section 2 - Skeleton

    // Carrega o HEADER do Section 2
    memcpy(&emdSec2Header, (emdBufferData+emdFileSection[2]), sizeof(EMD_SECTION_2_HEADER));

    emdSec2RelPos   = (EMD_SEC2_RELPOS_T*) malloc (sizeof(EMD_SEC2_RELPOS_T) * emdSec2Header.count);
    emdSec2Armature = (EMD_SEC2_ARMATURE_T*) malloc (sizeof(EMD_SEC2_ARMATURE_T) * emdSec2Header.count);

    for (unsigned int i = 0; i < emdSec2Header.count; i++) {
        memcpy(&emdSec2RelPos[i].x,(emdBufferData+emdFileSection[2]+8+(i*sizeof(EMD_SEC2_RELPOS_T))), sizeof(short));
        memcpy(&emdSec2RelPos[i].y,(emdBufferData+emdFileSection[2]+10+(i*sizeof(EMD_SEC2_RELPOS_T))), sizeof(short));
        memcpy(&emdSec2RelPos[i].z,(emdBufferData+emdFileSection[2]+12+(i*sizeof(EMD_SEC2_RELPOS_T))), sizeof(short));

    }

    int relOffset = (emdFileSection[2]+emdSec2Header.relposOffset);

    for (unsigned int i = 0; i < emdSec2Header.count; i++ ) {
        memcpy(&emdSec2Armature[i].meshCount, (emdBufferData+relOffset + (i*sizeof(EMD_SEC2_ARMATURE_T))), sizeof(unsigned short));
        memcpy(&emdSec2Armature[i].offSet, (emdBufferData+relOffset+2+(i*sizeof(EMD_SEC2_ARMATURE_T))), sizeof(unsigned short));        
    }

    emdSec2Mesh    = (unsigned char**) malloc (0x8 * emdSec2Header.count);
    
    for (unsigned int i = 0; i < emdSec2Header.count; i++) {
    	emdSec2Mesh[i] = (unsigned char*)    malloc (0x8 * emdSec2Armature[i].meshCount);
    }

	for (unsigned int i = 0; i < emdSec2Header.count; i++ ) {
		for (unsigned int a = 0; a < emdSec2Armature[i].meshCount; a++) { 
			memcpy(&emdSec2Mesh[i][a], (emdBufferData + relOffset + emdSec2Armature[i].offSet + a), sizeof(unsigned char));
		}
	}
    
    // Section 4
    memcpy(&emdSec4Header, (emdBufferData+emdFileSection[4]), sizeof(EMD_SECTION_2_HEADER));


    // Section 7 - Mesh 

    memcpy(&emdSec7Header.lenght, (emdBufferData+emdFileSection[7]), sizeof(unsigned int));
    memcpy(&emdSec7Header.unk32,  (emdBufferData+emdFileSection[7]+4), sizeof(unsigned int));
    memcpy(&emdSec7Header.objCount, (emdBufferData+emdFileSection[7]+8), sizeof(unsigned int));

    int totalObj = (emdSec7Header.objCount / 2);
    emdTotalObj = totalObj;

    emdObjectBuffer = (EMD_MODEL_OBJECT_T*) malloc (sizeof(EMD_MODEL_OBJECT_T) * totalObj);
    
    unsigned int s7ModelOffset = (emdFileSection[7]+12);


    // Precisamos colocar essa parte do Sec2 aqui, afinal precisamos das animações né ;)
    // O número de vetores é o número de meshs/objetos, então precisamos carregar o emdTotalObj
    // entretanto ele não foi carregado no section 2, por isso essa parte do código fica aqui !
    EMD_SEC2_DATA_T emdSec2Node;

    emdSec2Node.angles = (unsigned char*) malloc(emdSec2Header.size-12);
    emdSec2AnimInfo = (EMD_ANIM_INFO*) malloc (sizeof(EMD_ANIM_INFO) * (sizeSec1Header/4));
    totalAnim = (sizeSec1Header/4);

    unsigned int aux = 0;
    for (unsigned short i = 0; i < totalAnim; i++) {
        for(unsigned short j = 0; j < emdSec1Header[i].dataCount; j++) {
            aux = 0;
            memcpy(&emdSec2Node.xOffset, (emdBufferData+emdFileSection[2]+emdSec2Header.lenght+((emdSec1AnimSkelT[i][j]& 0x0FFF) * emdSec2Header.size)), sizeof(short));
            memcpy(&emdSec2Node.yOffset, (emdBufferData+emdFileSection[2]+emdSec2Header.lenght+2+((emdSec1AnimSkelT[i][j]& 0x0FFF) * emdSec2Header.size)), sizeof(short));
            memcpy(&emdSec2Node.zOffset, (emdBufferData+emdFileSection[2]+emdSec2Header.lenght+4+((emdSec1AnimSkelT[i][j]& 0x0FFF) * emdSec2Header.size)), sizeof(short));
            memcpy(&emdSec2Node.xSpeed,  (emdBufferData+emdFileSection[2]+emdSec2Header.lenght+6+((emdSec1AnimSkelT[i][j]& 0x0FFF) * emdSec2Header.size)), sizeof(short));
            memcpy(&emdSec2Node.ySpeed,  (emdBufferData+emdFileSection[2]+emdSec2Header.lenght+8+((emdSec1AnimSkelT[i][j]& 0x0FFF) * emdSec2Header.size)), sizeof(short));
            memcpy(&emdSec2Node.zSpeed,  (emdBufferData+emdFileSection[2]+emdSec2Header.lenght+10+((emdSec1AnimSkelT[i][j]& 0x0FFF) * emdSec2Header.size)), sizeof(short));

            for (unsigned short a = 0; a < (emdSec2Header.size-12); a++) {
                memcpy(&emdSec2Node.angles[a], (emdBufferData+emdFileSection[2]+emdSec2Header.lenght+12+a+((emdSec1AnimSkelT[i][j] & 0x0FFF) * emdSec2Header.size)), sizeof(unsigned char));
            }
            
            /* Temos 12 bits por coordenada temos que fazer agora a transformação do
            angles para as nossas coordenadas */

            for (unsigned short i = 0; i < emdTotalObj; i++) {
                if ((i % 2) == 0) {
                    emdSec2Node.vector[i].x = (emdSec2Node.angles[aux] | ((emdSec2Node.angles[aux+1] & 0b00001111) << 8));
                    emdSec2Node.vector[i].x = ((emdSec2Node.vector[i].x / 4096) * 360);
                    emdSec2Node.vector[i].y = ((emdSec2Node.angles[aux+1] & 0b11110000) >> 4) | (emdSec2Node.angles[aux+2] << 4);
                    emdSec2Node.vector[i].y = ((emdSec2Node.vector[i].y / 4096) * 360);                 
                    emdSec2Node.vector[i].z = (emdSec2Node.angles[aux+3] | ((emdSec2Node.angles[aux+4] & 0b00001111) << 8));
                    emdSec2Node.vector[i].z = ((emdSec2Node.vector[i].z / 4096) * 360);
                    aux += 4;
                } else {
                    emdSec2Node.vector[i].x = ((emdSec2Node.angles[aux] & 0b11110000) >> 4) | (emdSec2Node.angles[aux+1] << 4);
                    emdSec2Node.vector[i].x = ((emdSec2Node.vector[i].x / 4096) * 360);      
                    emdSec2Node.vector[i].y = (emdSec2Node.angles[aux+2] | ((emdSec2Node.angles[aux+3] & 0b00001111) << 8));
                    emdSec2Node.vector[i].y = ((emdSec2Node.vector[i].y / 4096) * 360);
                    emdSec2Node.vector[i].z = ((emdSec2Node.angles[aux+3] & 0b11110000) >> 4) | (emdSec2Node.angles[aux+4] << 4);
                    emdSec2Node.vector[i].z = ((emdSec2Node.vector[i].z / 4096) * 360);
                    aux += 5;                    
                }

           }

           emdSec2Data.push_back(emdSec2Node);
        }

        emdSec2AnimInfo[i].animCount = emdSec1Header[i].dataCount;

    }
    
    int animStart = 0;
    for (unsigned int i = 0; i < totalAnim; i++) {
        emdSec2AnimInfo[i].animStart = animStart;
        animStart += (emdSec2AnimInfo[i].animCount);
    }

    free(emdSec2Node.angles);
    emdSec2Node.angles = NULL;


    // Precisamos colocar essa parte do Sec4 aqui, afinal precisamos das animações né ;)
    // O número de vetores é o número de meshs/objetos, então precisamos carregar o emdTotalObj
    // entretanto ele não foi carregado no section 4, por isso essa parte do código fica aqui !
    EMD_SEC2_DATA_T emdSec4Node;

    emdSec4Node.angles = (unsigned char*) malloc(emdSec4Header.size-12);
    emdSec4AnimInfo = (EMD_ANIM_INFO*) malloc (sizeof(EMD_ANIM_INFO) * (sizeSec3Header/4));
    totalAnim = (sizeSec3Header/4);

   
    for (unsigned short i = 0; i < totalAnim; i++) {
        for(unsigned short j = 0; j < emdSec3Header[i].dataCount; j++) {
            aux = 0;
            memcpy(&emdSec4Node.xOffset, (emdBufferData+emdFileSection[4]+emdSec4Header.lenght+((emdSec3AnimSkelT[i][j]& 0x0FFF) * emdSec4Header.size)), sizeof(short));
            memcpy(&emdSec4Node.yOffset, (emdBufferData+emdFileSection[4]+emdSec4Header.lenght+2+((emdSec3AnimSkelT[i][j]& 0x0FFF) * emdSec4Header.size)), sizeof(short));
            memcpy(&emdSec4Node.zOffset, (emdBufferData+emdFileSection[4]+emdSec4Header.lenght+4+((emdSec3AnimSkelT[i][j]& 0x0FFF) * emdSec4Header.size)), sizeof(short));
            memcpy(&emdSec4Node.xSpeed,  (emdBufferData+emdFileSection[4]+emdSec4Header.lenght+6+((emdSec3AnimSkelT[i][j]& 0x0FFF) * emdSec4Header.size)), sizeof(short));
            memcpy(&emdSec4Node.ySpeed,  (emdBufferData+emdFileSection[4]+emdSec4Header.lenght+8+((emdSec3AnimSkelT[i][j]& 0x0FFF) * emdSec4Header.size)), sizeof(short));
            memcpy(&emdSec4Node.zSpeed,  (emdBufferData+emdFileSection[4]+emdSec4Header.lenght+10+((emdSec3AnimSkelT[i][j]& 0x0FFF) * emdSec4Header.size)), sizeof(short));

            for (unsigned short a = 0; a < (emdSec4Header.size-12); a++) {
                memcpy(&emdSec4Node.angles[a], (emdBufferData+emdFileSection[4]+emdSec4Header.lenght+12+a+((emdSec3AnimSkelT[i][j] & 0x0FFF) * emdSec4Header.size)), sizeof(unsigned char));
            }
            
            /* Temos 12 bits por coordenada temos que fazer agora a transformação do
            angles para as nossas coordenadas */

            for (unsigned short i = 0; i < emdTotalObj; i++) {
                if ((i % 2) == 0) {
                    emdSec4Node.vector[i].x = (emdSec4Node.angles[aux] | ((emdSec4Node.angles[aux+1] & 0b00001111) << 8));
                    emdSec4Node.vector[i].x = ((emdSec4Node.vector[i].x / 4096) * 360);
                    emdSec4Node.vector[i].y = ((emdSec4Node.angles[aux+1] & 0b11110000) >> 4) | (emdSec4Node.angles[aux+2] << 4);
                    emdSec4Node.vector[i].y = ((emdSec4Node.vector[i].y / 4096) * 360);                 
                    emdSec4Node.vector[i].z = (emdSec4Node.angles[aux+3] | ((emdSec4Node.angles[aux+4] & 0b00001111) << 8));
                    emdSec4Node.vector[i].z = ((emdSec4Node.vector[i].z / 4096) * 360);
                    aux += 4;
                } else {
                    emdSec4Node.vector[i].x = ((emdSec4Node.angles[aux] & 0b11110000) >> 4) | (emdSec4Node.angles[aux+1] << 4);
                    emdSec4Node.vector[i].x = ((emdSec4Node.vector[i].x / 4096) * 360);      
                    emdSec4Node.vector[i].y = (emdSec4Node.angles[aux+2] | ((emdSec4Node.angles[aux+3] & 0b00001111) << 8));
                    emdSec4Node.vector[i].y = ((emdSec4Node.vector[i].y / 4096) * 360);
                    emdSec4Node.vector[i].z = ((emdSec4Node.angles[aux+3] & 0b11110000) >> 4) | (emdSec4Node.angles[aux+4] << 4);
                    emdSec4Node.vector[i].z = ((emdSec4Node.vector[i].z / 4096) * 360);
                    aux += 5;                    
                }

           }

           emdSec4Data.push_back(emdSec4Node);
        }

        emdSec4AnimInfo[i].animCount = emdSec3Header[i].dataCount;

    }
    
    animStart = 0;
    for (unsigned int i = 0; i < totalAnim; i++) {
        emdSec4AnimInfo[i].animStart = animStart;
        animStart += (emdSec4AnimInfo[i].animCount);
    }

    free(emdSec4Node.angles);
    emdSec4Node.angles = NULL;   


    // Carrega Quadrados e Triângulos
    unsigned int index = 0; 
    int offset = s7ModelOffset;
    for (index = 0; index < emdTotalObj; index++) {
        memcpy(&emdObjectBuffer[index].triangles, (emdBufferData + offset), sizeof(EMD_MODEL_TRIANGLES_T));
        offset += 28;
        memcpy(&emdObjectBuffer[index].quads, (emdBufferData + offset), sizeof(EMD_MODEL_QUADS_T));
        offset += 28;
    }

    // Alloca e Carrega Vertex e Normal

    emdVertex       = (EMD_VERTEX_T**) malloc (sizeof(EMD_VERTEX_T) * emdTotalObj);
    emdquadVertex   = (EMD_VERTEX_T**) malloc (sizeof(EMD_VERTEX_T) * emdTotalObj);
    emdNormal       = (EMD_NORMAL_T**) malloc (sizeof(EMD_NORMAL_T) * emdTotalObj);
    emdTriangle     = (EMD_TRIANGLE_T**) malloc (sizeof(EMD_TRIANGLE_T) * emdTotalObj);
    emdQuad         = (EMD_QUAD_T**) malloc (sizeof(EMD_QUAD_T) * emdTotalObj);
    emdquadNormal   = (EMD_NORMAL_T**) malloc (sizeof(EMD_NORMAL_T) * emdTotalObj);
    emdQuadTexture  = (EMD_QUADTEXTURE_T**) malloc (sizeof(EMD_QUADTEXTURE_T) * emdTotalObj);
    emdTritexture   = (EMD_TRITEXTURE_T**)  malloc (sizeof(EMD_TRITEXTURE_T)  * emdTotalObj);


    for (unsigned int i = 0; i < emdTotalObj; i++) {
        emdVertex[i]       = (EMD_VERTEX_T*)      malloc (sizeof(EMD_VERTEX_T) * emdObjectBuffer[i].triangles.vertexCount);
        emdquadVertex[i]   = (EMD_VERTEX_T*)      malloc (sizeof(EMD_VERTEX_T) * emdObjectBuffer[i].quads.vertexCount);
        emdNormal[i]       = (EMD_NORMAL_T*)      malloc (sizeof(EMD_NORMAL_T) * emdObjectBuffer[i].triangles.normalCount);
        emdquadNormal[i]   = (EMD_NORMAL_T*)      malloc (sizeof(EMD_NORMAL_T) * emdObjectBuffer[i].quads.normalCount);
        emdTriangle[i]     = (EMD_TRIANGLE_T*)    malloc (sizeof(EMD_TRIANGLE_T) * emdObjectBuffer[i].triangles.triCount);
        emdQuad[i]         = (EMD_QUAD_T*)        malloc (sizeof(EMD_QUAD_T) * emdObjectBuffer[i].quads.quadCount);
        emdQuadTexture[i]  = (EMD_QUADTEXTURE_T*) malloc (sizeof(EMD_QUADTEXTURE_T) * emdObjectBuffer[i].quads.quadCount);
        emdTritexture[i]   = (EMD_TRITEXTURE_T*)  malloc (sizeof(EMD_TRITEXTURE_T)  * emdObjectBuffer[i].triangles.triCount);
    }
    
    for (unsigned int x = 0; x < emdTotalObj; x++) {
        for (unsigned int y = 0; y < emdObjectBuffer[x].triangles.vertexCount; y++) {

            memcpy(&emdVertex[x][y].x,(emdBufferData + (s7ModelOffset + emdObjectBuffer[x].triangles.vertexOffset+(y*8))), sizeof(signed short));
            memcpy(&emdVertex[x][y].y,(emdBufferData + (s7ModelOffset + emdObjectBuffer[x].triangles.vertexOffset+2+(y*8))), sizeof(signed short));
            memcpy(&emdVertex[x][y].z,(emdBufferData + (s7ModelOffset + emdObjectBuffer[x].triangles.vertexOffset+4+(y*8))), sizeof(signed short));
            memcpy(&emdVertex[x][y].zero,(emdBufferData + (s7ModelOffset + emdObjectBuffer[x].triangles.vertexOffset+6+(y*8))), sizeof(signed short));

        }

        for (unsigned int y = 0; y < emdObjectBuffer[x].quads.vertexCount; y++) {

            memcpy(&emdquadVertex[x][y].x,(emdBufferData + (s7ModelOffset + emdObjectBuffer[x].quads.vertexOffset+(y*8))), sizeof(signed short));
            memcpy(&emdquadVertex[x][y].y,(emdBufferData + (s7ModelOffset + emdObjectBuffer[x].quads.vertexOffset+2+(y*8))), sizeof(signed short));
            memcpy(&emdquadVertex[x][y].z,(emdBufferData + (s7ModelOffset + emdObjectBuffer[x].quads.vertexOffset+4+(y*8))), sizeof(signed short));
            memcpy(&emdquadVertex[x][y].zero,(emdBufferData + (s7ModelOffset + emdObjectBuffer[x].quads.vertexOffset+6+(y*8))), sizeof(signed short));

        }
        
        for (unsigned int y = 0; y < emdObjectBuffer[x].triangles.normalCount; y++) {
            memcpy(&emdNormal[x][y].x,(emdBufferData + (s7ModelOffset + emdObjectBuffer[x].triangles.normalOffset+(y*8))), sizeof(signed short));
            memcpy(&emdNormal[x][y].y,(emdBufferData + (s7ModelOffset + emdObjectBuffer[x].triangles.normalOffset+2+(y*8))), sizeof(signed short));
            memcpy(&emdNormal[x][y].z,(emdBufferData + (s7ModelOffset + emdObjectBuffer[x].triangles.normalOffset+4+(y*8))), sizeof(signed short));
            memcpy(&emdNormal[x][y].zero,(emdBufferData + (s7ModelOffset + emdObjectBuffer[x].triangles.normalOffset+6+(y*8))), sizeof(signed short));
        }
 
        for (unsigned int y = 0; y < emdObjectBuffer[x].quads.normalCount; y++) {
            memcpy(&emdquadNormal[x][y].x,(emdBufferData + (s7ModelOffset + emdObjectBuffer[x].quads.normalOffset+(y*8))), sizeof(signed short));
            memcpy(&emdquadNormal[x][y].y,(emdBufferData + (s7ModelOffset + emdObjectBuffer[x].quads.normalOffset+2+(y*8))), sizeof(signed short));
            memcpy(&emdquadNormal[x][y].z,(emdBufferData + (s7ModelOffset + emdObjectBuffer[x].quads.normalOffset+4+(y*8))), sizeof(signed short));
            memcpy(&emdquadNormal[x][y].zero,(emdBufferData + (s7ModelOffset + emdObjectBuffer[x].quads.normalOffset+6+(y*8))), sizeof(signed short));
        }

        for (unsigned int y = 0; y < emdObjectBuffer[x].triangles.triCount; y++) {
            memcpy(&emdTriangle[x][y],(emdBufferData + (s7ModelOffset + emdObjectBuffer[x].triangles.triOffset+(y*sizeof(EMD_TRIANGLE_T)))), sizeof(EMD_TRIANGLE_T));
            memcpy(&emdTritexture[x][y],(emdBufferData + (s7ModelOffset + emdObjectBuffer[x].triangles.triTextureOffset+(y*sizeof(EMD_TRITEXTURE_T)))), sizeof(EMD_TRITEXTURE_T));

        }

        for (unsigned int y = 0; y < emdObjectBuffer[x].quads.quadCount; y++) {
            memcpy(&emdQuad[x][y],(emdBufferData + (s7ModelOffset + emdObjectBuffer[x].quads.quadOffset+(y*sizeof(EMD_QUAD_T)))), sizeof(EMD_QUAD_T));
            memcpy(&emdQuadTexture[x][y], (emdBufferData + (s7ModelOffset + emdObjectBuffer[x].quads.quadTextureOffset+(y*sizeof(EMD_QUADTEXTURE_T)))), sizeof(EMD_QUADTEXTURE_T));
        }
    } 
    
    fclose(emdFile);
    return true;
}
