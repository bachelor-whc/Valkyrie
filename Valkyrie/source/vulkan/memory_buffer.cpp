#include "valkyrie/vulkan/memory_buffer.h"
#include "valkyrie/vulkan/physical_device.h"
#include "valkyrie/vulkan/device.h"
using namespace Vulkan;

MemoryBuffer::MemoryBuffer() : 
	m_sizes(),
	m_offsets(),
	m_information_pointers() {

}

MemoryBuffer::~MemoryBuffer() {
	for(auto& p_information : m_information_pointers) {
		if (p_information != nullptr) {
			delete p_information;
		}
	}
}

VkResult MemoryBuffer::allocate(const std::vector<VkBufferUsageFlags>& usages, const std::vector<uint32_t>& sizes, VkBufferCreateInfo buffer_create) {
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

	m_sizes = sizes;
	m_offsets.resize(m_sizes.size());
	m_offsets[0] = 0;
	for (int i = 1; i < m_sizes.size(); ++i) {
		m_offsets[i] = m_offsets[i - 1] + m_sizes[i - 1];
	}

	m_information_pointers.resize(m_sizes.size());
	for (auto& p_information : m_information_pointers)
		p_information = nullptr;
	for (auto size : m_sizes)
		m_total_size += size;
	buffer_create.usage = usage;
	buffer_create.size = m_total_size;

	result = vkCreateBuffer(g_device_handle, &buffer_create, nullptr, &handle);

	VkMemoryRequirements memory_requirements = {};
	vkGetBufferMemoryRequirements(g_device_handle, handle, &memory_requirements);

	VkMemoryAllocateInfo memory_allocate = {};
	memory_allocate.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate.allocationSize = m_total_size;
	bool found = PhysicalDevice::setMemoryType(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memory_allocate.memoryTypeIndex);
	result = vkAllocateMemory(g_device_handle, &memory_allocate, nullptr, &memory);
	result = vkBindBufferMemory(g_device_handle, handle, memory, 0);
	return result;
}

VkResult MemoryBuffer::write(const std::vector<void*>& data, const std::vector<uint32_t>& offsets, const std::vector<uint32_t>& counts, const std::vector<uint32_t>& unit_sizes, const std::vector<uint32_t>& strides) {
	VkResult result;
	void* destination;
	
	result = vkMapMemory(g_device_handle, memory, 0, m_total_size, NULL, &destination);
	auto ptr_count = data.size();
	for(int i = 0; i < ptr_count; ++i) {
		auto p_data = (unsigned char*)data[i];
		auto d_data = (unsigned char*)destination;
		auto unit_size = unit_sizes[i];
		auto stride = strides[i];
		auto count = counts[i];
		auto offset = offsets[i];
		for(int j = 0; j < count; ++j) {
			auto d_shift = offset + j * stride;
			auto s_shift = j * unit_size;
			memcpy(d_data + d_shift, p_data + s_shift, unit_size);
		}
	}
	vkUnmapMemory(g_device_handle, memory);
	
	return result;
}

void* MemoryBuffer::startWriting(const int index) {
	m_writing_state = true;

	VkResult result;
	void *destination;

	result = vkMapMemory(g_device_handle, memory, m_offsets[index], m_sizes[index], NULL, &destination);
	assert(result == VK_SUCCESS);
	return destination;
}

void MemoryBuffer::endWriting() {
	VkResult result;
	vkUnmapMemory(g_device_handle, memory);
	m_writing_state = false;
}

VkDescriptorBufferInfo* MemoryBuffer::getInformationPointer(int index) {
	assert(index >= 0 && index < m_information_pointers.size());
	auto& p_information = m_information_pointers[index];
	if (m_information_pointers[index] == nullptr) {
		p_information = NEW_NT VkDescriptorBufferInfo;
		assert(p_information != nullptr);
		p_information->buffer = handle;
		p_information->range = m_sizes[index];
		p_information->offset = m_offsets[index];
	}
	return p_information;
}

void Vulkan::DestroyMemoryBuffer(MemoryBuffer & buffer) {
	if (buffer.handle != VK_NULL_HANDLE)
		vkDestroyBuffer(g_device_handle, buffer.handle, nullptr);
	if (buffer.memory != VK_NULL_HANDLE)
		vkFreeMemory(g_device_handle, buffer.memory, nullptr);
	buffer.handle = VK_NULL_HANDLE;
	buffer.memory = VK_NULL_HANDLE;
}
