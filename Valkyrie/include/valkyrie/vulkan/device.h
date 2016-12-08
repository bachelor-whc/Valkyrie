#ifndef _VULKAN_DEVICE_H 
#define _VULKAN_DEVICE_H 
#include "utility.h"

namespace Vulkan {
	struct PhysicalDevice;

	struct Device {
		VkDevice handle = NULL;
		VkCommandPool commandPool = NULL;
	};

	VkResult CreateDevice(const PhysicalDevice& physical_device, Device& device);
	void DestroyDevice(Device& device);
}

#endif