#include <imgui.h>
#include "valkyrie.h"
#include "valkyrie/vulkan/debug.h"
#include "valkyrie/UI/user_input.h"
#include "valkyrie/UI/window.h"
#include "valkyrie/UI/window_manager.h"
#include "valkyrie/utility/sdl_manager.h"
#include "valkyrie/utility/vulkan_manager.h"
#include "valkyrie/renderer.h"
#include "valkyrie/utility/task_manager.h"
#include "valkyrie/scene/object_manager.h"

ValkyrieEngine* ValkyrieEngine::gp_valkyrie = nullptr;
bool ValkyrieEngine::SDLInitialized = false;
VkInstance g_instance_handle = VK_NULL_HANDLE;
VkDevice g_device_handle = VK_NULL_HANDLE;
VkPhysicalDevice g_physical_device_handle = VK_NULL_HANDLE;

int ValkyrieEngine::initializeValkyrieEngine() {
	if (gp_valkyrie != nullptr)
		return 0;
	gp_valkyrie = NEW_NT ValkyrieEngine("Valkyrie");
	if (gp_valkyrie == nullptr)
		return 1;
	int result_am = Valkyrie::AssetManager::initialize();
	int result_sm = Valkyrie::SDLManager::initialize();
	int result_wm = Valkyrie::WindowManager::initialize();
	int result_vm = Valkyrie::VulkanManager::initialize();
	int result_tm = Valkyrie::TaskManager::initialize();
	int result_om = Valkyrie::ObjectManager::initialize();
	if (result_am != 0)
		return 2;
	if (result_sm != 0)
		return 3;
	if (result_wm != 0)
		return 4;
	if (result_vm != 0)
		return 5;
	if (result_tm != 0)
		return 6;
	if (result_om != 0)
		return 7;
	const int width = 1024;
	const int height = 768;
	std::string title("Playground");
	ValkyrieEngine::initializeValkyrieEngine();
	auto& window_manager = Valkyrie::WindowManager::instance();
	window_manager.createMainWindow(title, width, height);
	auto& main_window_ptr = window_manager.getMainWindowPtr();
	gp_valkyrie->m_renderer_ptr = MAKE_SHARED(Valkyrie::Renderer)(main_window_ptr);
	gp_valkyrie->initialize();
	return 0;
}

void ValkyrieEngine::closeValkyrieEngine() {
	if (gp_valkyrie != nullptr)
		delete gp_valkyrie;
	gp_valkyrie = nullptr;
	Valkyrie::ObjectManager::close();
	Valkyrie::TaskManager::close();
	Valkyrie::VulkanManager::close();
	Valkyrie::WindowManager::close();
	Valkyrie::SDLManager::close();
	Valkyrie::AssetManager::close();
}

Valkyrie::RendererPtr ValkyrieEngine::getRenderContextPtr() {
	return m_renderer_ptr;
}

ValkyrieEngine::ValkyrieEngine(std::string application_name) :
	m_application_name(application_name),
	m_renderer_ptr() {
	
}

ValkyrieEngine::~ValkyrieEngine() {
	
}

void ValkyrieEngine::initializeImGuiInput() {
	auto& imgui_io = ImGui::GetIO();
	imgui_io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
	imgui_io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
	imgui_io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
	imgui_io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
	imgui_io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
	imgui_io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
	imgui_io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
	imgui_io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
	imgui_io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
	imgui_io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
	imgui_io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
	imgui_io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
	imgui_io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
	imgui_io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
	imgui_io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
	imgui_io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
	imgui_io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
	imgui_io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
	imgui_io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;
}

void ValkyrieEngine::updateUserInput(const SDL_Event& s_event) {
	auto& imgui_io = ImGui::GetIO();
	if (s_event.type == SDL_MOUSEMOTION) {
		imgui_io.MousePos.x = s_event.motion.x;
		imgui_io.MousePos.y = s_event.motion.y;
	}
	memcpy(imgui_io.MouseDown, userInput.mousePressed, 3);
	imgui_io.MouseWheel = userInput.mouseWheel;
	userInput.mouseWheel = 0.0f;
}

void ValkyrieEngine::updateTime() {
	double time = (double)SDL_GetTicks() / 1000.0;
	m_current_timestamp = time;
	m_deltatime = m_current_timestamp - m_previous_timestamp;
	m_previous_timestamp = m_current_timestamp;
	auto& imgui_io = ImGui::GetIO();
	imgui_io.DeltaTime = (float)m_deltatime;
}

void ValkyrieEngine::updateFPS() {
	m_FPS = 1.0f / ImGui::GetIO().DeltaTime;
}

void ValkyrieEngine::updateFPSStatus() {
	Valkyrie::WindowManager::instance().getMainWindowPtr()->appendWindowTitle(
		std::to_string(m_FPS)
	);
}

bool ValkyrieEngine::execute() {
	static SDL_Event s_event;
	while (SDL_PollEvent(&s_event)) {
		switch (s_event.type) {
		case SDL_QUIT:
			return false;
		case SDL_TEXTINPUT:
			userInput.handleSDLCharEvent(s_event);
			break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			userInput.handleSDLKeyBoardEvent(s_event);
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			userInput.handleSDLMouseButtonEvent(s_event);
			break;
		case SDL_MOUSEWHEEL:
			userInput.handleSDLScrollEvent(s_event);
			break;
		}
		updateUserInput(s_event);
	}
	updateTime();
	updateFPS();
	updateFPSStatus();
	m_renderer_ptr->render();
	return true;
}

VkResult ValkyrieEngine::initialize() {
	VkResult result;

	SDL_StartTextInput();
	initializeImGuiInput();

	return VK_SUCCESS;
}