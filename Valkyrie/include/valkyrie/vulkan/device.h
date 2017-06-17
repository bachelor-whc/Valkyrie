#ifndef _VULKAN_DEVICE_H 
#define _VULKAN_DEVICE_H 
#include "valkyrie/common.h"

namespace Vulkan {
	struct PhysicalDevice;

	struct Device {
		VkDevice handle = VK_NULL_HANDLE;
	};

	VkResult CreateDevice(Device& device);
	void DestroyDevice(Device& device);
}

#endif