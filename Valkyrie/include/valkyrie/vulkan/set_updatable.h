#ifndef _VULKAN_SET_UPDATABLE_H
#define _VULKAN_SET_UPDATABLE_H
#include <vulkan/vulkan.h>

namespace Vulkan {
	struct SetUpdable {
		virtual VkWriteDescriptorSet getWriteSet() = 0;
	};
}

#endif