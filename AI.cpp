#include "AI.h"


AI::AI() {

}

AI::~AI() {

}


void AI::zombie_re_2(player* p, enemy *e) {
	switch (e->getState()) {
		case AI_STATE_BEGIN:
			e->setAnimSection(EMD_SECTION_4);
			e->setAnimType(ZOMBIE_SEC4_ANIM_IDLE);
			e->setState(AI_STATE_IDLE);
		break;

		case AI_STATE_IDLE: {

			if (eMath.collisionRectangleHW(e->getX(), e->getY(), e->getZ(),
										   p->getX(), p->getY(), p->getZ(),
										   8192, 8192)) {

				e->setAnimSection(EMD_SECTION_2);
				e->setAnimType(ZOMBIE_SEC2_ANIM_AWALK);
				e->setState(AI_STATE_FOLLOW);
			}

		}
		break;

		case AI_STATE_FOLLOW: {
	        float angle = (atan2(((p->getX() - e->getX())), 
    					  (p->getZ() - e->getZ())) / (M_PI / 180));

    		e->setAngle((angle-90));

			float x = e->getX() + cos((e->getAngle() * M_PI/180)) * 10.0;
			float z = e->getZ() - sin((e->getAngle() * M_PI/180)) * 10.0;

	    	if (!eMath.collisionRectangle(p->getX(), p->getY(), p->getZ(),
	 			                          x, e->getY(),z)) {

					e->setX(x);
					e->setZ(z);

		    } else {
		    	if (!((p->getState() == PLAYER_STATE_BEGIN_HIT) || (p->getState() == PLAYER_STATE_HIT))) {
			    	e->setTmrAnim(2000 + SDL_GetTicks());
			    	e->setAnimSection(EMD_SECTION_4);
					e->setAnimType(ZOMBIE_SEC4_ANIM_W_ATTACK);
					e->setState(AI_STATE_ATTACK);
					p->setState(PLAYER_STATE_BEGIN_HIT);
		    	}
		    }
		}

		break;

		case AI_STATE_PUSH: { 
			float x = e->getX() - cos((e->getAngle() * M_PI/180)) * 20.0;
			float z = e->getZ() + sin((e->getAngle() * M_PI/180)) * 20.0;

			e->setX(x);
			e->setZ(z);

			if (e->getAnimCount() == 57) {
				e->setAnimSection(EMD_SECTION_2);
				e->setAnimType(ZOMBIE_SEC2_ANIM_AWALK);
				e->setState(AI_STATE_FOLLOW);
			}
		}
		break;

		case AI_STATE_ATTACK: {
			if (e->getTmrAnim() < SDL_GetTicks()) {
				e->setTmrAnim(0);
				e->setAnimType(ZOMBIE_SEC4_ANIM_W_PUSH, false);
				e->setState(AI_STATE_PUSH);
				p->setState(PLAYER_STATE_NORMAL);
				p->setAnimSection(EMD_SECTION_4);
				p->setAnimType(STAND_SEC4_ANIM_IDLE);
			}
		}
		break;

		case AI_STATE_HIT: {
			if (e->getHitPoints() <= 0) {

				e->setAnimSection(EMD_SECTION_4);
				e->setAnimType(ZOMBIE_SEC4_ANIM_DEATH_1,false);
				e->setState(AI_STATE_DEATH);
			} else {
				e->setHitPoints(e->getHitPoints() - 1);
				if (e->getAnimSection() != EMD_SECTION_2) {
					e->setAnimSection(EMD_SECTION_2);
					e->setAnimType(ZOMBIE_SEC2_ANIM_AWALK);
					e->setState(AI_STATE_FOLLOW);
				}
				e->setState(AI_STATE_FOLLOW);
			}
		}
		break;

		case AI_STATE_DEATH: {
			EMD_SEC2_DATA_T node = e->getAnimFrame();

			e->setY(2000 + node.yOffset);
		}


		break;
	}
}


