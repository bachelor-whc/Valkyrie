#ifndef _DEFAULT_CREATE_INFO_H
#define _DEFAULT_CREATE_INFO_H
#include <vulkan/vulkan.h>

const VkBufferCreateInfo VK_DEFAULT_BUFFER_CREATE_INFO = {
	VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
	nullptr,
	NULL,
	NULL,
	NULL,
	VK_SHARING_MODE_EXCLUSIVE,
	NULL,
	nullptr
};

#endif