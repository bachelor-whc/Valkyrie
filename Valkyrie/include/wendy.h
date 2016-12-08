#ifndef _WENDY_WINDOW_H 
#define _WENDY_WINDOW_H
#ifdef _WIN32
#include <Windows.h>
#include <string>
#endif
namespace Wendy {
#ifdef _WIN32
	LRESULT CALLBACK DefaultWindowProcesse(HWND handle, UINT message, WPARAM uintptr_parameter, LPARAM longptr_parameter);
	typedef LRESULT (CALLBACK*PnfWindowProcesse)(HWND, UINT , WPARAM , LPARAM);
#endif
	class Window {
	public:
		Window() = delete;
		Window(const int width, const int height);
		virtual ~Window();
		virtual bool create(const std::string& window_title) = 0;

		int getWidth() const;
		int getHeight() const;

	private:
		int m_width;
		int m_height;
	};
#ifdef _WIN32
	class Win32Window : public Window {
	public:
		Win32Window() = delete;
		Win32Window(const int width, const int height, const HINSTANCE instance_handle, PnfWindowProcesse processe = DefaultWindowProcesse);
		virtual ~Win32Window();
		virtual bool create(const std::string& window_title);

		HINSTANCE getInstanceHandle() const;
		HWND getWindowHandle() const;

	private:
		HINSTANCE m_instance_handle;
		HWND m_window_handle;
		PnfWindowProcesse mpnf_window_processe;
	};
#endif
}

#endif