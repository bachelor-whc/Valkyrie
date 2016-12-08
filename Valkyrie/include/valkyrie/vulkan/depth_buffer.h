#ifndef _VULKAN_DEPTH_BUFFER_H
#define _VULKAN_DEPTH_BUFFER_H
#include "utility.h"
#include "wendy.h"
#include "valkyrie/vulkan/attachment.h"

namespace Vulkan{
	struct Device;
	struct PhysicalDevice;
	struct CommandBuffer;

	struct DepthBuffer : public Attachment {
		DepthBuffer() = delete;
		DepthBuffer(const PhysicalDevice& physical_device);
		virtual ~DepthBuffer();

		VkResult initializeImages(const Device& device, const PhysicalDevice& physical_device, CommandBuffer& buffer, const Wendy::Window& window);
		virtual VkAttachmentDescription getAttachmentDescription() const;

		VkFormat format = VK_FORMAT_UNDEFINED;
		VkImage image = NULL;
		VkDeviceMemory memory = NULL;
		VkImageView view = NULL;
	};
}

#endif