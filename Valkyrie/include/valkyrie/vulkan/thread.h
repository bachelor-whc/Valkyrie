#ifndef _VULKAN_THREAD_H 
#define _VULKAN_THREAD_H 
#include "common.h"
#include "command_pool.h"
#include "queue.h"

namespace Vulkan {
	class ThreadCommandPool : public CommandPool {
	public:
		ThreadCommandPool() = delete;
		ThreadCommandPool(const Queue& queue);
		virtual ~ThreadCommandPool();

		void initializeSecondaryCommandBuffers(const uint32_t count);
		std::vector<VkCommandBuffer> commands = {};
	};

	typedef std::shared_ptr<ThreadCommandPool> ThreadCommandPoolPtr;
}

#endif