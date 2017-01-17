#ifndef _VULKAN_SWAPCHAIN_H
#define _VULKAN_SWAPCHAIN_H
#include "valkyrie/vulkan/image.h"
#include "common.h"

namespace Vulkan {
	struct Device;
	struct PhysicalDevice;
	struct Surface;
	struct CommandBuffer;
	struct RenderPass;
	struct Queue;
	
	struct SwapChainBuffer : public Image {
		/// \brief This function is useless.
		/// vkGetSwapchainImagesKHR will create image and allocate memory.
		virtual VkImageCreateInfo getImageCreate() const;
		virtual VkImageViewCreateInfo getImageViewCreate() const;
	};

	struct Framebuffers {
		Framebuffers() = delete;
		Framebuffers(const uint32_t count, const std::vector<SwapChainBuffer>& buffers);
		~Framebuffers();
		void initialize(const RenderPass& render_pass, const int width, const int height);
		
		std::vector<VkFramebuffer> handles;
		std::vector<VkImageView> extendedAttachments;

	private:
		uint32_t m_count;
		std::vector<VkImageView> m_swap_chain_views;
	};

	class SwapChain {
	public:
		SwapChain() = delete;
		SwapChain(const Surface& surface);
		virtual ~SwapChain();

		VkResult initializeImages(const Surface& surface);
		void initializeFramebuffers(const RenderPass& render_pass, const VkImageView* extended_attachments, int count);

		VkSwapchainKHR handle = NULL;

		inline uint32_t getImageCount() const { return (uint32_t)m_buffers.size(); }
		inline uint32_t getCurrent() const { return m_current_buffer; }
		inline Framebuffers* getFramebuffers() const { return mp_framebuffers; }
		inline VkImage getCurrentImage() const { return m_buffers[m_current_buffer].handle; }
		inline VkImage getImage(int index) const { return m_buffers[index].handle; }
		VkResult acquireNextImage(uint64_t timeout, const VkSemaphore semaphore, const VkFence fence);
		VkResult queuePresent(const VkQueue& queue);

	private:
		std::vector<SwapChainBuffer> m_buffers;
		Framebuffers* mp_framebuffers;
		uint32_t m_current_buffer;
		int m_width;
		int m_height;
		bool m_images_initialized;
	};

	void DestroySwapChain(SwapChain& swapchain);
	void DestroyFramebuffers(Framebuffers& framebuffers);
}

#endif