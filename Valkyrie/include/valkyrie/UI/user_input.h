#ifndef _VALKYRIE_USER_INPUT_H 
#define _VALKYRIE_USER_INPUT_H  
#include "common.h"

struct UserInput {
	void handleSDLMouseButtonEvent(const SDL_Event& s_event);
	void handleSDLKeyBoardEvent(const SDL_Event& s_event);
	void handleSDLCharEvent(const SDL_Event& s_event);
	void handleSDLScrollEvent(const SDL_Event& s_event);

	bool mousePressed[3] = { false, false, false };
	float mouseWheel = 0.0f;
};

#endif