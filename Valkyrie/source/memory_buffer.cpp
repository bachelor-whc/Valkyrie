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

VkResult MemoryBuffer::allocate(const Device& device, PhysicalDevice& physical_device, const VkBufferUsageFlags usage, uint32_t size) {
	VkResult result;
	VkBufferCreateInfo buffer_create = {};
	buffer_create.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create.usage = usage;
	buffer_create.size = size;
	buffer_create.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

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
	assert(memory != NULL && m_size != 0);
	m_offset = offset;
	
	VkResult result;
	void *destination;
	
	result = vkMapMemory(device.handle, memory, m_offset, m_size, 0, &destination);
	memcpy(destination, data, m_size);
	vkUnmapMemory(device.handle, memory);
	result = vkBindBufferMemory(device.handle, handle, memory, m_offset);

	return result;
}

VkDescriptorBufferInfo* MemoryBuffer::getInformationPointer() {
	if (mp_information == nullptr) {
		mp_information = NEW_NT VkDescriptorBufferInfo;
		mp_information->buffer = handle;
		mp_information->range = m_size;
		mp_information->offset = m_offset;
	}
	return mp_information;
}