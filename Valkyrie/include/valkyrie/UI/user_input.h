#ifndef _VALKYRIE_USER_INPUT_H 
#define _VALKYRIE_USER_INPUT_H  
#include "common.h"

struct UserInput {
	bool mousePressed[3] = { false, false, false };
	float mouseWheel = 0.0f;
};

//void GLFWMouseButtonCallback(GLFWwindow* p_window, int button, int action, int mods);
//void GLFWKeyBoardCallback(GLFWwindow* p_window, int key, int scancode, int action, int mods);
//void GLFWCharCallback(GLFWwindow* p_window, unsigned int c);
//void GLFWScrollCallback(GLFWwindow*, double x_offset, double y_offset);
#endif