#include "valkyrie/vulkan/instance.h"
#include "valkyrie/vulkan/physical_device.h"
#include "utility.h"
using namespace Vulkan;

VkPhysicalDeviceProperties PhysicalDevice::properties = {};
VkPhysicalDeviceMemoryProperties PhysicalDevice::memoryProperties = {};
std::vector<VkQueueFamilyProperties> PhysicalDevice::queueFamilyProperties = {};

VkResult Vulkan::CreatePhysicalDevice(Instance& instance, PhysicalDevice& physical_device) {
	VkResult result;
	uint32_t number_of_devices = 1;

	std::vector<VkPhysicalDevice> physical_devices;
	result = vkEnumeratePhysicalDevices(instance.handle, &number_of_devices, NULL);
	assert(number_of_devices > 0);

	physical_devices.resize(number_of_devices);
	result = vkEnumeratePhysicalDevices(instance.handle, &number_of_devices, physical_devices.data());
	physical_device.handle = physical_devices[0];

	vkGetPhysicalDeviceProperties(physical_device.handle, &physical_device.properties);
	vkGetPhysicalDeviceMemoryProperties(physical_device.handle, &physical_device.memoryProperties);

	uint32_t count;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device.handle, &count, NULL);

	physical_device.queueFamilyProperties.resize(count);
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device.handle, &count, physical_device.queueFamilyProperties.data());

	return result;
}

bool PhysicalDevice::setMemoryType(uint32_t type_bits, VkFlags requirements_mask, uint32_t& type_index) {
	for (uint32_t i = 0; i < 32; i++) {
		if ((type_bits & 1) == 1) {
			if ((memoryProperties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
				type_index = i;
				return true;
			}
		}
		type_bits >>= 1;
	}
	return false;
}