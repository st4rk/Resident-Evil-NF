#include "TIM.h"


TIM::TIM() {
	// Inicializa variaveis
	timSize = 0;
	timData = NULL;
	rawTexture = NULL;
}

TIM::~TIM() {
	// Limpa memória

	for (unsigned int i = 0; i < timTexture.numbCluts; i++) {
		free(colorTable[i]);
	}
	
	if (rawTexture != NULL)
		free(rawTexture);
	
	rawTexture = NULL;
	colorTable = NULL;
	timData    = NULL;
}


void TIM::readFromPtr(unsigned char *data) {
	timData = data;

	// Leitura do Header até numbCluts, numbCluts é o tamanho do 
	// clutData que valos alocar dinamicamente

	memcpy(&timTexture.MAGIC_NUM, timData, sizeof(unsigned int));

	if (timTexture.MAGIC_NUM != 0x10) {
		std::cout << "TIM INVALIDO !" << std::endl;
		return;
	}

	memcpy(&timTexture.TIM_BPP, timData+4, sizeof(unsigned int));
	memcpy(&timTexture.UNK_32, timData+8, sizeof(unsigned int));
	memcpy(&timTexture.paletteOrgX, timData+12, sizeof(unsigned short));
	memcpy(&timTexture.paletteOrgY, timData+14, sizeof(unsigned short));
	memcpy(&timTexture.unk, timData+16, sizeof(unsigned short));
	memcpy(&timTexture.numbCluts, timData+18, sizeof(unsigned short));


	// Criamos uma ColorTable onde vai ficar todas as cores em formato RGB555(15bpp)
	// Que é suportado pelo OpenGL

	colorTable = (unsigned int **) malloc (sizeof(unsigned int) * timTexture.numbCluts);

	unsigned short colorByte = 0;

	// Pega o número de cluts, e aloca uma matriz com as cores
	// Como as cores está no padrão do TIM 8bpp, temos que converter
	// Nesse caso para 15bpp(TIM aqui é 8bpp)
	for (unsigned int i = 0; i < timTexture.numbCluts; i++) {
		colorTable[i] = (unsigned int *) malloc (sizeof (unsigned int) * 256);

		for (unsigned int a = 0; a < 256; a++) {
			memcpy(&colorByte, (timData+20+(a*sizeof(unsigned short))+ (512 * i)), sizeof(unsigned short));

			unsigned char r = ((colorByte >> 10) & 0x1F);
			unsigned char g = ((colorByte >> 5)  & 0x1F);
			unsigned char b = ((colorByte)       & 0x1F);

			r = ((r << 3) | (r >> 2));
			g = ((g << 3) | (g >> 2));
			b = ((b << 3) | (b >> 2));

			colorTable[i][a] = (r << 16) | (g << 8) | (b);
		}
	}

	// Desloca para o último byte após o array e começa a ler o resto do header
	unsigned short offsetRel = (20+timTexture.numbCluts*512);

	memcpy(&timTexture.UNK_32_2, timData+offsetRel, sizeof(unsigned int));
	memcpy(&timTexture.imageOrgX, timData+offsetRel+4, sizeof(unsigned short));
	memcpy(&timTexture.imageOrgY, timData+offsetRel+6, sizeof(unsigned short));
	memcpy(&timTexture.imageWidth,timData+offsetRel+8, sizeof(unsigned short));
	memcpy(&timTexture.imageHeight, timData+offsetRel+10, sizeof(unsigned short));

	rawTexture = (unsigned char*) malloc (timTexture.imageHeight * (timTexture.imageWidth * 2) * 3);
	
	unsigned int offset = 0;
	unsigned char yOffset = 0;
	unsigned int color = 0;
	for (unsigned short y = 0; y < timTexture.imageHeight; y++) {
		for(unsigned short x = 0; x < (timTexture.imageWidth*2); x++) {
			memcpy(&yOffset,(timData+offsetRel+12+x+(y*timTexture.imageWidth*2)), sizeof(unsigned char));

			color = colorTable[x / 128][yOffset];

			rawTexture[offset+2]   = ((color & 0x00FF0000) >> 16);
			rawTexture[offset+1]   = ((color & 0x0000FF00) >> 8);
			rawTexture[offset]     =  (color & 0x000000FF);

			offset+=3;
		}
	}




}

// Leitura de arquivo .TIM, o nome do arquivo será passado
// por um std::string
void TIM::timLoad(std::string imgName) {
	// Inicializa variavel de arquivos
	FILE *timFile = NULL;

	// Abre o arquivo .tim
	timFile = fopen(imgName.c_str(), "rb");

	// Verifica se o arquivo foi realmente aberto
	if (timFile == NULL) {
		std::cout << "TIM File" << imgName << " não encontrado !" << std::endl;
	}

	// Pula para o final do arquivo, pega o tamanho, reorganiza ele
	fseek(timFile, 0, SEEK_END);
	timSize = ftell(timFile);
	rewind(timFile);

	// Faz alocação dinâmica com o tamanho do arquivo
	timData = (unsigned char*) malloc (8 * timSize);

	// Leitura de todos os dados para o timData, FILE é inútil a partir de agora
	fread(timData, timSize, 1, timFile);
	fclose(timFile);

	// Leitura do Header até numbCluts, numbCluts é o tamanho do 
	// clutData que valos alocar dinamicamente

	memcpy(&timTexture.MAGIC_NUM, timData, sizeof(unsigned int));

	if (timTexture.MAGIC_NUM != 0x10) {
		std::cout << "TIM INVALIDO !" << std::endl;
		return;
	}

	memcpy(&timTexture.TIM_BPP, timData+4, sizeof(unsigned int));
	memcpy(&timTexture.UNK_32, timData+8, sizeof(unsigned int));
	memcpy(&timTexture.paletteOrgX, timData+12, sizeof(unsigned short));
	memcpy(&timTexture.paletteOrgY, timData+14, sizeof(unsigned short));
	memcpy(&timTexture.unk, timData+16, sizeof(unsigned short));
	memcpy(&timTexture.numbCluts, timData+18, sizeof(unsigned short));


	// Criamos uma ColorTable onde vai ficar todas as cores em formato RGB555(15bpp)
	// Que é suportado pelo OpenGL

	colorTable = (unsigned int **) malloc (sizeof(unsigned int) * timTexture.numbCluts);

	unsigned short colorByte = 0;

	// Pega o número de cluts, e aloca uma matriz com as cores
	// Como as cores está no padrão do TIM 8bpp, temos que converter
	// Nesse caso para 15bpp(TIM aqui é 8bpp)
	for (unsigned int i = 0; i < timTexture.numbCluts; i++) {
		colorTable[i] = (unsigned int *) malloc (sizeof (unsigned int) * 256);

		for (unsigned int a = 0; a < 256; a++) {
			memcpy(&colorByte, (timData+20+(a*sizeof(unsigned short))+ (512 * i)), sizeof(unsigned short));

			unsigned char r = ((colorByte >> 10) & 0x1F);
			unsigned char g = ((colorByte >> 5)  & 0x1F);
			unsigned char b = ((colorByte)       & 0x1F);

			r = ((r << 3) | (r >> 2));
			g = ((g << 3) | (g >> 2));
			b = ((b << 3) | (b >> 2));

			colorTable[i][a] = (r << 16) | (g << 8) | (b);
		}
	}

	// Desloca para o último byte após o array e começa a ler o resto do header
	unsigned short offsetRel = (20+timTexture.numbCluts*512);

	memcpy(&timTexture.UNK_32_2, timData+offsetRel, sizeof(unsigned int));
	memcpy(&timTexture.imageOrgX, timData+offsetRel+4, sizeof(unsigned short));
	memcpy(&timTexture.imageOrgY, timData+offsetRel+6, sizeof(unsigned short));
	memcpy(&timTexture.imageWidth,timData+offsetRel+8, sizeof(unsigned short));
	memcpy(&timTexture.imageHeight, timData+offsetRel+10, sizeof(unsigned short));

	rawTexture = (unsigned char*) malloc (timTexture.imageHeight * (timTexture.imageWidth * 2) * 3);
	
	unsigned int offset = 0;
	unsigned char yOffset = 0;
	unsigned int color = 0;
	for (unsigned short y = 0; y < timTexture.imageHeight; y++) {
		for(unsigned short x = 0; x < (timTexture.imageWidth*2); x++) {
			memcpy(&yOffset,(timData+offsetRel+12+x+(y*timTexture.imageWidth*2)), sizeof(unsigned char));

			color = colorTable[x / 128][yOffset];

			rawTexture[offset+2]   = ((color & 0x00FF0000) >> 16);
			rawTexture[offset+1]   = ((color & 0x0000FF00) >> 8);
			rawTexture[offset]     =  (color & 0x000000FF);

			offset+=3;
		}
	}


}