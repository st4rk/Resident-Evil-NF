/*===========================================*
 * Engine AI                                 * 
 *-------------------------------------------*
 * Here will handle that AI must be used     *
 * and has AI implementations                *
 *===========================================*
 */



#ifndef AI_H
#define AI_H

#include "gameStruct.h"
#include "gameMath.h"
#include <cmath>


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

enum G_VIRUS_SEC2_FORM_2_ANIMATION {
   G_VIRUS_SEC2_FORM_2_WALK    = 1,
   G_VIRUS_SEC2_FORM_2_ATTACK  = 7,
   G_VIRUS_SEC2_FORM_2_DEATH   = 10,
   G_VIRUS_SEC2_FORM_2_PRESENT = 23
};


enum AI_STATE {
	AI_STATE_BEGIN  = 0,
	AI_STATE_IDLE   = 1,
	AI_STATE_FOLLOW = 2,
   AI_STATE_DEATH  = 3,
   AI_STATE_HIT    = 4,
   AI_STATE_ATTACK = 5,
   AI_STATE_PUSH   = 6
};

enum AI_LIST {
	AI_TYPE_ZOMBIE_RE2 = 0,
	AI_TYPE_TYRANT_RE1 = 1,
   AI_TYPE_G_VIRUS_2  = 2
};

class AI {
public:

	AI();
   ~AI();

   void zombie_re_2(player *p, enemy *e);
   void g_virus_2(player *p, enemy *e);

private:

	gameMath eMath;
};


#endif