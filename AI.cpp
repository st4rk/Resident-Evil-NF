#include "AI.h"


AI::AI() {

}

AI::~AI() {

}


void AI::zombie_re_2(player* p, enemy *e) {
	switch (e->getState()) {
		case ZOMBIE_RE2_STATE_BEGIN:
			e->setAnimSection(EMD_SECTION_4);
			e->setAnimType(ZOMBIE_SEC4_ANIM_W_HIT);
			e->setState(ZOMBIE_RE2_STATE_IDLE);
		break;

		case ZOMBIE_RE2_STATE_IDLE:

		break;
	}
}


