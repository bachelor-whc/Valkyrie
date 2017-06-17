#ifndef _VALKYRIE_RENDER_CONTEXT_H
#define _VALKYRIE_RENDER_CONTEXT_H
#include <vulkan/vulkan.h>
#include <tbb/task_group.h>
#include "valkyrie/vulkan/command_buffer.h"
#include "valkyrie/vulkan/surface.h"
#include "valkyrie/vulkan/swapchain.h"
#include "valkyrie/vulkan/depth_buffer.h"
#include "valkyrie/vulkan/swapchain.h"
#include "valkyrie/vulkan/render_pass.h"
#include "valkyrie/UI/window.h"

namespace Valkyrie {
	class Renderer {
	public:
		Renderer() = delete;
		Renderer(const WindowPtr& window_ptr);
		virtual ~Renderer();

		void prepareFrame();
		virtual VkResult render();
		inline VkRenderPass getRenderPassHandle() const { return m_render_pass.handle; }
		inline VkFramebuffer getFramebuffer(int index) const { return mp_swapchain->getFramebuffers()->handles[index]; }
		inline VkImage getSwapChainImage(int index) const { return mp_swapchain->getImage(index); }
		inline int getCurrentBuffer() const { return mp_swapchain->getCurrent(); }
		VkRenderPassBeginInfo getRenderPassBegin() const;
		std::vector<Vulkan::CommandBuffer> renderCommands;

		void commandSetViewport(const Vulkan::CommandBuffer& command_buffer);
		void commandSetScissor(const Vulkan::CommandBuffer& command_buffer);

	private:
		void initializeSwapChain();
		void initializeDepthBuffer();
		void initializeRenderPass();
		void initializeFramebuffers();
		void initializeSurface();
		
		WindowPtr m_window_ptr;
		Vulkan::CommandBuffer m_present_command_buffer;
		Vulkan::Surface m_surface;
		Vulkan::SwapChain* mp_swapchain;
		Vulkan::DepthBuffer* mp_depth_buffer;
		Vulkan::RenderPass m_render_pass;
		VkViewport m_viewport = {};
		VkRect2D m_scissor = {};
		VkSemaphore m_present_semaphore = VK_NULL_HANDLE;
		VkSemaphore m_render_semaphore = VK_NULL_HANDLE;
		VkFence m_fence = VK_NULL_HANDLE;
		VkSubmitInfo m_submit = {};
		VkPipelineStageFlags m_submit_pipeline_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	};

	using RendererPtr = std::shared_ptr<Renderer>;
}

#endif