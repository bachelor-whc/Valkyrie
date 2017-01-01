#ifndef _VULKAN_COMMAND_BUFFER_H
#define _VULKAN_COMMAND_BUFFER_H
#include "common.h"

namespace Vulkan {
	struct Device;
	struct Queue;

	struct CommandBuffer {
		VkCommandBuffer handle = NULL;
		VkResult begin();
		VkResult end();
		VkResult submit(const Queue& queue);
	};
}

#endif