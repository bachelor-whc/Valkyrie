#ifndef _VALKYRIE_H 
#define _VALKYRIE_H
#define GLFW_INCLUDE_VULKAN
#include "common.h"
#include "valkyrie/vulkan/instance.h"
#include "valkyrie/vulkan/physical_device.h"
#include "valkyrie/vulkan/device.h"
#include "valkyrie/vulkan/queue.h"
#include "valkyrie/vulkan/surface.h"
#include "valkyrie/vulkan/swapchain.h"
#include "valkyrie/vulkan/render_pass.h"
#include "valkyrie/vulkan/depth_buffer.h"
#include "valkyrie/vulkan/command_buffer.h"
#include "valkyrie/vulkan/pipeline.h"
#include "valkyrie/vulkan/vertex_input.h"
#include "valkyrie/vulkan/memory_buffer.h"
#include "valkyrie/vulkan/descriptor.h"
#include "valkyrie/vulkan/shader.h"
#include "valkyrie/vulkan/command_pool.h"
#include "valkyrie/thread/thread.h"
#include "valkyrie/image.h"
#include "valkyrie/vulkan/texture.h"
#include "valkyrie/render_function.h"
#include "valkyrie/UI/user_input.h"
#include "valkyrie/asset/lavy_loader.h"
#include "valkyrie/memory_chunk.h"
#include "valkyrie/thread/thread_manager.h"
#include "valkyrie/asset/asset_manager.h"
#include "valkyrie/asset/mesh_asset.h"
#include "valkyrie/component/mesh_renderer.h"
#include "valkyrie/scene/object.h"

class ValkyrieEngine {
public:
	enum State {
		UNINITIALIZED, INITIALIZING, INITIALIZED, EXECUTE, EXIT
	};
	static int initializeValkyrieEngine();
	static void closeValkyrieEngine();
	static ValkyrieEngine* getGlobalValkyriePtr() { return gp_valkyrie; }
	static bool SDLInitialized;
	
public:
	virtual ~ValkyrieEngine();

	virtual bool execute();
	virtual VkResult initialize();
	virtual VkResult render();

	void initializeWindow(int width, int height, const std::string& title);
	void initializePipelineLayout(const std::string& pipeline_name);
	void initializeDescriptorSetLayouts();
	void createPipelineModule(const std::string& pipename_name);
	void initializeShaderModules();
	void initializePipeline(const std::string& pipename_name);
	void initializeDescriptorPool();
	void initializeDescriptorSets();
	void writeSets(const std::vector<VkWriteDescriptorSet>& writes);
	void commandSetViewport(const Vulkan::CommandBuffer& command_buffer);
	void commandSetScissor(const Vulkan::CommandBuffer& command_buffer);
	void initailizeTexture(Vulkan::Texture& texture);
	bool registerRenderFunction(std::string name, Valkyrie::RenderPFN pfn);
	void executeRenderFunction(std::string name, const std::vector<void*>& data);
	inline int getCurrentBuffer() { return mp_swapchain->getCurrent(); }
	Vulkan::CommandBuffer createCommandBuffer();
	Vulkan::SecondaryCommandBuffers createSecondaryCommandBuffers(uint32_t count);

	inline VkRenderPass getRenderPassHandle() { return m_render_pass.handle; }
	inline VkFramebuffer getFramebuffer(int index) { return mp_swapchain->getFramebuffers()->handles[index]; }
	inline VkImage getSwapChainImage(int index) { return mp_swapchain->getImage(index); }
	inline Vulkan::Queue& getGraphicsQueue() { return m_graphics_queue; }

	inline double getFPS() { return (1 / m_deltatime); }

	std::vector<Vulkan::CommandBuffer> renderCommands;
	std::map<std::string, Vulkan::VertexInputPointer> vertexInputs;
	std::vector<Vulkan::MemoryBuffer> memoryBuffers;
	std::map<std::string, Vulkan::PipelinePointer> pipelines;
	std::map<std::string, Vulkan::ShaderPointer> shaders;
	Vulkan::DescriptorPool descriptorPool;
	UserInput userInput = {};
	
private:
	ValkyrieEngine() = delete;
	ValkyrieEngine(std::string application_name);
	static ValkyrieEngine* gp_valkyrie;

private:
	void initializeInstance();
	void initializePhysicalDevice();
	void initializeDevice();
	void initializeSurface();
	void initializeThreads();
	void initializeSwapChain(Vulkan::CommandBuffer& command_bufer);
	void initializeDepthBuffer(Vulkan::CommandBuffer& command_bufer);
	void initializeRenderPass();
	void initializeFramebuffers();
	void initializePipelineCache();
	void initializeImGuiInput();
	
	void updateUserInput(const SDL_Event& s_event);
	void updateTime();

private:
	Vulkan::Instance m_instatnce;
	Vulkan::PhysicalDevice m_physical_device;
	Vulkan::Device m_device;
	Vulkan::Queue m_graphics_queue;
	Vulkan::Surface m_surface;
	Vulkan::SwapChain* mp_swapchain;
	Vulkan::DepthBuffer* mp_depth_buffer;
	Vulkan::RenderPass m_render_pass;
	Vulkan::CommandBuffer m_setup_command_buffer;
	Vulkan::CommandBuffer m_present_command_buffer;
	Vulkan::CommandPoolPtr m_command_pool_ptr;
	VkViewport m_viewport = {};
	VkRect2D m_scissor = {};
	std::map<std::string, Valkyrie::RenderPFN> m_render_pfns;

	VkFence m_present_fence = VK_NULL_HANDLE;
	VkSemaphore m_present_semaphore = VK_NULL_HANDLE;

private:
	std::string m_application_name;
	SDL_Window* mp_window;
	double m_previous_timestamp = 0.0;
	double m_current_timestamp = 0.0;
	double m_deltatime = 0.0;
};

#endif