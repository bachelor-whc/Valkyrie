#ifndef _VULKAN_SWAPCHAIN_H
#define _VULKAN_SWAPCHAIN_H
#include "utility.h"
#include "wendy.h"

namespace Vulkan {
	struct Device;
	struct PhysicalDevice;
	struct Surface;
	struct CommandBuffer;
	struct RenderPass;
	struct Queue;
	
	struct SwapChainBuffer {
		VkImage image = NULL;
		VkImageView view = NULL;
	};

	struct Framebuffers {
		Framebuffers() = delete;
		Framebuffers(const uint32_t count, const std::vector<SwapChainBuffer>& buffers);
		~Framebuffers();
		void initialize(const Device& device, const RenderPass& render_pass, const int width, const int height);
		
		std::vector<VkFramebuffer> handles;
		std::vector<VkImageView> extendedAttachments;

	private:
		uint32_t m_count;
		std::vector<VkImageView> m_swap_chain_views;
	};

	class SwapChain {
	public:
		SwapChain() = delete;
		SwapChain(const Device& device, const PhysicalDevice& physical_device, const Surface& surface, const Wendy::Window& window);
		virtual ~SwapChain();

		VkResult initializeImages(const Device& device, const Surface& surface, CommandBuffer& buffer);
		void initializeFramebuffers(const Device& device, const RenderPass& render_pass, const VkImageView* extended_attachments, int count);

		VkSwapchainKHR handle = NULL;

		inline uint32_t getImageCount() const { return m_buffers.size(); }
		inline uint32_t getCurrent() const { return m_current_buffer; }
		inline Framebuffers* getFramebuffers() const { return mp_framebuffers; }
		inline VkImage getCurrentImage() const { return m_buffers[m_current_buffer].image; }
		inline VkImage getImage(int index) const { return m_buffers[index].image; }
		VkResult acquireNextImage(const Device& device, uint64_t timeout, const VkSemaphore semaphore, const VkFence fence);
		VkResult queuePresent(const Queue& queue);

	private:
		std::vector<SwapChainBuffer> m_buffers;
		Framebuffers* mp_framebuffers;
		uint32_t m_current_buffer;
		int m_width;
		int m_height;
		bool m_images_initialized;
	};

	void DestroySwapChain(const Device& device, SwapChain& swapchain);
	void DestroyFramebuffers(const Device& device, Framebuffers& framebuffers);
}

#endif