#include "gameEvent.h"


gameEvent::gameEvent() {
	eventEnable = false;
	eventAction = NULL;
	eventNum = 0;
}


gameEvent::~gameEvent() {
	delete [] eventAction;
	eventAction = NULL;
}


void gameEvent::nextEvent() {
	if (eventNum < maxEvent)
		eventNum++;
	else
		eventEnable = false;
}

void gameEvent::setMaxEvent(int max) {
	maxEvent = max;
	eventAction = new int [max];
}

void gameEvent::setEventEnable() { eventEnable = true; }

void gameEvent::setEventAction(int evt, int act) { eventAction[evt] = act; }

bool gameEvent::isEventEnable() { return eventEnable; }

int  gameEvent::getEventNum() { return eventNum; }

int  gameEvent::getEventAction() { return eventAction[eventNum]; }