#include <imgui.h>
#include "valkyrie.h"
#include "valkyrie/UI/user_input.h"

void GLFWMouseButtonCallback(GLFWwindow* p_window, int button, int action, int mods) {
	if (button >= 0 && button < 3) {
		auto& user_input = Valkyrie::getGlobalValkyriePtr()->userInput;
		if(action == GLFW_PRESS)
			user_input.mousePressed[button] = true;
		else if(action == GLFW_RELEASE)
			user_input.mousePressed[button] = false;
	}
}

void GLFWKeyBoardCallback(GLFWwindow * p_window, int key, int scancode, int action, int mods) {
	auto& imgui_io = ImGui::GetIO();
	if (action == GLFW_PRESS)
		imgui_io.KeysDown[key] = true;
	if (action == GLFW_RELEASE)
		imgui_io.KeysDown[key] = false;

	imgui_io.KeyCtrl = imgui_io.KeysDown[GLFW_KEY_LEFT_CONTROL] || imgui_io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	imgui_io.KeyShift = imgui_io.KeysDown[GLFW_KEY_LEFT_SHIFT] || imgui_io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	imgui_io.KeyAlt = imgui_io.KeysDown[GLFW_KEY_LEFT_ALT] || imgui_io.KeysDown[GLFW_KEY_RIGHT_ALT];
	imgui_io.KeySuper = imgui_io.KeysDown[GLFW_KEY_LEFT_SUPER] || imgui_io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

void GLFWCharCallback(GLFWwindow *, unsigned int c) {
	auto& imgui_io = ImGui::GetIO();
	if (c > 0 && c < 0x10000)
		imgui_io.AddInputCharacter((unsigned short)c);
}

void GLFWScrollCallback(GLFWwindow*, double x_offset, double y_offset) {
	auto& user_input = Valkyrie::getGlobalValkyriePtr()->userInput;
	user_input.mouseWheel += (float)y_offset;
}