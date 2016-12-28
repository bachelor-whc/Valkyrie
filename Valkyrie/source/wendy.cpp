#include <cassert>
#include "wendy.h"
using namespace Wendy;

LRESULT CALLBACK Wendy::DefaultWindowProcesse(HWND handle, UINT message, WPARAM uintptr_parameter, LPARAM longptr_parameter) {
	switch (message) {
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		break;
	default:
		break;
	}

	return (DefWindowProc(handle, message, uintptr_parameter, longptr_parameter));
}

Window::Window(const int width, const int height) :
	m_width(width), m_height (height)
{
	assert(m_width > 0);
	assert(m_height > 0);

}

Window::~Window() {

}

int Window::getWidth() const {
	return m_width;
}

int Window::getHeight() const {
	return m_height;
}

Win32Window::Win32Window(const int width, const int height, const HINSTANCE instance_handle, PnfWindowProcesse pnf_window_processe) : 
	Window(width, height), 
	m_instance_handle(instance_handle), 
	mpnf_window_processe(pnf_window_processe)
{

}

Win32Window::~Win32Window() {

}

bool Win32Window::create(const std::string& window_title) {
	WNDCLASSEX win_class;

	win_class.cbSize = sizeof(WNDCLASSEX);
	win_class.style = CS_HREDRAW | CS_VREDRAW;
	win_class.lpfnWndProc = mpnf_window_processe;
	win_class.cbClsExtra = 0;
	win_class.cbWndExtra = 0;
	win_class.hInstance = m_instance_handle;
	win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	win_class.lpszMenuName = NULL;
	win_class.lpszClassName = "WendyWindow";
	win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	ATOM result = RegisterClassEx(&win_class);
	assert(result != NULL);

	RECT window_rectangle = { 0, 0, getWidth(), getHeight() };
	AdjustWindowRect(&window_rectangle, WS_OVERLAPPEDWINDOW, FALSE);
	m_window_handle = CreateWindowEx(
		0,
		"WendyWindow",
		window_title.c_str(),
		WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		window_rectangle.right - window_rectangle.left,
		window_rectangle.bottom - window_rectangle.top,
		NULL,
		NULL,
		m_instance_handle,
		NULL);

	return m_window_handle != NULL;
}

HINSTANCE Win32Window::getHINSTANCE() const {
	return m_instance_handle;
}

HWND Win32Window::getHWND() const {
	return m_window_handle;
}

ElectronWin32Window::ElectronWin32Window(const int width, const int height, const HWND window_handle) :
	Window(width, height),
	m_window_handle(window_handle) {

}

ElectronWin32Window::~ElectronWin32Window() {

}

HINSTANCE Wendy::ElectronWin32Window::getHINSTANCE() const {
	return (HINSTANCE)GetWindowLongPtr(m_window_handle, GWLP_HINSTANCE);
}

HWND ElectronWin32Window::getHWND() const {
	return m_window_handle;
}