#include <imgui.h>
#include "valkyrie/valkyrie.h"
#include "valkyrie/UI/user_input.h"

void UserInput::handleSDLMouseButtonEvent(const SDL_Event& s_event) {
	bool s = s_event.type == SDL_MOUSEBUTTONDOWN ? true : false;
	int index;
	switch (s_event.button.button) {
	case SDL_BUTTON_LEFT:
		index = 0;
		break;
	case SDL_BUTTON_RIGHT:
		index = 1;
		break;
	case SDL_BUTTON_MIDDLE:
	default:
		index = 2;
		break;
	}
	mousePressed[index] = s;
}

void UserInput::handleSDLKeyBoardEvent(const SDL_Event& s_event) {
	auto& imgui_io = ImGui::GetIO();
	if(s_event.type == SDL_KEYDOWN)
		imgui_io.KeysDown[s_event.key.keysym.scancode] = true;
	else if(s_event.type == SDL_KEYUP)
		imgui_io.KeysDown[s_event.key.keysym.scancode] = false;

	imgui_io.KeyCtrl = imgui_io.KeysDown[SDL_SCANCODE_LCTRL] || imgui_io.KeysDown[SDL_SCANCODE_RCTRL];
	imgui_io.KeyShift = imgui_io.KeysDown[SDL_SCANCODE_LSHIFT] || imgui_io.KeysDown[SDL_SCANCODE_RSHIFT];
	imgui_io.KeyAlt = imgui_io.KeysDown[SDL_SCANCODE_LALT] || imgui_io.KeysDown[SDL_SCANCODE_LALT];
}

void UserInput::handleSDLCharEvent(const SDL_Event& s_event) {
	auto& imgui_io = ImGui::GetIO();
	const char* c_str = s_event.text.text;
	imgui_io.AddInputCharactersUTF8(c_str);
}

void UserInput::handleSDLScrollEvent(const SDL_Event& s_event) {
	mouseWheel += (float)s_event.wheel.y;
}
