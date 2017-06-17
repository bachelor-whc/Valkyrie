#ifndef _VULKAN_DEPTH_BUFFER_H
#define _VULKAN_DEPTH_BUFFER_H
#include "valkyrie/common.h"
#include "valkyrie/vulkan/image.h"
#include "valkyrie/vulkan/attachment.h"

namespace Vulkan{
	struct Device;
	struct PhysicalDevice;
	struct CommandBuffer;

	struct DepthBuffer : 
		public Image,
		public Attachment {
		DepthBuffer() = delete;
		DepthBuffer(uint32_t width, uint32_t height);
		virtual ~DepthBuffer();

		virtual VkAttachmentDescription getAttachmentDescription() const;
		virtual VkImageCreateInfo getImageCreate() const;
		virtual VkImageViewCreateInfo getImageViewCreate() const;
		virtual VkFlags getMemoryType() const;

	private:
		uint32_t m_width;
		uint32_t m_height;
	};
}

#endif