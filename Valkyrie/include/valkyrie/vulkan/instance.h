#ifndef _VULKAN_INSTANCE_H 
#define _VULKAN_INSTANCE_H
#include "valkyrie/common.h"

namespace Vulkan {

	struct Instance {
		VkInstance handle = NULL;
	};

	VkResult CreateInstance(const char* application_name, Instance& instance);
	void DestroyInstance(Instance& instance);
}

#endif