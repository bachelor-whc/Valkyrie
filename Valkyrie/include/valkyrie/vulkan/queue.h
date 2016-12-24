#ifndef _VULKAN_QUEUE_H 
#define _VULKAN_QUEUE_H 
#include "utility.h"

namespace Vulkan {
	struct PhysicalDevice;

	struct Queue {
		uint32_t index = -1;
		VkQueue handle = NULL;
		VkResult waitIdle();
	};

	bool GetQueue(VkQueueFlags flag, Queue& queue);
}

#endif