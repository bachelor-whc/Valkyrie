#ifndef _VALKYRIE_H 
#define _VALKYRIE_H
#include "utility.h"
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
#include "valkyrie/thread.h"
#include "valkyrie/image.h"
#include "valkyrie/vulkan/texture.h"
#include "valkyrie/vulkan/image_texture.h"
#include "valkyrie/vulkan/memory_texture.h"
#include "valkyrie/render_function.h"
#include "wendy.h"

class ValkyrieWindow;

class Valkyrie {
public:
	enum State {
		UNINITIALIZED, INITIALIZING, INITIALIZED, EXECUTE, EXIT
	};
	static Valkyrie* getGlobalValkyriePtr() { return gp_valkyrie; };
	
public:
	Valkyrie() = delete;
	Valkyrie(std::string application_name);
	virtual ~Valkyrie();

	virtual VkResult initialize();
	virtual VkResult render();
	virtual bool execute();

	void setWindowPointer(Wendy::Window* window_ptr);
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
	bool registerRenderFunction(std::string name, ValkyrieRenderPFN pfn);
	void executeRenderFunction(std::string name, const std::vector<void*>& data);
	inline int getCurrentBuffer() { return mp_swapchain->getCurrent(); }
	Vulkan::CommandBuffer createCommandBuffer();
	Vulkan::SecondaryCommandBuffers createSecondaryCommandBuffers(uint32_t count);

	inline VkRenderPass getRenderPassHandle() { return m_render_pass.handle; }
	inline VkFramebuffer getFramebuffer(int index) { return mp_swapchain->getFramebuffers()->handles[index]; }
	inline VkImage getSwapChainImage(int index) { return mp_swapchain->getImage(index); }

	inline Vulkan::Queue& getGraphicsQueue() { return m_graphics_queue; }

	std::vector<Vulkan::CommandBuffer> renderCommands;
	std::map<std::string, Vulkan::VertexInputPointer> vertexInputs;
	std::vector<Vulkan::MemoryBuffer> memoryBuffers;
	std::map<std::string, Vulkan::PipelinePointer> pipelines;
	std::map<std::string, Vulkan::ShaderPointer> shaders;
	Vulkan::DescriptorPool descriptorPool;
	
private:
	static Valkyrie* gp_valkyrie;
	typedef ValkyrieThread* ThreadPointer;

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

private:
	Vulkan::Instance m_instatnce;
	Vulkan::PhysicalDevice m_physical_device;
	Vulkan::Device m_device;
	Vulkan::Queue m_graphics_queue;
	Vulkan::Surface m_surface;
	Vulkan::SwapChain* mp_swapchain;
	Vulkan::DepthBuffer* mp_depth_buffer;
	Vulkan::RenderPass m_render_pass;
	std::vector<ThreadPointer> m_thread_ptrs;
	Vulkan::CommandBuffer m_setup_command_buffer;
	Vulkan::CommandBuffer m_present_command_buffer;
	VkViewport m_viewport = {};
	VkRect2D m_scissor = {};
	std::map<std::string, ValkyrieRenderPFN> m_render_pfns;

private:
	State m_state = UNINITIALIZED;
	std::string m_application_name;
	Wendy::Window* mp_window;

};

#endif