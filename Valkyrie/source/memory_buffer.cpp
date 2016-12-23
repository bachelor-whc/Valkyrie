#include "valkyrie/vulkan/memory_buffer.h"
#include "valkyrie/vulkan/physical_device.h"
#include "valkyrie/vulkan/device.h"
using namespace Vulkan;

MemoryBuffer::MemoryBuffer() : 
	m_size(0),
	m_offset(0),
	mp_information(nullptr) {

}

MemoryBuffer::~MemoryBuffer() {
	if (mp_information != nullptr)
		delete mp_information;
}

VkResult MemoryBuffer::allocate(const Device& device, PhysicalDevice& physical_device, const VkBufferUsageFlags usage, uint32_t size, VkBufferCreateInfo buffer_create) {
	if (handle != VK_NULL_HANDLE) {
		vkDestroyBuffer(device.handle, handle, nullptr);
		handle = VK_NULL_HANDLE;
	}
	if (memory != VK_NULL_HANDLE) {
		vkFreeMemory(device.handle, memory, nullptr);
		memory = VK_NULL_HANDLE;
	}
	VkResult result;
	buffer_create.usage = usage;
	buffer_create.size = size;

	result = vkCreateBuffer(device.handle, &buffer_create, nullptr, &handle);

	VkMemoryRequirements memory_requirements = {};
	vkGetBufferMemoryRequirements(device.handle, handle, &memory_requirements);

	VkMemoryAllocateInfo memory_allocate = {};
	memory_allocate.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate.allocationSize = size;
	bool found = physical_device.setMemoryType(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memory_allocate.memoryTypeIndex);
	result = vkAllocateMemory(device.handle, &memory_allocate, nullptr, &memory);

	if(result == VK_SUCCESS)
		m_size = size;

	return VK_SUCCESS;
}

VkResult MemoryBuffer::write(const Device& device, const void *data, uint32_t offset) {
	assert(memory != NULL && m_size != 0 && !m_writing_state);
	m_offset = offset;
	
	VkResult result;
	void *destination;
	
	result = vkMapMemory(device.handle, memory, m_offset, m_size, 0, &destination);
	memcpy(destination, data, m_size);
	vkUnmapMemory(device.handle, memory);
	result = vkBindBufferMemory(device.handle, handle, memory, m_offset);

	return result;
}

void* MemoryBuffer::startWriting(const Device& device, uint32_t offset) {
	assert(memory != NULL && m_size != 0);
	m_writing_state = true;
	m_offset = offset;

	VkResult result;
	void *destination;

	result = vkMapMemory(device.handle, memory, m_offset, m_size, 0, &destination);
	assert(result == VK_SUCCESS);
	return destination;
}

VkResult MemoryBuffer::endWriting(const Device& device) {
	VkResult result;
	vkUnmapMemory(device.handle, memory);
	result = vkBindBufferMemory(device.handle, handle, memory, m_offset);
	m_writing_state = false;
	return result;
}

VkDescriptorBufferInfo* MemoryBuffer::getInformationPointer() {
	if (mp_information == nullptr) {
		mp_information = NEW_NT VkDescriptorBufferInfo;
		assert(mp_information != nullptr);
		mp_information->buffer = handle;
		mp_information->range = m_size;
		mp_information->offset = m_offset;
	}
	return mp_information;
}