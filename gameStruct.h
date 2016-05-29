/***************************************************************************
** gameStruct é utilizada para todas as estruturas/classes relacionadas   **
** ao Player, Inimigos, Objetos(?) e por aí vai                           **
** Criado por Lucas P. Stark                                              **
** Nightmare Fiction Engine - NFE                                         **
***************************************************************************/

#ifndef GAME_STRUCT_H
#define GAME_STRUCT_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "EMD.h"

/*
 * EMD Animation Section
 */

enum EMD_SECTION_LIST {
   EMD_SECTION_2 = 0,
   EMD_SECTION_4 = 1
};

/*
 * Player EMD Animation
 * emdSec2 Standard Data
 * emdSec4 Standard Data
 */
enum STANDARD_SEC2_ANIMATION {
   STAND_SEC2_ANIM_BACKWARD  = 0,
   STAND_SEC2_ANIM_SBACKWARD = 1,
   STAND_SEC2_ANIM_DEATH     = 2,
   STAND_SEC2_ANIM_HIT       = 3,
   STAND_SEC2_ANIM_BACK_HIT  = 4,
   STAND_SEC2_ANIM_HIT_2     = 5,
   STAND_SEC2_ANIM_GET_ITEM  = 6,
   STAND_SEC2_ANIM_PUSH      = 7,
   STAND_SEC2_ANIM_PUSHING   = 8,
   STAND_SEC2_ANIM_INJU_WALK = 9
};

enum STANDARD_SEC4_ANIMATION {
   STAND_SEC4_ANIM_WALK      = 0,
   STAND_SEC4_ANIM_RUNNING   = 1,
   STAND_SEC4_ANIM_IDLE      = 2,
   STAND_SEC4_ANIM_INJU_WALK = 3,
   STAND_SEC4_ANIM_INJU_RUNN = 4,
   STAND_SEC4_ANIM_INJU_IDLE = 5,
   STAND_SEC4_ANIM_TINJ_WALK = 6,
   STAND_SEC4_ANIM_TINJ_RUNN = 7,
   STAND_SEC4_ANIM_TINJ_IDLE = 8,
   STAND_SEC4_ANIM_S_SHOOT   = 9,
   STAND_SEC4_ANIM_SHOOTING  = 10,
   STAND_SEC4_ANIM_AIM       = 11,
   STAND_SEC4_ANIM_USHOOTING = 12,
   STAND_SEC4_ANIM_UAIM      = 13,
   STAND_SEC4_ANIM_DSHOOTING = 14,
   STAND_SEC4_ANIM_DAIM      = 15,
   STAND_SEC4_ANIM_AMMO      = 16
};

/*
 * Special EMD Animation
 * emdSec2Data
 * emdSec4Data
 */

enum SPECIAL_SEC2_ANIMATION {
   SPECIAL_SEC2_ANIM_START    = 0,
   SPECIAL_SEC2_ANIM_POSE     = 1,
   SPECIAL_SEC2_ANIM_CHOICE   = 3
};

enum SPECIAL_SEC4_ANIMATION {
   SPECIAL_SEC4_ANIM_BHIT     = 0,
   SPECIAL_SEC4_ANIM_UP_SHAKE = 1,
   SPECIAL_SEC4_ANIM_BHIT_2   = 2,
   SPECIAL_SEC4_ANIM_BHIT_3   = 3,
   SPECIAL_SEC4_ANIM_EXAUSTED = 4,
   SPECIAL_SEC4_ANIM_FALL     = 5,
   SPECIAL_SEC4_ANIM_RISE     = 6,
   SPECIAL_SEC4_ANIM_DOWNFALL = 7
};

/* 
 * Resident Evil 2 - Zombie EMD Standard
 * emdSec2Data
 * emdSec4Data
 */

enum ZOMBIE_SEC2_ANIMATION {
   ZOMBIE_SEC2_ANIM_WALK     = 0,
   ZOMBIE_SEC2_ANIM_RUN      = 1,
   ZOMBIE_SEC2_ANIM_SWALK    = 2,
   ZOMBIE_SEC2_ANIM_SSWALK   = 3,
   ZOMBIE_SEC2_ANIM_AWALK    = 4,
   ZOMBIE_SEC2_ANIM_ARUN     = 5,
   ZOMBIE_SEC2_ANIM_ASWALK   = 6,
   ZOMBIE_SEC2_ANIM_ARUN2    = 7
};

enum ZOMBIE_SEC4_ANIMATION {
   ZOMBIE_SEC4_ANIM_IDLE     = 0,
   ZOMBIE_SEC4_ANIM_DEATH_1  = 1,
   ZOMBIE_SEC4_ANIM_DEATH_2  = 2,
   ZOMBIE_SEC4_ANIM_PUSH_1   = 3,
   ZOMBIE_SEC4_ANIM_PUSH_2   = 4,
   ZOMBIE_SEC4_ANIM_DRAGGING = 5,
   ZOMBIE_SEC4_ANIM_HIT_DRAG = 6,
   ZOMBIE_SEC4_ANIM_D_DEATH  = 7,
   ZOMBIE_SEC4_ANIM_RISE_1   = 8,
   ZOMBIE_SEC4_ANIM_RISE_2   = 9,
   ZOMBIE_SEC4_ANIM_FALL_1   = 10,
   ZOMBIE_SEC4_ANIM_W_HIT    = 11,
   ZOMBIE_SEC4_ANIM_W_ATTACK = 12,
   ZOMBIE_SEC4_ANIM_W_PUSH   = 13,
   ZOMBIE_SEC4_ANIM_D_GET    = 14,
   ZOMBIE_SEC4_ANIM_D_ATTACK = 15,
   ZOMBIE_SEC4_ANIM_D_PUSH   = 16,
   ZOMBIE_SEC4_ANIM_IDLE_HIT = 17,
   ZOMBIE_SEC4_ANIM_EATING_1 = 18,
   ZOMBIE_SEC4_ANIM_EATING_2 = 19,
   ZOMBIE_SEC4_ANIM_E_HIT    = 20,
   ZOMBIE_SEC4_ANIM_E_RISE   = 21,
   ZOMBIE_SEC4_ANIM_G_HIT_1  = 22,
   ZOMBIE_SEC4_ANIM_G_HIT_2  = 23,
   ZOMBIE_SEC4_ANIM_F_EAT    = 24,
   ZOMBIE_SEC4_ANIM_S_WARLK  = 25,
   ZOMBIE_SEC4_ANIM_S_NOT    = 26,
   ZOMBIE_SEC4_ANIM_S_STOP   = 27,
   ZOMBIE_SEC4_ANIM_A_HIT_1  = 28,
   ZOMBIE_SEC4_ANIM_A_HIT_2  = 29,
   ZOMBIE_SEC4_ANIM_A_HIT_3  = 30
};

/*
 * Player Stuff
 */ 

enum PLAYER_ACTION {
   PLAYER_ACTION_R_NONE  = 0,
   PLAYER_ACTION_R_LEFT  = 1,
   PLAYER_ACTION_R_RIGHT = 2
};

// Math 
#define PI 3.14159265
// All possible menus, dunno af
#define STATE_MAIN_MENU   1
#define STATE_CREDITS     2
#define STATE_SEL_CHAR    3
#define STATE_IN_GAME     4
#define STATE_IN_ROOM     5
#define STATE_IN_DEBUG    6


// MainMenu sub-menus
#define MAIN_MENU_LOGO         0x0
#define MAIN_MENU_ENGINE_LOGO  0x1
#define MAIN_MENU_START        0x2
#define MAIN_MENU_GAME         0x3

//
#define SEL_PLAYER_BEGIN       0x0
#define SEL_PLAYER_START       0x1

// InGame sub-menus/status
#define IN_GAME_BEGIN          0x0
#define IN_GAME_NORMAL         0x1

// Text Type
#define TEXT_TYPE_NORMAL       0x0
#define TEXT_TYPE_LITTLE       0x1


// Animation Stuff 

#define ANIM_TYPE_WALK         0x0
#define ANIM_TYPE_RUNNING      0x1
#define ANIM_TYPE_STOPPED      0x2

class playerClass {
public:
	playerClass();
   ~playerClass();

   void setPlayerEMD(unsigned short num);
   void setPlayerX(float x);
   void setPlayerY(float y);
   void setPlayerZ(float z);
   void setPlayerAnim(unsigned char num, bool repeat = true);
   void setPlayerAnimSection(unsigned char sec);
   void setPlayerRDT(unsigned int RDT);
   void setPlayerCam(unsigned char num);
   void setPlayerItem(int slotNum, int id);
   void setPlayerItemAmount(int slotNum, int amount);
   void setPlayerAngle(float n);
   void setPlayerAnimCount(int n);
   void setPlayerEMDAnim(EMD_SEC2_DATA_T n);
   void setPlayerInRotation(unsigned char inRotation);

   float getPlayerX();
   float getPlayerY();
   float getPlayerZ();
   float getPlayerAngle();
	
   bool getPlayerIsAmount(int slotNum);
   bool getPlayerRepeatAnim();
   
   unsigned char  getPlayerAnim();
   unsigned char  getPlayerCam();
   unsigned char  getPlayerAnimSection();
   unsigned char  getPlayerInRotation();
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

   // Repeat Animation ?
   bool animRepeat;

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

   /*
    * verify if player is in rotation
    */
   unsigned char inRotation;

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

/* Debug Stuff to room jump */
typedef struct debugRoom {
   float x;
   float y;
   float z;
   int roomNum;
   std::string roomName;

} debugRoom;

#endif