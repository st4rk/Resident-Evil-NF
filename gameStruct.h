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
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

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
   SPECIAL_SEC2_ANIM_CHOICE   = 2
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
 * Player Stuff
 */ 
enum PLAYER_STATE {
   PLAYER_STATE_NORMAL      = 0,
   PLAYER_STATE_BEGIN_HIT   = 1,
   PLAYER_STATE_HIT         = 2,
   PLAYER_STATE_BEGIN_GUN   = 3,
   PLAYER_STATE_GUN         = 4,
   PLAYER_STATE_BEGIN_DEATH = 5,
   PLAYER_STATE_DEATH       = 6,
   PLAYER_STATE_END         = 7
};

enum PLAYER_ACTION {
   PLAYER_ACTION_R_NONE  = 0,
   PLAYER_ACTION_R_LEFT  = 1,
   PLAYER_ACTION_R_RIGHT = 2
};

enum ENGINE_MENU_STATE {
   STATE_MAIN_MENU = 1,
   STATE_CREDITS   = 2,
   STATE_SEL_CHAR  = 3,
   STATE_IN_GAME   = 4,
   STATE_IN_ROOM   = 5,
   STATE_IN_DEBUG  = 6
};

enum MAIN_MENU_SUB_MENU {
   MAIN_MENU_LOGO         = 0,
   MAIN_MENU_ENGINE_LOGO  = 1,
   MAIN_MENU_START        = 2,
   MAIN_MENU_GAME         = 3
};

enum SEL_PLAYER_SUB_MENU {
   SEL_PLAYER_BEGIN      = 0,
   SEL_PLAYER_START      = 1
};

enum MENU_CREDITS {
   MENU_CREDITS_BEGIN = 0,
   MENU_CREDITS_END   = 1
};

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

class entity {
public:
      entity();
     ~entity();

      void setModel(unsigned int modelNum);
      
      void setTmrAnim(unsigned int tmrAnim);
      void setState(unsigned int state);
      void setHitPoints(unsigned int hitPoints);

      void setAnimSection(unsigned int animSection);
      void setAnimType(unsigned int animType, bool animRepeat = true);
      void setAnimCount(unsigned int animCount);
      void setAnimFrame(EMD_SEC2_DATA_T animFrame);
      void setAnimRotationDir(unsigned int animRotationDir);

      void setX(float x);
      void setY(float y);
      void setZ(float z);
      void setAngle(float angle);


      unsigned int      getModel();
      
      unsigned int      getHitPoints();
      unsigned int      getTmrAnim();
      unsigned int      getState();

      unsigned int      getAnimSection();
      unsigned int      getAnimType();
      unsigned int      getAnimCount();
      unsigned int      getAnimOldSection();
      unsigned int      getAnimOldCount();
      unsigned char     getAnimRotationDir();
      EMD_SEC2_DATA_T   getAnimFrame();
      bool              getAnimRepeat();

      float  getX();
      float  getY();
      float  getZ();
      float  getAngle();

private:
      
      unsigned int    modelNum;

      unsigned int    animSection;
      unsigned int    animCount;
      unsigned int    animOldSection;
      unsigned int    animOldCount;
      unsigned int    animType;
      unsigned char   animRotationDir;
      EMD_SEC2_DATA_T animFrame;
      bool            animRepeat;

      float x;
      float y;
      float z;
      float angle;


      unsigned int    tmrAnim;

      unsigned int    hitPoints;
      
      unsigned int    state;

};

class enemy : public entity {
public:
   enemy();
  ~enemy();

  void setType(unsigned int type);
  void setDelta(float delta);

  unsigned int getType();

  float getDelta();
private:
   unsigned int type;

   float delta;
};


class player : public entity {
public:
   player();
  ~player();

  void setItemID(unsigned char slot, unsigned int iID);

  void setCam(unsigned int cam);

  unsigned int getCam();

  unsigned int getItemID(unsigned char slot);

private:
   unsigned int cam;
   unsigned int iID[8];
};



enum BITMAP_TYPE_FILE {
   BITMAP_TYPE_MONO     = 0x1,
   BITMAP_TYPE_16COLOR  = 0x4,
   BITMAP_TYPE_256COLOR = 0x8,
   BITMAP_TYPE_16BPP    = 0x10,
   BITMAP_TYPE_24BPP    = 0x18,
   BITMAP_TYPE_32BPP    = 0x20
};

/*
 * NFP - Nightamre Fiction Picture
 * for now using SDL_Surface
 */
class NFP {
public:
   NFP();
  ~NFP();

   bool loadImage(std::string fileName);  

   void* getPixelData();

   int   getWidth();
   int   getHeight();

private:
   SDL_Surface* texture;

};

class BITMAP {
public:	
	BITMAP();
	~BITMAP();

	void loaderFile(std::string fileName, int type);
	unsigned char *bmpBuffer;
	int bmpHeight;
	int bmpWidth;
private:
	unsigned char bmpHeader[54];
	unsigned int bmpDataInit;
	unsigned int bmpSize;
   unsigned short bpp;

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