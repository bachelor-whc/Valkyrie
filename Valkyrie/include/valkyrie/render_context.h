#ifndef _VALKYRIE_RENDER_CONTEXT_H
#define _VALKYRIE_RENDER_CONTEXT_H
#include <vulkan/vulkan.h>
#include "valkyrie/vulkan/command_buffer.h"
#include "valkyrie/vulkan/surface.h"
#include "valkyrie/vulkan/swapchain.h"
#include "valkyrie/vulkan/depth_buffer.h"
#include "valkyrie/vulkan/swapchain.h"
#include "valkyrie/vulkan/render_pass.h"
#include "valkyrie/UI/window.h"

namespace Valkyrie {
	class RenderContext {
	public:
		RenderContext() = delete;
		RenderContext(const WindowPtr& window_ptr);
		virtual ~RenderContext();

		virtual VkResult render();
		inline VkRenderPass getRenderPassHandle() { return m_render_pass.handle; }
		inline VkFramebuffer getFramebuffer(int index) { return mp_swapchain->getFramebuffers()->handles[index]; }
		inline VkImage getSwapChainImage(int index) { return mp_swapchain->getImage(index); }
		inline int getCurrentBuffer() { return mp_swapchain->getCurrent(); }
		VkRenderPassBeginInfo getRenderPassBegin();
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
		VkFence m_present_fence = VK_NULL_HANDLE;
		VkSemaphore m_present_semaphore = VK_NULL_HANDLE;
	};

	using RenderContextPtr = std::shared_ptr<RenderContext>;
}

#endif