#include "valkyrie/vulkan/physical_device.h"
#include "valkyrie/vulkan/device.h"
#include "valkyrie/vulkan/queue.h"
#include "utility.h"
using namespace Vulkan;

bool Vulkan::GetQueue(const Device& device, const PhysicalDevice& physical_device, VkQueueFlags flag, Queue& queue) {
	const uint32_t number_of_queues = physical_device.queueFamilyProperties.size();

	for (uint32_t i = 0; i < number_of_queues; ++i) {
		uint32_t queue_support = physical_device.queueFamilyProperties[i].queueFlags & flag;
		if (queue_support != 0) {
			queue.index = i;
			vkGetDeviceQueue(device.handle, queue.index, 0, &queue.handle);
			return queue.handle != VK_NULL_HANDLE;
		}
		
		/*
		if (queue_support != 0) {
			if (flag == VK_QUEUE_GRAPHICS_BIT) {
				VkBool32 support;
				VkSurfaceKHR surface = NULL;
				vkGetPhysicalDeviceSurfaceSupportKHR(physical_device.handle, i, surface, &support);
				assert(support == VK_TRUE);
			}
			queue.index = i;
			vkGetDeviceQueue(device.handle, queue.index, 0, &queue.handle);
			return queue.handle != VK_NULL_HANDLE;
		}
		else
			continue;
		*/
		
	}
	return false;
}

VkResult Queue::waitIdle() {
	return vkQueueWaitIdle(handle);
}