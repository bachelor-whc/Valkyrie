#ifndef _WENDY_WINDOW_H 
#define _WENDY_WINDOW_H
#if defined(_WIN32) || defined(_ELECTRON_WIN32)
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

	class ElectronWin32Window : public Window {
	public:
		ElectronWin32Window() = delete;
		ElectronWin32Window(const int width, const int height, const HWND window_handle);
		virtual ~ElectronWin32Window();
		virtual bool create(const std::string& window_title) { return true; };

		HINSTANCE getInstanceHandle() const;
		HWND getWindowHandle() const;

	private:
		HINSTANCE m_instance_handle;
		HWND m_window_handle;
	};
}

#endif