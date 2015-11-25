#ifndef GAME_EVENT_H
#define GAME_EVENT_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

enum EVENT_TYPE {
	EVENT_TYPE_NONE        = 0,
	EVENT_TYPE_MAINMENU    = 1,
	EVENT_TYPE_INGAME      = 2
};

class gameEvent {
public:
	gameEvent();
   ~gameEvent();

   bool isEventEnable();
   void setEventEnable();
   void nextEvent();
   void setMaxEvent(int max);
   void setEventAction(int evt, int act);
   int  getEventAction();
   int  getEventNum();



private:
	bool eventEnable;
	int eventNum;
	int maxEvent;
	int *eventAction;
};



#endif