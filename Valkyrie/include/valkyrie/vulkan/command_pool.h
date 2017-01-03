#ifndef _VULKAN_COMMAND_POOL_H 
#define _VULKAN_COMMAND_POOL_H 
#include "valkyrie/vulkan/command_buffer.h"
#include "common.h"

namespace Vulkan {
	struct Device;
	struct Queue;
	typedef std::vector<VkCommandBuffer> SecondaryCommandBuffers;

	struct CommandPool {
		CommandPool() = delete;
		CommandPool(const Vulkan::Queue& queue);
		virtual ~CommandPool();

		Vulkan::CommandBuffer createCommandBuffer();
		Vulkan::SecondaryCommandBuffers createSecondaryCommandBuffers(const uint32_t count);

		VkCommandPool handle = VK_NULL_HANDLE;
	};

	void DestroyCommandPool(CommandPool& command_pool);

	typedef std::shared_ptr<CommandPool> CommandPoolPtr;
}

#endif