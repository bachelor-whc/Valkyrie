#include "valkyrie/vulkan/memory_buffer.h"
#include "valkyrie/vulkan/physical_device.h"
#include "valkyrie/vulkan/device.h"
using namespace Vulkan;

MemoryBuffer::MemoryBuffer() {

}

MemoryBuffer::~MemoryBuffer() {
	if (mp_information != nullptr) {
		delete mp_information;
	}
}

VkResult MemoryBuffer::allocate(const std::vector<VkBufferUsageFlags>& usages, const uint32_t size, VkBufferCreateInfo buffer_create) {
	if (handle != VK_NULL_HANDLE) {
		vkDestroyBuffer(g_device_handle, handle, nullptr);
		handle = VK_NULL_HANDLE;
	}
	if (memory != VK_NULL_HANDLE) {
		vkFreeMemory(g_device_handle, memory, nullptr);
		memory = VK_NULL_HANDLE;
	}
	VkResult result;
	VkBufferUsageFlags usage = NULL;
	for (auto flag : usages) {
		usage |= flag;
	}

	m_size = size;

	buffer_create.usage = usage;
	buffer_create.size = m_size;

	result = vkCreateBuffer(g_device_handle, &buffer_create, nullptr, &handle);

	VkMemoryRequirements memory_requirements = {};
	vkGetBufferMemoryRequirements(g_device_handle, handle, &memory_requirements);

	VkMemoryAllocateInfo memory_allocate = {};
	memory_allocate.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate.allocationSize = m_size;
	bool found = PhysicalDevice::setMemoryType(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memory_allocate.memoryTypeIndex);
	result = vkAllocateMemory(g_device_handle, &memory_allocate, nullptr, &memory);
	result = vkBindBufferMemory(g_device_handle, handle, memory, 0);
	return result;
}

VkResult MemoryBuffer::write(const void *data, const uint32_t offset, const uint32_t size) {
	assert(memory != NULL);
	VkResult result;
	void *destination;
	
	result = vkMapMemory(g_device_handle, memory, offset, size, NULL, &destination);
	memcpy(destination, data, size);
	vkUnmapMemory(g_device_handle, memory);
	
	return result;
}

void* MemoryBuffer::startWriting(const uint32_t offset, const uint32_t size) {
	assert(memory != VK_NULL_HANDLE);
	m_writing_state = true;

	VkResult result;
	void *destination;

	result = vkMapMemory(g_device_handle, memory, offset, size, NULL, &destination);
	assert(result == VK_SUCCESS);
	return destination;
}

void MemoryBuffer::endWriting() {
	vkUnmapMemory(g_device_handle, memory);
	m_writing_state = false;
}

VkDescriptorBufferInfo* Vulkan::MemoryBuffer::getInformationPointer() {
	return getInformationPointer(0, m_size);
}

VkDescriptorBufferInfo* MemoryBuffer::getInformationPointer(const uint32_t offset, const uint32_t size) {
	if (mp_information == nullptr) {
		mp_information = NEW_NT VkDescriptorBufferInfo();
		assert(mp_information != nullptr);
		mp_information->buffer = handle;
		mp_information->range = size;
		mp_information->offset = offset;
	}
	return mp_information;
}

void Vulkan::DestroyMemoryBuffer(MemoryBuffer& buffer) {
	if (buffer.handle != VK_NULL_HANDLE)
		vkDestroyBuffer(g_device_handle, buffer.handle, nullptr);
	if (buffer.memory != VK_NULL_HANDLE)
		vkFreeMemory(g_device_handle, buffer.memory, nullptr);
	buffer.handle = VK_NULL_HANDLE;
	buffer.memory = VK_NULL_HANDLE;
}
