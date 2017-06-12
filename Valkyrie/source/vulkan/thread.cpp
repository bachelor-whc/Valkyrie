#include "valkyrie/vulkan/thread.h"
#include "valkyrie/utility/vulkan_manager.h"
#include "valkyrie/vulkan/command_pool.h"

using namespace Vulkan;

ThreadCommandPool::ThreadCommandPool(const Queue& queue) : CommandPool(queue) {
	
}

ThreadCommandPool::~ThreadCommandPool() {

}

void ThreadCommandPool::initializeSecondaryCommandBuffers(const uint32_t count) {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	VkCommandBufferAllocateInfo command_buffer_allocate = {};
	commands.resize(count);
	command_buffer_allocate.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate.commandPool = handle;
	command_buffer_allocate.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	command_buffer_allocate.commandBufferCount = count;
	VkResult result = vkAllocateCommandBuffers(device, &command_buffer_allocate, commands.data());
	assert(result == VK_SUCCESS);
}
