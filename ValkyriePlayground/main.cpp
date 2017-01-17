#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <valkyrie.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <imgui.h>
#include <stb_image.h>

using namespace Valkyrie;
int CALLBACK WinMain(HINSTANCE instance_handle, HINSTANCE, LPSTR command_line, int command_show) {
	const int width = 1024;
	const int height = 768;

	std::string title("Playground");
	ValkyrieEngine::initializeValkyrieEngine();
	auto& window_manager = *Valkyrie::WindowManager::getGlobalWindowManagerPtr();
	window_manager.createMainWindow(title, width, height);
	auto& valkyrie = *ValkyrieEngine::getGlobalValkyriePtr();
	valkyrie.initialize();
	
	auto& main_window_ptr = window_manager.getMainWindowPtr();
	RenderContextPtr render_context_ptr = MAKE_SHARED(RenderContext)(main_window_ptr);

	while (valkyrie.execute()) {

	}
	render_context_ptr.reset();
	ValkyrieEngine::closeValkyrieEngine();
	return 0;
}
