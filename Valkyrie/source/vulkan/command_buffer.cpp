#include "valkyrie/vulkan/device.h"
#include "valkyrie/vulkan/queue.h"
#include "valkyrie/vulkan/command_buffer.h"
using namespace Vulkan;

VkResult CommandBuffer::begin() {
	return vkBeginCommandBuffer(handle, &beginInformation);
}

VkResult CommandBuffer::end() {
	return vkEndCommandBuffer(handle);
}

VkResult CommandBuffer::submit(const Queue& queue) {
	VkSubmitInfo submit = {};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &handle;
	return vkQueueSubmit(queue.handle, 1, &submit, VK_NULL_HANDLE);
}