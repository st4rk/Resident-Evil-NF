/***************************************************************************
** gameStruct é utilizada para todas as estruturas/classes relacionadas   **
** ao Player, Inimigos, Objetos(?) e por aí vai                           **
** Criado por Lucas P. Stark                                              **
** Nightmare Fiction Engine - NFE                                         **
***************************************************************************/

#ifndef GAME_STRUCT_H
#define GAME_STRUCT_H

/* Leon 1.5 Special Custom .EMD Animações
** emdSec2Data
** 1º Andando para trás
** 2º Andando meio lento para trás
** 3º Morte
** 4º Ataque, provavelmente ataque de chefe ou inimigos grandes
** 5º Ataque(de costas), provavelmente '' '' '''
** 6º Ataque(de frente, certeira), ''
** 7º Pegar item no chão
** 8º Início do empurrar
** 9º Empurrando objeto
** 10º Andando cambaliando
** emdSec4Data
** 1º Caminhando
** 2º Correndo
** 3º Parado com movimento corporal
** 4º Machucado andando
** 5º Machucado correndo
** 6º Parado com mão na sintura
** 7º Cambaliando andando meio lento
** 8º cambaliando correndo(EHUEHUHEUE)
** 9º Cambaliado parado
** 10º Preparando para atirar(pra frente)
** 11º Atirando pra frente
*/

// Necessário para o calculo do SEN/COS para sabermos em que direção o player deve andar
// O Personagem no Resident Evil move em 360º quando você pressiona o botão de esquerda e 
// direita, enquanto para cima ele apenas vai para a posição que ele parou entre 0 a 360
// Como estamos trabalhando em 3 dimensões, apenas utilizamos o X/Z para movimento, onde o
// Y vai ser para mover o personagem para cima ou para baixo, como queremos apenas os valores
// a serem adiciona a X e Y, a gente utiliza COS/SEN, dependendo do quadrante que ele se encontra
// Saberemos quanto diminuir ou somar.

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "EMD.h"

// Math 
#define PI 3.14159265
// All possible menus, dunno af
#define STATE_MAIN_MENU   1
#define STATE_CREDITS     2
#define STATE_INVENTARY   3
#define STATE_IN_GAME     4
#define STATE_IN_ROOM     5


// MainMenu sub-menus
#define MAIN_MENU_LOGO         0x0
#define MAIN_MENU_ENGINE_LOGO  0x1
#define MAIN_MENU_START        0x2
#define MAIN_MENU_GAME         0x3

// InGame sub-menus/status
#define IN_GAME_BEGIN          0x0
#define IN_GAME_NORMAL         0x1

// Text Type
#define TEXT_TYPE_NORMAL       0x0
#define TEXT_TYPE_LITTLE       0x1

class playerClass {
public:
	playerClass();
   ~playerClass();

   void setPlayerEMD(unsigned short num);
   void setPlayerX(float x);
   void setPlayerY(float y);
   void setPlayerZ(float z);
   void setPlayerAnim(unsigned char num);
   void setPlayerAnimSection(unsigned char sec);
   void setPlayerInMove(bool mov);
   void setPlayerInRotate(bool rot);
   void setPlayerInRotatePos(unsigned char pos);
   void setPlayerInShoot(bool shoot);
   void setPlayerRDT(unsigned int RDT);
   void setPlayerCam(unsigned char num);
   void setPlayerItem(int slotNum, int id);
   void setPlayerItemAmount(int slotNum, int amount);
   void setPlayerRunning(bool);
   void setPlayerAngle(float n);
   void setPlayerAnimCount(int n);
   void setPlayerEMDAnim(EMD_SEC2_DATA_T n);

   float getPlayerX();
   float getPlayerY();
   float getPlayerZ();
   float getPlayerAngle();
	
   bool getPlayerInMove();
   bool getPlayerInRotate();
   bool getPlayerInShoot();
   bool getPlayerIsAmount(int slotNum);
   bool getPlayerRunning();
   
   unsigned char  getPlayerAnim();
   unsigned char  getPlayerCam();
   unsigned char  getPlayerAnimSection();
   unsigned char  getPlayerInRotatePos();
   unsigned short getPlayerEMD();
   unsigned int   getPlayerRDT();
   unsigned int   getPlayerAnimCount();

   int   getPlayerItemID(int slotNum);
   int   getPlayerItemAmount(int slotNum);

   EMD_SEC2_DATA_T getPlayerEMDAnim();

private:

   // Player Item Structure
   typedef struct pItem {
      int id;
      bool isAmount;
      int amount;
   } pItem;
	
   // Player EMD(Model)
	unsigned short playerEMD;

   // Animation Section
   unsigned char animSection;

   // Player Animation Num
	unsigned char playerAnimNum;

   // Player Animation Count
   unsigned int animCount;

   // Player EMD Animation
   EMD_SEC2_DATA_T anim;



   // Rotation position
   unsigned char inRotatePos;
   
   // CAM 
   unsigned char camNum;

   // RDT
   unsigned int rdtNum;

   // Some stuff to verify what the player is doing
   bool inRotate;
   bool inMove;
   bool inShoot;

   bool running;

   // Player Coord
	float playerX;
   float playerY;
   float playerZ;

   // PlayerHP
   unsigned short playerHP;

   // Player Items
   pItem item[8];

   // Player angle
   float p;

};


class enemyClass {
public:
   enemyClass();
   ~enemyClass();


   float getX();
   float getY();
   float getZ();
   float getAngle();


   int   getEMD();

   unsigned int getAnimCount();


   void  setX(float x);
   void  setY(float y);
   void  setZ(float z);
   void  setEMD(int n);
   void  setAngle(float n);
   void  setAnim(EMD_SEC2_DATA_T n);
   void  setAnimCount(unsigned int n);


   EMD_SEC2_DATA_T getAnim();


private:
   float x;
   float y;
   float z;
   float p;
   
   int emd;

   unsigned int    animCount;
   EMD_SEC2_DATA_T anim;




};


class bmp_loader_24bpp {
public:	
	bmp_loader_24bpp();
	~bmp_loader_24bpp();

	void bmpLoaderFile(std::string fileName, int type);
	unsigned char *bmpBuffer;
	int bmpHeight;
	int bmpWidth;
private:
	unsigned char bmpHeader[54];
	unsigned int bmpDataInit;
	unsigned int bmpSize;

};


#endif