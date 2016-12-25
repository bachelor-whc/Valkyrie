#include "valkyrie/thread.h"
#include "valkyrie/vulkan/device.h"
#include "valkyrie/vulkan/queue.h"

ValkyrieThread::ValkyrieThread(const Vulkan::Queue& queue) {
	VkResult result;
	VkCommandPoolCreateInfo command_pool_create = {};
	command_pool_create.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	command_pool_create.queueFamilyIndex = queue.index;
	command_pool_create.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	m_device = g_device_handle;
	result = vkCreateCommandPool(g_device_handle, &command_pool_create, nullptr, &m_command_pool);
	assert(result == VK_SUCCESS);
}

ValkyrieThread::~ValkyrieThread() {
	vkDestroyCommandPool(m_device, m_command_pool, nullptr);
}

Vulkan::CommandBuffer ValkyrieThread::createCommandBuffer() {
	Vulkan::CommandBuffer command_buffer;
	VkCommandBufferAllocateInfo command_buffer_allocate = {};
	command_buffer_allocate.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate.commandPool = m_command_pool;
	command_buffer_allocate.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_allocate.commandBufferCount = 1;
	VkResult result = vkAllocateCommandBuffers(m_device, &command_buffer_allocate, &command_buffer.handle);
	assert(result == VK_SUCCESS);
	return command_buffer;
}

Vulkan::SecondaryCommandBuffers ValkyrieThread::createSecondaryCommandBuffers(const uint32_t count) {
	Vulkan::SecondaryCommandBuffers command_buffers;
	VkCommandBufferAllocateInfo command_buffer_allocate = {};
	command_buffers.resize(count);
	command_buffer_allocate.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate.commandPool = m_command_pool;
	command_buffer_allocate.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	command_buffer_allocate.commandBufferCount = count;
	VkResult result = vkAllocateCommandBuffers(m_device, &command_buffer_allocate, command_buffers.data());
	assert(result == VK_SUCCESS);
	return command_buffers;
}