#include "valkyrie/UI/window_manager.h"
#include "valkyrie/UI/window.h"
#include "valkyrie/utility/sdl_manager.h"
#include "common.h"
using namespace Valkyrie;

WindowManager* WindowManager::gp_window_manager = nullptr;

int WindowManager::initialize() {
	if (!SDLManager::initialized()) {
		return -1;
	}
	if (gp_window_manager != nullptr)
		return 0;
	gp_window_manager = NEW_NT WindowManager;
	if (gp_window_manager == nullptr) {
		return 1;
	}
	return 0;
}

void WindowManager::close() {
	if (gp_window_manager != nullptr) {
		delete gp_window_manager;
		gp_window_manager = nullptr;
	}
}

WindowManager::WindowManager() {

}

WindowManager::~WindowManager() {

}

void Valkyrie::WindowManager::createMainWindow(const std::string& title, int width, int height) {
	mp_main_window = MAKE_SHARED(Window)(title, width, height);
}

WindowPtr Valkyrie::WindowManager::getMainWindowPtr() {
	return mp_main_window;
}
