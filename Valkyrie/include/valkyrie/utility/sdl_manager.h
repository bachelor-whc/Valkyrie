#ifndef _VALKYRIE_SDL_MANAGER_H
#define _VALKYRIE_SDL_MANAGER_H

namespace Valkyrie {
	class SDLManager {
	public:
		static int initialize();
		static void close();
		static bool initialized();
		static SDLManager* getGlobalSDLManagerPtr() { return gp_SDL_manager; }

		virtual ~SDLManager();

	private:
		static SDLManager* gp_SDL_manager;
		SDLManager();
	};
}

#endif