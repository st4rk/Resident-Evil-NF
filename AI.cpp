#include "AI.h"


AI::AI() {

}

AI::~AI() {

}


void AI::zombie_re_2(player* p, enemy *e) {
	switch (e->getState()) {
		case ZOMBIE_RE2_STATE_BEGIN:
			e->setAnimSection(EMD_SECTION_4);
			e->setAnimType(ZOMBIE_SEC4_ANIM_IDLE);
			e->setState(ZOMBIE_RE2_STATE_IDLE);
		break;

		case ZOMBIE_RE2_STATE_IDLE: {

			if (eMath.collisionRectangleHW(e->getX(), e->getY(), e->getZ(),
										   p->getX(), p->getY(), p->getZ(),
										   8192, 8192)) {

				e->setAnimSection(EMD_SECTION_2);
				e->setAnimType(ZOMBIE_SEC2_ANIM_AWALK);
				e->setState(ZOMBIE_RE2_STATE_FOLLOW);
			}

		}
		break;

		case ZOMBIE_RE2_STATE_FOLLOW: {
	         float angle = (atan2(((p->getX() - e->getX())), 
    					  (p->getZ() - e->getZ())) / (M_PI / 180));

    		e->setAngle((angle-90));

			float x = e->getX() + cos((e->getAngle() * M_PI/180)) * 10.0;
			float z = e->getZ() - sin((e->getAngle() * M_PI/180)) * 10.0;

	 		if (!eMath.collisionRectangle(p->getX(), p->getY(), p->getZ(),
	 			                          x, e->getY(),z)) {

					e->setX(x);
					e->setZ(z);

	    	}
		}

		break;

		case ZOMBIE_RE2_STATE_HIT: {
			if (e->getHitPoints() <= 0) {

				e->setAnimSection(EMD_SECTION_4);
				e->setAnimType(ZOMBIE_SEC4_ANIM_DEATH_1,false);
				e->setState(ZOMBIE_RE2_STATE_DEATH);
			} else {
				e->setHitPoints(e->getHitPoints() - 1);
				printf("hitpoints: %d\n", e->getHitPoints());
				if (e->getAnimSection() != EMD_SECTION_2) {
					e->setAnimSection(EMD_SECTION_2);
					e->setAnimType(ZOMBIE_SEC2_ANIM_AWALK);
					e->setState(ZOMBIE_RE2_STATE_FOLLOW);
				}
				e->setState(ZOMBIE_RE2_STATE_FOLLOW);
			}
		}
		break;

		case ZOMBIE_RE2_STATE_DEATH: {
			EMD_SEC2_DATA_T node = e->getAnimFrame();

			e->setY(2000 + node.yOffset);
		}


		break;
	}
}


