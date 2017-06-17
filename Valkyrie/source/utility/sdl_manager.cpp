#include <SDL2/SDL.h>
#include "valkyrie/common.h"
#include "valkyrie/utility/sdl_manager.h"
using namespace Valkyrie;

SDLManager* SDLManager::gp_SDL_manager = nullptr;

int Valkyrie::SDLManager::initialize() {
	if (gp_SDL_manager != nullptr)
		return 0;
	gp_SDL_manager = NEW_NT SDLManager;
	if (gp_SDL_manager == nullptr) {
		return 1;
	}
	return 0;
}

void Valkyrie::SDLManager::close() {
	if (gp_SDL_manager != nullptr) {
		delete gp_SDL_manager;
		gp_SDL_manager = nullptr;
	}
}

bool Valkyrie::SDLManager::initialized() {
	return gp_SDL_manager != nullptr;
}

Valkyrie::SDLManager::~SDLManager() {
	SDL_Quit();
}

Valkyrie::SDLManager::SDLManager() {
	int sdl_result = SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	if(sdl_result != 0)
		throw std::exception("SDL initialization failed.");
}
