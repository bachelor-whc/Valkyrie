#ifndef _VALKYRIE_THREAD_H 
#define _VALKYRIE_THREAD_H 
#include "valkyrie/vulkan/command_buffer.h"
#include "utility.h"

namespace Vulkan {
	struct Device;
	struct Queue;
	typedef std::vector<VkCommandBuffer> SecondaryCommandBuffers;
}

struct ValkyrieThread {
	ValkyrieThread() = delete;
	ValkyrieThread(const Vulkan::Device& device, const Vulkan::Queue& queue);
	virtual ~ValkyrieThread();

	Vulkan::CommandBuffer createCommandBuffer();
	Vulkan::SecondaryCommandBuffers ValkyrieThread::createSecondaryCommandBuffer(const uint32_t count);

private:
	VkDevice m_device = VK_NULL_HANDLE;
	VkCommandPool m_command_pool = VK_NULL_HANDLE;
	VkCommandBufferLevel m_level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
};

#endif