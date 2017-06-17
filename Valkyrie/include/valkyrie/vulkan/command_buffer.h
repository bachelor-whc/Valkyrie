#ifndef _VULKAN_COMMAND_BUFFER_H
#define _VULKAN_COMMAND_BUFFER_H
#include "valkyrie/common.h"

namespace Vulkan {
	struct Device;
	struct Queue;

	struct CommandBuffer {
		CommandBuffer() {}
		virtual ~CommandBuffer() {}
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

	struct InheritanceCommandBuffer : public CommandBuffer {
		InheritanceCommandBuffer() = delete;
		InheritanceCommandBuffer(VkCommandBuffer src, VkCommandBufferInheritanceInfo src_inheritance) : 
			CommandBuffer(),
			inheritance(src_inheritance) {
			handle = src;
			beginInformation.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
			beginInformation.pInheritanceInfo = &inheritance;
		}
		virtual ~InheritanceCommandBuffer() {

		}
		VkCommandBufferInheritanceInfo inheritance;
	};
}

#endif