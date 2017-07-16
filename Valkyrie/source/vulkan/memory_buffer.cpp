#include "valkyrie/vulkan/memory_buffer.h"
#include "valkyrie/vulkan/physical_device.h"
#include "valkyrie/vulkan/device.h"
#include "valkyrie/utility/vulkan_manager.h"
using namespace Vulkan;

MemoryBuffer::MemoryBuffer() {

}

MemoryBuffer::~MemoryBuffer() {
	if (mp_information != nullptr) {
		delete mp_information;
	}
}

VkResult MemoryBuffer::allocate(const std::vector<VkBufferUsageFlags>& usages, const uint32_t size, VkBufferCreateInfo buffer_create) {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	if (handle != VK_NULL_HANDLE) {
		vkDestroyBuffer(device, handle, nullptr);
		handle = VK_NULL_HANDLE;
	}
	if (memory != VK_NULL_HANDLE) {
		vkFreeMemory(device, memory, nullptr);
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

	result = vkCreateBuffer(device, &buffer_create, nullptr, &handle);

	VkMemoryRequirements memory_requirements = {};
	vkGetBufferMemoryRequirements(device, handle, &memory_requirements);

	VkMemoryAllocateInfo memory_allocate = {};
	memory_allocate.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	bool found = PhysicalDevice::setMemoryType(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memory_allocate.memoryTypeIndex);
	memory_allocate.allocationSize = memory_requirements.size;
	result = vkAllocateMemory(device, &memory_allocate, nullptr, &memory);
	result = vkBindBufferMemory(device, handle, memory, 0);
	return result;
}

VkResult MemoryBuffer::write(const void *data, const uint32_t offset, const uint32_t size) {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	assert(memory != NULL);
	VkResult result;
	void *destination;
	
	result = vkMapMemory(device, memory, offset, size, NULL, &destination);
	memcpy(destination, data, size);
	vkUnmapMemory(device, memory);
	
	return result;
}

void* MemoryBuffer::startWriting(const uint32_t offset, const uint32_t size) {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	assert(memory != VK_NULL_HANDLE);
	m_writing_state = true;

	VkResult result;
	void *destination;

	result = vkMapMemory(device, memory, offset, size, NULL, &destination);
	assert(result == VK_SUCCESS);
	return destination;
}

void MemoryBuffer::endWriting() {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	vkUnmapMemory(device, memory);
	m_writing_state = false;
}

VkWriteDescriptorSet MemoryBuffer::getWriteSet() {
	VkWriteDescriptorSet write = {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.descriptorCount = 1;
	write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	write.pBufferInfo = getInformationPointer();
	return write;
}

VkDescriptorBufferInfo* MemoryBuffer::getInformationPointer() {
	if (mp_information == nullptr) {
		mp_information = NEW_NT VkDescriptorBufferInfo();
		assert(mp_information != nullptr);
		mp_information->buffer = handle;
		mp_information->range = m_size;
		mp_information->offset = 0;
	}
	return mp_information;
}

void Vulkan::DestroyMemoryBuffer(MemoryBuffer& buffer) {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	if (buffer.handle != VK_NULL_HANDLE)
		vkDestroyBuffer(device, buffer.handle, nullptr);
	if (buffer.memory != VK_NULL_HANDLE)
		vkFreeMemory(device, buffer.memory, nullptr);
	buffer.handle = VK_NULL_HANDLE;
	buffer.memory = VK_NULL_HANDLE;
}
