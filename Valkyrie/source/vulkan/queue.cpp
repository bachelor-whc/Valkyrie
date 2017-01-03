#include "valkyrie/vulkan/physical_device.h"
#include "valkyrie/vulkan/device.h"
#include "valkyrie/vulkan/queue.h"
#include "common.h"
using namespace Vulkan;

bool Vulkan::GetQueue(VkQueueFlags flag, Queue& queue) {
	const uint32_t number_of_queues = PhysicalDevice::queueFamilyProperties.size();

	for (uint32_t i = 0; i < number_of_queues; ++i) {
		uint32_t queue_support = PhysicalDevice::queueFamilyProperties[i].queueFlags & flag;
		if (queue_support != 0) {
			queue.index = i;
			vkGetDeviceQueue(g_device_handle, queue.index, 0, &queue.handle);
			return queue.handle != VK_NULL_HANDLE;
		}
	}
	return false;
}

VkResult Queue::waitIdle() {
	return vkQueueWaitIdle(handle);
}