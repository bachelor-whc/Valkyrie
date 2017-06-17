#ifndef _VALKYRIE_H 
#define _VALKYRIE_H
#define GLFW_INCLUDE_VULKAN
#include "common.h"
#include "valkyrie/vulkan/texture.h"
#include "valkyrie/UI/user_input.h"
#include "valkyrie/asset/lavy_loader.h"
#include "valkyrie/asset/asset_manager.h"
#include "valkyrie/asset/mesh_asset.h"
#include "valkyrie/component/component_attacher.h"
#include "valkyrie/component/mesh_renderer.h"
#include "valkyrie/component/camera.h"
#include "valkyrie/scene/object.h"
#include "valkyrie/graphics/renderer.h"
#include "valkyrie/utility/vulkan_manager.h"
#include "valkyrie/scene/object_manager.h"
#include "valkyrie/factory/object.h"
#include "valkyrie/factory/mesh.h"
#include "valkyrie/graphics/pipeline.h"
#include "valkyrie/vulkan/thread.h"
#include "valkyrie/utility/task_manager.h"

class ValkyrieEngine {
public:
	enum State {
		UNINITIALIZED, INITIALIZING, INITIALIZED, EXECUTE, EXIT
	};
	static int initializeValkyrieEngine();
	static void closeValkyrieEngine();
	static ValkyrieEngine& instance() { return *gp_valkyrie; }
	static bool SDLInitialized;
	
public:
	virtual ~ValkyrieEngine();

	virtual bool execute();

	inline double getFPS() { return (1 / m_deltatime); }

	std::vector<Vulkan::MemoryBuffer> memoryBuffers;
	UserInput userInput = {};
	Valkyrie::Renderer& getRenderer();
	
private:
	ValkyrieEngine() = delete;
	ValkyrieEngine(std::string application_name);
	static ValkyrieEngine* gp_valkyrie;

private:
	virtual VkResult initialize();
	void initializeImGuiInput();
	void updateUserInput(const SDL_Event& s_event);
	void updateTime();
	void updateFPS();
	void updateFPSStatus();

private:
	Valkyrie::RendererPtr m_renderer_ptr;
	std::string m_application_name;
	double m_previous_timestamp = 0.0;
	double m_current_timestamp = 0.0;
	double m_deltatime = 0.0;
	float m_FPS = 0.0;
};

#endif