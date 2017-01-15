#ifndef _VULKAN_DEPTH_BUFFER_H
#define _VULKAN_DEPTH_BUFFER_H
#include "common.h"
#include "valkyrie/vulkan/attachment.h"

namespace Vulkan{
	struct Device;
	struct PhysicalDevice;
	struct CommandBuffer;

	struct DepthBuffer : public Attachment {
		DepthBuffer();
		virtual ~DepthBuffer();

		VkResult initializeImages();
		virtual VkAttachmentDescription getAttachmentDescription() const;

		VkFormat format = VK_FORMAT_UNDEFINED;
		VkImage image = NULL;
		VkDeviceMemory memory = NULL;
		VkImageView view = NULL;
	};
}

#endif