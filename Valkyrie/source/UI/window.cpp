#include <SDL2/SDL.h>
#include <imgui.h>
#include "valkyrie/utility/sdl_manager.h"
#include "valkyrie/UI/window.h"
#include "common.h"
using namespace Valkyrie;

Window::Window(const std::string& title, int width, int height) : 
	m_title(title) {
	if (width < 1 || height < 1) {
		throw std::exception("Wrong window size.");
	}
	if (!SDLManager::initialized()) {
		throw std::exception("SDL manager should be initialized first.");
	}
	m_width = width;
	m_height = height;
	m_ratio = (float)m_width / (float)m_height;
	const SDL_WindowFlags flags = SDL_WINDOW_SHOWN;
	mp_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
	SDL_GetWindowWMInfo(mp_window, &m_window_information);
	auto& imgui_io = ImGui::GetIO();
	imgui_io.DisplaySize.x = width;
	imgui_io.DisplaySize.y = height;
}

Window::~Window() {
	if (mp_window != nullptr) {
		SDL_DestroyWindow(mp_window);
		mp_window = nullptr;
	}
}

float Window::getRatio() const {
	return m_ratio;
}

float Window::getWidth() const {
	return m_width;
}

float Window::getHeight() const {
	return m_height;
}

#ifdef _WIN32
HWND Window::getWindowHandle() const {
	return m_window_information.info.win.window;
}

HINSTANCE Window::getWindowInstance() const {
	return (HINSTANCE)GetWindowLongPtr(getWindowHandle(), GWLP_HINSTANCE);
}

void Window::setWindowTitle(const std::string& title) {
	m_title = title;
	SDL_SetWindowTitle(mp_window, m_title.c_str());
}

void Window::appendWindowTitle(const std::string& str) {
	SDL_SetWindowTitle(mp_window, (m_title + str).c_str());
}

#endif

