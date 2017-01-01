#ifndef _VULKAN_TOOL_H
#define _VULKAN_TOOL_H
#include "common.h"

namespace Vulkan {
	void setImageLayout(Vulkan::CommandBuffer& command_buffer, VkImage image, VkImageAspectFlags aspect_mask, VkImageLayout old_image_layout, VkImageLayout new_image_layout);
}

#endif