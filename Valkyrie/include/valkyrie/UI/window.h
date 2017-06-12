#ifndef _VALKYRIE_WINDOW_H
#define _VALKYRIE_WINDOW_H
#include <string>
#include <new>
#include <memory>
#include <SDL2/SDL_syswm.h>
struct SDL_Window;

namespace Valkyrie {
	class Window {
	public:
		Window() = delete;
		Window(const Window&) = delete;
		Window(const std::string& title, int width, int height);
		virtual ~Window();

		Window& operator=(const Window&) = delete;

		float getRatio() const;
		float getWidth() const;
		float getHeight() const;

#ifdef _WIN32
		HWND getWindowHandle() const;
		HINSTANCE getWindowInstance() const;
#endif

		void setWindowTitle(const std::string& title);
		void appendWindowTitle(const std::string& str);

	private:
		SDL_Window* mp_window = nullptr;
		SDL_SysWMinfo m_window_information = {};
		std::string m_title;
		float m_width;
		float m_height;
		float m_ratio;
	};
	using WindowPtr = std::shared_ptr<Window>;
}

#endif