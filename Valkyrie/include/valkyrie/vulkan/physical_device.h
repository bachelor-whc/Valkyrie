#ifndef _VULKAN_PHYSICAL_DEVICE_H 
#define _VULKAN_PHYSICAL_DEVICE_H
#include "utility.h"

namespace Vulkan {
	struct Instance;

	struct PhysicalDevice {
		VkPhysicalDevice handle = NULL;
		VkPhysicalDeviceProperties properties = {};
		VkPhysicalDeviceMemoryProperties memoryProperties = {};
		std::vector<VkQueueFamilyProperties> queueFamilyProperties;

		bool setMemoryType(uint32_t type_bits, VkFlags requirements_mask, uint32_t& type_index) const;
	};

	VkResult CreatePhysicalDevice(Instance& instance, PhysicalDevice& physical_device);
}

#endif