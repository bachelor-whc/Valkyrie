#ifndef _VULKAN_COMMAND_BUFFER_H
#define _VULKAN_COMMAND_BUFFER_H
#include "common.h"

namespace Vulkan {
	struct Device;
	struct Queue;

	struct CommandBuffer {
		VkCommandBuffer handle = VK_NULL_HANDLE;
		VkCommandBufferBeginInfo beginInformation = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			NULL,
			nullptr
		};
		VkResult begin();
		VkResult end();
		VkResult submit(const Queue& queue);
	};
}

#endif